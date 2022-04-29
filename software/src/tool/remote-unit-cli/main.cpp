#include <iostream>
#include <getopt.h>
#include <string.h>
#include <iomanip>
#include "remote-unit-packet.h"
#include "serial.h"

#define PACKET_BUFFER_SIZE 64

// TODO:
// serial header file
// support multiple commands
// handle packet creation error

void commands()
{
    std::cerr << "Available commands: remote-command-raw\n";
}

void usage(const char *argv0)
{
    std::cerr << "usage: " << argv0 << " <command> [parameters]\n";
    exit(EXIT_FAILURE);
}

void required(const char *argv0, const char *param)
{
    std::cerr << argv0 << ": option '" << param << "' is required\n";
}

void invalid(const char *argv0, const char *param, const char *value)
{
    std::cerr << argv0 << ": value " << value << " is not valid for option '" << param << "'\n";
    exit(EXIT_FAILURE);
}

void remoteCommandRawUsage(const char *argv0)
{
    std::cerr << "usage: " << argv0 << " remote-command-raw --device <device> --node <nodeId> --command <command> [--data <data>]\n";
    exit(EXIT_FAILURE);
}

void remoteCommandRaw(int argc, char **argv)
{
    struct option options[] = {{"device", 1, nullptr, 't'},
                               {"node", 1, nullptr, 'n'},
                               {"command", 1, nullptr, 'c'},
                               {"data", 1, nullptr, 'b'},
                               {"no-node-prefix", 1, nullptr, 'x'},
                               {0, 0, 0, 0}};

    // Get the arguments
    const char *deviceStr = nullptr;
    const char *nodeStr = nullptr;
    const char *commandStr = nullptr;
    const char *dataStr = nullptr;
    bool noNodePrefix = false;
    while (true)
    {
        int option = getopt_long(argc, argv, "t:n:c:d:x", options, 0);
        if (option == -1)
        {
            break;
        }
        switch (option)
        {
        case 't':
            deviceStr = optarg;
            break;
        case 'n':
            nodeStr = optarg;
            break;
        case 'c':
            commandStr = optarg;
            break;
        case 'b':
            dataStr = optarg;
            break;
        case 'x':
            noNodePrefix = true;
            break;
        default /* '?' */:
            remoteCommandRawUsage(argv[0]);
        }
    }
    if (!deviceStr)
    {
        required(argv[0], "device");
        remoteCommandRawUsage(argv[0]);
    }
    if (!nodeStr)
    {
        required(argv[0], "node");
        remoteCommandRawUsage(argv[0]);
    }
    if (!commandStr)
    {
        required(argv[0], "command");
        remoteCommandRawUsage(argv[0]);
    }

    // Parse and validate the arguments
    unsigned nodeId = std::stoul(nodeStr, nullptr, 0);
    unsigned commandId = std::stoul(commandStr, nullptr, 0);
    IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand command =
        static_cast<IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand>(commandId);
    uint8_t data[16] = {0};

    unsigned dataSize = IrrigationSystem::RemoteUnitPacket::getCommandDataSize(command);

    if (nodeId > 0xffff)
    {
        invalid(argv[0], "node", nodeStr);
    }
    if (dataSize == IrrigationSystem::RemoteUnitPacket::commandDataSizeInvalid)
    {
        invalid(argv[0], "command", commandStr);
    }
    if (dataSize > 0)
    {
        if (!dataStr)
        {
            required(argv[0], "data");
            remoteCommandRawUsage(argv[0]);
        }
        if (strncasecmp(dataStr, "0x", 2) != 0 || strlen(dataStr) != 2 + 2 * dataSize)
        {
            invalid(argv[0], "data", dataStr);
        }
        for (unsigned i = 0; i < dataSize; ++i)
        {
            data[i] = std::stoul(std::string(dataStr, 2 + 2 * i, 2), nullptr, 16);
        }
    }

    // Debug output
    std::cout << "Command: 0x" << std::hex << std::setfill('0') << std::setw(2) << commandId;
    std::cout << ", node: 0x" << std::hex << std::setfill('0') << std::setw(4) << nodeId;
    std::cout << std::dec << ", dataSize: " << dataSize;
    std::cout << ", data: 0x";
    for (unsigned i = 0; i < dataSize; ++i)
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(data[i]);
    }
    std::cout << "\n";

    // Build the packet
    uint8_t buffer[PACKET_BUFFER_SIZE + 2];
    uint8_t *packetBuffer = buffer + 2;

    if (noNodePrefix)
    {
        packetBuffer = buffer;
    }
    else
    {
        // Add node ID to initial bytes for LoRa module
        buffer[0] = nodeId;
        buffer[1] = nodeId >> 8;
    }

    uint8_t *commandData;
    size_t packetSize = IrrigationSystem::RemoteUnitPacket::createPacket(packetBuffer, PACKET_BUFFER_SIZE, nodeId);
    packetSize = IrrigationSystem::RemoteUnitPacket::addCommandToPacket(packetBuffer, PACKET_BUFFER_SIZE, packetSize, command, &commandData);
    memcpy(commandData, data, dataSize);
    packetSize = IrrigationSystem::RemoteUnitPacket::finalisePacket(packetBuffer, PACKET_BUFFER_SIZE, packetSize, false);

    // Debug output
    std::cout << "Packet: 0x";
    for (unsigned i = 0; i < packetSize; ++i)
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(packetBuffer[i]);
    }
    std::cout << "\n";

    // Send the packet
    int serialPort = serialOpen(deviceStr);
    serialFlush(serialPort);
    if (serialPort == 0)
    {
        std::cerr << "Failed to open serial device '" << deviceStr << "'\n";
        exit(EXIT_FAILURE);
    }
    ssize_t result = serialWrite(serialPort, buffer, packetSize);
    if (result != packetSize)
    {
        std::cerr << "Failed to write to serial device '" << deviceStr << "'\n";
        exit(EXIT_FAILURE);
    }

    // Receive response
    result = serialRead(serialPort, packetBuffer, PACKET_BUFFER_SIZE);
    if (result == -1)
    {
        std::cerr << "Failed to read from serial device '" << deviceStr << "'\n";
        exit(EXIT_FAILURE);
    }
    else if (result == 0)
    {
        std::cerr << "Timed out waiting for response on serial device '" << deviceStr << "'\n";
        exit(EXIT_FAILURE);
    }

    // Debug output
    std::cout << "Received packet: 0x";
    for (unsigned i = 0; i < result; ++i)
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(packetBuffer[i]);
    }
    std::cout << "\n";

    // Parse response
    int numCommands = IrrigationSystem::RemoteUnitPacket::validatePacket(packetBuffer, result, true);
    if (numCommands < 0)
    {
        if (numCommands == -2)
        {
            std::cerr << "Response has invalid CRC\n";
        }
        else
        {
            std::cerr << "Response includes invalid commands or data\n";
        }
        exit(EXIT_FAILURE);
    }
    std::cout << "Received response from node 0x";
    std::cout << std::hex << std::setfill('0') << std::setw(4) << IrrigationSystem::RemoteUnitPacket::getNodeId(packetBuffer) << "\n";
    for (int i = 0; i < numCommands; ++i)
    {
        const uint8_t *responseData;
        IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand responseCommand =
            IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(packetBuffer, i, &responseData);
        unsigned responseDataSize = IrrigationSystem::RemoteUnitPacket::getCommandDataSize(responseCommand);
        std::cout << "Command: 0x" << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(responseCommand);
        std::cout << std::dec << ", dataSize: " << responseDataSize;
        if (responseDataSize > 0)
        {
            std::cout << ", data: 0x";
            for (unsigned i = 0; i < responseDataSize; ++i)
            {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(responseData[i]);
            }
        }
        std::cout << "\n";
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        commands();
        usage(argv[0]);
    }
    optind = 2;

    if (strcasecmp(argv[1], "remote-command-raw") == 0)
    {
        remoteCommandRaw(argc, argv);
    }
    else
    {
        std::cerr << argv[0] << ": invalid command '" << argv[1] << "'\n";
        commands();
        usage(argv[0]);
    }
}
