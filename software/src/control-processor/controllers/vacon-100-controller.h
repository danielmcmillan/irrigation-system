#ifndef VACON100_CONTROLLER_H
#define VACON100_CONTROLLER_H
#include "controller.h"
#include "controller-definition.h"
#include "vacon-100-modbus-client.h"
#include "vacon-100-controller-definition.h"

namespace IrrigationSystem
{
    class Vacon100Controller : public IrrigationSystem::Controller
    {
    public:
        Vacon100Controller(uint8_t controllerId);
        void setEventHandler(EventHandler &handler);
        void reset() override;
        void configure(uint8_t type, const uint8_t *data) override;
        bool begin() override;

        const IrrigationSystem::ControllerDefinition &getDefinition() const override;

        bool getPropertyValue(uint16_t id, uint32_t *value) const;
        uint32_t getPropertyDesiredValue(uint16_t id) const override;
        void setPropertyDesiredValue(uint16_t id, uint32_t value) override;
        uint16_t runCommand(const uint8_t *input, size_t inputSize, uint8_t *responseOut, size_t *responseSizeOut) override;

        void update() override;

    private:
        uint8_t controllerId;
        Vacon100ControllerDefinition definition;
        Vacon100Client vacon;
        Vacon100Data values;
        bool desiredMotorOn;
        bool motorRelayOn;
        bool desiredMotorOnIndeterminate;
        uint8_t lastIdMapUpdate;
        uint8_t lastUpdateTime;

        bool idMapUpdated;
        uint8_t errorCount;
        EventHandler *eventHandler;

        uint32_t getPropertyValueFromValues(const Vacon100Data &values, uint16_t id) const;
        void updateErrorCount(bool reset);
        void notifyError(uint8_t data);
    };
}

#endif
