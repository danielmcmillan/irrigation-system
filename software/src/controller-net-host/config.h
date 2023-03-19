#ifndef _CONTROL_NET_HOST_CONFIG
#define _CONTROL_NET_HOST_CONFIG
#include "control-i2c-master.h"
#include "controller-definition-manager.h"
#include "error-handler.h"

#define CONFIG_MAX_SIZE 256

class Config
{
public:
    Config(const ControlI2cMaster &control, IrrigationSystem::ControllerDefinitionManager &definitions, const ErrorHandler &errorHandler);
    bool setConfig(const uint8_t *data, size_t length);
    bool loop();

private:
    const ControlI2cMaster &control;
    IrrigationSystem::ControllerDefinitionManager &definitions;
    const ErrorHandler &errorHandler;
    /** Pending to be read from eeprom */
    bool pendingRead;
    /** Pending to be written to eeprom */
    bool pendingWrite;
    /** Pending to be applied to control processor */
    bool pendingApply;
    uint8_t configData[CONFIG_MAX_SIZE];
    size_t configLength;

    int readConfig();
    int writeConfig();
    int applyConfig();
};

#endif
