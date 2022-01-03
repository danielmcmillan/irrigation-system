#ifndef _CONTROLLER_H
#define _CONTROLLER_H
#include "controller-definition.h"

namespace IrrigationSystem
{
    class Controller
    {
    public:
        /** Get the underlying controller definition. */
        virtual const ControllerDefinition &getDefinition() const = 0;

        /** End communication and reset configuration. The underlying definition is also reset. */
        virtual void reset() = 0;
        /** Add a configuration. The underlying definition is also configured. */
        virtual void configure(uint8_t type, const uint8_t *data) = 0;
        /** Initialise after being configured */
        virtual void begin() = 0;

        virtual uint32_t getPropertyValue(uint16_t id) const = 0;
        virtual uint32_t getPropertyDesiredValue(uint16_t id) const = 0;
        virtual void setPropertyDesiredValue(uint16_t id, uint32_t value) = 0;

        /** Indicate that desired values have been changed and should be applied now */
        virtual void applyPropertyValues() = 0;
        /** Update status and apply pending changes if necessary */
        virtual void update() = 0;
    };
}

#endif
