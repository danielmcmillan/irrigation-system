#ifndef _CONTROL_NET_HOST_EVENTS
#define _CONTROL_NET_HOST_EVENTS
#include "control-i2c-master.h"
#include "mqtt-client.h"

class Events
{
public:
    Events(const ControlI2cMaster &control, const MqttClient &mqtt);
    void reset();
    int loop();

private:
    const ControlI2cMaster &control;
    const MqttClient &mqtt;
    uint16_t lastEvent;
    unsigned long lastPollTime;
};

#endif
