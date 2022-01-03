#include <Arduino.h>
#include "crc16.h"
#include <Wire.h>

#define LOG_INFO(msg) Serial.println(F("INFO " msg))
#define LOG_ERROR(msg) Serial.println(F("ERROR " msg))

void refreshPropertyValues();
void setMotorOn(bool on);

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
Property properties[] = {{.controller = 0x02, .id = 0x0002, .size = 1, .readOnly = false}};

void setup()
{
    Serial.begin(9600);

    if (!Wire.begin())
    {
        Serial.println("Failed to begin I2C");
    }
}

void loop()
{
    delay(2000);
    refreshPropertyValues();
    delay(2000);
    setMotorOn(properties[0].desiredValue[0] == 0);
}

void logProperties()
{
    for (Property property : properties)
    {
        sprintf(logBuffer, "Controller 0x%02x, id 0x%04x, value 0x%02x, desired 0x%02x", property.controller, property.id, property.value[0], property.desiredValue[0]);
        Serial.println(logBuffer);
    }
}

void refreshPropertyValues()
{
    uint8_t dataBuffer[32] = {0};

    unsigned long startMillis = millis();
    LOG_INFO("\nStarting property refresh");

    const int controlProcessorI2CAddress = 10;
    const uint8_t getPropertyMessageType = 0x20;
    const uint8_t ackMessageType = 0x61;

    // Build command to request property value
    dataBuffer[0] = getPropertyMessageType;
    dataBuffer[1] = properties[0].controller;
    dataBuffer[2] = properties[0].id;      // LSB
    dataBuffer[3] = properties[0].id >> 8; // MSB
    uint16_t crc = IrrigationSystem::CRC::crc16(dataBuffer, 4);
    dataBuffer[4] = crc;      // LSB
    dataBuffer[5] = crc >> 8; // MSB

    // Send command
    Wire.beginTransmission(controlProcessorI2CAddress);
    Wire.write(dataBuffer, 6);
    uint8_t writeResult = Wire.endTransmission(false);
    sprintf(logBuffer, "Write get message result: %d.", writeResult);
    Serial.println(logBuffer);

    // Read property value
    size_t responseLength = 1 /*ack*/ + properties[0].size * 2 /*value + desired value*/ + 2 /*crc*/;
    uint8_t bytesRead = Wire.requestFrom(controlProcessorI2CAddress, responseLength);
    Wire.readBytes(dataBuffer, responseLength);
    uint16_t responseCrc = IrrigationSystem::CRC::crc16(dataBuffer, responseLength);
    sprintf(logBuffer, "Get property response: %d bytes: 0x%02x%02x%02x%02x%02x. CRC: 0x%04x", bytesRead, dataBuffer[0], dataBuffer[1], dataBuffer[2], dataBuffer[3], dataBuffer[4], responseCrc);
    Serial.println(logBuffer);

    if (bytesRead != responseLength)
    {
        LOG_ERROR("Read wrong number of bytes");
    }
    else if (responseCrc != 0)
    {
        LOG_ERROR("Got a non-zero CRC");
    }
    else if (dataBuffer[0] != ackMessageType)
    {
        LOG_ERROR("Response wasn't ACK");
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
    Serial.println(logBuffer);
    logProperties();
}

void setMotorOn(bool on)
{
    uint8_t dataBuffer[32] = {0};
    LOG_INFO("\nStarting set motorOn");
    Serial.printf("Setting motorOn to %d\n", on);

    const int controlProcessorI2CAddress = 10;
    const uint8_t setPropertyMessageType = 0x21;
    const uint8_t ackMessageType = 0x61;

    // Build command to set property value
    dataBuffer[0] = setPropertyMessageType;
    dataBuffer[1] = properties[0].controller;
    dataBuffer[2] = properties[0].id;      // LSB
    dataBuffer[3] = properties[0].id >> 8; // MSB
    dataBuffer[4] = on;
    uint16_t crc = IrrigationSystem::CRC::crc16(dataBuffer, 5);
    dataBuffer[5] = crc;      // LSB
    dataBuffer[6] = crc >> 8; // MSB

    // Send command
    Wire.beginTransmission(controlProcessorI2CAddress);
    Wire.write(dataBuffer, 7);
    uint8_t writeResult = Wire.endTransmission(false);
    sprintf(logBuffer, "Write set message result: %d.", writeResult);
    Serial.println(logBuffer);

    // Read result
    size_t responseLength = 1 /*ack*/ + 2 /*crc*/;
    uint8_t bytesRead = Wire.requestFrom(controlProcessorI2CAddress, responseLength);
    Wire.readBytes(dataBuffer, responseLength);
    uint16_t responseCrc = IrrigationSystem::CRC::crc16(dataBuffer, responseLength);
    sprintf(logBuffer, "Set property response: %d bytes: 0x%02x%02x%02x. CRC: 0x%04x", bytesRead, dataBuffer[0], dataBuffer[1], dataBuffer[2], responseCrc);
    Serial.println(logBuffer);

    if (bytesRead != responseLength)
    {
        LOG_ERROR("Read wrong number of bytes");
    }
    else if (responseCrc != 0)
    {
        LOG_ERROR("Got a non-zero CRC");
    }
    else if (dataBuffer[0] != ackMessageType)
    {
        LOG_ERROR("Response wasn't ACK");
    }

    Serial.println("Finished set property");
    logProperties();
}
