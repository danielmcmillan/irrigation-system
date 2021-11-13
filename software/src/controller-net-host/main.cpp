#include <Arduino.h>
#include <Logger.h>
#include "crc16.h"

void refreshPropertyValues();

namespace IrrigationSystem
{
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
}

// Motor on property
IrrigationSystem::Property properties[] = {{.source = 0x0001, .id = 0x00000000, .size = 1, .value = {0x00}, .desiredValue = {0x00}}};

void setup()
{
    Serial.begin(9600);
    Logger::setLogLevel(Logger::VERBOSE);
}

void loop()
{
    refreshPropertyValues();
    delay(5000);
}

void refreshPropertyValues()
{
    Logger::verbose("Starting property refresh");
    // Send get property values command
    // Read response, updates propertyValues
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00};
    uint16_t crc = IrrigationSystem::CRC::crc16(data, sizeof(data));
    uint8_t *cp = reinterpret_cast<uint8_t *>(&crc);
    char s[128] = {0};

    sprintf(s, "CRC: 0x%04x, first: 0x%02x, second: 0x%02x\n", crc, cp[0], cp[1]);
    Logger::verbose(s);
}
