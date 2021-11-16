#include <Arduino.h>
#include <Logger.h>
#include "crc16.h"
#include <Wire.h>

void refreshPropertyValues();

// enum class PropertyFormat
// {
//     boolean,
//     signedInt,
//     unsignedInt
// };

struct Property
{
    uint16_t source;
    uint32_t id;
    /** The number of bytes for the value */
    uint8_t size;
    // /** Power of 10 that the value is multiplied by */
    // int8_t mul;
    // /** The format of the data */
    // PropertyFormat format;
    /** Property value, little-endian */
    uint8_t value[4];
    /** For a writeable property, the selected value for the property */
    uint8_t desiredValue[4];
};

// Motor on property
Property properties[] = {{.source = 0x0001, .id = 0x00000000, .size = 1, .value = {0x00}, .desiredValue = {0x00}}};

void setup()
{
    Serial.begin(9600);
    Logger::setLogLevel(Logger::VERBOSE);

    Wire.begin();
}

void loop()
{
    refreshPropertyValues();
    delay(100);
}

void refreshPropertyValues()
{
    char s[128] = {0};
    unsigned long startMillis = millis();
    Logger::verbose("Starting property refresh");

    // Send slave register address
    uint32_t propertyValuesSlaveRegister = 0x12345678;
    Wire.beginTransmission(10);
    Wire.write(0x56);
    Wire.write(0x34);
    Wire.write(0x12);
    uint8_t writeResult = Wire.endTransmission(false);
    sprintf(s, "Write result: %d.", writeResult);
    Logger::verbose(s);
    uint8_t bytesRead = Wire.requestFrom(10, 7);
    sprintf(s, "Bytes read: %d", bytesRead);
    Logger::verbose(s);
    uint16_t crc = IrrigationSystem::CRC::crc16Init;
    while (Wire.available() > 0)
    {
        int data = Wire.read();
        crc = IrrigationSystem::CRC::crc16Update(crc, data);
        sprintf(s, "%02x", data);
        // Serial.print(s);
    }
    // Serial.println();
    sprintf(s, "CRC: 0x%04x", crc);
    Logger::verbose(s);

    if (crc != 0)
    {
        Logger::error("Got a non-zero CRC");
        sprintf(s, "CRC: 0x%04x", crc);
        Logger::error(s);
    }

    sprintf(s, "Finished property refresh after %lu ms", millis() - startMillis);
    Logger::notice(s);

    // Read response, updates propertyValues
    // const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00};
    // uint16_t crc = IrrigationSystem::CRC::crc16(data, sizeof(data));
    // uint8_t *cp = reinterpret_cast<uint8_t *>(&crc);

    // sprintf(s, "CRC: 0x%04x, first: 0x%02x, second: 0x%02x\n", crc, cp[0], cp[1]);
    // Logger::verbose(s);
}
