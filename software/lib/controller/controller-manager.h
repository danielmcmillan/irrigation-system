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
        /**
         * Begin all registered controllers.
         *
         * Returns whether all controllers were successfully initialised.
         */
        bool beginControllers();
        /**
         * Reset all registered controllers.
         */
        void resetControllers();

    private:
        ControllerRegistration *controllers;
        unsigned int numControllers;
    };
}

#endif
