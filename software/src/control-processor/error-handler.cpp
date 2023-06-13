#include "error-handler.h"
#include "binary-util.h"

#define MAX_ERROR_SIZE 128
// Only re-publish the same error after 2 minutes
#define PUBLISH_REPEAT_INTERVAL 120000

const char *getErrorComponentString(ErrorComponent component)
{
    switch (component)
    {
    case ErrorComponent::Wifi:
        return "WiFi";
    case ErrorComponent::Mqtt:
        return "MQTT";
    case ErrorComponent::Config:
        return "Config";
    case ErrorComponent::Controller:
        return "Controller";
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
    snprintf(logBuffer, MAX_ERROR_SIZE, "ERROR [%s] %s (0x%04x)\n", getErrorComponentString(component), text, code);
    Serial.printf(logBuffer);

    if (shouldPublish(component, code))
    {
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
}

bool ErrorHandler::shouldPublish(ErrorComponent component, uint16_t code) const
{
    unsigned long now = millis();
    if (lastPublishedError.time == 0 ||
        lastPublishedError.component != component ||
        lastPublishedError.code != code ||
        (now - lastPublishedError.time) > PUBLISH_REPEAT_INTERVAL)
    {
        lastPublishedError.time = now;
        lastPublishedError.component = component;
        lastPublishedError.code = code;
        return true;
    }
    else
    {
        return false;
    }
}
