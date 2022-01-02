#include <Arduino.h>
#include <Logger.h>
#include <Wire.h>
#include "crc16.h"
#include "vacon-100-controller.h"
#include "controller-manager.h"

IrrigationSystem::Vacon100Controller vacon100Controller;
ControllerRegistration registeredControllers[] = {
    {0x02, &vacon100Controller}};

ControllerManager controllers(registeredControllers, sizeof(registeredControllers) / sizeof(registeredControllers[0]));

void onReceive(int length)
{
    char s[128] = {0};
    Serial.print("Received: 0x");
    while (Wire.available() > 0)
    {
        int data = Wire.read();
        sprintf(s, "%02x", data);
        Serial.print(s);
    }
    Serial.println();
}

void onRequest()
{
    uint8_t data[] = {0x50, 0x00, 0x01, 0x00, 0x00};
    uint16_t crc = IrrigationSystem::CRC::crc16(data, 3);
    *((uint16_t *)&data[3]) = crc;
    Wire.write(data, 5);
}

void setup()
{
    Serial.begin(9600);
    Logger::setLogLevel(Logger::VERBOSE);
    Wire.begin(10);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);

    vacon100Controller.reset();
    vacon100Controller.begin();
}

void loop()
{
    delay(3000);
    vacon100Controller.update();
    bool motorOn = vc.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorOn);
    uint16_t status = vc.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::status);
    uint16_t motorVoltage = vc.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorVoltage);
    Serial.print("Motor on: ");
    Serial.print(motorOn);
    Serial.println();
    Serial.print("Status: ");
    Serial.print(status);
    Serial.println();
    Serial.print("Motor voltage: ");
    Serial.print(motorVoltage);
    Serial.println();
}
