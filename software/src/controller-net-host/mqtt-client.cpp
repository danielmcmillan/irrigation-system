#include "mqtt-client.h"
#include "logging.h"

void messageReceived(String &topic, String &payload)
{
    // Note: Do not use the client in the callback to publish, subscribe or unsubscribe
    Serial.println("incoming: " + topic + " - " + payload);
}

MqttClient::MqttClient(const char *endpoint, int port, const char *clientId, const char *caCertificate, const char *certificate, const char *privateKey)
    : wifiClient(), mqttClient(512), endpoint(endpoint), port(port), clientId(clientId)
{
    // Configure WiFiClientSecure to use the configured credentials
    this->wifiClient.setCACert(caCertificate);
    this->wifiClient.setCertificate(certificate);
    this->wifiClient.setPrivateKey(privateKey);

    mqttClient.begin(endpoint, port, this->wifiClient);
    mqttClient.onMessage(messageReceived);
    // mqttClient.setKeepAlive() TODO
    // mqttClient.setWill("topic", "payload", retained, qos) TODO
}

bool MqttClient::loop()
{
    // TODO Need to manually send ping, or library handles that?
    if (mqttClient.loop())
    {
        return true;
    }

    LOG_INFO("[MQTT] Connecting");
    for (int i = 0; i < 10; ++i)
    {
        if (mqttClient.connect(clientId))
        {
            LOG_INFO("[MQTT] Connected successfully");
            // TODO subscribe topics
            return true;
        }
        delay(500);
    }
    LOG_ERROR("[MQTT] Connection failed");
    return false;
}

bool MqttClient::publish(const char *topic, const uint8_t *payload, int length) const
{
    if (this->mqttClient.publish(topic, (char *)payload, length))
    {
        return true;
    }
    else
    {
        Serial.println("[MQTT] Failed to publish");
        return false;
    }
}
