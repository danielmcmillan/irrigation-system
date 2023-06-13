#include "event-sender.h"
#include "settings.h"
#include "binary-util.h"
#include "logging.h"

#define MAX_EVENT_BATCH_SIZE 256
#define MAX_EVENT_SIZE 14
#define POLL_INTERVAL 2500

EventSender::EventSender(const EventHistory &events, PublishEventData publishEventData, const ErrorHandler &errorHandler)
    : events(events), publishEventData(publishEventData), lastEvent(0xffff), lastPollTime(0), errorHandler(errorHandler)
{
}

void EventSender::reset()
{
    lastEvent = 0xffff;
}

bool EventSender::loop()
{
    unsigned long now = millis();
    if ((now - lastPollTime) < POLL_INTERVAL)
    {
        return true;
    }
    lastPollTime = now;
    // Get all pending available events up to a maximum size
    uint8_t pendingEvents[MAX_EVENT_BATCH_SIZE];
    // size_t eventSize;
    size_t eventsSize = 0;
    uint16_t nextLastEvent = lastEvent;
    while (eventsSize + MAX_EVENT_SIZE < MAX_EVENT_BATCH_SIZE)
    {
        bool idMatched = false;
        EventHistoryRecord event;
        if (events.getNextEvent(nextLastEvent, &idMatched, &event))
        {
            write16LE(&pendingEvents[eventsSize], event.id);
            pendingEvents[eventsSize + 2] = event.type;
            pendingEvents[eventsSize + 3] = event.payloadSize;
            // Write payload data
            for (int i = 0; i < event.payloadSize; ++i)
            {
                pendingEvents[eventsSize + EVENT_HEADER_SIZE + i] = event.payload[i];
            }

            nextLastEvent = event.id;
            eventsSize += EVENT_HEADER_SIZE + event.payloadSize;
        }
        // TODO send warning if idMatched is false?
        // if (control.getNextEvent(nextLastEvent, &pendingEvents[eventsSize], &eventSize))
        // {
        //     nextLastEvent = read16LE(&pendingEvents[eventsSize]);
        //     eventsSize += eventSize;
        // }
        else
        {
            break;
        }
    }
    if (eventsSize > 0)
    {
        LOG_INFO("Publishing events");
        if (publishEventData(pendingEvents, eventsSize))
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
