#include "controller-definition-manager.h"

namespace IrrigationSystem
{
    ControllerDefinitionManager::ControllerDefinitionManager(ControllerDefinitionRegistration *definitions, unsigned int num) : definitions(definitions),
                                                                                                                                numDefinitions(num)
    {
    }

    const ControllerDefinition *ControllerDefinitionManager::getControllerDefinition(uint8_t controllerId) const
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

    ControllerDefinition *ControllerDefinitionManager::getControllerDefinition(uint8_t controllerId)
    {
        return const_cast<ControllerDefinition *>(const_cast<const ControllerDefinitionManager *>(this)->getControllerDefinition(controllerId));
    }

    void ControllerDefinitionManager::resetControllerDefinitions()
    {
        for (unsigned int i = 0; i < numDefinitions; ++i)
        {
            definitions[i].controllerDefinition->reset();
        }
    }
}
