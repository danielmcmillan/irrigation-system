#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "controller-manager.h"
#include "vacon-100-controller.h"
#include "remote-unit-controller.h"

namespace IrrigationSystem
{
    class ControllerBuilder
    {
    public:
        ControllerBuilder();
        ControllerManager buildManager();

    private:
        Vacon100Controller vacon100Controller;
        RemoteUnitController remoteUnitController;
        ControllerRegistration registeredControllers[2];
    };
}

#endif
