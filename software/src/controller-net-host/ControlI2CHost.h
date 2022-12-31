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

    int sendRawData(const uint8_t *data, size_t size, size_t *resultLength, uint8_t *result)
    {
        size_t bufferSize = 32;
        uint8_t dataBuffer[bufferSize] = {0};
        memcpy(dataBuffer, data, size);

        unsigned long startMillis = millis();
        LOG_INFO("Starting sendRawData");

        uint16_t crc = IrrigationSystem::CRC::crc16(data, size);
        dataBuffer[size] = crc;          // LSB
        dataBuffer[size + 1] = crc >> 8; // MSB

        // Send command
        Wire.beginTransmission(controlProcessorI2CAddress);
        Wire.write(dataBuffer, size + 2);
        uint8_t writeResult = Wire.endTransmission(false);
        sprintf(logBuffer, "Write result: %d.", writeResult);
        Serial.println(logBuffer);

        // Read property value
        uint8_t bytesRead = Wire.requestFrom(controlProcessorI2CAddress, bufferSize);
        Wire.readBytes(dataBuffer, bytesRead);
        if (bytesRead == 0)
        {
            LOG_ERROR("Failed to receive any data from I2C");
            return 1;
        }

        uint8_t responseLength = dataBuffer[0];
        if (responseLength < 1 || responseLength > bytesRead)
        {
            LOG_ERROR("Response included invalid length");
            return 2;
        }

        uint16_t responseCrc = IrrigationSystem::CRC::crc16(dataBuffer + 1, responseLength - 1);
        sprintf(logBuffer, "Response: %d/%d bytes. CRC: 0x%04x", responseLength, bytesRead, responseCrc);
        Serial.println(logBuffer);
        if (responseCrc != 0)
        {
            LOG_ERROR("Got a non-zero CRC");
            return 3;
        }

        sprintf(logBuffer, "Finished sendRawData after %lu ms", millis() - startMillis);
        Serial.println(logBuffer);

        *resultLength = responseLength - 3;
        memcpy(result, dataBuffer + 1, *resultLength);

        if (dataBuffer[1] != ackMessageType)
        {
            LOG_ERROR("Response wasn't ACK");
            return 4;
        }
        return 0;
    }

    int getPropertyValue(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, bool readOnly, uint8_t *resultValue, uint8_t *resultDesiredValue)
    {
        uint8_t dataBuffer[32] = {0};
        // Build command to request property value
        dataBuffer[0] = getPropertyMessageType;
        dataBuffer[1] = controllerId;
        dataBuffer[2] = propertyId;      // LSB
        dataBuffer[3] = propertyId >> 8; // MSB

        size_t resultLength;
        int result = sendRawData(dataBuffer, 4, &resultLength, dataBuffer);
        if (result != 0)
        {
            return result;
        }

        // TODO further validation of the response data packet (should have value/s of correct length)
        // Copy value to result
        memcpy(resultValue, &dataBuffer[1], valueSize);
        if (!readOnly)
        {
            memcpy(resultDesiredValue, &dataBuffer[1 + valueSize], valueSize);
        }
        return 0;
    }

    int setPropertyDesiredValue(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, const uint8_t *value)
    {
        uint8_t dataBuffer[32] = {0};
        // Build command to set property value
        dataBuffer[0] = setPropertyMessageType;
        dataBuffer[1] = controllerId;
        dataBuffer[2] = propertyId;      // LSB
        dataBuffer[3] = propertyId >> 8; // MSB
        for (uint8_t i = 0; i < valueSize; ++i)
        {
            dataBuffer[4 + i] = value[i];
        }

        size_t resultLength;
        // TODO further validation of the response data packet (should have no data part)
        return sendRawData(dataBuffer, 4 + valueSize, &resultLength, dataBuffer);
    }
};
#endif
