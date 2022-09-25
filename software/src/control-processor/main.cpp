#include <Arduino.h>
#include "logging.h"
#include "crc16.h"
#include "vacon-100-controller.h"
#include "controller-manager.h"
#include "controller-definition-manager.h"
#include "i2c-interface.h"
#include "message-handler.h"

using namespace IrrigationSystem;

Vacon100Controller vacon100Controller;
ControllerRegistration registeredControllers[] = {
    {0x02, &vacon100Controller}};

ControllerManager controllers(registeredControllers, sizeof(registeredControllers) / sizeof(registeredControllers[0]));

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
    vacon100Controller.update();
    vacon100Controller.applyPropertyValues();
    bool available = vacon100Controller.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::available);
    bool motorOn = vacon100Controller.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorOn);
    bool desiredMotorOn = vacon100Controller.getPropertyDesiredValue(IrrigationSystem::Vacon100ControllerProperties::motorOn);
    uint16_t status = vacon100Controller.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::status);
    uint16_t motorVoltage = vacon100Controller.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorVoltage);
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
