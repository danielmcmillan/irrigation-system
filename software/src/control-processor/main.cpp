#include <Arduino.h>
#include "logging.h"
#include "crc16.h"
#include "controllers.h"
#include "i2c-interface.h"
#include "message-handler.h"
#include "vacon-100-controller-definition.h"

using namespace IrrigationSystem;

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

    bool available = controllers.getController(0x02)->getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::available);
    bool motorOn = controllers.getController(0x02)->getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorOn);
    bool desiredMotorOn = controllers.getController(0x02)->getPropertyDesiredValue(IrrigationSystem::Vacon100ControllerProperties::motorOn);
    uint16_t status = controllers.getController(0x02)->getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::status);
    uint16_t motorVoltage = controllers.getController(0x02)->getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorVoltage);
    Serial.print("Vacon available: ");
    Serial.print(available);
    Serial.println();
    Serial.print("Motor on: ");
    Serial.print(motorOn);
    Serial.println();
    Serial.print("Desired motor on: ");
    Serial.print(desiredMotorOn);
    Serial.println();
    Serial.print("Status: ");
    Serial.print(status);
    Serial.println();
    Serial.print("Motor voltage: ");
    Serial.print(motorVoltage);
    Serial.println();
}
