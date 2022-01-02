#ifndef _VACON100_CONTROLLER_DEFINITION_H
#define _VACON100_CONTROLLER_DEFINITION_H
#include "controller-definition.h"

namespace IrrigationSystem
{
    class Vacon100ControllerDefinition : public IrrigationSystem::ControllerDefinition
    {
    public:
        void reset();
        void configure(uint8_t type, const uint8_t *data);

        int getPropertyLength(uint16_t id);
        bool getPropertyReadOnly(uint16_t id);
    };

    namespace Vacon100ControllerProperties
    {
        enum : uint16_t
        {
            available = 0x01,
            motorOn = 0x02,
            status = 0x10,
            actualSpeed = 0x11,
            outputFrequency = 0x12,
            motorSpeed = 0x13,
            motorCurrent = 0x14,
            motorTorque = 0x15,
            motorPower = 0x16,
            motorVoltage = 0x17,
            dcLinkVoltage = 0x18,
            activeFaultCode = 0x19,
        };
    }
}

#endif
