#ifndef _CONTROLLER_DEFINITION_PROVIDER_H
#define _CONTROLLER_DEFINITION_PROVIDER_H
#include "controller-definition.h"

namespace IrrigationSystem
{
    class ControllerDefinitionProvider
    {
    public:
        /**
         * Get the controller definition for the controller with the specified id.
         *
         * Returns nullptr if there is no such controller definition.
         */
        virtual const ControllerDefinition *getControllerDefinition(uint8_t controllerId) const = 0;
    };
}

#endif
