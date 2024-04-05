#ifndef CONTROLLERS_H
#define CONTROLLERS_H

#include "controller-manager.h"
#include "vacon-100-controller.h"
#include "fertigation-controller.h"
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
        FertigationController fertigationController;
        RemoteUnitController remoteUnitController;
        ControllerRegistration registeredControllers[3];
    };
}

#endif
