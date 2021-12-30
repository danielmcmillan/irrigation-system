#include <Arduino.h>
#include <Logger.h>
#include "crc16.h"
#include <Wire.h>

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
}

void loop()
{
}
