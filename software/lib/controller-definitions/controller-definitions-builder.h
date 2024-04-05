#ifndef CONTROLLER_DEFINITIONS_H
#define CONTROLLER_DEFINITIONS_H

#include "controller-definition-manager.h"
#include "vacon-100-controller-definition.h"
#include "fertigation-controller-definition.h"
#include "remote-unit-controller-definition.h"

namespace IrrigationSystem
{
    /**
     * Build a ControllerDefinitionManager with a set of controller definitions.
     */
    class ControllerDefinitionsBuilder
    {
    public:
        ControllerDefinitionsBuilder();
        ControllerDefinitionManager buildManager();

    private:
        Vacon100ControllerDefinition vacon100Definition;
        FertigationControllerDefinition fertigationDefinition;
        RemoteUnitControllerDefinition remoteUnitDefinition;
        IrrigationSystem::ControllerDefinitionRegistration registeredDefinitions[3];
    };
}

#endif
