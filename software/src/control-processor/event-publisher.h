#ifndef _CONTROL_PROCESSOR_EVENT_PUBLISHER
#define _CONTROL_PROCESSOR_EVENT_PUBLISHER
#include "event-buffer.h"
#include "mqtt-client.h"
#include "error-handler.h"
#include "controllers.h"

#define MAX_EVENT_BATCH_SIZE 256

typedef std::function<bool(const uint8_t *data, size_t size)> PublishEventData;

class EventPublisher
{
public:
    EventPublisher(EventBuffer &events, PublishEventData publishEventData, const ErrorHandler &errorHandler);
    void reset();
    bool loop();

private:
    EventBuffer &events;
    PublishEventData publishEventData;
    const ErrorHandler &errorHandler;
    uint8_t publishBuffer[MAX_EVENT_BATCH_SIZE];
    size_t batchSize = 0;
    unsigned long lastPollTime;
};

#endif
