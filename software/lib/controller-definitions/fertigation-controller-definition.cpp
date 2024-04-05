#include <Arduino.h>
#include "fertigation-controller-definition.h"

namespace IrrigationSystem
{
    void FertigationControllerDefinition::reset()
    {
        enable = false;
    }

    void FertigationControllerDefinition::configure(uint8_t type, const uint8_t *data)
    {
        switch (type)
        {
        case FertigationConfigType::Enable:
            enable = data[0] > 0;
            break;
        default:
            break;
        }
    }

    unsigned int FertigationControllerDefinition::getConfigLength(uint8_t type) const
    {
        switch (type)
        {
        case FertigationConfigType::Enable:
            return 1;
        default:
            return 0;
        }
    }

    unsigned int FertigationControllerDefinition::getPropertyCount() const
    {
        return enable ? 1 : 0;
    }

    uint16_t FertigationControllerDefinition::getPropertyIdAt(unsigned int index) const
    {
        return FertigationControllerProperties::flowRate;
    }

    unsigned int FertigationControllerDefinition::getPropertyLength(uint16_t id) const
    {
        switch (id)
        {
        case FertigationControllerProperties::fertigationOn:
            return 1;
        case FertigationControllerProperties::flowRate:
            return 2;
        default:
            return 0;
        }
    }

    bool FertigationControllerDefinition::getPropertyReadOnly(uint16_t id) const
    {
        switch (id)
        {
        case FertigationControllerProperties::fertigationOn:
            return false;
        case FertigationControllerProperties::flowRate:
            return true;
        default:
            return true;
        }
    }

#ifdef INCLUDE_CONTROLLER_METADATA
    int FertigationControllerDefinition::getPropertyIndex(uint16_t id) const
    {
        switch (id)
        {
        case FertigationControllerProperties::fertigationOn:
            return 0;
        case FertigationControllerProperties::flowRate:
            return 1;
        default:
            return -1;
        }
    }

    uint8_t FertigationControllerDefinition::getName(char *nameOut, uint8_t maxLen) const
    {
        *nameOut = 'c';
        return 1;
    }

    uint8_t FertigationControllerDefinition::getPropertyObjectName(uint16_t id, char *nameOut, uint8_t maxLen) const
    {
        return getName(nameOut, maxLen);
    }

    uint8_t FertigationControllerDefinition::getPropertyName(uint16_t id, char *nameOut, uint8_t maxLen) const
    {
        *nameOut = 'p';
        return 1;
    }

    PropertyFormat FertigationControllerDefinition::getPropertyFormat(uint16_t id) const
    {
        return PropertyFormat{PropertyValueType::UnsignedInt, {.mul = {1, 0}}, ""};
    }
#endif
}
