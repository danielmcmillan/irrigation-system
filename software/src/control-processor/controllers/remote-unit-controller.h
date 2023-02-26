#ifndef REMOTE_UNIT_CONTROLLER_H
#define REMOTE_UNIT_CONTROLLER_H
#include "controller.h"
#include "controller-definition.h"
#include "remote-unit-controller-definition.h"

namespace IrrigationSystem
{
    struct RemoteUnitPropertyValues
    {
        bool available;
        /** Tenths of a volt */
        uint8_t batteryVoltage;
    };
    struct RemoteUnitSolenoidPropertyValues
    {
        uint8_t on : 1;
        uint8_t desiredOn : 1;
    };

    class RemoteUnitController : public IrrigationSystem::Controller
    {
    public:
        RemoteUnitController(uint8_t controllerId);
        void setEventHandler(EventHandler &handler);
        void reset() override;
        void configure(uint8_t type, const uint8_t *data) override;
        bool begin() override;

        const IrrigationSystem::ControllerDefinition &getDefinition() const override;

        uint32_t getPropertyValue(uint16_t id) const override;
        uint32_t getPropertyDesiredValue(uint16_t id) const override;
        void setPropertyDesiredValue(uint16_t id, uint32_t value) override;

        void applyPropertyValues() override;
        void update() override;

    private:
        uint8_t controllerId;
        RemoteUnitControllerDefinition definition;
        EventHandler *eventHandler;
        RemoteUnitPropertyValues remoteUnitValues[MAX_REMOTE_UNITS];
        RemoteUnitSolenoidPropertyValues solenoidValues[MAX_SOLENOIDS];
        /** Time that heartbeat was last completed for all remote units */
        unsigned long lastHeartbeatMillis;
        /**
         * The next remote unit to perform heartbeat check on.
         * If there is no heartbeat check in progress, this would be equal to definition.remoteUnitCount.
         */
        uint8_t remoteUnitHeartbeatIndex;
        uint8_t remoteUnitErrorCount;

        void notifyError(uint8_t errorType, uint8_t remoteUnitId = 0);
        /** Write the default config to RF module. Returns whether successful. */
        bool applyRfConfig();
        void updateRemoteUnitAvailable(int index, bool available);
        bool readFromRemoteUnit(int index);
    };
}

#endif
