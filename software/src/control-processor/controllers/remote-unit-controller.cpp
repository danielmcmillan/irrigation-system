#include <Arduino.h>
#include "remote-unit-controller.h"
#include "logging.h"
extern "C"
{
#include "yl-800t.h"
}
#include "remote-unit-packet.h"
#include "binary-util.h"

#define SERIAL_OBJ Serial1
#if HW_REV == 2
#define SERIAL_RX 25 // UART RX
#define SERIAL_TX 26 // UART TX
#else
#define SERIAL_RX 18 // UART RX
#define SERIAL_TX 19 // UART TX
#define RF_ENABLE_PIN 5
#endif
#define RF_MODULE_RESPONSE_TIMEOUT 2000
#define RF_FREQUENCY (434l * 1l << 14) // 434 MHz
#define RF_TX_POWER 7

// Time to wait for data on Serial after sending a request to a remote unit
#define REMOTE_UNIT_TIMEOUT 8000
// Number of times communication with a remote unit can fail before considering it unavailable
#define AVAILABLE_FAILURE_COUNT 2
// Number of times to do an immediate retry after failure to apply changes before applying backoff delay
#define ACTIVE_RETRY_COUNT 2
// Time in 2^14 milliseconds between heartbeats for each remote units
#define REMOTE_UNIT_UPDATE_INTERVAL 73 // ~20 minutes
// Time in 2^14 milliseconds between heartbeats for remote units with active solenoids
#define REMOTE_UNIT_ACTIVE_UPDATE_INTERVAL 11 // ~3 minutes

#define PACKET_BUFFER_SIZE 31
// Flag indicating an indeterminate state of a remote unit's solenoids
#define SOLENOID_ON_INDETERMINATE 0xff

namespace IrrigationSystem
{
    RemoteUnitController::RemoteUnitController(uint8_t controllerId) : controllerId(controllerId),
                                                                       definition(),
                                                                       eventHandler(nullptr),
                                                                       remoteUnits({})
    {
    }

    void RemoteUnitController::setEventHandler(EventHandler &handler)
    {
        this->eventHandler = &handler;
    }

    void RemoteUnitController::configure(uint8_t type, const uint8_t *data)
    {
        definition.configure(type, data);
    }

    bool RemoteUnitController::begin()
    {
        if (definition.getRemoteUnitCount() == 0)
        {
            return true;
        }
        SERIAL_OBJ.begin(9600, SERIAL_8N1, SERIAL_RX, SERIAL_TX);
#ifdef RF_ENABLE_PIN
        // Enable RF module
        pinMode(RF_ENABLE_PIN, OUTPUT);
        digitalWrite(RF_ENABLE_PIN, LOW);
        delay(500);
#endif

        if (!applyRfConfig())
        {
            notifyError(0x01);
            LOG_ERROR("Failed to configure RF module");
            return false;
        }
        return true;
    }

    void RemoteUnitController::reset()
    {
        memset(&remoteUnits, 0, sizeof remoteUnits);
        // Desired state of solenoids is initially indeterminate, so will take whatever is the actual solenoid state
        for (int i = 0; i < MAX_REMOTE_UNITS; ++i)
        {
            remoteUnits[i].solenoidDesiredOn = SOLENOID_ON_INDETERMINATE;
            remoteUnits[i].nextUpdateTime = millis() >> 14;
            remoteUnits[i].updateFailCount = 0;
        }
        definition.reset();
    }

    const IrrigationSystem::ControllerDefinition &RemoteUnitController::getDefinition() const
    {
        return definition;
    }

    uint32_t RemoteUnitController::getPropertyValue(uint16_t id) const
    {
        uint8_t type = id >> 8;
        uint8_t subId = id & 0xff;
        int index;

        switch (type)
        {
        case RemoteUnitPropertyType::RemoteUnitAvailable:
            index = definition.getRemoteUnitIndex(subId);
            if (index >= 0)
            {
                return remoteUnits[index].available;
            }
            break;
        case RemoteUnitPropertyType::RemoteUnitBattery:
            index = definition.getRemoteUnitIndex(subId);
            if (index >= 0)
            {
                return remoteUnits[index].batteryVoltage;
            }
            break;
        case RemoteUnitPropertyType::RemoteUnitSensor:
            index = definition.getRemoteUnitIndex(subId);
            if (index >= 0)
            {
                return remoteUnits[index].sensorValue;
            }
            break;
        case RemoteUnitPropertyType::RemoteUnitSolenoidOn:
            index = definition.getSolenoidIndex(subId);
            if (index >= 0)
            {
                Solenoid const &solenoid = definition.getSolenoidAt(index);
                index = definition.getRemoteUnitIndex(solenoid.remoteUnitId);
                // index must be valid, assuming valid configuration
                return (remoteUnits[index].solenoidOn & (1u << solenoid.numberAtRemoteUnit)) > 0;
            }
            break;
        }
        LOG_ERROR("getPropertyValue with unknown Remote Unit property");
        return 0;
    }

