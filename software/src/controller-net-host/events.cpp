#include "events.h"
#include "settings.h"
#include "binary-util.h"
#include "logging.h"

#define MAX_EVENT_BATCH_SIZE 256
#define MAX_EVENT_SIZE 14
#define EVENT_TOPIC "icu-out/" MQTT_CLIENT_ID "/event"
#define POLL_INTERVAL 2500

Events::Events(const ControlI2cMaster &control, const MqttClient &mqtt) : control(control), mqtt(mqtt), lastEvent(0xffff), lastPollTime(0)
{
}

void Events::reset()
{
    lastEvent = 0xffff;
}

int Events::loop()
{
    unsigned long now = millis();
    if ((now - lastPollTime) < POLL_INTERVAL)
    {
        return 0;
    }
    // Get all pending available events up to a maximum size
    uint8_t events[MAX_EVENT_BATCH_SIZE];
    size_t eventSize;
    int result;
    size_t eventsSize = 0;
    uint16_t nextLastEvent = lastEvent;
    while (eventsSize + MAX_EVENT_SIZE < MAX_EVENT_BATCH_SIZE)
    {
        result = control.getNextEvent(nextLastEvent, &events[eventsSize], &eventSize);
        if (result)
        {
            nextLastEvent = read16LE(&events[eventsSize]);
            eventsSize += eventSize;
        }
        else
        {
            // No more events
            break;
        }
    }
    if (eventsSize > 0)
    {
        LOG_INFO("Publishing events");
        if (mqtt.publish(EVENT_TOPIC, events, eventsSize))
        {
            lastEvent = nextLastEvent;
        }
        else
        {
            // TODO handle error
        }
    }
    lastPollTime = now;

    // TODO do something when first event was not found (like trigger retrieving all properties)?
    return result;
}
