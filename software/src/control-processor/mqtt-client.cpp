#include "mqtt-client.h"
#include "logging.h"
#include "settings.h"

#define INCOMING_TOPIC_PREFIX "icu-in/" MQTT_CLIENT_ID "/"
#define INCOMING_TOPIC_FILTER INCOMING_TOPIC_PREFIX "#"

MqttClient::MqttClient(const char *endpoint, int port, const char *clientId, const char *caCertificate, const char *certificate, const char *privateKey, MqttClientMessageHandler handler, const ErrorHandler &errorHandler)
    : handler(handler), errorHandler(errorHandler), wifiClient(), mqttClient(512), endpoint(endpoint), port(port), clientId(clientId), subscribed(false), messageQueue{0}, messageQueueEnd(0), messageQueueFull(false)
{
    // Configure WiFiClientSecure to use the configured credentials
    this->wifiClient.setCACert(caCertificate);
    this->wifiClient.setCertificate(certificate);
    this->wifiClient.setPrivateKey(privateKey);

    mqttClient.begin(endpoint, port, this->wifiClient);
    mqttClient.setOptions(
        10,    // keepAlive
        false, // cleanSession, use persistent sessions
        10000  // timeout
    );
    mqttClient.onMessageAdvanced([this](MQTTClient *client, char topic[], char bytes[], int length)
                                 { this->queueMessage(client, topic, bytes, length); });
}

bool MqttClient::loop()
{
    bool connected = mqttClient.loop();

    if (!connected)
    {
        LOG_INFO("[MQTT] Connecting");
        for (int i = 0; i < 10; ++i)
        {
            if (mqttClient.connect(clientId))
            {
                connected = true;
                break;
            }
            delay(500);
        }
        if (connected)
        {
            if (mqttClient.sessionPresent())
            {
                LOG_INFO("[MQTT] Connected with existing session");
            }
            else
            {
                LOG_INFO("[MQTT] Connected with new session");
                subscribed = false;
            }
        }
        else
        {
            errorHandler.handleError(ErrorComponent::Mqtt, 1 | ((uint8_t)(-mqttClient.lastError()) << 8), "Connection failed");
        }
    }
    if (connected && !subscribed)
    {
        LOG_INFO("[MQTT] Subscribing");
        for (int i = 0; i < 10; ++i)
        {
            if (mqttClient.subscribe(INCOMING_TOPIC_FILTER, 1))
            {
                LOG_INFO("[MQTT] Subscribed successfully");
                subscribed = true;
                break;
            }
            delay(500);
        }
        if (!subscribed)
        {
            errorHandler.handleError(ErrorComponent::Mqtt, 2 | ((uint8_t)(-mqttClient.lastError()) << 8), "Subscribe failed");
        }
    }

    // Call handler for any pending messages in queue
    if (messageQueueEnd > 0)
    {
        for (int i = 0; i < messageQueueEnd;)
        {
            IncomingMessageType type = (IncomingMessageType)messageQueue[i];
            uint8_t length = messageQueue[i + 1];
            if (type == IncomingMessageType::Invalid)
            {
                errorHandler.handleError(ErrorComponent::Mqtt, 3, "Unexpected message topic");
            }
            else
            {
                handler(type, &messageQueue[i + 2], length);
            }
            i += length + 2;
        }
        messageQueueEnd = 0;
    }
    if (messageQueueFull)
    {
        errorHandler.handleError(ErrorComponent::Mqtt, 4, "Incoming message queue full");
        messageQueueFull = false;
    }

    return connected && subscribed;
}

bool MqttClient::publish(const char *topic, const uint8_t *payload, int length, bool ignoreError) const
{
    if (this->mqttClient.publish(topic, (char *)payload, length, false, 1))
    {
        return true;
    }
    else
    {
        if (!ignoreError)
        {
            errorHandler.handleError(ErrorComponent::Mqtt, 5 | ((uint8_t)(-mqttClient.lastError()) << 8), "Publish failed");
        }
        return false;
    }
}

void MqttClient::queueMessage(MQTTClient *client, char topic[], char bytes[], int length)
{
    IncomingMessageType type;
    size_t prefixLength = sizeof(INCOMING_TOPIC_PREFIX) - sizeof(INCOMING_TOPIC_PREFIX[0]);
    if (strncmp(INCOMING_TOPIC_PREFIX, topic, prefixLength) != 0)
    {
        type = IncomingMessageType::Invalid;
    }
    else
    {
        char *segment = topic + prefixLength;
        if (strcmp(segment, "setConfig") == 0)
        {
            type = IncomingMessageType::SetConfig;
        }
        else if (strcmp(segment, "getConfig") == 0)
        {
            type = IncomingMessageType::GetConfig;
        }
        else if (strcmp(segment, "getProperties") == 0)
        {
            type = IncomingMessageType::GetProperties;
        }
        else if (strcmp(segment, "setProperty") == 0)
        {
            type = IncomingMessageType::SetProperty;
        }
        else if (strcmp(segment, "command") == 0)
        {
            type = IncomingMessageType::Command;
        }
        else if (strcmp(segment, "update") == 0)
        {
            type = IncomingMessageType::Update;
        }
        else
        {
            type = IncomingMessageType::Invalid;
        }
    }

    // Copies the message into a queue, since handling it may require further mqtt interaction which is not supported
    if (MESSAGE_QUEUE_SIZE - messageQueueEnd >= length + 2)
    {
        messageQueue[messageQueueEnd] = (uint8_t)type;
        messageQueue[messageQueueEnd + 1] = length;
        messageQueueEnd += 2;
        memcpy(&messageQueue[messageQueueEnd], bytes, length);
        messageQueueEnd += length;
    }
    else
    {
        messageQueueFull = true;
    }
}
