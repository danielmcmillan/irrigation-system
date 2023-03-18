#include "mqtt-client.h"
#include "logging.h"
#include "settings.h"

#define INCOMING_TOPIC "icu-in/" MQTT_CLIENT_ID "/#"

MqttClient::MqttClient(const char *endpoint, int port, const char *clientId, const char *caCertificate, const char *certificate, const char *privateKey, MqttClientMessageHandler handler)
    : wifiClient(), mqttClient(512), endpoint(endpoint), port(port), clientId(clientId), subscribed(false)
{
    // Configure WiFiClientSecure to use the configured credentials
    this->wifiClient.setCACert(caCertificate);
    this->wifiClient.setCertificate(certificate);
    this->wifiClient.setPrivateKey(privateKey);

    mqttClient.begin(endpoint, port, this->wifiClient);
    mqttClient.onMessageAdvanced([handler](MQTTClient *client, char topic[], char bytes[], int length)
                                 { handler(topic, (uint8_t *)bytes, length); });
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
            if (mqttClient.subscribe(INCOMING_TOPIC))
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
