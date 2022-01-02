#ifndef _CONTROLLER_MANAGER_H
#define _CONTROLLER_MANAGER_H
#include "controller.h"

struct ControllerRegistration
{
    const uint8_t controllerId;
    IrrigationSystem::Controller *controller;
};

class ControllerManager
{
public:
    ControllerManager(ControllerRegistration *controllers, unsigned int num);
    IrrigationSystem::Controller *getController(uint8_t controllerId);

private:
    ControllerRegistration *controllers;
    size_t numControllers;
};
#endif
