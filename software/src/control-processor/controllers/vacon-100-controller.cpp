#include <Arduino.h>
#include <errno.h>
#include "vacon-100-controller.h"
#include "logging.h"

#define MAX485_RE A3
#define MAX485_DE A2
#define MAX485_RO 9
#define MAX485_DI 8

namespace IrrigationSystem
{
    Vacon100Controller::Vacon100Controller(uint8_t controllerId) : controllerId(controllerId),
                                                                   definition(),
                                                                   serial(MAX485_RO, MAX485_DI),
                                                                   vacon(serial, MAX485_RE, MAX485_DE, MAX485_DI),
                                                                   values(),
                                                                   desiredMotorOn(false),
                                                                   available(false),
                                                                   eventHandler(nullptr)
    {
    }

    void Vacon100Controller::setEventHandler(EventHandler &handler)
    {
        this->eventHandler = &handler;
    }

    void Vacon100Controller::reset()
    {
        vacon.end();
        serial.end();
        definition.reset();
    }

    void Vacon100Controller::configure(uint8_t type, const uint8_t *data)
    {
        definition.configure(type, data);
    }

    void Vacon100Controller::begin()
    {
        serial.begin(9600);
        if (!vacon.begin())
        {
            notifyError(0x00);
            LOG_ERROR("Failed to start Vacon 100 client");
            vacon.printError();
        }
        while (!vacon.initIdMapping())
        {
            notifyError(0x01);
            LOG_ERROR("Failed to set up Vacon 100 ID mappings");
            vacon.printError();
        }
        // TODO is any other action required when begin fails?
    }

    const IrrigationSystem::ControllerDefinition &Vacon100Controller::getDefinition() const
    {
        return definition;
    }

    uint32_t Vacon100Controller::getPropertyValue(uint16_t id) const
    {
        if (id == Vacon100ControllerProperties::available)
        {
            return available;
        }
        else
        {
            return this->getPropertyValueFromValues(values, id);
        }
    }

    uint32_t Vacon100Controller::getPropertyDesiredValue(uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            return desiredMotorOn;
        default:
            LOG_ERROR("getPropertyDesiredValue with unknown Vacon 100 property");
            return 0;
        }
    }

    void Vacon100Controller::setPropertyDesiredValue(uint16_t id, uint32_t value)
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            desiredMotorOn = value > 0;
            eventHandler->handlePropertyDesiredValueChanged(controllerId, id, 1, value);
            break;
        default:
            LOG_ERROR("setPropertyDesiredValue with unknown Vacon 100 property");
            return;
        }
    }

    void Vacon100Controller::applyPropertyValues()
    {
        if (getPropertyValue(Vacon100ControllerProperties::motorOn) != desiredMotorOn)
        {
            if (!vacon.setStart(desiredMotorOn))
            {
                setAvailable(false);
                notifyError(0x02);
                LOG_ERROR("Failed to write to Vacon 100");
                vacon.printError();
            }
        }
    }

    void Vacon100Controller::update()
    {
        Vacon100Data oldValues = values;
        if (vacon.readInputRegisters(&values))
        {
            setAvailable(true);
            // Raise events for changes to vacon data, all properties except for first one (available)
            for (unsigned int i = 1; i < definition.getPropertyCount(); ++i)
            {
                uint8_t propertyId = definition.getPropertyIdAt(i);
                uint32_t newValue = getPropertyValueFromValues(values, propertyId);
                if (newValue != getPropertyValueFromValues(oldValues, propertyId))
                {
                    eventHandler->handlePropertyValueChanged(controllerId, propertyId, definition.getPropertyLength(propertyId), newValue);
                }
            }
        }
        else
        {
            setAvailable(false);
            notifyError(0x03);
            LOG_ERROR("Failed to read from Vacon 100");
            vacon.printError();
        }
    }

    uint32_t Vacon100Controller::getPropertyValueFromValues(const Vacon100Data &values, uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            return (values.statusWord & Vacon100StatusWordMask::run) > 0;
        case Vacon100ControllerProperties::status:
            return values.statusWord;
        case Vacon100ControllerProperties::actualSpeed:
            return values.actualSpeed;
        case Vacon100ControllerProperties::outputFrequency:
            return values.outputFrequency;
        case Vacon100ControllerProperties::motorSpeed:
            return values.motorSpeed;
        case Vacon100ControllerProperties::motorCurrent:
            return values.motorCurrent;
        case Vacon100ControllerProperties::motorTorque:
            return values.motorTorque;
        case Vacon100ControllerProperties::motorPower:
            return values.motorPower;
        case Vacon100ControllerProperties::motorVoltage:
            return values.motorVoltage;
        case Vacon100ControllerProperties::dcLinkVoltage:
            return values.dcLinkVoltage;
        case Vacon100ControllerProperties::activeFaultCode:
            return values.activeFaultCode;
        case Vacon100ControllerProperties::feedbackPressure:
            return values.feedbackPressure;
        case Vacon100ControllerProperties::driveTemp:
            return values.driveTemp;
        case Vacon100ControllerProperties::motorTemp:
            return values.motorTemp;
        case Vacon100ControllerProperties::energyUsed:
            return values.energyUsed;
        case Vacon100ControllerProperties::runTime:
            return values.runTime;
        default:
            LOG_ERROR("getPropertyValue with unknown Vacon 100 property");
            return 0;
        }
    }

    void Vacon100Controller::setAvailable(bool available)
    {
        if (this->available != available)
        {
            eventHandler->handlePropertyValueChanged(controllerId, Vacon100ControllerProperties::available, 1, available ? 1 : 0);
            this->available = available;
        }
    }
    void Vacon100Controller::notifyError(uint8_t data)
    {
        uint16_t errorCode = vacon.getErrorCode();
        uint8_t errorPayload[] = {controllerId, data, (uint8_t)errorCode, (uint8_t)(errorCode >> 8)};
        eventHandler->handleEvent(EventType::controllerError, sizeof errorPayload, errorPayload);
    }
}
