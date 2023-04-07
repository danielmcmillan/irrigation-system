#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include "error-handler.h"

#define MESSAGE_QUEUE_SIZE 256

enum class IncomingMessageType
{
    ResetController,
    SetConfig,
    GetConfig,
    GetProperties,
    SetProperty,
    Command,
    Invalid
};
typedef std::function<void(IncomingMessageType type, const uint8_t *payload, int length)> MqttClientMessageHandler;

class MqttClient
{
public:
    MqttClient(const char *endpoint, int port, const char *clientId, const char *caCertificate, const char *certificate, const char *privateKey, MqttClientMessageHandler handler, const ErrorHandler &errorHandler);
    /** Maintain MQTT connection. Returns whether connection is currently active. */
    bool loop();

    bool publish(const char *topic, const uint8_t *payload, int length, bool ignoreError = false) const;

private:
    MqttClientMessageHandler handler;
    const ErrorHandler &errorHandler;
    WiFiClientSecure wifiClient;
    mutable MQTTClient mqttClient;
    const char *endpoint;
    int port;
    const char *clientId;
    bool subscribed;

    uint8_t messageQueue[MESSAGE_QUEUE_SIZE];
    size_t messageQueueEnd;
    bool messageQueueFull;
    void queueMessage(MQTTClient *client, char topic[], char bytes[], int length);
};

#endif
