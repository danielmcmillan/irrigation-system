#ifndef _CONTROL_NET_HOST_EVENTS
#define _CONTROL_NET_HOST_EVENTS
#include "control-i2c-master.h"
#include "mqtt-client.h"
#include "error-handler.h"

typedef std::function<bool(const uint8_t *data, size_t size)> PublishEventData;

class Events
{
public:
    Events(const ControlI2cMaster &control, PublishEventData publishEventData, const ErrorHandler &errorHandler);
    void reset();
    bool loop();

private:
    const ControlI2cMaster &control;
    PublishEventData publishEventData;
    const ErrorHandler &errorHandler;
    uint16_t lastEvent;
    unsigned long lastPollTime;
};

#endif
