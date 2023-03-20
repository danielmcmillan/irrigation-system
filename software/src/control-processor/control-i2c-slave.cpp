#include "control-i2c-slave.h"
#include <Arduino.h>
#include <control-processor/ArduinoTwoWire/Wire.h>
#include "logging.h"
#include "binary-util.h"

ControlI2cSlave::ControlI2cSlave(const ControllerManager &controllers, const ControlProcessorMessageHandler &handler)
    : packet(controllers), handler(handler)
{
}

ControlI2cSlave ControlI2cSlave::initialise(const ControllerManager &controllers, const ControlProcessorMessageHandler &handler)
{
    static ControlI2cSlave instance(controllers, handler);
    Wire.onRequest([](uint8_t *input, uint8_t inputSize, uint8_t *output, uint8_t *outputSize)
                   { instance.handleRequest(input, inputSize, output, outputSize); });
    return instance;
}

void ControlI2cSlave::setup()
{
    Wire.begin(CONTROL_PROCESSOR_I2C_SLAVE_ADDRESS);
}

void ControlI2cSlave::handleRequest(uint8_t *input, uint8_t inputSize, uint8_t *output, uint8_t *outputSize)
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
        ControlProcessorPacket::MessageType type = packet.getMessageType(input);
        const uint8_t *data;
        size_t dataSize = packet.getMessageData(input, inputSize, &data);
        result = this->handleMessage(type, data, dataSize, responseData, &responseDataSize);
        if (result != 0)
        {
            // Failed to handle is represented by error > 2
            result += 2;
        }
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
int ControlI2cSlave::handleMessage(ControlProcessorPacket::MessageType type, const uint8_t *data, size_t dataSize, uint8_t *responseDataOut, size_t *responseDataSizeOut)
{
    switch (type)
    {
    case ControlProcessorPacket::MessageType::ConfigStart:
        *responseDataSizeOut = 0;
        return this->handler.configStart();
    case ControlProcessorPacket::MessageType::ConfigEnd:
        *responseDataSizeOut = 0;
        return this->handler.configEnd();
    case ControlProcessorPacket::MessageType::ConfigAdd:
        *responseDataSizeOut = 0;
        return this->handler.configAdd(data[0], data[1], &data[2]);
    case ControlProcessorPacket::MessageType::PropertyRead:
        return this->handler.propertyRead(data[0], read16LE(&data[1]), responseDataOut, responseDataSizeOut);
    case ControlProcessorPacket::MessageType::PropertySet:
        *responseDataSizeOut = 0;
        return this->handler.propertyWrite(data[0], read16LE(&data[1]), data + 3);
    case ControlProcessorPacket::MessageType::EventGetNext:
        this->handler.eventGetNext(
            read16LE(data),
            responseDataOut,
            responseDataOut + 1,
            responseDataSizeOut);
        *responseDataSizeOut += 1;
        break;
    case ControlProcessorPacket::MessageType::RunControllerCommand:
        *responseDataSizeOut = 0;
        return this->handler.runControllerCommand(data[0], &data[1], dataSize - 1);
        break;
    case ControlProcessorPacket::MessageType::GetControllerCommandResult:
        return this->handler.getControllerCommandResult(responseDataOut, responseDataSizeOut);
        break;
    default:
        break;
    }
    return 0;
}
