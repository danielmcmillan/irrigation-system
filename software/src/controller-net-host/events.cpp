#include "events.h"
#include "settings.h"
#include "binary-util.h"
#include "logging.h"

#define MAX_EVENT_BATCH_SIZE 256
#define MAX_EVENT_SIZE 14
#define POLL_INTERVAL 2500

Events::Events(const ControlI2cMaster &control, PublishEventData publishEventData, const ErrorHandler &errorHandler)
    : control(control), publishEventData(publishEventData), lastEvent(0xffff), lastPollTime(0), errorHandler(errorHandler)
{
}

void Events::reset()
{
    lastEvent = 0xffff;
}

bool Events::loop()
{
    unsigned long now = millis();
    if ((now - lastPollTime) < POLL_INTERVAL)
    {
        return true;
    }
    lastPollTime = now;
    // Get all pending available events up to a maximum size
    uint8_t events[MAX_EVENT_BATCH_SIZE];
    size_t eventSize;
    size_t eventsSize = 0;
    uint16_t nextLastEvent = lastEvent;
    while (eventsSize + MAX_EVENT_SIZE < MAX_EVENT_BATCH_SIZE)
    {
        if (control.getNextEvent(nextLastEvent, &events[eventsSize], &eventSize))
        {
            nextLastEvent = read16LE(&events[eventsSize]);
            eventsSize += eventSize;
        }
        else
        {
            break;
        }
    }
    if (eventsSize > 0)
    {
        LOG_INFO("Publishing events");
        if (publishEventData(events, eventsSize))
        {
            lastEvent = nextLastEvent;
        }
        else
        {
            return false;
        }
    }
    return true;

    // TODO do something when first event was not found (trigger retrieving all properties)?
}
