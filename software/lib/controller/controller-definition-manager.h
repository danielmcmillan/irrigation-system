#ifndef _CONTROLLER_DEFINITION_MANAGER_H
#define _CONTROLLER_DEFINITION_MANAGER_H
#include "controller-definition.h"

struct ControllerDefinitionRegistration
{
    const uint8_t controllerId;
    IrrigationSystem::ControllerDefinition *controllerDefinition;
};

class ControllerDefinitionManager
{
public:
    ControllerDefinitionManager(ControllerDefinitionRegistration *definitions, unsigned int num);
    IrrigationSystem::ControllerDefinition *getControllerDefinition(uint8_t controllerId);
    void resetControllerDefinitions();

private:
    ControllerDefinitionRegistration *definitions;
    unsigned int numDefinitions;
};
#endif
