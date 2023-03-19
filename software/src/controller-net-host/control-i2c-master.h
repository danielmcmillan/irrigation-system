#ifndef _CONTROLLER_NET_HOST_I2C_INTERFACE
#define _CONTROLLER_NET_HOST_I2C_INTERFACE
#include "controller-definition-provider.h"
#include "control-processor-packet.h"
#include "error-handler.h"

using namespace IrrigationSystem;

#define CONTROLLER_NET_HOST_I2C_SLAVE_ADDRESS 10
#define CONTROLLER_NET_HOST_I2C_PACKET_BUFFER_SIZE 32

enum class MessageResultType : uint8_t
{
    Success,
    SendFailed,
    ResponseInvalid,
    ResponseError
};

class ControlI2cMaster
{
public:
    ControlI2cMaster(const ControllerDefinitionProvider &controllers, const ErrorHandler &errorHandler);
    void setup();

    bool getNextEvent(uint16_t lastEvent, uint8_t *eventOut, size_t *eventSizeOut) const;
    bool configStart() const;
    bool configAdd(const uint8_t *data, size_t length) const;
    bool configEnd() const;

private:
    const ErrorHandler &errorHandler;
    bool sendMessage(ControlProcessorPacket::MessageType type, const uint8_t *data, size_t dataSize, const uint8_t **responseOut, size_t *responseSizeOut) const;
    ControlProcessorPacket packet;
    mutable uint8_t packetBuffer[CONTROLLER_NET_HOST_I2C_PACKET_BUFFER_SIZE];

    /**
     * @param reason
     *   SendFailed: see Wire.endTransmission
     *   ResponseInvalid: 1=Timeout, 2=InvalidResponseLength, 3=InvalidResponseType, 4=InvalidDataSize, 5=CrcError
     *   ResponseError: control processor error response
     */
    void handleError(ControlProcessorPacket::MessageType messageType, MessageResultType resultType, uint8_t reason, const char *text) const;
};

#endif
