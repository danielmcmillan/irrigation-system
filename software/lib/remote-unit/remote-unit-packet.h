#ifndef _REMOTE_UNIT_PACKET_H
#define _REMOTE_UNIT_PACKET_H
#include <stddef.h>
#include <inttypes.h>

namespace IrrigationSystem
{
    namespace RemoteUnitPacket
    {
        extern const uint8_t commandDataSizeInvalid;
        const uint8_t responseFlag = 0x80;

        /** Identifier of each command. */
        enum class RemoteUnitCommand : uint8_t
        {
            Invalid = 0x00,
            // Read operations
            GetSolenoidState = 0x10,
            GetBatteryVoltage = 0x11,
            GetFaults = 0x12,
            GetBatteryRaw = 0x13,
            GetSignalStrength = 0x14,
            GetConfig = 0x15,
            // Write operations
            SetSolenoidState = 0x20,
            ClearFaults = 0x22,
            SetConfig = 0x25,
            // Other operations
            PersistConfig = 0x30,
            ApplyRfConfig = 0x31,
            // Corresponding responses
            GetSolenoidStateResponse = responseFlag | 0x10,
            GetBatteryVoltageResponse = responseFlag | 0x11,
            GetFaultsResponse = responseFlag | 0x12,
            GetBatteryRawResponse = responseFlag | 0x13,
            GetSignalStrengthResponse = responseFlag | 0x14,
            GetConfigResponse = responseFlag | 0x15,
            SetSolenoidStateResponse = responseFlag | 0x20,
            ClearFaultsResponse = responseFlag | 0x22,
            SetConfigResponse = responseFlag | 0x25,
            PersistConfigResponse = responseFlag | 0x30,
            ApplyRfConfigResponse = responseFlag | 0x31,
            ErrorResponse = responseFlag

        };

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

        /**
         * Get the response command for the specified command.
         */
        RemoteUnitCommand getResponseForCommand(RemoteUnitCommand command);

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
         * If dataPtr is is not null, then it will be set to point to the data portion for the command in the packet buffer.
         *
         * Returns the intermediate packet size.
         * Returns 0 if the buffer is too small.
         */
        size_t addCommandToPacket(uint8_t *packetBuffer, size_t bufferSize, size_t intermediatePacketSize, RemoteUnitCommand command, uint8_t **dataPtr);

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
#endif
