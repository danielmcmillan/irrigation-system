#include "control-i2c-master.h"
#include <Arduino.h>
#include <Wire.h>
#include "logging.h"
// #include "crc16.h"
#include "binary-util.h"

ControlI2cMaster::ControlI2cMaster(const ControllerDefinitionProvider &controllers) : packet(controllers)
{
}

void ControlI2cMaster::setup()
{
    if (!Wire.begin())
    {
        LOG_ERROR("[Control] Failed to begin I2C");
    }
}

int ControlI2cMaster::getNextEvent(uint16_t lastEvent, uint8_t *eventOut, size_t *eventSizeOut) const
{
    uint8_t data[2];
    write16LE(data, lastEvent);
    const uint8_t *response;
    size_t responseSize;
    MessageResultInfo result = sendMessage(ControlProcessorPacket::MessageType::EventGetNext, data, 2, &response, &responseSize);
    if (result.type == MessageResultType::Success && response[0] != 0)
    {
        *eventSizeOut = responseSize - 1;
        memcpy(eventOut, &response[1], *eventSizeOut);
        return 1;
    }
    // TODO differentiate error, no result, and missed events
    return 0;
}

MessageResultInfo ControlI2cMaster::sendMessage(ControlProcessorPacket::MessageType type, const uint8_t *data, size_t dataSize, const uint8_t **responseOut, size_t *responseSizeOut) const
{
    *responseSizeOut = 0;
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
        LOG_ERROR("[Control] Failed to write to I2C");
        return {MessageResultType::SendFailed, writeResult};
    }

    // Read response
    uint8_t bytesRead = Wire.requestFrom(CONTROLLER_NET_HOST_I2C_SLAVE_ADDRESS, CONTROLLER_NET_HOST_I2C_PACKET_BUFFER_SIZE);
    Wire.readBytes(packetBuffer, bytesRead);
    if (bytesRead == 0)
    {
        LOG_ERROR("[Control] Failed to receive any data from I2C");
        return {MessageResultType::ResponseInvalid, 1};
    }

    // Validate response
    size_t responseLength = packetBuffer[0];
    if (responseLength == 0 || responseLength > bytesRead)
    {
        LOG_ERROR("[Control] Response included invalid length");
        return {MessageResultType::ResponseInvalid, 2};
    }
    packetSize = responseLength - 1;
    uint8_t responseValidateResult = packet.validatePacket(packetBuffer + 1, packetSize);
    if (responseValidateResult != 0)
    {
        LOG_ERROR("[Control] Response packet was invalid");
        return {MessageResultType::ResponseInvalid, (uint8_t)(responseValidateResult + 3u)};
    }
    const uint8_t *responseData;
    ControlProcessorPacket::MessageType responseType = packet.getMessageType(packetBuffer + 1, &responseData);
    if (responseType == ControlProcessorPacket::MessageType::Err)
    {
        LOG_ERROR("[Control] Received error response");
        return {MessageResultType::ResponseError, *responseData};
    }
    else if (responseType != ControlProcessorPacket::MessageType::Ack)
    {
        LOG_ERROR("[Control] Received invalid response type");
        return {MessageResultType::ResponseInvalid, 3};
    }

    // TODO Packet lib should provide data length?
    // Split the I2C part (send+read+response length) out into separate function?
    *responseOut = responseData;
    *responseSizeOut = responseLength - 3;
    return {MessageResultType::Success, 0};

    // sprintf(logBuffer, "Response: %d/%d bytes. CRC: 0x%04x", responseLength, bytesRead, responseCrc);
}
