#include <Arduino.h>
#include "crc16.h"
#include <Wire.h>
#include "logging.h"
#include "controller-definitions-builder.h"
#include "http.h"
#include "wifi-manager.h"
#include "settings.h"
#include "mqtt-client.h"

IrrigationSystem::ControllerDefinitionsBuilder definitionsBuilder;
IrrigationSystem::ControllerDefinitionManager definitions = definitionsBuilder.buildManager();

WiFiManager wifi(WIFI_SSID, WIFI_PASSWORD);
MqttClient mqtt(MQTT_BROKER_ENDPOINT, MQTT_BROKER_PORT, MQTT_CLIENT_ID, MQTT_BROKER_CA_CERT, MQTT_CLIENT_CERT, MQTT_CLIENT_KEY);

unsigned long lastPublish = 0;

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

    unsigned long now = millis();
    if (connected && (now - lastPublish) > 900000)
    {
        LOG_INFO("Publishing hello world");
        mqtt.publish("$aws/rules/davis_test", "Hello world!");
        lastPublish = now;
    }
}
