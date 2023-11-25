#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H
#include "error-handler.h"

class WiFiManager
{
public:
    WiFiManager(const char *ssid, const char *password, const ErrorHandler &errorHandler);
    void setup();
    /**
     * Maintain WiFi connection. Returns whether connnection is currently active.
     */
    bool loop();

private:
    const ErrorHandler &errorHandler;
    const char *ssid;
    const char *password;
};

#endif
