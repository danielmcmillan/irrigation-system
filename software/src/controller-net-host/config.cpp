#include "config.h"
#include <Arduino.h>
#include "logging.h"

Config::Config(const ControlI2cMaster &control, IrrigationSystem::ControllerDefinitionManager &definitions, const ErrorHandler &errorHandler)
    : control(control), definitions(definitions), pendingRead(true), pendingWrite(false), pendingApply(false), configData{0}, configLength(0), errorHandler(errorHandler)
{
}

bool Config::setConfig(const uint8_t *data, size_t length)
{
    if (length > CONFIG_MAX_SIZE)
    {
        errorHandler.handleError(ErrorComponent::Config, 1, "Config is too big");
        return false;
    }
    configLength = length;
    memcpy(configData, data, configLength);
    pendingRead = false;
    pendingWrite = true;
    pendingApply = true;
    return true;
}

bool Config::loop()
{
    if (pendingRead)
    {
        return readConfig();
    }
    if (pendingApply)
    {
        return applyConfig();
    }
    if (pendingWrite)
    {
        return writeConfig();
    }
    return true;
}

bool Config::readConfig()
{
    pendingRead = false;
    pendingWrite = false;
    // pendingApply = true;
    return true;
}

bool Config::writeConfig()
{
    pendingRead = false;
    pendingWrite = false;
    return true;
}

bool Config::applyConfig()
{
    LOG_INFO("[Config] Applying configuration");
    // Reset control processor and local definitions
    if (!control.configStart())
    {
        return false;
    }
    definitions.resetControllerDefinitions();
    // Apply each value from configData to control processor and local definitions
    for (int i = 0; i < configLength;)
    {
        uint8_t nextLength = configData[i];
        if (i + nextLength > configLength || nextLength < 3)
        {
            errorHandler.handleError(ErrorComponent::Config, 2, "Invalid length of config packet");
            return false;
        }
        uint8_t controllerId = configData[i + 1];
        uint8_t configType = configData[i + 2];
        IrrigationSystem::ControllerDefinition *definition = definitions.getControllerDefinition(controllerId);
        if (definition->getConfigLength(configType) != nextLength - 3)
        {
            errorHandler.handleError(ErrorComponent::Config, 2, "Config data length doesn't match config type");
            return false;
        }
        if (!control.configAdd(&configData[i + 1], nextLength - 1))
        {
            return false;
        }
        definition->configure(configType, &configData[i + 3]);
        i += nextLength;
    }
    // Tell control processor to start with the provided configuration
    if (!control.configEnd())
    {
        return false;
    }
    LOG_INFO("[Config] Configured successfully");
    pendingApply = false;
    return 0;
}
