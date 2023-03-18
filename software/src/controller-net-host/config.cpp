#include "config.h"
#include <Arduino.h>
#include "logging.h"

Config::Config(const ControlI2cMaster &control)
    : control(control), pendingRead(true), pendingWrite(false), pendingApply(false), configData{0}, configLength(0)
{
}

bool Config::setConfig(const uint8_t *data, size_t length)
{
    if (length > CONFIG_MAX_SIZE)
    {
        LOG_ERROR("[Config] Config is too big");
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
        return readConfig() == 0;
    }
    if (pendingApply)
    {
        return applyConfig() == 0;
    }
    if (pendingWrite)
    {
        return writeConfig() == 0;
    }
    return true;
}

int Config::readConfig()
{
    pendingRead = false;
    pendingWrite = false;
    // pendingApply = true;
    return 0;
}

int Config::writeConfig()
{
    pendingRead = false;
    pendingWrite = false;
    return 0;
}

int Config::applyConfig()
{
    if (!control.configStart())
    {
        return 1;
    }
    LOG_INFO("[Config] Started configuration");
    for (int i = 0; i < configLength;)
    {
        uint8_t nextLength = configData[i];
        if (i + nextLength > configLength)
        {
            LOG_ERROR("[Config] Malformed config packet");
            return 2;
        }
        if (!control.configAdd(&configData[i + 1], nextLength - 1))
        {
            return 3;
        }
        i += nextLength;
    }
    if (!control.configEnd())
    {
        return 4;
    }
    LOG_INFO("[Config] Configured successfully");
    pendingApply = false;
    return 0;
}
