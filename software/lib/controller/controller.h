#ifndef _CONTROLLER_H
#define _CONTROLLER_H
#include "controller-definition.h"
#include "event-handler.h"

namespace IrrigationSystem
{
    /**
     * Controls a set of devices.
     * Provides access to devices by exposing a set of properties.
     */
    class Controller
    {
    public:
        /** Get the underlying controller definition. */
        virtual const ControllerDefinition &getDefinition() const = 0;

        /**
         * Set the event handler to handle events generated by the controller.
         * It's lifetime should match or exceed the lifetime of the controller.
         */
        virtual void setEventHandler(EventHandler &handler) = 0;

        /** Add a configuration. The underlying definition is also configured. */
        virtual void configure(uint8_t type, const uint8_t *data) = 0;
        /**
         * Initialise after being configured.
         * The operation should be idempotent, and may be called multiple times in failure scenarios.
         *
         * Returns whether initialisation was successful.
         */
        virtual bool begin() = 0;
        /**
         * End communication and reset configuration. The underlying definition is also reset.
         *
         * The operation should be idempotent. It may be called multiple times and before configure or begin is called.
         */
        virtual void reset() = 0;

        virtual uint32_t getPropertyValue(uint16_t id) const = 0;
        virtual uint32_t getPropertyDesiredValue(uint16_t id) const = 0;
        virtual void setPropertyDesiredValue(uint16_t id, uint32_t value) = 0;
        virtual int runCommand(const uint8_t *input, unsigned int inputSize, uint8_t *responseOut, unsigned int *responseSizeOut) = 0;

        /** Update status and apply pending changes if necessary */
        virtual void update() = 0;
    };
}

#endif
