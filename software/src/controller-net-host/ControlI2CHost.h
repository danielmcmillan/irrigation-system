#ifndef _CONTROL_I2C_HOST_H
#define _CONTROL_I2C_HOST_H
#include <Arduino.h>
#include <Wire.h>
#include "crc16.h"
#include "logging.h"

char logBuffer[128] = {0};
const int controlProcessorI2CAddress = 10;
const uint8_t getPropertyMessageType = 0x20;
const uint8_t setPropertyMessageType = 0x21;
const uint8_t ackMessageType = 0x61;

class ControlI2CHost
{
public:
    void begin()
    {
        if (!Wire.begin())
        {
            Serial.println("Failed to begin I2C");
        }
    }

    void getPropertyValue(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, bool readOnly, uint8_t *resultValue, uint8_t *resultDesiredValue)
    {
        uint8_t dataBuffer[32] = {0};

        unsigned long startMillis = millis();
        LOG_INFO("Starting getPropertyValue");

        // Build command to request property value
        dataBuffer[0] = getPropertyMessageType;
        dataBuffer[1] = controllerId;
        dataBuffer[2] = propertyId;      // LSB
        dataBuffer[3] = propertyId >> 8; // MSB
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
        size_t responseLength = 1 /*ack*/ + valueSize * 2 /*value + desired value*/ + 2 /*crc*/;
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
            memcpy(resultValue, &dataBuffer[1], valueSize);
            if (!readOnly)
            {
                memcpy(resultDesiredValue, &dataBuffer[1 + valueSize], valueSize);
            }
        }

        sprintf(logBuffer, "Finished property refresh after %lu ms", millis() - startMillis);
        Serial.println(logBuffer);
    }

    void setPropertyDesiredValue(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint8_t *value)
    {
        uint8_t dataBuffer[32] = {0};
        LOG_INFO("Starting setPropertyValue");

        // Build command to set property value
        dataBuffer[0] = setPropertyMessageType;
        dataBuffer[1] = controllerId;
        dataBuffer[2] = propertyId;      // LSB
        dataBuffer[3] = propertyId >> 8; // MSB
        for (uint8_t i = 0; i < valueSize; ++i)
        {
            dataBuffer[4 + i] = value[i];
        }
        uint16_t crc = IrrigationSystem::CRC::crc16(dataBuffer, 4 + valueSize);
        dataBuffer[4 + valueSize] = crc;          // LSB
        dataBuffer[4 + valueSize + 1] = crc >> 8; // MSB

        // Send command
        Wire.beginTransmission(controlProcessorI2CAddress);
        Wire.write(dataBuffer, 6 + valueSize);
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
    }
};
#endif
