#ifndef FERTIGATION_CONTROLLER_H
#define FERTIGATION_CONTROLLER_H
#include <Arduino.h>
#include "controller.h"
#include "controller-definition.h"
#include "fertigation-controller-definition.h"

namespace IrrigationSystem
{
    class FertigationController : public IrrigationSystem::Controller
    {
    public:
        FertigationController(uint8_t controllerId);
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
        FertigationControllerDefinition definition;
        EventHandler *eventHandler;

        unsigned long lastFlowRateCalculationMillis = 0;
        uint16_t flowRate = 0; // counts per 1000 000 ms
    };
}

#endif
