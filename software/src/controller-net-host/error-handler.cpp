#include "error-handler.h"
#include "binary-util.h"

#define MAX_ERROR_SIZE 128

const char *getErrorComponentString(ErrorComponent component)
{
    switch (component)
    {
    case ErrorComponent::Wifi:
        return "Wifi";
    case ErrorComponent::Mqtt:
        return "Mqtt";
    case ErrorComponent::Config:
        return "Config";
    case ErrorComponent::ControlI2c:
        return "ControlI2c";
    default:
        return "";
    }
}

ErrorHandler::ErrorHandler(PublishErrorData publishErrorData) : publishErrorData(publishErrorData)
{
}

void ErrorHandler::handleError(ErrorComponent component, uint16_t code, const char *text) const
{
    uint8_t buffer[MAX_ERROR_SIZE];
    // Write to Serial
    char *logBuffer = (char *)buffer;
    snprintf(logBuffer, MAX_ERROR_SIZE, "ERROR [%s] %s (0x%02x)\n", getErrorComponentString(component), text, code);
    Serial.printf(logBuffer);

    // Publish error data
    buffer[0] = (uint8_t)component;
    IrrigationSystem::write16LE(&buffer[1], code);
    size_t msgLength = 3;
    if (text != nullptr)
    {
        msgLength = (uint8_t *)stpncpy((char *)&buffer[msgLength], text, MAX_ERROR_SIZE - msgLength) - buffer;
    }

    publishErrorData(buffer, msgLength);
}
