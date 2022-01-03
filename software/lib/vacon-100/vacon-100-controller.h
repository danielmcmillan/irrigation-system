#ifndef VACON100_CONTROLLER
#define VACON100_CONTROLLER
#include "controller.h"
#include "controller-definition.h"
#include "vacon-100-modbus-client.h"
#include "vacon-100-controller-definition.h"
#include <NeoSWSerial.h>

namespace IrrigationSystem
{
    /**
     * Controls a set of devices.
     * Provides access to devices by exposing a set of properties.
     */
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
    };
}

#endif
