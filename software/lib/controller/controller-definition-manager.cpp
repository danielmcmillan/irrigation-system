#include "controller-definition-manager.h"

ControllerDefinitionManager::ControllerDefinitionManager(ControllerDefinitionRegistration *definitions, unsigned int num) : definitions(definitions),
                                                                                                                            numDefinitions(num)
{
}

IrrigationSystem::ControllerDefinition *ControllerDefinitionManager::getControllerDefinition(uint8_t controllerId)
{
    for (unsigned int i = 0; i < numDefinitions; ++i)
    {
        if (definitions[i].controllerId == controllerId)
        {
            return definitions[i].controllerDefinition;
        }
    }
    return nullptr;
}

void ControllerDefinitionManager::resetControllerDefinitions()
{
    for (unsigned int i = 0; i < numDefinitions; ++i)
    {
        definitions[i].controllerDefinition->reset();
    }
}
