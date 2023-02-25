#include <Arduino.h>
#include "remote-unit-controller.h"
#include "logging.h"
extern "C"
{
#include "yl-800t.h"
}

#define RF_ENABLE_PIN 6
#define RF_MODULE_RESPONSE_TIMEOUT 2000
#define RF_FREQUENCY (434l * 1l << 14) // 434 MHz
#define RF_TX_POWER 5

namespace IrrigationSystem
{
    RemoteUnitController::RemoteUnitController(uint8_t controllerId) : controllerId(controllerId),
                                                                       definition(),
                                                                       eventHandler(nullptr)
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
        // TODO Serial begin
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
                return remoteUnitValues[index].available;
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
        // TODO: heartbeat remote units, periodically check battery voltage, send event when values change
    }

    void RemoteUnitController::notifyError(uint8_t data)
    {
        if (eventHandler != nullptr)
        {
            // uint16_t errorCode = vacon.getErrorCode();
            uint8_t errorPayload[] = {controllerId, data};
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
}