    uint32_t RemoteUnitController::getPropertyDesiredValue(uint16_t id) const
    {
        uint8_t type = id >> 8;
        uint8_t subId = id & 0xff;
        switch (type)
        {
        case RemoteUnitPropertyType::RemoteUnitSolenoidOn:
            int index = definition.getSolenoidIndex(subId);
            if (index >= 0)
            {
                Solenoid const &solenoid = definition.getSolenoidAt(index);
                // index must be valid, assuming valid configuration
                index = definition.getRemoteUnitIndex(solenoid.remoteUnitId);
                uint8_t solenoidDesiredOn = remoteUnits[index].solenoidDesiredOn;
                // If desired value is indeterminate then it implicitly matches the actual state
                if (solenoidDesiredOn == SOLENOID_ON_INDETERMINATE)
                {
                    solenoidDesiredOn = remoteUnits[index].solenoidOn;
                }
                return (solenoidDesiredOn & (1u << solenoid.numberAtRemoteUnit)) > 0;
            }
            break;
        }
        LOG_ERROR("getPropertyDesiredValue with unknown Remote Unit property");
        return 0;
    }

    void RemoteUnitController::setPropertyDesiredValue(uint16_t id, uint32_t value)
    {
        uint8_t type = id >> 8;
        uint8_t subId = id & 0xff;
        switch (type)
        {
        case RemoteUnitPropertyType::RemoteUnitSolenoidOn:
        {
            int index = definition.getSolenoidIndex(subId);
            if (index >= 0)
            {
                Solenoid const &solenoid = definition.getSolenoidAt(index);
                // index must be valid, given valid configuration
                index = definition.getRemoteUnitIndex(solenoid.remoteUnitId);
                // setting any solenoid value causes all desired values to become determinate
                if (remoteUnits[index].solenoidDesiredOn == SOLENOID_ON_INDETERMINATE)
                {
                    remoteUnits[index].solenoidDesiredOn = remoteUnits[index].solenoidOn;
                }
                // Update the bit that corresponds to the solenoid
                uint8_t mask = 1u << solenoid.numberAtRemoteUnit;
                uint8_t previousValue = remoteUnits[index].solenoidDesiredOn & mask;
                if (value > 0)
                {
                    remoteUnits[index].solenoidDesiredOn |= mask;
                }
                else
                {
                    remoteUnits[index].solenoidDesiredOn &= ~mask;
                }
                if (eventHandler != nullptr && (value > 0) != (previousValue > 0))
                {
                    eventHandler->handlePropertyDesiredValueChanged(controllerId, id, 1, value);
                }
                // Schedule immediate update for the remote unit if the desired value differs from the actual
                if (remoteUnits[index].solenoidDesiredOn != remoteUnits[index].solenoidOn)
                {
                    remoteUnits[index].nextUpdateTime = millis() >> 14;
                    remoteUnits[index].updateFailCount = 0;
                }
            }
            break;
        }
        default:
            LOG_ERROR("setPropertyDesiredValue with unknown Remote Unit property");
            break;
        }
    }

