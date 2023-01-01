#include <Arduino.h>
#include "logging.h"
#include "crc16.h"
#include "controllers/controller-builder.h"
#include "i2c-interface.h"
#include "message-handler.h"

using namespace IrrigationSystem;

ControllerBuilder controllerBuilder;
ControllerManager controllers = controllerBuilder.buildManager();

ControlProcessorMessageHandler handler(controllers);
ControlProcessorI2cInterface i2c = ControlProcessorI2cInterface::initialise(controllers, handler);

void setup()
{
    Serial.begin(9600);
    i2c.setup();

    controllers.resetControllers();
    controllers.beginControllers(); // TODO only begin when requested after configuration
}

void loop()
{
    delay(3000);
    controllers.getController(0x02)->update();
    controllers.getController(0x02)->applyPropertyValues();
}
