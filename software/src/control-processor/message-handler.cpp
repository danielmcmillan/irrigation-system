#include "message-handler.h"
#include "binary-util.h"

using namespace IrrigationSystem;

ControlProcessorMessageHandler::ControlProcessorMessageHandler(ControllerManager &controllers, EventHistory &events)
    : controllers(controllers), events(events)
{
}

int ControlProcessorMessageHandler::propertyRead(uint8_t controllerId, uint16_t propertyId, uint8_t *valueOut, size_t *valueSizeOut) const
{
    IrrigationSystem::Controller *controller = this->controllers.getController(controllerId);
    const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
    unsigned int valueLength = definition.getPropertyLength(propertyId);
    bool readOnly = definition.getPropertyReadOnly(propertyId);

    uint32_t value = controller->getPropertyValue(propertyId);
    for (unsigned int i = 0; i < valueLength; ++i)
    {
        valueOut[i] = value; // LSB
        value >>= 8;
    }

    if (readOnly)
    {
        *valueSizeOut = valueLength;
    }
    else
    {
        value = controller->getPropertyDesiredValue(propertyId);
        for (unsigned int i = 0; i < valueLength; ++i)
        {
            valueOut[valueLength + i] = value; // LSB
            value >>= 8;
        }
        *valueSizeOut = valueLength * 2;
    }
    return 0;
}

int ControlProcessorMessageHandler::propertyWrite(uint8_t controllerId, uint16_t propertyId, const uint8_t *value) const
{
    IrrigationSystem::Controller *controller = controllers.getController(controllerId);
    const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
    unsigned int valueLength = definition.getPropertyLength(propertyId);

    uint32_t desiredValue = 0;
    for (unsigned int i = 0; i < valueLength; ++i)
    {
        desiredValue |= value[i] << i * 8;
    }

    controller->setPropertyDesiredValue(propertyId, desiredValue);
    return 0;
}

int ControlProcessorMessageHandler::eventGetNext(uint16_t afterId, uint8_t *resultIndicatorOut, uint8_t *eventOut, size_t *eventSizeOut) const
{
    bool timeMatched;
    EventHistoryRecord event;

    if (events.getNextEvent(afterId, &timeMatched, &event))
    {
        *resultIndicatorOut = timeMatched ? 0x01 : 0x02;
        *eventSizeOut = EVENT_HEADER_SIZE + event.payloadSize;
        // Write the event data
        write16LE(eventOut, event.id);
        eventOut[2] = event.type;
        eventOut[3] = event.payloadSize;
        // Write payload data
        for (int i = 0; i < event.payloadSize; ++i)
        {
            eventOut[EVENT_HEADER_SIZE + i] = event.payload[i];
        }
    }
    else
    {
        *resultIndicatorOut = 0x00;
        *eventSizeOut = 0;
    }

    return 0;
}
