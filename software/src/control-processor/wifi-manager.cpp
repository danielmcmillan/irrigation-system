#include "wifi-manager.h"
#include <WiFi.h>
#include "logging.h"

WiFiManager::WiFiManager(const char *ssid, const char *password, const ErrorHandler &errorHandler) : ssid(ssid), password(password), errorHandler(errorHandler)
{
    WiFi.mode(WIFI_STA);
    WiFi.setHostname("esp32-irrigation");
}

bool WiFiManager::loop()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        if (WiFi.localIP() == INADDR_NONE)
        {
            return false;
        }
        return true;
    }

    LOG_INFO("[WiFi] Connecting");
    WiFi.begin(ssid, password);

    for (int i = 0; i < 20; ++i)
    {
        delay(500);
        if (WiFi.status() == WL_CONNECTED)
        {
            LOG_INFO("[WiFi] Connected successfully");
            return true;
        }
        else if (WiFi.status() == WL_CONNECT_FAILED)
        {
            errorHandler.handleError(ErrorComponent::Wifi, 1, "Connection failed");
            return false;
        }
    }
    errorHandler.handleError(ErrorComponent::Wifi, 2, "Connection timed out");
    return false;

    // Serial.printf("[WiFi] Connected to \"%s\". Local IP is %s. DNS IP is %s.\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str(), WiFi.dnsIP().toString().c_str());
    // WiFi.config(WiFi.localIP(), WiFi.gatewayIP(), WiFi.subnetMask(), IPAddress(1, 1, 1, 1));
    // delay(10);
    // Serial.printf("[WiFi] Changed DNS IP to %s.\n", WiFi.dnsIP().toString().c_str());
}
