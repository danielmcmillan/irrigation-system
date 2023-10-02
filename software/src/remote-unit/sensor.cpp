#include "sensor.h"
#include <Arduino.h>
#include "crc16.h"

uint8_t rs485ReadWaterPotentialCommand[] = {0x02,
                                            0x03,
                                            0x00,
                                            0x00,
                                            0x00,
                                            0x01,
                                            0x84,
                                            0x39};

RemoteUnitSensor::RemoteUnitSensor(uint8_t rxPin, uint8_t txPin, uint8_t reDePin, uint8_t rs485EnablePin)
    : txPin(txPin), reDePin(reDePin), rs485EnablePin(rs485EnablePin), ss(rxPin, txPin)
{
}

void RemoteUnitSensor::setup()
{
    rs485Setup();
}

int RemoteUnitSensor::readValue(uint8_t sensor, uint16_t *valueOut)
{
    int result = 0;
    // Clear any previous data in Serial buffer
    while (ss.available() > 0)
    {
        ss.read();
    }

    // Start up sensor and RS485 transmitter
    rs485On();
    rs485Mode(true);
    delayMicroseconds(50);

    // Write out command to read water potential value
    ss.write(rs485ReadWaterPotentialCommand, sizeof(rs485ReadWaterPotentialCommand));
    ss.flush();
    delayMicroseconds(50);

    // Disable RS485 transmitter and enable receive
    rs485Mode(false);
    uint8_t response[7] = {0};
    size_t responseLength = ss.readBytes(response, 7);

    if (responseLength == 7 && response[0] == 2 && response[1] == 3 && response[2] == 2)
    {
        uint16_t crc = IrrigationSystem::CRC::crc16(response, responseLength);
        if (crc == 0)
        {
            result = 0;
            *valueOut = ((uint16_t)response[3] << 8) + response[4];
        }
        else
        {
            result = 2;
        }
    }
    else
    {
        result = 1;
    }

    rs485Off();
    return result;
}

void RemoteUnitSensor::rs485Setup()
{
    pinMode(rs485EnablePin, OUTPUT);
    rs485Off();
}
void RemoteUnitSensor::rs485Off()
{
    ss.end();
    pinMode(reDePin, INPUT);
    pinMode(txPin, INPUT);
    digitalWrite(rs485EnablePin, LOW);
}
void RemoteUnitSensor::rs485On()
{
    // SoftwareSerial library is setting up the pin only in the constructor
    digitalWrite(txPin, HIGH);
    pinMode(txPin, OUTPUT);
    ss.begin(9600);
    ss.setTimeout(2000);
    digitalWrite(rs485EnablePin, HIGH);
    delay(200);
}
void RemoteUnitSensor::rs485Mode(bool tx)
{
    digitalWrite(reDePin, tx ? HIGH : LOW);
    pinMode(reDePin, OUTPUT);
}