    uint16_t RemoteUnitController::runCommand(const uint8_t *input, size_t inputSize, uint8_t *responseOut, size_t *responseSizeOut)
    {
        if (inputSize >= 4 && input[0] == 1)
        {
            // Request to send remote unit command
            uint16_t nodeId = read16LE(&input[1]);
            RemoteUnitPacket::RemoteUnitCommand command = (RemoteUnitPacket::RemoteUnitCommand)input[3];
            const uint8_t *requestData = &input[4];
            size_t requestDataSize = inputSize - 4;

            // Build request packet
            uint8_t buffer[PACKET_BUFFER_SIZE + 2];
            uint8_t *packet = buffer + 2;
            size_t packetSize = RemoteUnitPacket::createPacket(packet, PACKET_BUFFER_SIZE, nodeId);
            uint8_t *packetData;
            packetSize = RemoteUnitPacket::addCommandToPacket(packet, PACKET_BUFFER_SIZE, packetSize, command, &packetData);
            if (packetSize == 0)
            {
                return 8;
            }
            memcpy(packetData, requestData, requestDataSize);
            packetSize = RemoteUnitPacket::finalisePacket(packet, PACKET_BUFFER_SIZE, packetSize, false);
            // Add big-endian node ID to initial bytes for LoRa module
            buffer[0] = nodeId >> 8;
            buffer[1] = nodeId;
            packetSize += 2;

            // Send request packet
            if (SERIAL_OBJ.write(buffer, packetSize) != packetSize)
            {
                return 2;
            }

            // Read response packet
            SERIAL_OBJ.setTimeout(REMOTE_UNIT_TIMEOUT);
            size_t read = SERIAL_OBJ.readBytes(buffer, 1);
            if (read == 0)
            {
                return 3;
            }
            SERIAL_OBJ.setTimeout(100);
            read = SERIAL_OBJ.readBytes(buffer + 1, PACKET_BUFFER_SIZE - 1);

            // Handle response packet
            packetSize = RemoteUnitPacket::getPacket(buffer, read + 1, &packet);
            int result = IrrigationSystem::RemoteUnitPacket::validatePacket(packet, packetSize, true);
            if (result < 0)
            {
                return 4 - result;
            }
            else if (result != 1)
            {
                return 4;
            }
            if (RemoteUnitPacket::getNodeId(packet) != nodeId)
            {
                return 7;
            }
            const uint8_t *responsePacketData;
            size_t responsePacketDataSize = packetSize - 6; // data size excludes node id, command, end byte, crc
            IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand responseCommand = IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(packet, 0, &responsePacketData);

            // Write response
            responseOut[0] = (uint8_t)responseCommand;
            memcpy(&responseOut[1], responsePacketData, responsePacketDataSize);
            *responseSizeOut = 1 + responsePacketDataSize;

            SERIAL_OBJ.flush();
            return 0;
        }
        else
        {
            return 1;
        }
    }

    void RemoteUnitController::update()
    {
        uint16_t now = millis() >> 14;
        // Find the next remote unit to be updated.
        // Prioritise first by whether change is due to be applied, followed by amount of time past since nextUpdateTime
        int remoteUnitIndex = -1;
        bool anyChangeDue = false;
        uint16_t maxOverdue = 0;
        for (int i = 0; i < definition.getRemoteUnitCount(); ++i)
        {
            // Compare time to nextUpdateTime, correctly handling overflow of the time values
            uint16_t overdue = now - remoteUnits[i].nextUpdateTime;
            if (overdue < (1 << 15))
            {
                bool isChangeDue = remoteUnits[i].solenoidDesiredOn != SOLENOID_ON_INDETERMINATE && remoteUnits[i].solenoidOn != remoteUnits[i].solenoidDesiredOn;
                if ((isChangeDue && !anyChangeDue) || (isChangeDue == anyChangeDue && overdue >= maxOverdue))
                {
                    remoteUnitIndex = i;
                    anyChangeDue = isChangeDue;
                    maxOverdue = overdue;
                }
            }
        }

        if (remoteUnitIndex >= 0)
        {
            if (updateRemoteUnit(remoteUnitIndex))
            {
                // Successful - schedule to update again later
                setRemoteUnitAvailable(remoteUnitIndex, true);
                remoteUnits[remoteUnitIndex].updateFailCount = 0;
                uint8_t updateInterval = (remoteUnits[remoteUnitIndex].solenoidOn | remoteUnits[remoteUnitIndex].solenoidDesiredOn) == 0
                                             ? REMOTE_UNIT_UPDATE_INTERVAL
                                             : REMOTE_UNIT_ACTIVE_UPDATE_INTERVAL;
                remoteUnits[remoteUnitIndex].nextUpdateTime = now + updateInterval;
            }
            else
            {
                uint8_t failCount = remoteUnits[remoteUnitIndex].updateFailCount;
                // Increment failCount to a maximum of 7 to limit backoff delay to ~70 minutes
                if (failCount < 7)
                {
                    ++failCount;
                }
                remoteUnits[remoteUnitIndex].updateFailCount = failCount;
                uint16_t delay = 0;
                if (!anyChangeDue || failCount > ACTIVE_RETRY_COUNT)
                {
                    if (anyChangeDue)
                    {
                        failCount -= ACTIVE_RETRY_COUNT;
                    }
                    delay = 2 * 1 << remoteUnits[remoteUnitIndex].updateFailCount;
                }
                remoteUnits[remoteUnitIndex].nextUpdateTime = now + delay;
                if (remoteUnits[remoteUnitIndex].updateFailCount > AVAILABLE_FAILURE_COUNT)
                {
                    setRemoteUnitAvailable(remoteUnitIndex, false);
                }
            }
        }
    }

