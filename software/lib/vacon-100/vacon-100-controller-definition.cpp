#include <Arduino.h>
#include "vacon-100-controller-definition.h"

namespace IrrigationSystem
{
    void Vacon100ControllerDefinition::reset()
    {
    }

    void Vacon100ControllerDefinition::configure(uint8_t type, const uint8_t *data) {}

    int Vacon100ControllerDefinition::getPropertyLength(uint16_t id)
    {
        switch (id)
        {
        case Vacon100ControllerProperties::available:
        case Vacon100ControllerProperties::motorOn:
            return 1;
        case Vacon100ControllerProperties::status:
        case Vacon100ControllerProperties::actualSpeed:
        case Vacon100ControllerProperties::outputFrequency:
        case Vacon100ControllerProperties::motorSpeed:
        case Vacon100ControllerProperties::motorCurrent:
        case Vacon100ControllerProperties::motorTorque:
        case Vacon100ControllerProperties::motorPower:
        case Vacon100ControllerProperties::motorVoltage:
        case Vacon100ControllerProperties::dcLinkVoltage:
        case Vacon100ControllerProperties::activeFaultCode:
            return 2;
        default:
            return 0;
        }
    }

    bool Vacon100ControllerDefinition::getPropertyReadOnly(uint16_t id)
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
