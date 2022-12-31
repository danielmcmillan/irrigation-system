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
        Vacon100Controller();
        void reset();
        void configure(uint8_t type, const uint8_t *data);
        void begin();

        const IrrigationSystem::ControllerDefinition &getDefinition() const;

        uint32_t getPropertyValue(uint16_t id) const;
        uint32_t getPropertyDesiredValue(uint16_t id) const;
        void setPropertyDesiredValue(uint16_t id, uint32_t value);

        void applyPropertyValues();
        void update();

    private:
        Vacon100ControllerDefinition definition;
        NeoSWSerial serial;
        Vacon100Client vacon;
        Vacon100Data values;
        bool desiredMotorOn;
        bool available;
    };
}

#endif
