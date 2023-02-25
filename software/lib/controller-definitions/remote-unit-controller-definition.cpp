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
                remoteUnits[remoteUnitCount].nodeNumber = read16LE(&data[1]);
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

    unsigned int RemoteUnitControllerDefinition::getPropertyCount() const
    {
        return remoteUnitCount * 2u + solenoidCount;
    }

    uint16_t RemoteUnitControllerDefinition::getPropertyIdAt(unsigned int index) const
    {
        // Order: all remote unit available, then remote unit battery, then solenoid on
        if (index < remoteUnitCount)
        {
            return (RemoteUnitPropertyType::RemoteUnitAvailable << 8) + index;
        }
        else if (index < remoteUnitCount * 2u)
        {
            return (RemoteUnitPropertyType::RemoteUnitBattery << 8) + (index - remoteUnitCount);
        }
        else if (index < (remoteUnitCount * 2u + solenoidCount))
        {
            return (RemoteUnitPropertyType::RemoteUnitSolenoidOn << 8) + (index - remoteUnitCount * 2);
        }
        else
        {
            return 0;
        }
    }

    unsigned int RemoteUnitControllerDefinition::getPropertyLength(uint16_t id) const
    {
        return 1;
    }

    bool RemoteUnitControllerDefinition::getPropertyReadOnly(uint16_t id) const
    {
        return (id >> 8) != RemoteUnitPropertyType::RemoteUnitSolenoidOn;
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
}
