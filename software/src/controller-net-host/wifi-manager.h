#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

class WiFiManager
{
public:
    WiFiManager(const char *ssid, const char *password);
    void begin();

    /**
     * Checks whether connection is established, and performs reconnection if required.
     */
    bool check();

private:
    const char *ssid;
    const char *password;
};

#endif
