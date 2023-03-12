#include <Arduino.h>
#include "logging.h"
#include "events/event-history.h"
#include "controllers/controller-builder.h"
#include "i2c-interface.h"
#include "message-handler.h"
#include "state.h"

using namespace IrrigationSystem;

ControllerBuilder controllerBuilder;
ControllerManager controllers = controllerBuilder.buildManager();

ControlProcessorState state;

EventHistory events;

ControlProcessorMessageHandler handler(controllers, state, events);
ControlProcessorI2cInterface i2c = ControlProcessorI2cInterface::initialise(controllers, handler);

void setup()
{
    Serial.begin(9600, SERIAL_8N1);
    i2c.setup();

    controllers.setEventHandler(events);
    controllers.resetControllers();
    events.handleEvent(EventType::started, 0, nullptr);

    // TODO temp config for remote unit
    // uint8_t configData[5];
    // configData[0] = 5;
    // configData[1] = 1;
    // configData[2] = 0;
    // controllers.getController(0x04)->configure(0x01, configData);
    // configData[0] = 9;
    // configData[1] = 5;
    // configData[2] = 0;
    // controllers.getController(0x04)->configure(0x02, configData);
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
