#include <Arduino.h>
#include "vacon-100-controller.h"

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
                                               desiredMotorOn(false)
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
        vacon.begin(); // TODO handle error
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
            return 1; // TODO track availability
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
        default:
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
            return 0;
        }
    }

    void Vacon100Controller::setPropertyDesiredValue(uint16_t id, uint32_t value)
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            desiredMotorOn = value > 0;
        }
    }

    void Vacon100Controller::applyPropertyValues()
    {
        if (getPropertyValue(Vacon100ControllerProperties::motorOn) != desiredMotorOn)
        {
            vacon.setStart(desiredMotorOn);
        }
    }

    void Vacon100Controller::update()
    {
        if (!vacon.readInputRegisters(&values))
        {
            // TODO handle error
        }
    }

}
