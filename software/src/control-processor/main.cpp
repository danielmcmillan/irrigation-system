#include <Arduino.h>
#include <Logger.h>
#include "crc16.h"

void setup()
{
    Serial.begin(9600);
    Logger::setLogLevel(Logger::VERBOSE);
}

void loop()
{
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00};
    uint16_t crc = IrrigationSystem::CRC::crc16(data, sizeof(data));
    uint8_t *cp = reinterpret_cast<uint8_t *>(&crc);
    char s[128] = {0};

    sprintf(s, "CRC: 0x%04x, first: 0x%02x, second: 0x%02x\n", crc, cp[0], cp[1]);
    Logger::verbose(s);
    delay(5000);
}
