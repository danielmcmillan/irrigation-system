#include <Arduino.h>
#include "remote-unit-controller.h"
#include "logging.h"

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
        // TODO Serial begin, configure RF module
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
}
