#include "controller-manager.h"

namespace IrrigationSystem
{
    ControllerManager::ControllerManager(ControllerRegistration *controllers, unsigned int num) : controllers(controllers),
                                                                                                  numControllers(num)
    {
    }

    IrrigationSystem::Controller *ControllerManager::getController(uint8_t controllerId)
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            if (controllers[i].controllerId == controllerId)
            {
                return controllers[i].controller;
            }
        }
        return nullptr;
    }

    const IrrigationSystem::ControllerDefinition *ControllerManager::getControllerDefinition(uint8_t controllerId) const
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            if (controllers[i].controllerId == controllerId)
            {
                return &controllers[i].controller->getDefinition();
            }
        }
        return nullptr;
    }

    void ControllerManager::resetControllers()
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            controllers[i].controller->reset();
        }
    }

    void ControllerManager::beginControllers()
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            controllers[i].controller->begin();
        }
    }
}
