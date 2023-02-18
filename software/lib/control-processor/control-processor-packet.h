#ifndef _CONTROL_PROCESSOR_PACKET_H
#define _CONTROL_PROCESSOR_PACKET_H
#include <stddef.h>
#include <inttypes.h>
#include "controller-definition-provider.h"

namespace IrrigationSystem
{
    class ControlProcessorPacket
    {
        const ControllerDefinitionProvider &controllerDefinitions;

    public:
        ControlProcessorPacket(const ControllerDefinitionProvider &controllerDefinitions);

        /** Identifier of each message type. */
        enum class MessageType : uint8_t
        {
            // Configuration
            ConfigStart = 0x10,
            ConfigAdd = 0x11,
            ConfigEnd = 0x12,

            // Properties
            PropertyRead = 0x20,
            PropertySet = 0x21,

            // Events
            EventGetNext = 0x30,

            // Response
            Ack = 0x61,
            Err = 0xe3
        };

        /**
         * Validate a control processor message data packet.
         * Checks that property values are of the correct size, but does no check the value is valid.
         *
         * On success, returns 0.
         * Returns 0x01 if the packet includes invalid message type or data.
         * Returns 0x02 if the packet fails CRC.
         */
        uint8_t validatePacket(const uint8_t *packet, size_t packetSize);

        /**
         * Get the message type from a packet.
         * If `dataOut` is not null, then it will be set to point to the data portion of the message.
         */
        MessageType getMessageType(const uint8_t *packet, const uint8_t **dataOut = nullptr);

        /**
         * Initialises a new data packet.
         * `messageTypePtr` will be set to point to the type of the message.
         * If `dataPtr` is not null, then it will be set to point to the data portion of the message.
         */
        void createPacket(uint8_t *packetBuffer, MessageType **messageTypePtr, uint8_t **dataPtr = nullptr);

        /**
         * Finalises a packet after data has been written.
         *
         * Returns the total packet size.
         */
        size_t finalisePacket(uint8_t *packetBuffer, size_t dataSize);

    private:
        uint8_t validateData(MessageType messageType, const uint8_t *data, size_t dataSize);
    };
}
#endif
