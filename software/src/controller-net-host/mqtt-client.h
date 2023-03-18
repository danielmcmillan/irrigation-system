#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include <WiFiClientSecure.h>
#include <MQTTClient.h>

class MqttClient
{
public:
    MqttClient(const char *endpoint, int port, const char *clientId, const char *caCertificate, const char *certificate, const char *privateKey);
    /** Maintain MQTT connection. Returns whether connection is currently active. */
    bool loop();

    bool publish(const char *topic, const uint8_t *payload, int length) const;

private:
    WiFiClientSecure wifiClient;
    mutable MQTTClient mqttClient;
    const char *endpoint;
    int port;
    const char *clientId;
};

#endif
