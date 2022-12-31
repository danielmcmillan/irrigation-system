#include "controller-definitions-builder.h"

namespace IrrigationSystem
{
    ControllerDefinitionsBuilder::ControllerDefinitionsBuilder() : vacon100Definition(),
                                                                   registeredDefinitions({{0x02, &this->vacon100Definition}})
    {
    }

    ControllerDefinitionManager ControllerDefinitionsBuilder::buildManager()
    {
        return ControllerDefinitionManager(&this->registeredDefinitions[0], sizeof(this->registeredDefinitions) / sizeof(this->registeredDefinitions[0]));
    }
}
