#include "control-processor-packet.h"
#include "crc16.h"
#include "binary-util.h"

namespace IrrigationSystem
{
    ControlProcessorPacket::ControlProcessorPacket(const ControllerDefinitionProvider &controllerDefinitions)
        : controllerDefinitions(controllerDefinitions)
    {
    }

    uint8_t ControlProcessorPacket::validatePacket(const uint8_t *packet, size_t packetSize) const
    {
        // Check that the packet includes a valid CRC
        if (packetSize < 3 || CRC::crc16(packet, packetSize) != 0)
        {
            return 2;
        }
        // Check that data is valid, based on message type
        const uint8_t *data;
        MessageType type = this->getMessageType(packet);
        size_t dataSize = this->getMessageData(packet, packetSize, &data);
        return this->validateData(type, data, dataSize);
    }

    ControlProcessorPacket::MessageType ControlProcessorPacket::getMessageType(const uint8_t *packet) const
    {
        return (ControlProcessorPacket::MessageType)(packet[0]);
    }

    size_t ControlProcessorPacket::getMessageData(const uint8_t *packet, size_t packetSize, const uint8_t **dataOut) const
    {
        *dataOut = &packet[1];
        return packetSize - 3; // Minus message type and CRC
    }

    void ControlProcessorPacket::createPacket(uint8_t *packetBuffer, MessageType **messageTypePtr, uint8_t **dataPtr) const
    {
        // Message type
        *messageTypePtr = (MessageType *)packetBuffer;
        // Pointer to data
        if (dataPtr != nullptr)
        {
            *dataPtr = packetBuffer + 1;
        }
    }

    size_t ControlProcessorPacket::finalisePacket(uint8_t *packetBuffer, size_t dataSize) const
    {
        // CRC
        uint16_t crc = CRC::crc16(packetBuffer, 1 + dataSize);
        write16LE(packetBuffer + 1 + dataSize, crc);

        return 1 + dataSize + 2;
    }

    uint8_t ControlProcessorPacket::validateData(MessageType messageType, const uint8_t *data, size_t dataSize) const
    {
        if (messageType == MessageType::PropertyRead || messageType == MessageType::PropertySet)
        {
            bool write = messageType == MessageType::PropertySet;
            if (dataSize < 3 || (!write && dataSize != 3))
            {
                // No controller and property id
                return 1;
            }
            uint8_t controllerId = data[0];
            uint16_t propertyId = read16LE(data + 1);
            const ControllerDefinition *definition = this->controllerDefinitions.getControllerDefinition(controllerId);
            if (definition == nullptr)
            {
                // Unknown controller
                return 1;
            }
            size_t valueSize = definition->getPropertyLength(propertyId);
            if (valueSize == 0)
            {
                // Unknown property
                return 1;
            }
            bool readOnly = definition->getPropertyReadOnly(propertyId);
            if (write)
            {
                if (readOnly)
                {
                    // Writing read-only property
                    return 1;
                }
                if (dataSize != 3 + valueSize)
                {
                    // Incorrect size of value
                    return 1;
                }
            }
            return 0;
        }
        if (messageType == MessageType::ConfigAdd)
        {
            if (dataSize < 2)
            {
                // No controller and config type
                return 1;
            }
            uint8_t controllerId = data[0];
            uint8_t configType = data[1];
            const ControllerDefinition *definition = this->controllerDefinitions.getControllerDefinition(controllerId);
            if (definition == nullptr)
            {
                // Unknown controller
                return 1;
            }
            size_t configLength = definition->getConfigLength(configType);
            if (configLength == 0)
            {
                // Unknown config type
                return 1;
            }
            if (dataSize != 2 + configLength)
            {
                // Incorrect size of config value
                return 1;
            }
            return 0;
        }
        switch (messageType)
        {
        case MessageType::ConfigStart:
        case MessageType::ConfigEnd:
        case MessageType::GetState:
            if (dataSize != 0)
            {
                return 1;
            }
            break;
        case MessageType::ConfigAdd:
            break;
        case MessageType::EventGetNext:
            if (dataSize != 2)
            {
                return 1;
            }
            break;
        case MessageType::RunControllerCommand:
            // Must include controller id in first byte
            if (dataSize < 1 || this->controllerDefinitions.getControllerDefinition(data[0]) == nullptr)
            {
                return 1;
            }
            break;
        case MessageType::GetControllerCommandResult:
            if (dataSize != 0)
            {
                return 1;
            }
            break;
        case MessageType::Err:
            if (dataSize != 1)
            {
                return 1;
            }
            break;
        case MessageType::Ack:
            break;
        default:
            // Unknown message type
            return 1;
        }
        return 0;
    }
}
