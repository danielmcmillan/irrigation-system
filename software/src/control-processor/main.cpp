#include <Arduino.h>
#include "logging.h"
#include "events/event-history.h"
#include "controllers/controller-builder.h"
#include "control-i2c-slave.h"
#include "message-handler.h"
#include "control-processor-state.h"

using namespace IrrigationSystem;

ControllerBuilder controllerBuilder;
ControllerManager controllers = controllerBuilder.buildManager();

ControlProcessorState state;

EventHistory events;

ControlProcessorMessageHandler handler(controllers, state, events);
ControlI2cSlave i2c = ControlI2cSlave::initialise(controllers, handler);

void setup()
{
    Serial.begin(9600, SERIAL_8N1);
    i2c.setup();

    controllers.setEventHandler(events);
    controllers.resetControllers();
    events.handleEvent(EventType::started, 0, nullptr);
}

void loop()
{
    if (state.status == ControlProcessorStatus::Ready)
    {
        controllers.updateControllers();
        handler.runPendingControllerCommand();
    }
    else if (state.status == ControlProcessorStatus::Initializing)
    {
        if (controllers.beginControllers())
        {
            state.status = ControlProcessorStatus::Ready;
            events.handleEvent(EventType::ready, 0, nullptr);
        }
        else
        {
            delay(10000);
        }
    }
    delay(3000);
}
