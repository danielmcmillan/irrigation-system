#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "controller-manager.h"
#include "controllers/vacon-100-controller.h"

namespace IrrigationSystem
{
    class ControllerBuilder
    {
    public:
        ControllerBuilder();
        ControllerManager buildManager();

    private:
        Vacon100Controller vacon100Controller;
        ControllerRegistration registeredControllers[1];
    };
}

#endif
