#ifndef _CONTROL_I2C_ERROR_HANDLER_H
#define _CONTROL_I2C_ERROR_HANDLER_H
#include <Arduino.h>

enum class ErrorComponent : uint8_t
{
    Wifi,
    Mqtt,
    Config,
    ControlI2c
};

typedef std::function<void(const uint8_t *data, size_t size)> PublishErrorData;

class ErrorHandler
{
public:
    ErrorHandler(PublishErrorData publishErrorData);
    void handleError(ErrorComponent component, uint16_t code, const char *text) const;

private:
    PublishErrorData publishErrorData;
};

#endif
