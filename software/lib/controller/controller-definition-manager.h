#ifndef _CONTROLLER_DEFINITION_MANAGER_H
#define _CONTROLLER_DEFINITION_MANAGER_H
#include "controller-definition.h"
#include "controller-definition-provider.h"

namespace IrrigationSystem
{
    struct ControllerDefinitionRegistration
    {
        const uint8_t controllerId;
        ControllerDefinition *controllerDefinition;
    };

    class ControllerDefinitionManager : public ControllerDefinitionProvider
    {
    public:
        ControllerDefinitionManager(ControllerDefinitionRegistration *definitions, unsigned int num);
        ControllerDefinition *getControllerDefinition(uint8_t controllerId);
        const ControllerDefinition *getControllerDefinition(uint8_t controllerId) const override;
        void resetControllerDefinitions();

    private:
        ControllerDefinitionRegistration *definitions;
        unsigned int numDefinitions;
    };
}

#endif
