#include "config.h"
#include <Arduino.h>
#include "logging.h"
#include <EEPROM.h>
#include "crc16.h"

Config::Config(Controllers &control, const ErrorHandler &errorHandler)
    : control(control), errorHandler(errorHandler), pendingRead(true), pendingWrite(false), pendingApply(false), configData{0}, configLength(0), lastAttempt(0)
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

size_t Config::getConfig(uint8_t *dataOut) const
{
    memcpy(dataOut, configData, configLength);
    return configLength;
}

bool Config::loop()
{
    if (control.getStatus() == ControllersStatus::Unconfigured)
    {
        pendingApply = true;
    }
    unsigned long now = millis();
    if ((now - lastAttempt) < CONFIG_MIN_ATTEMPT_DELAY)
    {
        return true;
    }
    if (pendingRead)
    {
        return readConfig();
    }
    else if (pendingApply)
    {
        lastAttempt = now;
        return applyConfig();
    }
    else if (pendingWrite)
    {
        lastAttempt = now;
        return writeConfig();
    }
    return true;
}

bool Config::readConfig()
{
    pendingRead = false;
    EEPROM.begin(CONFIG_MAX_SIZE + 7);
    if (EEPROM.read(0) != 0x2a)
    {
        // No config stored
        errorHandler.handleError(ErrorComponent::Config, 5, "No config");
        EEPROM.end();
        return true;
    }
    size_t length = EEPROM.readUInt(1);
    if (length > CONFIG_MAX_SIZE)
    {
        errorHandler.handleError(ErrorComponent::Config, 6, "Read bad config length");
        EEPROM.end();
        return false;
    }
    configLength = 0;
    for (int i = 0; i < length + 2; ++i)
    {
        configData[i] = EEPROM.read(5 + i);
    }
    if (CRC::crc16(configData, length + 2))
    {
        errorHandler.handleError(ErrorComponent::Config, 7, "Config had non-zero CRC");
        EEPROM.end();
        return false;
    }
    // Successfully read new config which should be applied
    LOG_INFO("[Config] Retrieved");
    configLength = length;
    pendingWrite = false;
    pendingApply = true;
    EEPROM.end();
    return true;
}

bool Config::writeConfig()
{
    EEPROM.begin(configLength + 7);
    EEPROM.write(0, 0x2a); // arbitrary value indicating config exists
    EEPROM.writeUInt(1, configLength);
    for (int i = 0; i < configLength; ++i)
    {
        EEPROM.write(5 + i, configData[i]);
    }
    EEPROM.writeUShort(5 + configLength, CRC::crc16(configData, configLength));
    bool success = EEPROM.commit();
    EEPROM.end();
    if (success)
    {
        LOG_INFO("[Config] Persisted");
        pendingWrite = false;
        return true;
    }
    else
    {
        errorHandler.handleError(ErrorComponent::Config, 4, "Persist failed");
        return false;
    }
}

bool Config::applyConfig()
{
    LOG_INFO("[Config] Applying");
    // Reset controllers
    if (!control.configStart())
    {
        return false;
    }
    // Apply each value from configData to controllers
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
        // TODO validate in configAdd function?
        // const IrrigationSystem::ControllerDefinition *definition = control.getDefinitions().getControllerDefinition(controllerId);
        // if (definition->getConfigLength(configType) != nextLength - 3)
        // {
        //     errorHandler.handleError(ErrorComponent::Config, 3, "Config data length doesn't match config type");
        //     return false;
        // }
        if (!control.configAdd(&configData[i + 1], nextLength - 1))
        {
            return false;
        }
        i += nextLength;
    }
    // Tell controllers to start with the provided configuration
    if (!control.configEnd())
    {
        return false;
    }
    LOG_INFO("[Config] Applied");
    pendingApply = false;
    return true;
}
