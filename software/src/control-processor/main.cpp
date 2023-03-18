#include <Arduino.h>
#include "logging.h"
#include "events/event-history.h"
#include "controllers/controller-builder.h"
#include "control-i2c-slave.h"
#include "message-handler.h"
#include "state.h"

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
        controllers.getController(0x02)->update();
        controllers.getController(0x04)->update();
    }
    else if (state.status == ControlProcessorStatus::Initializing)
    {
        if (controllers.beginControllers())
        {
            state.status = ControlProcessorStatus::Ready;
        }
    }
    delay(3000);
}
