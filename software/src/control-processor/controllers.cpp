#include "controllers.h"
#include "controllers/vacon-100-controller.h"

using namespace IrrigationSystem;

Vacon100Controller vacon100Controller;

ControllerRegistration registeredControllers[] = {
    {0x02, &vacon100Controller}};

ControllerManager controllers(registeredControllers, sizeof(registeredControllers) / sizeof(registeredControllers[0]));
