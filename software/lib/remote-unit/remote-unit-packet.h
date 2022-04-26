#include <stddef.h>
#include <inttypes.h>

namespace IrrigationSystem
{
    namespace RemoteUnitPacket
    {
        extern const uint8_t commandDataSizeInvalid;

        /** Identifier of each command. */
        enum class RemoteUnitCommand : uint8_t;

        /**
         * Validate a remote unit command or response data packet.
         *
         * On success, returns a value >= 0 indicating the number of commands included in the packet.
         * Returns -1 if the packet includes invalid command or data.
         * Return -2 if the packet fails CRC.
         */
        int validatePacket(const uint8_t *packet, size_t packetSize, bool isResponse);

        /**
         * Get the size of the data portion of the given command.
         * Returns 0xff if the given command is invalid.
         */
        uint8_t getCommandDataSize(RemoteUnitCommand command);

        /** Check whether the given packet is a response */
        bool commandIsResponse(uint8_t commandByte);

        /** Get the node id of the given packet */
        uint16_t getNodeId(const uint8_t *packet);

        /**
         * Get the command at a certain index within the given packet.
         * Behaviour is undefined when the index is invalid.
         * `dataOut` will be set to point to the data portion of the command within the packet.
         */
        RemoteUnitCommand getCommandAtIndex(const uint8_t *packet, size_t commandIndex, const uint8_t **dataOut);

        /**
         * Initialises a new packet for communication with a given node id.
         *
         * Returns the intermediate packet size.
         * Returns 0 if the buffer is too small.
         */
        size_t createPacket(uint8_t *packetBuffer, size_t bufferSize, uint16_t nodeId);

        /**
         * Adds a command to the given packet.
         * The packet in the buffer should have been initialised with `createPacket` and not yet finalised.
         *
         * Returns the intermediate packet size.
         * Returns 0 if the buffer is too small.
         */
        size_t addCommandToPacket(uint8_t *packetBuffer, size_t bufferSize, size_t intermediatePacketSize, RemoteUnitCommand command, const uint8_t *data);

        // Add end part of packet. Return packet size. Return 0 if invalid.
        /**
         * Finalises a packet with the end byte and CRC.
         *
         * Returns the final packet size.
         * Returns 0 if the buffer is too small.
         */
        size_t finalisePacket(uint8_t *packetBuffer, size_t bufferSize, size_t intermediatePacketSize, bool isResponse);
    }
}
