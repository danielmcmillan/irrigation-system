#include <string.h>
#include "remote-unit-packet.h"
#include "crc16.h"

#define PACKET_HEADER_SIZE 2
#define PACKET_FOOTER_SIZE 3
#define END_COMMAND_BYTE 0x03
#define END_RESPONSE_BYTE responseFlag | 0x03

uint16_t read16LE(const uint8_t *data)
{
    return (uint16_t)data[0] | (uint16_t)data[1] << 8;
}

void write16LE(uint8_t *data, uint16_t value)
{
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
}

namespace IrrigationSystem
{
    namespace RemoteUnitPacket
    {
        const uint8_t commandDataSizeInvalid = 0xff;

        int validatePacket(const uint8_t *packet, size_t packetSize, bool isResponse)
        {
            if (CRC::crc16(packet, packetSize))
            {
                return -2;
            }
            size_t packetOffset = 2;
            int commandCount = 0;
            // Check each command
            while (packetOffset < packetSize - PACKET_FOOTER_SIZE)
            {
                uint8_t commandByte = packet[packetOffset];
                uint8_t dataSize = getCommandDataSize((RemoteUnitCommand)commandByte);
                // Check command is valid and matches isResponse
                if (dataSize == 0xff || commandIsResponse(commandByte) != isResponse)
                {
                    return -1;
                }
                packetOffset += 1 + dataSize;
                ++commandCount;
            }
            // Check footer size
            if (packetOffset + PACKET_FOOTER_SIZE != packetSize)
            {
                return -1;
            }
            // Check end command
            if (packet[packetOffset] != (isResponse ? END_RESPONSE_BYTE : END_COMMAND_BYTE))
            {
                return -1;
            }
            return commandCount;
        }

        uint8_t getCommandDataSize(RemoteUnitCommand command)
        {
            switch (command)
            {
            case RemoteUnitCommand::GetSolenoidState:
                return 0;
            case RemoteUnitCommand::GetSolenoidStateResponse:
                return 1;
            case RemoteUnitCommand::SetSolenoidState:
                return 1;
            case RemoteUnitCommand::SetSolenoidStateResponse:
                return 1;
            case RemoteUnitCommand::GetBatteryVoltage:
                return 0;
            case RemoteUnitCommand::GetBatteryVoltageResponse:
                return 1;
            case RemoteUnitCommand::GetFaults:
                return 0;
            case RemoteUnitCommand::GetFaultsResponse:
                return 1;
            case RemoteUnitCommand::ClearFaults:
                return 0;
            case RemoteUnitCommand::ClearFaultsResponse:
                return 1;
            case RemoteUnitCommand::GetBatteryRaw:
                return 0;
            case RemoteUnitCommand::GetBatteryRawResponse:
                return 2;
            case RemoteUnitCommand::GetSignalStrength:
                return 0;
            case RemoteUnitCommand::GetSignalStrengthResponse:
                return 1;
            case RemoteUnitCommand::GetConfig:
                return 0;
            case RemoteUnitCommand::GetConfigResponse:
                return 13;
            case RemoteUnitCommand::SetConfig:
                return 13;
            case RemoteUnitCommand::SetConfigResponse:
                return 13;
            case RemoteUnitCommand::ErrorResponse:
                return 0;
            default:
                return 0xff;
            }
        }

        bool commandIsResponse(uint8_t commandByte)
        {
            return (commandByte & responseFlag) != 0;
        }

        RemoteUnitCommand getResponseForCommand(RemoteUnitCommand command)
        {
            return (RemoteUnitCommand)((uint8_t)command | responseFlag);
        }

        uint16_t getNodeId(const uint8_t *packet)
        {
            return read16LE(packet);
        }

        RemoteUnitCommand getCommandAtIndex(const uint8_t *packet, size_t commandIndex, const uint8_t **dataOut)
        {
            size_t packetOffset = 2;
            // Find the command
            while (true)
            {
                RemoteUnitCommand command = (RemoteUnitCommand)packet[packetOffset];
                if (commandIndex == 0)
                {
                    *dataOut = packet + packetOffset + 1;
                    return command;
                }
                packetOffset += 1 + getCommandDataSize(command);
                --commandIndex;
            }
        }

        size_t createPacket(uint8_t *packetBuffer, size_t bufferSize, uint16_t nodeId)
        {
            if (bufferSize < PACKET_HEADER_SIZE)
            {
                return 0;
            }
            // Node id
            write16LE(packetBuffer, nodeId);
            return PACKET_HEADER_SIZE;
        }

        size_t addCommandToPacket(uint8_t *packetBuffer, size_t bufferSize, size_t intermediatePacketSize, RemoteUnitCommand command, uint8_t **dataPtr)
        {
            uint8_t dataSize = getCommandDataSize(command);
            size_t newSize = intermediatePacketSize + 1 + dataSize;
            if (dataSize == 0xff || newSize > bufferSize)
            {
                return 0;
            }
            // Command
            packetBuffer[intermediatePacketSize] = (uint8_t)command;
            // Pointer to command data
            if (dataPtr != nullptr)
            {
                *dataPtr = packetBuffer + intermediatePacketSize + 1;
            }
            return newSize;
        }

        size_t finalisePacket(uint8_t *packetBuffer, size_t bufferSize, size_t intermediatePacketSize, bool isResponse)
        {
            size_t newSize = intermediatePacketSize + PACKET_FOOTER_SIZE;
            if (newSize > bufferSize)
            {
                return 0;
            }
            // End command
            packetBuffer[intermediatePacketSize] = isResponse ? END_RESPONSE_BYTE : END_COMMAND_BYTE;
            // CRC
            uint16_t crc = CRC::crc16(packetBuffer, intermediatePacketSize + 1);
            write16LE(packetBuffer + intermediatePacketSize + 1, crc);
            return newSize;
        }
    }
}
