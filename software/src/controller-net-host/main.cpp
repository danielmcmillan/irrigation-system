#include <Arduino.h>
#include "crc16.h"
#include <Wire.h>
#include "logging.h"
#include "controller-definitions-builder.h"
#include "http.h"
#include "wifi-manager.h"
#include "settings.h"

IrrigationSystem::ControllerDefinitionsBuilder definitionsBuilder;
IrrigationSystem::ControllerDefinitionManager definitions = definitionsBuilder.buildManager();

WiFiManager wifi(WIFI_SSID, WIFI_PASSWORD);

void setup()
{
    Serial.begin(9600);
    wifi.begin();

    setupHttp(definitions);
}

void loop()
{
    bool wifiConnected = wifi.check();
}
