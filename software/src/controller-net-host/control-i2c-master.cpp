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
    packetSize = responseLength - 1;
    uint8_t responseValidateResult = packet.validatePacket(packetBuffer + 1, packetSize);
    if (responseValidateResult != 0)
    {
        handleError(type, MessageResultType::ResponseInvalid, (uint8_t)(responseValidateResult + 3u), "Response packet was invalid");
        return false;
    }
    const uint8_t *responseData;
    ControlProcessorPacket::MessageType responseType = packet.getMessageType(packetBuffer + 1, &responseData);
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

    // TODO Packet lib should provide data length?
    // Split the I2C part (send+read+response length) out into separate function?
    if (responseOut != nullptr)
    {
        *responseOut = responseData;
        *responseSizeOut = responseLength - 3;
    }
    return true;
}

void ControlI2cMaster::handleError(ControlProcessorPacket::MessageType messageType, MessageResultType resultType, uint8_t reason, const char *text) const
{
    // code = <1 byte message type><4 bit result type><4 bit reason>
    errorHandler.handleError(ErrorComponent::ControlI2c, (reason & 0x0f) | (((uint8_t)resultType << 4) & 0xf0) | ((uint8_t)messageType << 8), text);
}
