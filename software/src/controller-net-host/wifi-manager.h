#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WiFiManager
{
public:
    WiFiManager(const char *ssid, const char *password);
    /**
     * Maintain WiFi connection. Returns whether connnection is currently active.
     */
    bool loop();

private:
    const char *ssid;
    const char *password;
};

#endif