    void RemoteUnitController::notifyError(uint8_t errorType, uint8_t remoteUnitId)
    {
        if (eventHandler != nullptr)
        {
            uint8_t errorPayload[] = {controllerId, errorType, remoteUnitId};
            eventHandler->handleEvent(EventType::controllerError, sizeof errorPayload, errorPayload);
        }
    }

    bool RemoteUnitController::applyRfConfig()
    {
        SERIAL_OBJ.setTimeout(RF_MODULE_RESPONSE_TIMEOUT);
        YL800TReadWriteAllParameters params = {
            .serialBaudRate = YL_800T_BAUD_RATE_9600,
            .serialParity = YL_800T_PARITY_NONE,
            .rfFrequency = RF_FREQUENCY,
            .rfSpreadingFactor = YL_800T_RF_SPREADING_FACTOR_2048,
            .mode = YL_800T_RF_MODE_CENTRAL,
            .rfBandwidth = YL_800T_RF_BANDWIDTH_125K,
            .nodeId = 0,
            .netId = 0,
            .rfTransmitPower = RF_TX_POWER,
            .breathCycle = YL_800T_BREATH_CYCLE_2S,
            .breathTime = YL_800T_BREATH_TIME_32MS};
        uint8_t message[25] = {0};
        uint8_t length = yl800tSendWriteAllParameters(&params, message);
        while (SERIAL_OBJ.available())
        {
            SERIAL_OBJ.read();
        }
        SERIAL_OBJ.write(message, length);
        SERIAL_OBJ.flush();
        SERIAL_OBJ.readBytes(message, 25);
        return yl800tReceiveWriteAllParameters(message) == 0;
    }

    // Increment the error count for a remote unit, or set it back to 0 on success
    void RemoteUnitController::setRemoteUnitAvailable(int index, bool available)
    {
        remoteUnits[index].available = available;
        if (eventHandler != nullptr)
        {
            eventHandler->handlePropertyValueChanged(controllerId, definition.getPropertyId(RemoteUnitPropertyType::RemoteUnitAvailable, index), 1, available);
        }
    }

