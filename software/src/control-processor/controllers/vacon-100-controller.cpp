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
    Vacon100Controller::Vacon100Controller() : definition(),
                                               serial(MAX485_RO, MAX485_DI),
                                               vacon(serial, MAX485_RE, MAX485_DE, MAX485_DI),
                                               values(),
                                               desiredMotorOn(false),
                                               available(false)
    {
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
            // TODO handle error
            LOG_ERROR("Failed to start Vacon 100 client");
            vacon.printError();
        }
        while (!vacon.initIdMapping())
        {
            // TODO handle error
            LOG_ERROR("Failed to set up Vacon 100 ID mappings");
            vacon.printError();
        }
    }

    const IrrigationSystem::ControllerDefinition &Vacon100Controller::getDefinition() const
    {
        return definition;
    }

    uint32_t Vacon100Controller::getPropertyValue(uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::available:
            return available;
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
            break;
        default:
            LOG_ERROR("setPropertyDesiredValue with unknown Vacon 100 property");
            break;
        }
    }

    void Vacon100Controller::applyPropertyValues()
    {
        if (getPropertyValue(Vacon100ControllerProperties::motorOn) != desiredMotorOn)
        {
            if (!vacon.setStart(desiredMotorOn))
            {
                // TODO handle error
                available = false;
                LOG_ERROR("Failed to write to Vacon 100");
                vacon.printError();
            }
        }
    }

    void Vacon100Controller::update()
    {
        if (vacon.readInputRegisters(&values))
        {
            available = true;
        }
        else
        {
            available = false;
            LOG_ERROR("Failed to read from Vacon 100");
            vacon.printError();
        }
    }

}
