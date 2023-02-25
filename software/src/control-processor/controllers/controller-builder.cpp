#include "controller-builder.h"

namespace IrrigationSystem
{
    ControllerBuilder::ControllerBuilder() : vacon100Controller(0x02),
                                             remoteUnitController(0x04),
                                             registeredControllers({{0x02, &this->vacon100Controller}, {0x04, &this->remoteUnitController}})
    {
    }

    ControllerManager ControllerBuilder::buildManager()
    {
        return ControllerManager(&this->registeredControllers[0], sizeof(this->registeredControllers) / sizeof(this->registeredControllers[0]));
    }
}
