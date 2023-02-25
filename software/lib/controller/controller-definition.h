#ifndef _CONTROLLER_DEFINITION_H
#define _CONTROLLER_DEFINITION_H
#include <inttypes.h>

namespace IrrigationSystem
{
    /** Provides information about a controller given a set of configuration. */
    class ControllerDefinition
    {
    public:
        virtual void reset() = 0;
        virtual void configure(uint8_t type, const uint8_t *data) = 0;
        /**
         * Get the number of available properties.
         */
        virtual unsigned int getPropertyCount() const = 0;
        /**
         * Get the id of the property at the given index, assuming some ordering of properties.
         * The order of properties is irrelevant, so long as it is consistent.
         */
        virtual uint16_t getPropertyIdAt(unsigned int index) const = 0;
        /**
         * Get the length of the property with the specified id in bytes.
         *
         * Returns 0 if the property does not exist.
         */
        virtual unsigned int getPropertyLength(uint16_t id) const = 0;
        /**
         * Returns true if the property with the specified id is read-only.
         * Otherwise returns false.
         */
        virtual bool getPropertyReadOnly(uint16_t id) const = 0;

        // TODO get config length
        // virtual unsigned int getConfigValueLength(uint8_t type) const = 0;
    };
}
#endif
