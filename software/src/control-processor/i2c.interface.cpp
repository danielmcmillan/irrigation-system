#include "i2c-interface.h"
#include <Arduino.h>
#include <Wire.h>
#include "logging.h"
#include "binary-util.h"

ControlProcessorI2cInterface::ControlProcessorI2cInterface(const ControllerManager &controllers, const ControlProcessorMessageHandler &handler)
    : packet(controllers), handler(handler)
{
}

ControlProcessorI2cInterface ControlProcessorI2cInterface::initialise(const ControllerManager &controllers, const ControlProcessorMessageHandler &handler)
{
    static ControlProcessorI2cInterface instance(controllers, handler);
    Wire.onReceive([](int _) {});
    Wire.onRequest([]()
                   { instance.handleRequest(); });
    return instance;
}

void ControlProcessorI2cInterface::setup()
{
    Wire.begin(CONTROL_PROCESSOR_I2C_SLAVE_ADDRESS);
}

void ControlProcessorI2cInterface::handleRequest()
{
    static uint8_t buffer[32];
    size_t length = Wire.readBytes(buffer, Wire.available());
    uint8_t result = packet.validatePacket(buffer, length);

    static uint8_t responseBuffer[32];
    ControlProcessorPacket::MessageType *responseType;
    uint8_t *responseData;
    size_t responseDataSize = 0;
    packet.createPacket(responseBuffer + 1, &responseType, &responseData);

    if (result == 2)
    {
        LOG_ERROR("Received I2C packet with invalid CRC");
    }
    else if (result == 1)
    {
        LOG_ERROR("Received I2C packet with invalid data");
    }
    else if (result == 0)
    {
        const uint8_t *data;
        ControlProcessorPacket::MessageType type = packet.getMessageType(buffer, &data);
        result = this->handleMessage(type, data, responseData, &responseDataSize);
    }

    if (result == 0)
    {
        *responseType = ControlProcessorPacket::MessageType::Ack;
    }
    else
    {
        *responseType = ControlProcessorPacket::MessageType::Nak;
        *responseData = result;
        responseDataSize = 1;
    }
    size_t responseSize = packet.finalisePacket(responseBuffer + 1, responseDataSize) + 1;
    responseBuffer[0] = responseSize;
    Wire.write(responseBuffer, responseSize);
}

// Returns 0 on success, otherwise a NAK failure reason
int ControlProcessorI2cInterface::handleMessage(ControlProcessorPacket::MessageType type, const uint8_t *data, uint8_t *responseDataOut, size_t *responseDataSizeOut)
{
    switch (type)
    {
    case ControlProcessorPacket::MessageType::ConfigStart:
    case ControlProcessorPacket::MessageType::ConfigEnd:
    case ControlProcessorPacket::MessageType::ConfigAdd:
        LOG_ERROR("Config commands not implemented.");
        break;
    case ControlProcessorPacket::MessageType::PropertyRead:
        return this->handler.propertyRead(data[0], read16LE(&data[1]), responseDataOut, responseDataSizeOut);
    case ControlProcessorPacket::MessageType::PropertySet:
        return this->handler.propertyWrite(data[0], read16LE(&data[1]), data + 3);
        *responseDataSizeOut = 0;
        break;
    default:
        break;
    }
    return 0;
}
