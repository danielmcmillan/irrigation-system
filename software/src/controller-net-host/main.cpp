#include <Arduino.h>
#include "crc16.h"
#include <Wire.h>
#include "logging.h"
#include "controller-definitions-builder.h"
#include "http.h"
#include "wifi-manager.h"
#include "settings.h"
#include "control-i2c-master.h"
#include "mqtt-client.h"
#include "events.h"
#include "config.h"
#include "error-handler.h"

#define ERROR_TOPIC "icu-out/" MQTT_CLIENT_ID "/error"
#define EVENT_TOPIC "icu-out/" MQTT_CLIENT_ID "/event"

IrrigationSystem::ControllerDefinitionsBuilder definitionsBuilder;
IrrigationSystem::ControllerDefinitionManager definitions = definitionsBuilder.buildManager();

bool publishErrorData(const uint8_t *data, size_t size);
bool publishEventData(const uint8_t *data, size_t size);
void handleMessage(IncomingMessageType type, const uint8_t *payload, int length);

ErrorHandler errorHandler(publishErrorData);
WiFiManager wifi(WIFI_SSID, WIFI_PASSWORD, errorHandler);
MqttClient mqtt(MQTT_BROKER_ENDPOINT, MQTT_BROKER_PORT, MQTT_CLIENT_ID, MQTT_BROKER_CA_CERT, MQTT_CLIENT_CERT, MQTT_CLIENT_KEY, handleMessage, errorHandler);
ControlI2cMaster control(definitions, errorHandler);
Events events(control, publishEventData, errorHandler);
Config config(control, definitions, errorHandler);

void setup()
{
    Serial.begin(9600);

    setupHttp(definitions);
}

void loop()
{
    bool connected = false;
    if (wifi.loop())
    {
        connected = mqtt.loop();
    }
    if (!config.loop())
    {
        // TODO do something?
        delay(10000);
    }

    if (connected)
    {
        if (!events.loop())
        {
            // TODO do something?
        }
    }
}

void handleMessage(IncomingMessageType type, const uint8_t *payload, int length)
{
    // Note: Do not use mqttClient here
    switch (type)
    {
    case IncomingMessageType::Config:
        config.setConfig(payload, length);
        break;
    case IncomingMessageType::Retrieve:
        break;
    case IncomingMessageType::Set:
        control.setPropertyValue(payload, length);
        break;
    }
}

bool publishErrorData(const uint8_t *data, size_t size)
{
    return mqtt.publish(ERROR_TOPIC, data, size);
}

bool publishEventData(const uint8_t *data, size_t size)
{
    return mqtt.publish(EVENT_TOPIC, data, size);
}