    bool RemoteUnitController::updateRemoteUnit(int index)
    {
        const RemoteUnit &remoteUnit = definition.getRemoteUnitAt(index);
        unsigned long timeout = REMOTE_UNIT_TIMEOUT;
        uint8_t buffer[PACKET_BUFFER_SIZE + 2];

        // Build request packet: get battery + clear faults + get/set solenoid state
        uint8_t *packet = buffer + 2;
        size_t packetSize = RemoteUnitPacket::createPacket(packet, PACKET_BUFFER_SIZE, remoteUnit.nodeId);
        packetSize = RemoteUnitPacket::addCommandToPacket(packet, PACKET_BUFFER_SIZE, packetSize, RemoteUnitPacket::RemoteUnitCommand::GetBatteryVoltage, nullptr);
        packetSize = RemoteUnitPacket::addCommandToPacket(packet, PACKET_BUFFER_SIZE, packetSize, RemoteUnitPacket::RemoteUnitCommand::ClearFaults, nullptr);
        if (remoteUnits[index].solenoidDesiredOn == SOLENOID_ON_INDETERMINATE)
        {
            packetSize = RemoteUnitPacket::addCommandToPacket(packet, PACKET_BUFFER_SIZE, packetSize, RemoteUnitPacket::RemoteUnitCommand::GetSolenoidState, nullptr);
        }
        else
        {
            uint8_t *solenoidState;
            packetSize = RemoteUnitPacket::addCommandToPacket(packet, PACKET_BUFFER_SIZE, packetSize, RemoteUnitPacket::RemoteUnitCommand::SetSolenoidState, &solenoidState);
            *solenoidState = remoteUnits[index].solenoidDesiredOn;
        }
        if (remoteUnit.hasSensor)
        {
            uint8_t *sensorId;
            packetSize = RemoteUnitPacket::addCommandToPacket(packet, PACKET_BUFFER_SIZE, packetSize, RemoteUnitPacket::RemoteUnitCommand::GetSensorValue, &sensorId);
            *sensorId = 0;
        }
        packetSize = RemoteUnitPacket::finalisePacket(packet, PACKET_BUFFER_SIZE, packetSize, false);
        // Add big-endian node ID to initial bytes for LoRa module
        buffer[0] = remoteUnit.nodeId >> 8;
        buffer[1] = remoteUnit.nodeId;
        packetSize += 2;

        // Send request packet
        if (SERIAL_OBJ.write(buffer, packetSize) != packetSize)
        {
            notifyError(0x02, remoteUnit.id);
            LOG_ERROR("Failed to write to Serial");
            return false;
        }

        // Read response packet
        // All of the data should arrive at once, so apply timeout only to first byte
        SERIAL_OBJ.setTimeout(timeout);
        size_t read = SERIAL_OBJ.readBytes(buffer, 1);
        if (read == 0)
        {
            notifyError(0x03, remoteUnit.id);
            LOG_ERROR("Timeout waiting for response on Serial");
            return false;
        }
        SERIAL_OBJ.setTimeout(100);
        read = SERIAL_OBJ.readBytes(buffer + 1, PACKET_BUFFER_SIZE - 1);

        // Handle response packet
        packetSize = RemoteUnitPacket::getPacket(buffer, read + 1, &packet);

        int numCommands = IrrigationSystem::RemoteUnitPacket::validatePacket(packet, packetSize, true);
        if (numCommands != (remoteUnit.hasSensor ? 4 : 3))
        {
            if (numCommands == -2)
            {
                notifyError(0x04, remoteUnit.id);
                LOG_ERROR("Remote unit response has invalid CRC");
            }
            else
            {
                notifyError(0x05, remoteUnit.id);
                LOG_ERROR("Remote unit response includes invalid commands or data");
            }
            return false;
        }
        if (RemoteUnitPacket::getNodeId(packet) != remoteUnit.nodeId)
        {
            notifyError(0x06, remoteUnit.id);
            LOG_ERROR("Remote unit response is valid but for unexpected node id");
            return false;
        }
        if (!handleRemoteUnitResponse(remoteUnit, index, packet))
        {
            notifyError(0x07, remoteUnit.id);
            LOG_ERROR("Remote unit response is for unexpected commands");
            return false;
        }

        SERIAL_OBJ.flush();
        return true;
    }

