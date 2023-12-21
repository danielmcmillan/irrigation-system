#ifndef REMOTE_UNIT_CONTROLLER_H
#define REMOTE_UNIT_CONTROLLER_H
#include "controller.h"
#include "controller-definition.h"
#include "remote-unit-controller-definition.h"

namespace IrrigationSystem
{
    struct RemoteUnitState
    {
        bool available;
        /** Tenths of a volt */
        uint8_t batteryVoltage;

        uint8_t solenoidOn;
        uint8_t solenoidDesiredOn;
        uint16_t sensorValue;

        /** Last update time, as multiples of 2^14 milliseconds (~16s) */
        uint8_t lastUpdated;
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
        uint16_t runCommand(const uint8_t *input, size_t inputSize, uint8_t *responseOut, size_t *responseSizeOut) override;

        void update() override;

    private:
        uint8_t controllerId;
        RemoteUnitControllerDefinition definition;
        EventHandler *eventHandler;
        RemoteUnitState remoteUnits[MAX_REMOTE_UNITS];

        void notifyError(uint8_t errorType, uint8_t remoteUnitId = 0);
        /** Write the default config to RF module. Returns whether successful. */
        bool applyRfConfig();
        void setRemoteUnitAvailable(int index, bool available);
        bool updateRemoteUnit(int index, bool updateSensor);
        bool handleRemoteUnitResponse(const RemoteUnit &remoteUnit, int remoteUnitIndex, uint8_t *packet, bool updateSensor);
        void handleSolenoidValuesChanged(const RemoteUnit &remoteUnit, int remoteUnitIndex, uint8_t previousSolenoidOn, uint8_t previousSolenoidDesiredOn);
    };
}

#endif
