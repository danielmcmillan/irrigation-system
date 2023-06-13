#ifndef _CONTROL_NET_HOST_CONFIG
#define _CONTROL_NET_HOST_CONFIG
#include "controller-definition-manager.h"
#include "error-handler.h"
#include "controllers.h"

#define CONFIG_MAX_SIZE 256
#define CONFIG_MIN_ATTEMPT_DELAY 10000

class Config
{
public:
    Config(Controllers &control, const ErrorHandler &errorHandler);
    bool setConfig(const uint8_t *data, size_t length);
    /**
     * Retrieve the current config data.
     * Provided buffer size should be at least CONFIG_MAX_SIZE.
     * Returns the length of the data, or 0 if there is none.
     */
    size_t getConfig(uint8_t *dataOut) const;
    bool loop();

private:
    Controllers &control;
    const ErrorHandler &errorHandler;
    /** Pending to be read from eeprom */
    bool pendingRead;
    /** Pending to be written to eeprom */
    bool pendingWrite;
    /** Pending to be applied to controllers */
    bool pendingApply;
    uint8_t configData[CONFIG_MAX_SIZE + 2]; // Allow for additional CRC data
    size_t configLength;
    unsigned long lastAttempt;

    bool readConfig();
    bool writeConfig();
    bool applyConfig();
};

#endif
