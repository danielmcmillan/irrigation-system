#include "mqtt-client.h"
#include "logging.h"
#include "settings.h"

#define INCOMING_TOPIC_PREFIX "icu-in/" MQTT_CLIENT_ID "/"
#define INCOMING_TOPIC_FILTER INCOMING_TOPIC_PREFIX "#"

MqttClient::MqttClient(const char *endpoint, int port, const char *clientId, const char *caCertificate, const char *certificate, const char *privateKey, MqttClientMessageHandler handler, const ErrorHandler &errorHandler)
    : wifiClient(), mqttClient(512), endpoint(endpoint), port(port), clientId(clientId), subscribed(false), errorHandler(errorHandler)
{
    // Configure WiFiClientSecure to use the configured credentials
    this->wifiClient.setCACert(caCertificate);
    this->wifiClient.setCertificate(certificate);
    this->wifiClient.setPrivateKey(privateKey);

    mqttClient.begin(endpoint, port, this->wifiClient);
    mqttClient.onMessageAdvanced([handler](MQTTClient *client, char topic[], char bytes[], int length)
                                 {
                                    size_t prefixLength = sizeof(INCOMING_TOPIC_PREFIX) - sizeof(INCOMING_TOPIC_PREFIX[0]);
                                    if (strncmp(INCOMING_TOPIC_PREFIX, topic, prefixLength) != 0)
                                    {
                                        LOG_ERROR("[MQTT] Message topic has unexpected prefix");
                                    }
                                    char *segment = topic + prefixLength;
                                    IncomingMessageType type;
                                    if (strcmp(segment, "config") == 0)
                                    {
                                        type = IncomingMessageType::Config;
                                    }
                                    else if (strcmp(segment, "retrieve") == 0)
                                    {
                                        type = IncomingMessageType::Retrieve;
                                    }
                                    else if (strcmp(segment, "set") == 0)
                                    {
                                        type = IncomingMessageType::Set;
                                    }
                                    else
                                    {
                                        LOG_ERROR("[MQTT] Message topic has unexpected suffix");
                                        return;
                                    }
                                    handler(type, (uint8_t *)bytes, length); });
    // mqttClient.setKeepAlive() TODO
    // mqttClient.setWill("topic", "payload", retained, qos) TODO
}

bool MqttClient::loop()
{
    bool connected = mqttClient.loop();

    if (!connected)
    {
        subscribed = false;

        LOG_INFO("[MQTT] Connecting");
        for (int i = 0; i < 10; ++i)
        {
            if (mqttClient.connect(clientId))
            {
                LOG_INFO("[MQTT] Connected successfully");
                connected = true;
                break;
            }
            delay(500);
        }
        if (!connected)
        {
            LOG_ERROR("[MQTT] Connection failed"); // mqttClient.lastError()
        }
    }
    if (connected && !subscribed)
    {
        LOG_INFO("[MQTT] Subscribing");
        for (int i = 0; i < 10; ++i)
        {
            if (mqttClient.subscribe(INCOMING_TOPIC_FILTER))
            {
                LOG_INFO("[MQTT] Subscribed successfully");
                subscribed = true;
                break;
            }
            delay(500);
        }
        if (!subscribed)
        {
            LOG_ERROR("[MQTT] Subscribe failed"); // mqttClient.lastError()
        }
    }
    return connected && subscribed;
}

bool MqttClient::publish(const char *topic, const uint8_t *payload, int length) const
{
    if (this->mqttClient.publish(topic, (char *)payload, length))
    {
        return true;
    }
    else
    {
        LOG_ERROR("[MQTT] Failed to publish");
        return false;
    }
}
