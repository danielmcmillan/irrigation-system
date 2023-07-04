#include "event-buffer.h"

#define HEADER_PAYLOAD_SIZE_INDEX 3

IrrigationSystem::EventBuffer::EventBuffer() : eventData{0},
                                               dataStartIndex(0),
                                               dataSize(0),
                                               nextEventId(1)
{
}

void IrrigationSystem::EventBuffer::handleEvent(uint8_t type, uint8_t payloadSize, const uint8_t *payload)
{
    uint8_t size = EVENT_HEADER_SIZE + payloadSize;
    if (dataSize + size <= EVENT_BUFFER_CAPACITY)
    {
        uint8_t header[EVENT_HEADER_SIZE] = {(uint8_t)nextEventId, (uint8_t)(nextEventId >> 8), type, payloadSize};
        pushData(EVENT_HEADER_SIZE, header);
        pushData(payloadSize, payload);
    }
    else
    {
        bufferFullFault = true;
    }
    ++nextEventId;
}

void IrrigationSystem::EventBuffer::pushData(uint8_t size, const uint8_t *data)
{
    for (uint8_t i = 0; i < size; ++i)
    {
        eventData[(dataStartIndex + dataSize + i) % EVENT_BUFFER_CAPACITY] = data[i];
    }
    dataSize += size;
}

uint8_t IrrigationSystem::EventBuffer::peek() const
{
    if (dataSize > 0)
    {
        return EVENT_HEADER_SIZE + eventData[(dataStartIndex + HEADER_PAYLOAD_SIZE_INDEX) % EVENT_BUFFER_CAPACITY];
    }
    else
    {
        return 0;
    }
}

uint8_t IrrigationSystem::EventBuffer::pop(uint8_t *eventOut)
{
    uint8_t size = peek();
    if (size > 0 && eventOut != nullptr)
    {
        for (uint8_t i = 0; i < size; ++i)
        {
            eventOut[i] = eventData[(dataStartIndex + i) % EVENT_BUFFER_CAPACITY];
        }
        dataStartIndex = (dataStartIndex + size) % EVENT_BUFFER_CAPACITY;
        dataSize -= size;
        return size;
    }
    else
    {
        return 0;
    }
}

bool IrrigationSystem::EventBuffer::checkBufferFullFault(bool clear)
{
    bool result = bufferFullFault;
    if (clear)
    {
        bufferFullFault = false;
    }
    return result;
}
