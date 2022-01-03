#include <Arduino.h>
#include <Logger.h>
#include <Wire.h>
#include "crc16.h"
#include "vacon-100-controller.h"
#include "controller-manager.h"

#define I2C_SLAVE_ADDRESS 10

#define LOG_INFO(msg) Serial.println(F("INFO " msg))
#define LOG_ERROR(msg) Serial.println(F("ERROR " msg))

IrrigationSystem::Vacon100Controller vacon100Controller;
ControllerRegistration registeredControllers[] = {
    {0x02, &vacon100Controller}};

ControllerManager controllers(registeredControllers, sizeof(registeredControllers) / sizeof(registeredControllers[0]));

void onReceive(int length)
{
    // char s[128] = {0};
    // Serial.print("Received: 0x");
    // while (Wire.available() > 0)
    // {
    //     int data = Wire.read();
    //     sprintf(s, "%02x", data);
    //     Serial.print(s);
    // }
    // Serial.println();
}

void onRequest()
{
    static uint8_t buffer[32] = {0};
    size_t length = Wire.readBytes(buffer, Wire.available());

    uint16_t crc = IrrigationSystem::CRC::crc16(buffer, length);
    if (crc != 0)
    {
        LOG_ERROR("CRC error");
        return;
    }

    if (length == 6 && buffer[0] == 0x20 /*get property value*/)
    {
        LOG_INFO("Recieved getPropertyValue message");
        IrrigationSystem::Controller *controller = controllers.getController(buffer[1]);
        const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
        uint16_t propertyId = (uint16_t)buffer[2] | ((uint16_t)buffer[3] << 8);
        int valueLength = definition.getPropertyLength(propertyId);
        bool readOnly = definition.getPropertyReadOnly(propertyId);

        buffer[0] = 0x61; // ACK
        length = 1 + (readOnly ? valueLength : (valueLength * 2)) + 2;

        uint32_t value = controller->getPropertyValue(propertyId);
        for (int i = 0; i < valueLength; ++i)
        {
            buffer[1 + i] = value; // LSB
            value >>= 8;
        }

        if (!readOnly)
        {
            value = controller->getPropertyDesiredValue(propertyId);
            for (int i = 0; i < valueLength; ++i)
            {
                buffer[1 + valueLength + i] = value; // LSB
                value >>= 8;
            }
        }

        crc = IrrigationSystem::CRC::crc16(buffer, length - 2);
        buffer[length - 2] = crc;      // LSB
        buffer[length - 1] = crc >> 8; // MSB
    }
    else if (length >= 6 && buffer[0] == 0x21 /*set property value*/)
    {
        LOG_INFO("Received setPropertyValue message");
        IrrigationSystem::Controller *controller = controllers.getController(buffer[1]);
        const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
        uint16_t propertyId = (uint16_t)buffer[2] | ((uint16_t)buffer[3] << 8);
        int valueLength = definition.getPropertyLength(propertyId);
        bool readOnly = definition.getPropertyReadOnly(propertyId);

        if (readOnly)
        {
            LOG_ERROR("setPropertyValue for readonly property");
            return;
        }
        if (length != 6 + valueLength)
        {
            LOG_ERROR("setPropertyValue with incorrect value length");
            return;
        }

        uint32_t value = 0;
        for (int i = 0; i < valueLength; ++i)
        {
            value |= buffer[4 + i] << i * 8;
        }

        controller->setPropertyDesiredValue(propertyId, value);

        buffer[0] = 0x61; // ACK
        length = 3;
        crc = IrrigationSystem::CRC::crc16(buffer, length - 2);
        buffer[length - 2] = crc;      // LSB
        buffer[length - 1] = crc >> 8; // MSB
    }
    else
    {
        LOG_ERROR("Invalid I2C message");
    }

    Wire.write(buffer, length);
}

void setup()
{
    Serial.begin(9600);

    Wire.begin(I2C_SLAVE_ADDRESS);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);

    controllers.resetControllers();
    controllers.beginControllers(); // TODO only begin after requested
}

void loop()
{
    delay(3000);
    vacon100Controller.update();
    bool motorOn = vacon100Controller.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorOn);
    bool desiredMotorOn = vacon100Controller.getPropertyDesiredValue(IrrigationSystem::Vacon100ControllerProperties::motorOn);
    uint16_t status = vacon100Controller.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::status);
    uint16_t motorVoltage = vacon100Controller.getPropertyValue(IrrigationSystem::Vacon100ControllerProperties::motorVoltage);
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
