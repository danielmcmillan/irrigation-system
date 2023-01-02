#ifndef _CONTROLLER_MANAGER_H
#define _CONTROLLER_MANAGER_H
#include "controller.h"
#include "controller-definition-provider.h"

namespace IrrigationSystem
{
    struct ControllerRegistration
    {
        const uint8_t controllerId;
        IrrigationSystem::Controller *controller;
    };

    class ControllerManager : public ControllerDefinitionProvider
    {
    public:
        ControllerManager(ControllerRegistration *controllers, unsigned int num);
        Controller *getController(uint8_t controllerId);
        const ControllerDefinition *getControllerDefinition(uint8_t controllerId) const override;
        void setEventHandler(EventHandler &handler);
        void resetControllers();
        void beginControllers();

    private:
        ControllerRegistration *controllers;
        unsigned int numControllers;
    };
}

#endif
