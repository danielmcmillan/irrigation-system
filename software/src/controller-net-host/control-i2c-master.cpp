#include "control-i2c-master.h"
#include <Arduino.h>
#include <Wire.h>
#include "logging.h"
// #include "crc16.h"
#include "binary-util.h"

ControlI2cMaster::ControlI2cMaster(const ControllerDefinitionProvider &controllers, const ErrorHandler &errorHandler) : packet(controllers), errorHandler(errorHandler)
{
}

void ControlI2cMaster::setup()
{
    if (!Wire.begin())
    {
        LOG_ERROR("[Control] Failed to begin I2C");
    }
}

bool ControlI2cMaster::getNextEvent(uint16_t lastEvent, uint8_t *eventOut, size_t *eventSizeOut) const
{
    uint8_t data[2];
    write16LE(data, lastEvent);
    const uint8_t *response;
    size_t responseSize;
    bool success = sendMessage(ControlProcessorPacket::MessageType::EventGetNext, data, 2, &response, &responseSize);
    if (success && response[0] != 0)
    {
        *eventSizeOut = responseSize - 1;
        memcpy(eventOut, &response[1], *eventSizeOut);
        return true;
    }
    // TODO differentiate whether any event was missed
    return false;
}

bool ControlI2cMaster::configStart() const
{
    return sendMessage(ControlProcessorPacket::MessageType::ConfigStart, nullptr, 0, nullptr, nullptr);
}

bool ControlI2cMaster::configAdd(const uint8_t *data, size_t length) const
{
    return sendMessage(ControlProcessorPacket::MessageType::ConfigAdd, data, length, nullptr, nullptr);
}

bool ControlI2cMaster::configEnd() const
{
    return sendMessage(ControlProcessorPacket::MessageType::ConfigEnd, nullptr, 0, nullptr, nullptr);
}

bool ControlI2cMaster::getPropertyValue(uint8_t controllerId, uint16_t propertyId, uint8_t *valuesOut) const
{
    uint8_t data[3];
    data[0] = controllerId;
    write16LE(&data[1], propertyId);
    const uint8_t *response;
    size_t responseSize;
    if (sendMessage(ControlProcessorPacket::MessageType::PropertyRead, data, 3, &response, &responseSize))
    {
        memcpy(valuesOut, response, responseSize);
        return true;
    }
    return false;
}

bool ControlI2cMaster::setPropertyValue(const uint8_t *data, size_t length) const
{
    return sendMessage(ControlProcessorPacket::MessageType::PropertySet, data, length, nullptr, nullptr);
}

bool ControlI2cMaster::sendMessage(ControlProcessorPacket::MessageType type, const uint8_t *data, size_t dataSize, const uint8_t **responseOut, size_t *responseSizeOut) const
{
    if (responseSizeOut != nullptr)
    {
        *responseSizeOut = 0;
    }
    // Build the packet
    ControlProcessorPacket::MessageType *packetType;
    uint8_t *packetData;
    packet.createPacket(packetBuffer, &packetType, &packetData);
    *packetType = type;
    memcpy(packetData, data, dataSize);
    size_t packetSize = packet.finalisePacket(packetBuffer, dataSize);

    // Send packet
    Wire.beginTransmission(CONTROLLER_NET_HOST_I2C_SLAVE_ADDRESS);
    Wire.write(packetBuffer, packetSize);
    uint8_t writeResult = Wire.endTransmission(false);
    if (writeResult != 0)
    {
        handleError(type, MessageResultType::SendFailed, writeResult, "Write failed");
        return false;
    }

    // Read response
    uint8_t bytesRead = Wire.requestFrom(CONTROLLER_NET_HOST_I2C_SLAVE_ADDRESS, CONTROLLER_NET_HOST_I2C_PACKET_BUFFER_SIZE);
    Wire.readBytes(packetBuffer, bytesRead);
    uint8_t *responsePacket = packetBuffer + 1;
    if (bytesRead == 0)
    {
        handleError(type, MessageResultType::ResponseInvalid, 1, "Failed to receive any data from I2C");
        return false;
    }

    // Validate response
    size_t responseLength = packetBuffer[0];
    if (responseLength == 0 || responseLength > bytesRead)
    {
        handleError(type, MessageResultType::ResponseInvalid, 2, "Response included invalid length");
        return false;
    }
    size_t responsePacketSize = responseLength - 1; // packet excludes length byte
    uint8_t responseValidateResult = packet.validatePacket(responsePacket, responsePacketSize);
    if (responseValidateResult != 0)
    {
        handleError(type, MessageResultType::ResponseInvalid, (uint8_t)(responseValidateResult + 3u), "Response packet was invalid");
        return false;
    }
    ControlProcessorPacket::MessageType responseType = packet.getMessageType(responsePacket);
    const uint8_t *responseData;
    size_t responseDataSize = packet.getMessageData(responsePacket, responsePacketSize, &responseData);
    if (responseType == ControlProcessorPacket::MessageType::Err)
    {
        handleError(type, MessageResultType::ResponseError, *responseData, "Received error response");
        return false;
    }
    else if (responseType != ControlProcessorPacket::MessageType::Ack)
    {
        handleError(type, MessageResultType::ResponseInvalid, 3, "Received invalid response type");
        return false;
    }

    // TODO Split the I2C part (send+read+response length) out into separate function?
    if (responseOut != nullptr)
    {
        *responseOut = responseData;
        *responseSizeOut = responseDataSize;
    }
    return true;
}

void ControlI2cMaster::handleError(ControlProcessorPacket::MessageType messageType, MessageResultType resultType, uint8_t reason, const char *text) const
{
    // code = <1 byte message type><4 bit result type><4 bit reason>
    errorHandler.handleError(ErrorComponent::ControlI2c, (reason & 0x0f) | (((uint8_t)resultType << 4) & 0xf0) | ((uint8_t)messageType << 8), text);
}

// if (dataSize > 0)
// {
//     Serial.printf("Writing: 0x");
//     for (int i = 0; i < dataSize; ++i)
//     {
//         Serial.printf("%02x", data[i]);
//     }
//     Serial.println();
// }
