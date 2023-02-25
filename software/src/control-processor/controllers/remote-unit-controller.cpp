#include <Arduino.h>
#include "remote-unit-controller.h"
#include "logging.h"
extern "C"
{
#include "yl-800t.h"
}
#include "remote-unit-packet.h"

#define RF_ENABLE_PIN 6
#define RF_MODULE_RESPONSE_TIMEOUT 2000
#define RF_FREQUENCY (434l * 1l << 14) // 434 MHz
#define RF_TX_POWER 5

#define PACKET_BUFFER_SIZE 16
#define REMOTE_UNIT_TIMEOUT 5000

// Number of consecutive errors beyond which connection to a remote unit is considered unavailable
#define MAX_ERROR_COUNT 2
// Number of times to retry communication with a rmeote unit before considering it a failure
#define RETRY_COUNT 3
// Time in milliseconds between starting heartbeat for all remote units
#define HEARTBEAT_INTERVAL 600000 // 10 minutes

namespace IrrigationSystem
{
    RemoteUnitController::RemoteUnitController(uint8_t controllerId) : controllerId(controllerId),
                                                                       definition(),
                                                                       eventHandler(nullptr),
                                                                       remoteUnitValues({}),
                                                                       solenoidValues({}),
                                                                       lastHeartbeatMillis(0),
                                                                       remoteUnitHeartbeatIndex(0)
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
        // Enable RF module
        pinMode(RF_ENABLE_PIN, OUTPUT);
        digitalWrite(RF_ENABLE_PIN, LOW);

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
        memset(&remoteUnitValues, 0, sizeof remoteUnitValues);
        memset(&solenoidValues, 0, sizeof solenoidValues);
        lastHeartbeatMillis = 0;
        remoteUnitHeartbeatIndex = 0;
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
                return remoteUnitValues[index].errorCount <= MAX_ERROR_COUNT;
            }
            break;
        case RemoteUnitPropertyType::RemoteUnitBattery:
            index = definition.getRemoteUnitIndex(subId);
            if (index >= 0)
            {
                return remoteUnitValues[index].batteryVoltage;
            }
            break;
        case RemoteUnitPropertyType::RemoteUnitSolenoidOn:
            index = definition.getSolenoidIndex(subId);
            if (index >= 0)
            {
                return solenoidValues[index].on;
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
                return solenoidValues[index].desiredOn;
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
            int index = definition.getSolenoidIndex(subId);
            if (index >= 0)
            {
                solenoidValues[index].desiredOn = value > 0;
                if (eventHandler != nullptr)
                {
                    eventHandler->handlePropertyDesiredValueChanged(controllerId, id, 1, value);
                }
            }
            break;
        }
        LOG_ERROR("setPropertyDesiredValue with unknown Remote Unit property");
    }

    void RemoteUnitController::applyPropertyValues()
    {
        // TODO write out desired solenoid state to remote units
    }

    void RemoteUnitController::update()
    {
        // Check if new heartbeat process should be started
        if (remoteUnitHeartbeatIndex >= definition.getRemoteUnitCount() && (millis() - lastHeartbeatMillis) > HEARTBEAT_INTERVAL)
        {
            remoteUnitHeartbeatIndex = 0;
        }
        // Continue heartbeat on next remote unit if it is in progress
        if (remoteUnitHeartbeatIndex < definition.getRemoteUnitCount())
        {
            bool success = readFromRemoteUnit(remoteUnitHeartbeatIndex);
            updateRemoteUnitErrorCount(remoteUnitHeartbeatIndex, success);

            if (success || remoteUnitValues[remoteUnitHeartbeatIndex].errorCount > MAX_ERROR_COUNT)
            {
                ++remoteUnitHeartbeatIndex;
                if (remoteUnitHeartbeatIndex == definition.getRemoteUnitCount())
                {
                    lastHeartbeatMillis = millis();
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
        Serial.setTimeout(RF_MODULE_RESPONSE_TIMEOUT);
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
        while (Serial.available())
        {
            Serial.read();
        }
        Serial.write(message, length);
        Serial.flush();
        Serial.readBytes(message, 25);
        return yl800tReceiveWriteAllParameters(message) == 0;
    }

    // Increment the error count for a remote unit, or set it back to 0 if reset is true
    void RemoteUnitController::updateRemoteUnitErrorCount(int index, bool reset)
    {
        RemoteUnitPropertyValues &values = remoteUnitValues[index];
        bool previousAvailable = values.errorCount <= MAX_ERROR_COUNT;
        if (reset)
        {
            values.errorCount = 0;
        }
        else if (values.errorCount < 255)
        {
            ++values.errorCount;
        }
        bool available = values.errorCount <= MAX_ERROR_COUNT;
        if (previousAvailable != available && eventHandler != nullptr)
        {
            eventHandler->handlePropertyValueChanged(controllerId, definition.getPropertyId(RemoteUnitPropertyType::RemoteUnitAvailable, index), 1, available ? 1 : 0);
        }
    }

    bool RemoteUnitController::readFromRemoteUnit(int index)
    {
        // TODO read more than just battery
        const RemoteUnit &remoteUnit = definition.getRemoteUnitAt(remoteUnitHeartbeatIndex);
        uint8_t buffer[PACKET_BUFFER_SIZE + 2];

        // Build request packet
        uint8_t *packet = buffer + 2;
        size_t packetSize = RemoteUnitPacket::createPacket(packet, PACKET_BUFFER_SIZE, remoteUnit.nodeId);
        packetSize = RemoteUnitPacket::addCommandToPacket(packet, PACKET_BUFFER_SIZE, packetSize, RemoteUnitPacket::RemoteUnitCommand::GetBatteryVoltage, nullptr);
        packetSize = RemoteUnitPacket::finalisePacket(packet, PACKET_BUFFER_SIZE, packetSize, false);
        // Add big-endian node ID to initial bytes for LoRa module
        buffer[0] = remoteUnit.nodeId >> 8;
        buffer[1] = remoteUnit.nodeId;
        packetSize += 2;

        // Send request packet
        if (Serial.write(buffer, packetSize) != packetSize)
        {
            notifyError(0x02, remoteUnit.id);
            LOG_ERROR("Failed to write to Serial");
            return false;
        }

        // Read response packet
        // All of the data should arrive at once, so apply timeout only to first byte
        Serial.setTimeout(REMOTE_UNIT_TIMEOUT);
        size_t read = Serial.readBytes(buffer, 1);
        if (read == 0)
        {
            notifyError(0x03, remoteUnit.id);
            LOG_ERROR("Timeout waiting for response on Serial");
            return false;
        }
        Serial.setTimeout(100);
        read = Serial.readBytes(buffer + 1, PACKET_BUFFER_SIZE - 1);

        // Handle response packet
        packetSize = RemoteUnitPacket::getPacket(buffer, read + 1, &packet);
        int numCommands = IrrigationSystem::RemoteUnitPacket::validatePacket(packet, packetSize, true);
        if (numCommands <= 0)
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
        // TODO assuming response is for one battery voltage command, 1 byte of data
        const uint8_t *responseData;
        IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand responseCommand =
            IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(packet, 0, &responseData);
        if (responseCommand != RemoteUnitPacket::RemoteUnitCommand::GetBatteryVoltageResponse)
        {
            notifyError(0x07, remoteUnit.id);
            LOG_ERROR("Remote unit response is for unexpected commands");
            return false;
        }

        // Update property values based on response
        uint8_t previousBatteryVoltage = remoteUnitValues[index].batteryVoltage;
        uint8_t batteryVoltage = responseData[0];
        remoteUnitValues[index].batteryVoltage = batteryVoltage;
        if (eventHandler != nullptr && previousBatteryVoltage != batteryVoltage)
        {
            eventHandler->handlePropertyValueChanged(controllerId, definition.getPropertyId(RemoteUnitPropertyType::RemoteUnitBattery, index), 1, batteryVoltage);
        }

        Serial.flush();
        return true;
    }
}
