#ifndef _CONTROL_NET_HOST_EVENTS
#define _CONTROL_NET_HOST_EVENTS
#include "control-i2c-master.h"
#include "mqtt-client.h"
#include "error-handler.h"

class Events
{
public:
    Events(const ControlI2cMaster &control, const MqttClient &mqtt, const ErrorHandler &errorHandler);
    void reset();
    bool loop();

private:
    const ControlI2cMaster &control;
    const MqttClient &mqtt;
    const ErrorHandler &errorHandler;
    uint16_t lastEvent;
    unsigned long lastPollTime;
};

#endif
