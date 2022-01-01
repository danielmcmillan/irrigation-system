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
        virtual void configure(uint8_t type, uint8_t *data) = 0;
        // TODO iterator over property ids
        virtual int getPropertyLength(uint16_t id) = 0;
        virtual bool getPropertyReadOnly(uint16_t id) = 0;
    };
}
#endif
