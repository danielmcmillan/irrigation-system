#include <Arduino.h>
#include "logging.h"
#include "events/event-history.h"
#include "controllers/controller-builder.h"
#include "i2c-interface.h"
#include "message-handler.h"

using namespace IrrigationSystem;

ControllerBuilder controllerBuilder;
ControllerManager controllers = controllerBuilder.buildManager();

EventHistory events;

ControlProcessorMessageHandler handler(controllers, events);
ControlProcessorI2cInterface i2c = ControlProcessorI2cInterface::initialise(controllers, handler);

bool controllersInitialised = false;

void setup()
{
    Serial.begin(9600);
    i2c.setup();

    controllers.setEventHandler(events);
    controllers.resetControllers();
}

void loop()
{
    if (controllersInitialised)
    {
        controllers.getController(0x02)->update();
        controllers.getController(0x02)->applyPropertyValues();
    }
    else
    {
        // TODO only begin when requested after configuration
        if (controllers.beginControllers())
        {
            controllersInitialised = true;
        }
    }
    delay(3000);
}
