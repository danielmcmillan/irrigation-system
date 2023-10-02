#include <Arduino.h>
#include "logging.h"
#include "event-buffer.h"
#include "event-publisher.h"
#include "controllers.h"
#include "crc16.h"
#include "binary-util.h"
#include "controller-definitions-builder.h"
#include "wifi-manager.h"
#include "settings.h"
#include "mqtt-client.h"
#include "config.h"
#include "error-handler.h"
#include "controllers/controller-builder.h"
#include "firmware-update.h"

/**
 * Software revision number.
 */
#define CONTROL_PROCESSOR_REVISION 9

using namespace IrrigationSystem;

#define EVENT_TOPIC "icu-out/all/" MQTT_CLIENT_ID "/event"
#define ERROR_TOPIC "icu-out/all/" MQTT_CLIENT_ID "/error"
#define COMMAND_RESULT_TOPIC "icu-out/all/" MQTT_CLIENT_ID "/commandResult"
#define CONFIG_TOPIC "icu-out/%.*s/" MQTT_CLIENT_ID "/config"
#define PROPERTIES_TOPIC "icu-out/%.*s/" MQTT_CLIENT_ID "/properties"
#define TOPIC_CLIENT_MAX_LENGTH 20
#define DISCONNECTED_RESET_TIME 120000 // 2 minutes

bool publishErrorData(const uint8_t *data, size_t size);
bool publishEventData(const uint8_t *data, size_t size);
void handleMessage(IncomingMessageType type, const uint8_t *payload, int length);
// void sendCommandResults();

ControllerBuilder controllerBuilder;
ControllerManager controllerManager(controllerBuilder.buildManager());
EventBuffer eventBuffer;
Controllers controllers(controllerManager, eventBuffer);
ErrorHandler errorHandler(publishErrorData);
WiFiManager wifi(WIFI_SSID, WIFI_PASSWORD, errorHandler);
MqttClient mqtt(MQTT_BROKER_ENDPOINT, MQTT_BROKER_PORT, MQTT_CLIENT_ID, MQTT_BROKER_CA_CERT, MQTT_CLIENT_CERT, MQTT_CLIENT_KEY, handleMessage, errorHandler);
EventPublisher eventPublisher(eventBuffer, publishEventData, errorHandler);
Config config(controllers, errorHandler);

unsigned long lastConnected = 0;

void setup()
{
    Serial.begin(9600, SERIAL_8N1);
    controllers.setup();
}

void loop()
{
    controllers.loop();

    bool connected = false;
    if (wifi.loop())
    {
        connected = mqtt.loop();
    }
    config.loop();

    unsigned long now = millis();
    if (connected)
    {
        lastConnected = now;

        eventPublisher.loop();
        // sendCommandResults();
    }
    else if ((now - lastConnected) > DISCONNECTED_RESET_TIME)
    {
        // Restart after prolonged period of connection failure
        ESP.restart();
    }
}

// #define COMMAND_RESULT_REQUEST_INTERVAL 2000
// #define COMMAND_RESULT_REQUEST_COUNT 6
// unsigned long lastCommandResultRequest = 0;
// uint8_t commandResultRequestCount = COMMAND_RESULT_REQUEST_COUNT;
// void sendCommandResults()
// {
//     unsigned int now = millis();
//     if (commandResultRequestCount < COMMAND_RESULT_REQUEST_COUNT && (now - lastCommandResultRequest) > COMMAND_RESULT_REQUEST_INTERVAL)
//     {
//         lastCommandResultRequest = now;
//         const uint8_t *response;
//         size_t responseSize;
//         if (control.getControllerCommandResult(&response, &responseSize))
//         {
//             mqtt.publish(COMMAND_RESULT_TOPIC, response, responseSize);
//             commandResultRequestCount = COMMAND_RESULT_REQUEST_COUNT;
//         }
//         else if (commandResultRequestCount == COMMAND_RESULT_REQUEST_COUNT - 1)
//         {
//             mqtt.publish(COMMAND_RESULT_TOPIC, nullptr, 0);
//             ++commandResultRequestCount;
//         }
//     }
// }

