#ifndef _REMOTE_UNIT_CONTROLLER_DEFINITION_H
#define _REMOTE_UNIT_CONTROLLER_DEFINITION_H
#include "controller-definition.h"

#define MAX_REMOTE_UNITS 20 // (5 bytes per remote unit)
#define MAX_SOLENOIDS 30    // (4 bytes per solenoid)

namespace IrrigationSystem
{
    struct RemoteUnit
    {
        /** Custom identifier for the remote unit */
        uint8_t id;
        /** RF node number for addressing */
        uint16_t nodeId;
    };
    struct Solenoid
    {
        uint8_t id;
        uint8_t remoteUnitId;
        uint8_t numberAtRemoteUnit;
    };
    enum RemoteUnitConfigType : uint8_t
    {
        AddRemoteUnitConfig = 0x01,
        AddRemoteUnitSolenoidConfig = 0x02
    };
    enum RemoteUnitPropertyType : uint8_t
    {
        RemoteUnitAvailable = 0x01,
        RemoteUnitBattery = 0x02,
        RemoteUnitSolenoidOn = 0x03
    };

    class RemoteUnitControllerDefinition : public IrrigationSystem::ControllerDefinition
    {
    public:
        void reset();
        void configure(uint8_t type, const uint8_t *data);

        unsigned int getPropertyCount() const;
        uint16_t getPropertyIdAt(unsigned int index) const;
        unsigned int getPropertyLength(uint16_t id) const;
        bool getPropertyReadOnly(uint16_t id) const;

        uint8_t getRemoteUnitCount() const;
        const RemoteUnit &getRemoteUnitAt(int index) const;
        /** Get the index of the RemoteUnit definition with specified id, or -1 if it doesn't exist */
        int getRemoteUnitIndex(uint8_t id) const;
        uint8_t getSolenoidCount() const;
        const Solenoid &getSolenoidAt(int index) const;
        /** Get the index of the Solenoid definition with specified id, or -1 if it doesn't exist  */
        int getSolenoidIndex(uint8_t id) const;
        /** Get the property id for the remote unit or solenoid at the given index */
        uint16_t getPropertyId(RemoteUnitPropertyType type, int index) const;

    private:
        RemoteUnit remoteUnits[MAX_REMOTE_UNITS];
        uint8_t remoteUnitCount;
        Solenoid solenoids[MAX_SOLENOIDS];
        uint8_t solenoidCount;
    };
}

#endif
