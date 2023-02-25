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
    Serial.begin(9600, SERIAL_8N1);
    i2c.setup();

    controllers.setEventHandler(events);
    controllers.resetControllers();

    // TODO temp config for remote unit
    uint8_t configData[5];
    configData[0] = 5;
    configData[1] = 1;
    configData[2] = 0;
    controllers.getController(0x04)->configure(0x01, configData);
    configData[0] = 9;
    configData[1] = 5;
    configData[2] = 0;
    controllers.getController(0x04)->configure(0x02, configData);
}

void loop()
{
    if (controllersInitialised)
    {
        controllers.getController(0x02)->update();
        controllers.getController(0x02)->applyPropertyValues();
        controllers.getController(0x04)->update();
        controllers.getController(0x04)->applyPropertyValues();
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
