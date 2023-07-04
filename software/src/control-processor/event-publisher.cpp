#include "event-publisher.h"
#include "settings.h"
#include "binary-util.h"
#include "logging.h"

#define POLL_INTERVAL 2500

EventPublisher::EventPublisher(EventBuffer &events, PublishEventData publishEventData, const ErrorHandler &errorHandler)
    : events(events), publishEventData(publishEventData), lastPollTime(0), errorHandler(errorHandler)
{
}

void EventPublisher::reset()
{
}

bool EventPublisher::loop()
{
    unsigned long now = millis();
    if ((now - lastPollTime) < POLL_INTERVAL)
    {
        return true;
    }
    lastPollTime = now;
    // Get all pending available events up to a maximum size
    while (true)
    {
        uint8_t size = events.peek();
        if (size == 0 || (batchSize + size) > MAX_EVENT_BATCH_SIZE)
        {
            break;
        }
        batchSize += events.pop(&publishBuffer[batchSize]);
    }
    if (batchSize > 0)
    {
        LOG_INFO("Publishing events");
        if (publishEventData(publishBuffer, batchSize))
        {
            // Clear publish buffer
            batchSize = 0;
        }
        else
        {
            return false;
        }
    }
    if (events.checkBufferFullFault(true))
    {
        errorHandler.handleError(ErrorComponent::Mqtt, 1, "Event buffer full");
    }
    return true;
}
