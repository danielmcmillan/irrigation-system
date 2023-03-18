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

IrrigationSystem::ControllerDefinitionsBuilder definitionsBuilder;
IrrigationSystem::ControllerDefinitionManager definitions = definitionsBuilder.buildManager();

WiFiManager wifi(WIFI_SSID, WIFI_PASSWORD);
ControlI2cMaster control(definitions);
MqttClient mqtt(MQTT_BROKER_ENDPOINT, MQTT_BROKER_PORT, MQTT_CLIENT_ID, MQTT_BROKER_CA_CERT, MQTT_CLIENT_CERT, MQTT_CLIENT_KEY);
Events events(control, mqtt);

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

    if (connected)
    {
        events.loop();
        // TODO handle result
    }
}
