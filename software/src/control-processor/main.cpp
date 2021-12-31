#include <Arduino.h>
#include <Logger.h>
#include <Wire.h>
#include <NeoSWSerial.h>
#include "crc16.h"
#include "Vacon100Modbus/vacon100-modbus.h"

// RS485
#define MAX485_RE A3
#define MAX485_DE A2
#define MAX485_RO 9
#define MAX485_DI 8
NeoSWSerial ss(MAX485_RO, MAX485_DI);
Vacon100Modbus::Client vc(ss, MAX485_RE, MAX485_DE, MAX485_DI);

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

    ss.begin(9600);
    vc.begin();
}

void loop()
{
    delay(3000);
    Vacon100Modbus::Data vcData;
    if (vc.readInputRegisters(&vcData))
    {
        vc.setStart(true, true);
        Serial.println(vcData.toString());
    }
    else
    {
        Serial.println("Failed to read from Modbus");
    }
}
