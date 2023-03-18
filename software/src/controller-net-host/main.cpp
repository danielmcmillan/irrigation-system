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

IrrigationSystem::ControllerDefinitionsBuilder definitionsBuilder;
IrrigationSystem::ControllerDefinitionManager definitions = definitionsBuilder.buildManager();

WiFiManager wifi(WIFI_SSID, WIFI_PASSWORD);
ControlI2cMaster control(definitions);
void handleMessage(IncomingMessageType type, const uint8_t *payload, int length);
MqttClient mqtt(MQTT_BROKER_ENDPOINT, MQTT_BROKER_PORT, MQTT_CLIENT_ID, MQTT_BROKER_CA_CERT, MQTT_CLIENT_CERT, MQTT_CLIENT_KEY, handleMessage);
Events events(control, mqtt);
Config config(control, definitions);

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
    Serial.printf("[TEST] Type %d message %s\n", type, payload);
    if (type == IncomingMessageType::Config)
    {
        config.setConfig(payload, length);
    }
}
