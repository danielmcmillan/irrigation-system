#ifndef _FERTIGATION_CONTROLLER_DEFINITION_H
#define _FERTIGATION_CONTROLLER_DEFINITION_H
#include "controller-definition.h"

namespace IrrigationSystem
{
    enum FertigationConfigType : uint8_t
    {
        Enable = 0x01,
    };

    class FertigationControllerDefinition : public IrrigationSystem::ControllerDefinition
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

        bool enable;
    };

    namespace FertigationControllerProperties
    {
        constexpr uint8_t propertyIds[] = {
            0x01, // fertigation on
            0x02, // flow rate
        };
        enum : uint8_t
        {
            fertigationOn = propertyIds[0],
            flowRate = propertyIds[1],
        };
    }
}

#endif
