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

        unsigned int getPropertyCount() const;
        uint16_t getPropertyIdAt(unsigned int index) const;
        unsigned int getPropertyLength(uint16_t id) const;
        bool getPropertyReadOnly(uint16_t id) const;
    };

    namespace Vacon100ControllerProperties
    {
        constexpr uint8_t propertyIds[] = {
            0x01,
            0x02,
            0x10,
            0x11,
            0x12,
            0x13,
            0x14,
            0x15,
            0x16,
            0x17,
            0x18,
            0x19,
            0x1a,
            0x1b,
            0x1c,
            0x1d,
            0x1e};
        enum : uint8_t
        {
            available = propertyIds[0],
            motorOn = propertyIds[1],
            status = propertyIds[2],
            actualSpeed = propertyIds[3],
            outputFrequency = propertyIds[4],
            motorSpeed = propertyIds[5],
            motorCurrent = propertyIds[6],
            motorTorque = propertyIds[7],
            motorPower = propertyIds[8],
            motorVoltage = propertyIds[9],
            dcLinkVoltage = propertyIds[10],
            activeFaultCode = propertyIds[11],
            feedbackPressure = propertyIds[12],
            driveTemp = propertyIds[13],
            motorTemp = propertyIds[14],
            energyUsed = propertyIds[15],
            runTime = propertyIds[16]
        };
    }
}

#endif
