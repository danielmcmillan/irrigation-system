#include <Arduino.h>
#include "remote-unit-controller-definition.h"
#include "binary-util.h"

namespace IrrigationSystem
{
    void RemoteUnitControllerDefinition::reset()
    {
        remoteUnitCount = 0;
        solenoidCount = 0;
    }

    void RemoteUnitControllerDefinition::configure(uint8_t type, const uint8_t *data)
    {
        switch (type)
        {
        case RemoteUnitConfigType::AddRemoteUnitConfig:
            if (remoteUnitCount < MAX_REMOTE_UNITS)
            {
                remoteUnits[remoteUnitCount].id = data[0];
                remoteUnits[remoteUnitCount].nodeId = read16LE(&data[1]);
                ++remoteUnitCount;
            }
            break;
        case RemoteUnitConfigType::AddRemoteUnitSolenoidConfig:
            if (solenoidCount < MAX_SOLENOIDS)
            {
                solenoids[solenoidCount].id = data[0];
                solenoids[solenoidCount].remoteUnitId = data[1];
                solenoids[solenoidCount].numberAtRemoteUnit = data[2];
                ++solenoidCount;
            }
            break;
        default:
            break;
        }
    }

    unsigned int RemoteUnitControllerDefinition::getConfigLength(uint8_t type) const
    {
        switch (type)
        {
        case RemoteUnitConfigType::AddRemoteUnitConfig:
            return 3;
        case RemoteUnitConfigType::AddRemoteUnitSolenoidConfig:
            return 3;
        default:
            return 0;
        }
    }

    unsigned int RemoteUnitControllerDefinition::getPropertyCount() const
    {
        return remoteUnitCount * 2u + solenoidCount;
    }

    uint16_t RemoteUnitControllerDefinition::getPropertyIdAt(unsigned int index) const
    {
        RemoteUnitPropertyType type;
        uint8_t subIndex; // Index within remote units or solenoids
        // Order: all remote unit available, then remote unit battery, then solenoid on
        if (index < remoteUnitCount)
        {
            type = RemoteUnitPropertyType::RemoteUnitAvailable;
            subIndex = index;
        }
        else if (index < remoteUnitCount * 2u)
        {
            type = RemoteUnitPropertyType::RemoteUnitBattery;
            subIndex = index - remoteUnitCount;
        }
        else if (index < (remoteUnitCount * 2u + solenoidCount))
        {
            type = RemoteUnitPropertyType::RemoteUnitSolenoidOn;
            subIndex = index - remoteUnitCount * 2u;
        }
        else
        {
            return 0;
        }
        return getPropertyId(type, subIndex);
    }

    unsigned int RemoteUnitControllerDefinition::getPropertyLength(uint16_t id) const
    {
        return 1;
    }

    bool RemoteUnitControllerDefinition::getPropertyReadOnly(uint16_t id) const
    {
        return (id >> 8) != RemoteUnitPropertyType::RemoteUnitSolenoidOn;
    }

#ifdef INCLUDE_CONTROLLER_METADATA
    uint8_t RemoteUnitControllerDefinition::getName(char *nameOut, uint8_t maxLen) const
    {
        return stpncpy(nameOut, "Remote Unit", maxLen) - nameOut;
    }

    uint8_t RemoteUnitControllerDefinition::getPropertyObjectName(uint16_t id, char *nameOut, uint8_t maxLen) const
    {
        RemoteUnitPropertyType type = (RemoteUnitPropertyType)(id >> 8);
        uint8_t subId = id & 0xff;
        int len;
        if (type == RemoteUnitPropertyType::RemoteUnitSolenoidOn)
        {
            len = snprintf(nameOut, maxLen, "Solenoid|%u", subId);
        }
        else
        {
            len = snprintf(nameOut, maxLen, "RemoteUnit|%u", subId);
        }
        if (len < maxLen)
        {
            return len;
        }
        else
        {
            return maxLen - 1;
        }
    }

    uint8_t RemoteUnitControllerDefinition::getPropertyName(uint16_t id, char *nameOut, uint8_t maxLen) const
    {
        RemoteUnitPropertyType type = (RemoteUnitPropertyType)(id >> 8);
        const char *result;
        switch (type)
        {
        case RemoteUnitPropertyType::RemoteUnitAvailable:
            result = "Available";
            break;
        case RemoteUnitPropertyType::RemoteUnitBattery:
            result = "Voltage";
            break;
        case RemoteUnitPropertyType::RemoteUnitSolenoidOn:
            result = "Open";
            break;
        default:
            return 0;
        }
        return stpncpy(nameOut, result, maxLen) - nameOut;
    }

    PropertyFormat RemoteUnitControllerDefinition::getPropertyFormat(uint16_t id) const
    {
        RemoteUnitPropertyType type = (RemoteUnitPropertyType)(id >> 8);
        switch (type)
        {
        case RemoteUnitPropertyType::RemoteUnitAvailable:
            return {PropertyValueType::BooleanFlags, {.booleanCount = 1}};
        case RemoteUnitPropertyType::RemoteUnitBattery:
        {
            // Won't allow returning initializer list directly. Compiler bug?
            PropertyFormat format = {PropertyValueType::UnsignedInt, {.mul = {10, -1}}, "V"};
            return format;
        }
        case RemoteUnitPropertyType::RemoteUnitSolenoidOn:
            return {PropertyValueType::BooleanFlags, {.booleanCount = 1}};
        default:
            return {};
        }
    }
#endif

    uint8_t RemoteUnitControllerDefinition::getRemoteUnitCount() const
    {
        return remoteUnitCount;
    }

    const RemoteUnit &RemoteUnitControllerDefinition::getRemoteUnitAt(int index) const
    {
        return remoteUnits[index];
    }

    int RemoteUnitControllerDefinition::getRemoteUnitIndex(uint8_t id) const
    {
        for (int i = 0; i < remoteUnitCount; ++i)
        {
            if (remoteUnits[i].id == id)
            {
                return i;
            }
        }
        return -1;
    }

    uint8_t RemoteUnitControllerDefinition::getSolenoidCount() const
    {
        return solenoidCount;
    }

    const Solenoid &RemoteUnitControllerDefinition::getSolenoidAt(int index) const
    {
        return solenoids[index];
    }

    int RemoteUnitControllerDefinition::getSolenoidIndex(uint8_t id) const
    {
        for (int i = 0; i < solenoidCount; ++i)
        {
            if (solenoids[i].id == id)
            {
                return i;
            }
        }
        return -1;
    }

    uint16_t RemoteUnitControllerDefinition::getPropertyId(RemoteUnitPropertyType type, int index) const
    {
        uint8_t id = (type == RemoteUnitPropertyType::RemoteUnitAvailable || type == RemoteUnitPropertyType::RemoteUnitBattery)
                         ? getRemoteUnitAt(index).id
                         : getSolenoidAt(index).id;
        return (type << 8) + id;
    }
}
