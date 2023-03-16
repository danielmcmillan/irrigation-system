#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include <WiFiClientSecure.h>
#include <MQTTClient.h>

class MqttClient
{
    WiFiClientSecure wifiClient;
    MQTTClient mqttClient;

public:
    MqttClient(const char *endpoint, int port, const char *clientId, const char *caCertificate, const char *certificate, const char *privateKey);
    /** Maintain MQTT connection. Returns whether connection is currently active. */
    bool loop();

    bool publish(const char *topic, const char *body);

private:
    const char *endpoint;
    int port;
    const char *clientId;
};

#endif
