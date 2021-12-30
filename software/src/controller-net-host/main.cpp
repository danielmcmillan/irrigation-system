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
    uint8_t controller;
    uint16_t id;
    /** The number of bytes for the value */
    uint8_t size;
    bool readOnly;
    // /** Power of 10 that the value is multiplied by */
    // int8_t mul;
    // /** The format of the data */
    // PropertyFormat format;
    /** Property value, little-endian */
    uint8_t value[4];
    /** For a writeable property, the selected value for the property */
    uint8_t desiredValue[4];
};

char logBuffer[128] = {0};
// Motor on property
Property properties[] = {{.controller = 0x02, .id = 0x0001, .size = 1}};

void setup()
{
    Serial.begin(9600);
    Logger::setLogLevel(Logger::VERBOSE);

    Wire.begin();
}

void loop()
{
    refreshPropertyValues();
    delay(2000);
}

void logProperties()
{
    for (Property property : properties)
    {
        sprintf(logBuffer, "Controller 0x%02x, id 0x%04x, value 0x%02x, desired 0x%02x", property.controller, property.id, property.value[0], property.desiredValue[0]);
        Logger::notice(logBuffer);
    }
}

void refreshPropertyValues()
{
    uint8_t dataBuffer[32] = {0};

    unsigned long startMillis = millis();
    Logger::verbose("Starting property refresh");

    const int controlProcessorI2CAddress = 10;
    const uint8_t requestPropertiesCommand = 0x02;
    const uint8_t ackCommand = 0x50;

    // Build command to request property value
    dataBuffer[0] = requestPropertiesCommand;
    dataBuffer[1] = properties[0].controller;
    *((uint16_t *)&dataBuffer[2]) = properties[0].id;
    *((uint16_t *)&dataBuffer[4]) = IrrigationSystem::CRC::crc16(dataBuffer, 5);

    // Send command
    Wire.beginTransmission(controlProcessorI2CAddress);
    Wire.write(dataBuffer, 7);
    uint8_t writeResult = Wire.endTransmission(false);
    sprintf(logBuffer, "Write result: %d.", writeResult);
    Logger::verbose(logBuffer);

    // Read property value
    size_t responseLength = 1 /*ack*/ + properties[0].size * 2 /*value + desired value*/ + 2 /*crc*/;
    uint8_t bytesRead = Wire.requestFrom(controlProcessorI2CAddress, responseLength);
    Wire.readBytes(dataBuffer, responseLength);
    uint16_t responseCrc = IrrigationSystem::CRC::crc16(dataBuffer, responseLength);
    sprintf(logBuffer, "Read: %d bytes: 0x%02x%02x%02x%02x%02x. CRC: 0x%04x", bytesRead, dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3], dataBuffer[4], responseCrc);
    Logger::verbose(logBuffer);

    if (bytesRead != responseLength)
    {
        Logger::error("Read wrong number of bytes");
    }
    else if (responseCrc != 0)
    {
        Logger::error("Got a non-zero CRC");
    }
    else if (dataBuffer[0] != ackCommand)
    {
        Logger::error("Response wasn't ACK");
    }
    else
    {
        // Copy property value to properties
        memcpy(properties[0].value, &dataBuffer[1], properties[0].size);
        if (!properties[0].readOnly)
        {
            memcpy(properties[0].desiredValue, &dataBuffer[1 + properties[0].size], properties[0].size);
        }
    }

    sprintf(logBuffer, "Finished property refresh after %lu ms", millis() - startMillis);
    Logger::notice(logBuffer);
    logProperties();

    // Read response, updates propertyValues
    // const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00};
    // uint16_t crc = IrrigationSystem::CRC::crc16(data, sizeof(data));
    // uint8_t *cp = reinterpret_cast<uint8_t *>(&crc);

    // sprintf(s, "CRC: 0x%04x, first: 0x%02x, second: 0x%02x\n", crc, cp[0], cp[1]);
    // Logger::verbose(s);
}
