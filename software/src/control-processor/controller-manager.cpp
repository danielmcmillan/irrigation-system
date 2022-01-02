#include "controller-manager.h"

ControllerManager::ControllerManager(ControllerRegistration *controllers, unsigned int num) : controllers(controllers),
                                                                                              numControllers(num)
{
}

IrrigationSystem::Controller *ControllerManager::getController(uint8_t controllerId)
{
    for (int i = 0; i < numControllers; ++i)
    {
        if (controllers[i].controllerId == controllerId)
        {
            return controllers[i].controller;
        }
    }
    return nullptr;
}
