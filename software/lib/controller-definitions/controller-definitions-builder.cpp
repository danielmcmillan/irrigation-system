#include "controller-definitions-builder.h"

namespace IrrigationSystem
{
    ControllerDefinitionsBuilder::ControllerDefinitionsBuilder() : vacon100Definition(),
                                                                   remoteUnitDefinition(),
                                                                   registeredDefinitions({{0x02, &this->vacon100Definition}, {0x04, &this->remoteUnitDefinition}})
    {
    }

    ControllerDefinitionManager ControllerDefinitionsBuilder::buildManager()
    {
        return ControllerDefinitionManager(&this->registeredDefinitions[0], sizeof(this->registeredDefinitions) / sizeof(this->registeredDefinitions[0]));
    }
}
