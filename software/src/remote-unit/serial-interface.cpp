#include <Arduino.h>
#include "serial-interface.h"

#define PACKET_BUFFER_SIZE 64
#define READ_TIMEOUT 10000

void RemoteUnitSerialInterface::handleCommand(RemoteUnitPacket::RemoteUnitCommand command, const uint8_t *data, uint8_t *responseData) const
{
    int result = 0;
    switch (command)
    {
    case RemoteUnitPacket::RemoteUnitCommand::GetSolenoidState:
        result = this->commands.getSolenoidState(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::SetSolenoidState:
        result = this->commands.setSolenoidState(*data, responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetBatteryVoltage:
        result = this->commands.getBatteryVoltage(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetBatteryRaw:
        uint16_t batteryRaw;
        result = this->commands.getBatteryRaw(&batteryRaw);
        responseData[0] = batteryRaw;
        responseData[1] = batteryRaw >> 8;
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetFaults:
        result = this->commands.getFaults(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::ClearFaults:
        result = this->commands.clearFaults(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetSignalStrength:
        result = this->commands.getSignalStrength(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetConfig:
        result = this->commands.getConfig(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::SetConfig:
        result = this->commands.setConfig(data, responseData);
        break;
    default:
        result = -1;
        break;
    }
    if (result != 0)
    {
        // When an error occurs, respond with all bits set in the data
        memset(responseData, 0xff, RemoteUnitPacket::getCommandDataSize(command));
    }
}

RemoteUnitSerialInterface::Result RemoteUnitSerialInterface::handlePacket(const uint8_t *packet, size_t size) const
{
    // Don't do anything if NODE_ID doesn't match
    if (size < 2 || this->nodeId != RemoteUnitPacket::getNodeId(packet))
    {
        return Result::invalidNodeId;
    }

    uint8_t response[PACKET_BUFFER_SIZE];
    size_t responseSize = RemoteUnitPacket::createPacket(response, PACKET_BUFFER_SIZE, this->nodeId);
    Result result = Result::success;

    int commandCount = RemoteUnitPacket::validatePacket(packet, size, false);
    if (commandCount > 0)
    {
        // Handle each command in the packet
        const uint8_t *commandData = nullptr;
        uint8_t *responseData = nullptr;
        for (int i = 0; i < commandCount; ++i)
        {
            RemoteUnitPacket::RemoteUnitCommand command = RemoteUnitPacket::getCommandAtIndex(packet, i, &commandData);
            RemoteUnitPacket::RemoteUnitCommand responseCommand = RemoteUnitPacket::getResponseForCommand(command);
            responseSize = RemoteUnitPacket::addCommandToPacket(response, PACKET_BUFFER_SIZE, responseSize, responseCommand, &responseData);
            if (responseSize == 0)
            {
                // Buffer too small
                return Result::invalidPacketResponseTooLarge;
            }

            handleCommand(command, commandData, responseData);
        }
    }
    else
    {
        // Respond with error
        responseSize = RemoteUnitPacket::addCommandToPacket(response, PACKET_BUFFER_SIZE, responseSize, RemoteUnitPacket::RemoteUnitCommand::ErrorResponse, nullptr);
        if (commandCount == -2)
        {
            result = Result::invalidPacketCrc;
        }
        else
        {
            result = Result::invalidPacketCommand;
        }
    }

    responseSize = RemoteUnitPacket::finalisePacket(response, PACKET_BUFFER_SIZE, responseSize, true);
    if (responseSize == 0)
    {
        // Buffer too small
        return Result::invalidPacketResponseTooLarge;
    }

    // Send the response
    size_t written = Serial.write(response, responseSize);
    if (written != responseSize)
    {
        result = Result::writeFailure;
    }
    return result;
}

RemoteUnitSerialInterface::RemoteUnitSerialInterface(uint16_t nodeId, const RemoteUnitCommandHandler &commands)
    : nodeId(nodeId), commands(commands)
{
}

/**
 * Receive a packet over the Serial interface and handle the encoded commands.
 *
 * Returns a value less than 0 on failure.
 */
RemoteUnitSerialInterface::Result RemoteUnitSerialInterface::receivePacket() const
{
    Serial.begin(9600, SERIAL_8N1);
    uint8_t buffer[PACKET_BUFFER_SIZE];

    // All of the data should arrive at once, so apply timeout only to first byte
    Serial.setTimeout(READ_TIMEOUT);
    size_t read = Serial.readBytes(buffer, 1);
    if (read == 0)
    {
        return Result::noData;
    }
    Serial.setTimeout(100);
    read = Serial.readBytes(buffer + 1, PACKET_BUFFER_SIZE - 1);

    Result result = this->handlePacket(buffer, read + 1);
    Serial.end();
    return result;
}
