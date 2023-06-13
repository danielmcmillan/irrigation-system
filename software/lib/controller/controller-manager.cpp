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

    unsigned int ControllerManager::getControllerCount() const
    {
        return numControllers;
    }

    uint8_t ControllerManager::getControllerIdAt(uint8_t index) const
    {
        if (index < numControllers)
        {
            return controllers[index].controllerId;
        }
        return 0;
    }

    void ControllerManager::setEventHandler(EventHandler &handler)
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            controllers[i].controller->setEventHandler(handler);
        }
    }

    bool ControllerManager::beginControllers()
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            if (!controllers[i].controller->begin())
            {
                return false;
            }
        }
        return true;
    }

    void ControllerManager::resetControllers()
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            controllers[i].controller->reset();
        }
    }

    void ControllerManager::updateControllers()
    {
        for (unsigned int i = 0; i < numControllers; ++i)
        {
            controllers[i].controller->update();
        }
    }
}
