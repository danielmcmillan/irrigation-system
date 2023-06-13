#ifndef _CONTROL_PROCESSOR_EVENT_SENDER
#define _CONTROL_PROCESSOR_EVENT_SENDER
#include "events/event-history.h"
#include "mqtt-client.h"
#include "error-handler.h"
#include "controllers.h"

typedef std::function<bool(const uint8_t *data, size_t size)> PublishEventData;

class EventSender
{
public:
    EventSender(const EventHistory &events, PublishEventData publishEventData, const ErrorHandler &errorHandler);
    void reset();
    bool loop();

private:
    const EventHistory &events;
    PublishEventData publishEventData;
    const ErrorHandler &errorHandler;
    uint16_t lastEvent;
    unsigned long lastPollTime;
};

#endif
