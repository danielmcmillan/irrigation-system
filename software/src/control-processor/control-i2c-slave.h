#ifndef _CONTROL_PROCESSOR_I2C_INTERFACE
#define _CONTROL_PROCESSOR_I2C_INTERFACE
#include "controller-manager.h"
#include "control-processor-packet.h"
#include "message-handler.h"

using namespace IrrigationSystem;

#define CONTROL_PROCESSOR_I2C_SLAVE_ADDRESS 10

class ControlI2cSlave
{
    ControlProcessorPacket packet;
    const ControlProcessorMessageHandler &handler;

public:
    static ControlI2cSlave initialise(const ControllerManager &controllers, const ControlProcessorMessageHandler &handler);
    void setup();

private:
    ControlI2cSlave(const ControllerManager &controllers, const ControlProcessorMessageHandler &handler);
    void handleRequest(uint8_t *input, uint8_t inputSize, uint8_t *output, uint8_t *outputSize);
    int handleMessage(ControlProcessorPacket::MessageType type, const uint8_t *data, size_t dataSize, uint8_t *responseData, size_t *responseDataSize);
};

#endif
