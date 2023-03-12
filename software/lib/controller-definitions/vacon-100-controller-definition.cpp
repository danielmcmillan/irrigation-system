#include <Arduino.h>
#include "vacon-100-controller-definition.h"

namespace IrrigationSystem
{
    void Vacon100ControllerDefinition::reset()
    {
    }

    void Vacon100ControllerDefinition::configure(uint8_t type, const uint8_t *data) {}

    unsigned int Vacon100ControllerDefinition::getConfigLength(uint8_t type) const
    {
        return 0;
    }

    unsigned int Vacon100ControllerDefinition::getPropertyCount() const
    {
        return (sizeof Vacon100ControllerProperties::propertyIds) / (sizeof Vacon100ControllerProperties::propertyIds[0]);
    }

    uint16_t Vacon100ControllerDefinition::getPropertyIdAt(unsigned int index) const
    {
        return Vacon100ControllerProperties::propertyIds[index];
    }

    unsigned int Vacon100ControllerDefinition::getPropertyLength(uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::available:
        case Vacon100ControllerProperties::motorOn:
        case Vacon100ControllerProperties::status:
            return 1;
        case Vacon100ControllerProperties::actualSpeed:
        case Vacon100ControllerProperties::outputFrequency:
        case Vacon100ControllerProperties::motorSpeed:
        case Vacon100ControllerProperties::motorCurrent:
        case Vacon100ControllerProperties::motorTorque:
        case Vacon100ControllerProperties::motorPower:
        case Vacon100ControllerProperties::motorVoltage:
        case Vacon100ControllerProperties::dcLinkVoltage:
        case Vacon100ControllerProperties::activeFaultCode:
        case Vacon100ControllerProperties::feedbackPressure:
        case Vacon100ControllerProperties::driveTemp:
        case Vacon100ControllerProperties::motorTemp:
            return 2;
        case Vacon100ControllerProperties::energyUsed:
        case Vacon100ControllerProperties::runTime:
            return 4;
        default:
            return 0;
        }
    }

    bool Vacon100ControllerDefinition::getPropertyReadOnly(uint16_t id) const
    {
        switch (id)
        {
        case Vacon100ControllerProperties::motorOn:
            return false;
        default:
            return true;
        }
    }
}
