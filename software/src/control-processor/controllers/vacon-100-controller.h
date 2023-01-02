#ifndef VACON100_CONTROLLER_H
#define VACON100_CONTROLLER_H
#include "controller.h"
#include "controller-definition.h"
#include "vacon-100-modbus-client.h"
#include "vacon-100-controller-definition.h"
#include <NeoSWSerial.h>

namespace IrrigationSystem
{

    class Vacon100Controller : public IrrigationSystem::Controller
    {
    public:
        Vacon100Controller(uint8_t controllerId);
        void setEventHandler(EventHandler &handler);
        void reset() override;
        void configure(uint8_t type, const uint8_t *data) override;
        void begin() override;

        const IrrigationSystem::ControllerDefinition &getDefinition() const override;

        uint32_t getPropertyValue(uint16_t id) const override;
        uint32_t getPropertyDesiredValue(uint16_t id) const override;
        void setPropertyDesiredValue(uint16_t id, uint32_t value) override;

        void applyPropertyValues() override;
        void update() override;

    private:
        uint8_t controllerId;
        Vacon100ControllerDefinition definition;
        NeoSWSerial serial;
        Vacon100Client vacon;
        Vacon100Data values;
        bool desiredMotorOn;
        bool available;
        EventHandler *eventHandler;

        uint32_t getPropertyValueFromValues(const Vacon100Data &values, uint16_t id) const;
        void setAvailable(bool available);
    };
}

#endif
