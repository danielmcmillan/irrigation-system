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
        unsigned int getConfigLength(uint8_t type) const override;

        unsigned int getPropertyCount() const;
        uint16_t getPropertyIdAt(unsigned int index) const;
        unsigned int getPropertyLength(uint16_t id) const;
        bool getPropertyReadOnly(uint16_t id) const;

#ifdef INCLUDE_CONTROLLER_METADATA
        int getPropertyIndex(uint16_t id) const;
        uint8_t getName(char *nameOut, uint8_t maxLen) const;
        uint8_t getPropertyObjectName(uint16_t id, char *nameOut, uint8_t maxLen) const;
        uint8_t getPropertyName(uint16_t id, char *nameOut, uint8_t maxLen) const;
        PropertyFormat getPropertyFormat(uint16_t id) const;
#endif
    };

    namespace Vacon100ControllerProperties
    {
        constexpr uint8_t propertyIds[] = {
            0x01, // available
            0x02, // motorOn
            0x10, // status
            0x11, // actualSpeed
            0x12, // outputFrequency
            0x13, // motorSpeed
            0x14, // motorCurrent
            0x15, // motorTorque
            0x16, // motorPower
            0x17, // motorVoltage
            0x18, // dcLinkVoltage
            0x19, // activeFaultCode
            0x1a, // feedbackPressure
            0x1b, // driveTemp
            0x1c, // motorTemp
            0x1d, // energyUsed
            0x1e  // runTime
        };
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
