#include <Arduino.h>
#include "remote-unit-packet.h"
#include "serial-interface.h"
#include "commands.h"

#define PACKET_BUFFER_SIZE 64
#define READ_TIMEOUT 10000

using namespace IrrigationSystem;

void serialInit()
{
    Serial.setTimeout(READ_TIMEOUT);
    Serial.begin(9600, SERIAL_8N1);
}

void handleCommand(RemoteUnitPacket::RemoteUnitCommand command, const uint8_t *data, uint8_t *responseData)
{
    int result = 0;
    switch (command)
    {
    case RemoteUnitPacket::RemoteUnitCommand::GetSolenoidState:
        result = getSolenoidState(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::SetSolenoidState:
        result = setSolenoidState(*data, responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetBatteryVoltage:
        result = getBatteryVoltage(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetBatteryRaw:
        uint16_t batteryRaw;
        result = getBatteryRaw(&batteryRaw);
        responseData[0] = batteryRaw;
        responseData[1] = batteryRaw >> 8;
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetFaults:
        result = getFaults(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::ClearFaults:
        result = clearFaults(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetSignalStrength:
        result = getSignalStrength(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::GetConfig:
        result = getConfig(responseData);
        break;
    case RemoteUnitPacket::RemoteUnitCommand::SetConfig:
        result = setConfig(data, responseData);
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

int handlePacket(uint16_t nodeId, const uint8_t *packet, size_t size)
{
    // Don't do anything if NODE_ID doesn't match
    if (size < 2 || nodeId != RemoteUnitPacket::getNodeId(packet))
    {
        return REMOTE_UNIT_INVALID_NODE_ID;
    }

    uint8_t response[PACKET_BUFFER_SIZE];
    size_t responseSize = RemoteUnitPacket::createPacket(response, PACKET_BUFFER_SIZE, nodeId);
    int result = 0;

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
                return REMOTE_UNIT_INVALID_PACKET_RESPONSE_TOO_LARGE;
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
            result = REMOTE_UNIT_INVALID_PACKET_CRC;
        }
        else
        {
            result = REMOTE_UNIT_INVALID_PACKET_COMMAND;
        }
    }

    responseSize = RemoteUnitPacket::finalisePacket(response, PACKET_BUFFER_SIZE, responseSize, true);
    if (responseSize == 0)
    {
        // Buffer too small
        return REMOTE_UNIT_INVALID_PACKET_RESPONSE_TOO_LARGE;
    }

    // Send the response
    size_t written = Serial.write(response, responseSize);
    if (written != responseSize)
    {
        result = REMOTE_UNIT_WRITE_FAILURE;
    }
    return result;
}

/**
 * Receive a packet over the Serial interface and handle the encoded commands.
 *
 * Returns a value less than 0 on failure.
 */
int receivePacket(uint16_t nodeId)
{
    uint8_t buffer[PACKET_BUFFER_SIZE];
    serialInit();
    size_t read = Serial.readBytes(buffer, PACKET_BUFFER_SIZE);

    if (read == 0)
    {
        return REMOTE_UNIT_NO_DATA;
    }
    int result = handlePacket(nodeId, buffer, read);
    Serial.end();
    return result;
}
