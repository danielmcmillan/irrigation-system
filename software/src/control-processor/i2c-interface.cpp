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
    Wire.onRequest([](uint8_t *input, uint8_t inputSize, uint8_t *output, uint8_t *outputSize)
                   { instance.handleRequest(input, inputSize, output, outputSize); });
    return instance;
}

void ControlProcessorI2cInterface::setup()
{
    Wire.begin(CONTROL_PROCESSOR_I2C_SLAVE_ADDRESS);
}

void ControlProcessorI2cInterface::handleRequest(uint8_t *input, uint8_t inputSize, uint8_t *output, uint8_t *outputSize)
{
    uint8_t result = packet.validatePacket(input, inputSize);

    ControlProcessorPacket::MessageType *responseType;
    uint8_t *responseData;
    size_t responseDataSize = 0;
    packet.createPacket(output + 1, &responseType, &responseData);

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
        ControlProcessorPacket::MessageType type = packet.getMessageType(input, &data);
        result = this->handleMessage(type, data, responseData, &responseDataSize);
    }

    if (result == 0)
    {
        *responseType = ControlProcessorPacket::MessageType::Ack;
    }
    else
    {
        *responseType = ControlProcessorPacket::MessageType::Err;
        *responseData = result;
        responseDataSize = 1;
    }
    *outputSize = packet.finalisePacket(output + 1, responseDataSize) + 1;
    output[0] = *outputSize;
}

// Returns 0 on success, otherwise an error reason
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
        *responseDataSizeOut = 0;
        return this->handler.propertyWrite(data[0], read16LE(&data[1]), data + 3);
    case ControlProcessorPacket::MessageType::EventGetNext:
        this->handler.eventGetNext(
            (uint32_t)data[0] + ((uint32_t)data[1] << 8) + ((uint32_t)data[2] << 16) + ((uint32_t)data[3] << 24),
            responseDataOut,
            responseDataOut + 1,
            responseDataSizeOut);
        *responseDataSizeOut += 1;
        break;
    default:
        break;
    }
    return 0;
}
