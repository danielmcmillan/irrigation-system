#include <iostream>
#include <vector>
#include <string>
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
    std::cerr << "usage: " << argv0 << " remote-command-raw --device <device> --node <nodeId> --command <command>[:<data>]\n";
    exit(EXIT_FAILURE);
}

void remoteCommandRaw(int argc, char **argv)
{
    struct option options[] = {{"device", 1, nullptr, 'd'},
                               {"node", 1, nullptr, 'n'},
                               {"command", 1, nullptr, 'c'},
                               {"no-node-prefix", 1, nullptr, 'x'},
                               {0, 0, 0, 0}};

    // Get the arguments
    const char *deviceStr = nullptr;
    const char *nodeStr = nullptr;
    std::vector<std::string> commandStrings;
    bool noNodePrefix = false;
    while (true)
    {
        int option = getopt_long(argc, argv, "d:n:c:x", options, 0);
        if (option == -1)
        {
            break;
        }
        switch (option)
        {
        case 'd':
            deviceStr = optarg;
            break;
        case 'n':
            nodeStr = optarg;
            break;
        case 'c':
            commandStrings.push_back(optarg);
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
    if (commandStrings.empty())
    {
        required(argv[0], "command");
        remoteCommandRawUsage(argv[0]);
    }

    unsigned nodeId = std::stoul(nodeStr, nullptr, 0);
    if (nodeId > 0xffff)
    {
        invalid(argv[0], "node", nodeStr);
    }

    // Debug output
    std::cout << "Node: 0x" << std::hex << std::setfill('0') << std::setw(4) << nodeId << "\n";

    // Initialise the packet
    uint8_t buffer[PACKET_BUFFER_SIZE + 2];
    uint8_t *packetBuffer = buffer + 2;
    size_t packetSize = IrrigationSystem::RemoteUnitPacket::createPacket(packetBuffer, PACKET_BUFFER_SIZE, nodeId);

    for (std::string commandString : commandStrings)
    {
        // Parse and validate the command argument
        std::string commandPart = commandString;
        std::string dataPart;
        size_t separatorIndex = commandString.find(':');
        if (separatorIndex != std::string::npos)
        {
            commandPart = commandString.substr(0, separatorIndex);
            dataPart = commandString.substr(separatorIndex + 1);
        }

        unsigned commandId = std::stoul(commandPart, nullptr, 0);
        IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand command =
            static_cast<IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand>(commandId);
        uint8_t data[16] = {0};

        unsigned dataSize = IrrigationSystem::RemoteUnitPacket::getCommandDataSize(command);

        if (dataSize == IrrigationSystem::RemoteUnitPacket::commandDataSizeInvalid)
        {
            invalid(argv[0], "command", commandString.c_str());
        }
        if (dataSize > 0)
        {
            if (dataPart.empty())
            {
                invalid(argv[0], "command", commandString.c_str());
                remoteCommandRawUsage(argv[0]);
            }
            if (dataPart.substr(0, 2).compare("0x") != 0 || dataPart.size() != 2 + 2 * dataSize)
            {
                invalid(argv[0], "data", commandString.c_str());
            }
            for (unsigned i = 0; i < dataSize; ++i)
            {
                data[i] = std::stoul(dataPart.substr(2 + 2 * i, 2), nullptr, 16);
            }
        }

        // Debug output
        std::cout << "Command: 0x" << std::hex << std::setfill('0') << std::setw(2) << commandId;
        std::cout << std::dec << ", dataSize: " << dataSize;
        if (dataSize > 0)
        {
            std::cout << ", data: 0x";
            for (unsigned i = 0; i < dataSize; ++i)
            {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(data[i]);
            }
        }
        std::cout << "\n";

        // Add command to packet
        uint8_t *commandData;
        packetSize = IrrigationSystem::RemoteUnitPacket::addCommandToPacket(packetBuffer, PACKET_BUFFER_SIZE, packetSize, command, &commandData);
        memcpy(commandData, data, dataSize);
    }

    packetSize = IrrigationSystem::RemoteUnitPacket::finalisePacket(packetBuffer, PACKET_BUFFER_SIZE, packetSize, false);

    if (!noNodePrefix)
    {
        // Add big-endian node ID to initial bytes for LoRa module
        buffer[0] = nodeId >> 8;
        buffer[1] = nodeId;
        packetBuffer = buffer;
        packetSize += 2;
    }

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
    ssize_t result = serialWrite(serialPort, packetBuffer, packetSize);
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
    uint8_t *receivedPacket;
    packetSize = IrrigationSystem::RemoteUnitPacket::getPacket(packetBuffer, result, &receivedPacket);

    // Debug output
    std::cout << "Received packet: 0x";
    for (unsigned i = 0; i < result; ++i)
    {
        std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(receivedPacket[i]);
    }
    std::cout << "\n";

    // Parse response
    int numCommands = IrrigationSystem::RemoteUnitPacket::validatePacket(receivedPacket, packetSize, true);
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
    std::cout << std::hex << std::setfill('0') << std::setw(4) << IrrigationSystem::RemoteUnitPacket::getNodeId(receivedPacket) << "\n";
    for (int i = 0; i < numCommands; ++i)
    {
        const uint8_t *responseData;
        IrrigationSystem::RemoteUnitPacket::RemoteUnitCommand responseCommand =
            IrrigationSystem::RemoteUnitPacket::getCommandAtIndex(receivedPacket, i, &responseData);
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
        if (responseDataSize == 1)
        {
            std::cout << " (" << std::dec << static_cast<unsigned>(*responseData) << ")";
        }
        if (responseDataSize == 2)
        {
            std::cout << " (" << std::dec << (static_cast<unsigned>(responseData[1]) << 8 | responseData[0]) << ")";
        }
        if (responseDataSize == 4)
        {
            std::cout << " (" << std::dec << (static_cast<unsigned>(responseData[3] << 24) | (responseData[2] << 16) | (responseData[1] << 8) | responseData[0]) << ")";
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