// TODO move property state retrieval functions to another file
#define CONTROLLER_STATE_BUFFER_SIZE 512
#define MAX_PROPERTY_NAME_SIZE 96
#define MAX_PROPERTY_UNIT_SIZE 16
#define MAX_PROPERTY_SIZE 256
size_t writePropertyDetail(const ControllerDefinition *definition, uint8_t controllerId, uint16_t propertyId, uint8_t *dest)
{
    uint8_t *ptr = dest;
    bool readOnly = definition->getPropertyReadOnly(propertyId);
    uint8_t valueLength = definition->getPropertyLength(propertyId);

    ptr[0] = controllerId;
    write16LE(&ptr[1], propertyId);
    ptr[3] = readOnly;
    ptr[4] = valueLength;
    ptr += 5;
    // Write the property value, and desired value if not read only
    if (!controllers.getPropertyValue(controllerId, propertyId, ptr))
    {
        return 0;
    }
    ptr += readOnly ? valueLength : (valueLength * 2);
    // Write the object name and property name strings
    ptr[0] = definition->getPropertyObjectName(propertyId, (char *)(ptr + 1), MAX_PROPERTY_NAME_SIZE);
    ptr += 1 + ptr[0];
    ptr[0] = definition->getPropertyName(propertyId, (char *)(ptr + 1), MAX_PROPERTY_NAME_SIZE);
    ptr += 1 + ptr[0];
    // Write the format struct
    PropertyFormat format = definition->getPropertyFormat(propertyId);
    ptr[0] = (uint8_t)format.valueType;
    ++ptr;
    if (format.valueType == PropertyValueType::BooleanFlags)
    {
        ptr[0] = format.options.booleanCount;
        ++ptr;
    }
    else
    {
        ptr[0] = format.options.mul.base;
        ptr[1] = format.options.mul.exponent;
        ptr += 2;
    }
    if (format.unit != nullptr)
    {
        ptr[0] = stpncpy((char *)&ptr[1], format.unit, MAX_PROPERTY_UNIT_SIZE) - (char *)(ptr + 1);
        ptr += 1 + ptr[0];
    }
    else
    {
        ptr[0] = 0;
        ++ptr;
    }
    return ptr - dest;
}
bool sendPropertyState(const uint8_t *subTopic, uint8_t length)
{
    char topic[64];
    sprintf(topic, PROPERTIES_TOPIC, length < TOPIC_CLIENT_MAX_LENGTH ? length : TOPIC_CLIENT_MAX_LENGTH, subTopic);
    uint8_t buffer[CONTROLLER_STATE_BUFFER_SIZE];
    size_t size = 0;

    size_t controllerCount = controllerManager.getControllerCount();
    for (uint8_t controllerIndex = 0; controllerIndex < controllerCount; ++controllerIndex)
    {
        uint8_t controllerId = controllerManager.getControllerIdAt(controllerIndex);
        const ControllerDefinition *definition = controllerManager.getControllerDefinition(controllerId);
        size_t propertyCount = definition->getPropertyCount();
        for (size_t propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex)
        {
            uint16_t propertyId = definition->getPropertyIdAt(propertyIndex);
            size_t result = writePropertyDetail(definition, controllerId, propertyId, &buffer[size]);
            if (result == 0)
            {
                return false;
            }
            size += result;

            // Publish if buffer is full
            if (CONTROLLER_STATE_BUFFER_SIZE - size < MAX_PROPERTY_SIZE)
            {
                if (!mqtt.publish(topic, buffer, size))
                {
                    return false;
                }
                size = 0;
            }
        }
    }
    return size == 0 || mqtt.publish(topic, buffer, size);
}

void handleMessage(IncomingMessageType type, const uint8_t *payload, int length)
{
    switch (type)
    {
    case IncomingMessageType::SetConfig:
        config.setConfig(payload, length);
        break;
    case IncomingMessageType::GetConfig:
    {
        char topic[128];
        sprintf(topic, CONFIG_TOPIC, length < TOPIC_CLIENT_MAX_LENGTH ? length : TOPIC_CLIENT_MAX_LENGTH, payload);
        uint8_t configData[CONFIG_MAX_SIZE];
        size_t configLength = config.getConfig(configData);
        mqtt.publish(topic, configData, configLength);
        break;
    }
    case IncomingMessageType::GetProperties:
        sendPropertyState(payload, length);
        break;
    case IncomingMessageType::SetProperty:
        controllers.setPropertyValue(payload, length);
        break;
    case IncomingMessageType::Command:
    {
        uint8_t commandResponse[128]; // TODO properly define max command payload sizes
        size_t responseSize = 0;
        if (controllers.runControllerCommand(payload, length, commandResponse, &responseSize))
        {
            // Todo error response
            mqtt.publish(COMMAND_RESULT_TOPIC, nullptr, 0);
        }
        // Todo success response (as in sendCommandResults)
        mqtt.publish(COMMAND_RESULT_TOPIC, commandResponse, responseSize);

        break;
    }
    case IncomingMessageType::Update:
    {
        char url[128];
        if (length > 0)
        {
            memcpy(url, payload, length);
            url[length] = 0;
            updateFirmware(url, errorHandler);
        }
        break;
    }
    }
}

bool publishErrorData(const uint8_t *data, size_t size)
{
    return mqtt.publish(ERROR_TOPIC, data, size, true);
}

bool publishEventData(const uint8_t *data, size_t size)
{
    return mqtt.publish(EVENT_TOPIC, data, size);
}
