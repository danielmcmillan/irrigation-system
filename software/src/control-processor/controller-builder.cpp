#include "controller-builder.h"

namespace IrrigationSystem
{
    ControllerBuilder::ControllerBuilder() : vacon100Controller(),
                                             registeredControllers({{0x02, &this->vacon100Controller}})
    {
    }

    ControllerManager ControllerBuilder::buildManager()
    {
        return ControllerManager(&this->registeredControllers[0], sizeof(this->registeredControllers) / sizeof(this->registeredControllers[0]));
    }
}
