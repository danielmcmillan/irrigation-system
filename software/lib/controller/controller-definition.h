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
        // TODO iterator over property ids
        virtual unsigned int getPropertyLength(uint16_t id) const = 0;
        virtual bool getPropertyReadOnly(uint16_t id) const = 0;
    };
}
#endif