    bool RemoteUnitController::handleRemoteUnitResponse(const RemoteUnit &remoteUnit, int remoteUnitIndex, uint8_t *packet)
    {
        const uint8_t *responseData;
        IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand responseCommand;

        // Update battery value based on response
        responseCommand = IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(packet, 0, &responseData);
        if (responseCommand != RemoteUnitPacket::RemoteUnitCommand::GetBatteryVoltageResponse)
        {
            return false;
        }
        uint8_t previousBatteryVoltage = remoteUnits[remoteUnitIndex].batteryVoltage;
        uint8_t batteryVoltage = responseData[0];
        remoteUnits[remoteUnitIndex].batteryVoltage = batteryVoltage;
        if (eventHandler != nullptr && previousBatteryVoltage != batteryVoltage)
        {
            eventHandler->handlePropertyValueChanged(controllerId, definition.getPropertyId(RemoteUnitPropertyType::RemoteUnitBattery, remoteUnitIndex), 1, batteryVoltage);
        }

        // Send events for faults
        responseCommand = IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(packet, 1, &responseData);
        if (responseCommand != RemoteUnitPacket::RemoteUnitCommand::ClearFaultsResponse && responseCommand != RemoteUnitPacket::RemoteUnitCommand::GetFaultsResponse)
        {
            return false;
        }
        if (eventHandler != nullptr)
        {
            for (uint8_t faultNumber = 0; faultNumber < 8; ++faultNumber)
            {
                if ((responseData[0] & (1u << faultNumber)) > 0)
                {
                    notifyError(0x80 | faultNumber, remoteUnit.id);
                }
            }
        }

        // Update solenoid state based on response
        responseCommand = IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(packet, 2, &responseData);
        if (responseCommand != RemoteUnitPacket::RemoteUnitCommand::SetSolenoidStateResponse && responseCommand != RemoteUnitPacket::RemoteUnitCommand::GetSolenoidStateResponse)
        {
            return false;
        }
        uint8_t previousSolenoidOn = remoteUnits[remoteUnitIndex].solenoidOn;
        remoteUnits[remoteUnitIndex].solenoidOn = responseData[0];
        uint8_t previousSolenoidDesiredOn = remoteUnits[remoteUnitIndex].solenoidDesiredOn;
        remoteUnits[remoteUnitIndex].solenoidDesiredOn = responseData[0];
        // Send events for any solenoid that changed state
        handleSolenoidValuesChanged(remoteUnit, remoteUnitIndex, previousSolenoidOn, previousSolenoidDesiredOn);

        if (remoteUnit.hasSensor)
        {
            responseCommand = IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(packet, 3, &responseData);
            if (responseCommand != RemoteUnitPacket::RemoteUnitCommand::GetSensorValueResponse)
            {
                return false;
            }
            uint16_t previousSensorValue = remoteUnits[remoteUnitIndex].sensorValue;
            uint8_t sensorReadResult = responseData[0];
            uint8_t sensorError = sensorReadResult & 0x3f;
            uint16_t sensorValue = read16LE(&responseData[1]);
            remoteUnits[remoteUnitIndex].sensorValue = sensorValue;
            if (eventHandler != nullptr)
            {
                uint16_t propertyId = definition.getPropertyId(RemoteUnitPropertyType::RemoteUnitSensor, remoteUnitIndex);
                // Send sensor value update event if it has been read successfully since last check or the actual value has changed
                if ((sensorReadResult & 0xc0) == 0xc0 /*=success&unread*/ || previousSensorValue != sensorValue)
                {
                    eventHandler->handlePropertyValueChanged(controllerId, propertyId, 2, sensorValue);
                }
                // Send property error if sensor read failed since last check
                if ((sensorReadResult & 0xc0) == 0x40 /*=!success&unread*/)
                {
                    eventHandler->handlePropertyError(controllerId, propertyId, 1, sensorReadResult & 0x3f);
                }
            }
        }

        return true;
    }

    // Handle updated solenoidOn and solenoidDesiredOn value for a remote unit.
    void RemoteUnitController::handleSolenoidValuesChanged(const RemoteUnit &remoteUnit, int remoteUnitIndex, uint8_t previousSolenoidOn, uint8_t previousSolenoidDesiredOn)
    {
        if (eventHandler == nullptr)
        {
            return;
        }
        // Send events if property values changed
        for (int solenoidIndex = 0; solenoidIndex < definition.getSolenoidCount(); ++solenoidIndex)
        {
            const Solenoid &solenoid = definition.getSolenoidAt(solenoidIndex);
            if (solenoid.remoteUnitId == remoteUnit.id)
            {
                uint16_t propertyId = definition.getPropertyId(RemoteUnitPropertyType::RemoteUnitSolenoidOn, solenoidIndex);
                uint8_t mask = 1u << solenoid.numberAtRemoteUnit;
                uint8_t value = (remoteUnits[remoteUnitIndex].solenoidOn & mask) > 0;
                uint8_t previousValue = (previousSolenoidOn & mask) > 0;
                uint8_t desiredValue = remoteUnits[remoteUnitIndex].solenoidDesiredOn == SOLENOID_ON_INDETERMINATE
                                           ? value
                                           : (remoteUnits[remoteUnitIndex].solenoidDesiredOn & mask) > 0;
                uint8_t previousDesiredValue = previousSolenoidDesiredOn == SOLENOID_ON_INDETERMINATE
                                                   ? previousValue
                                                   : (previousSolenoidDesiredOn & mask) > 0;

                if (value != previousValue)
                {
                    eventHandler->handlePropertyValueChanged(controllerId, propertyId, 1, value);
                }
                if (desiredValue != previousDesiredValue)
                {
                    eventHandler->handlePropertyDesiredValueChanged(controllerId, propertyId, 1, value);
                }
            }
        }
    }
}
