#include "event-history.h"
#include <Arduino.h>

EventHistory::EventHistory() : eventHistory{0},
                               historyStartIndex(0),
                               historyActualSize(0),
                               startTime(0),
                               lastEventTime(0)
{
}

void EventHistory::start()
{
    this->startTime = millis();
}

void EventHistory::handleEvent(uint8_t type, uint8_t payloadSize, const uint8_t *payload)
{
    // Get the event timestamp
    uint32_t time = millis() - this->startTime;
    if (time == this->lastEventTime)
    {
        time = this->lastEventTime + 1;
    }
    this->lastEventTime = time;

    // Remove events from the start if it won't fit
    unsigned int eventSize = EVENT_HEADER_SIZE + payloadSize;
    EventHistoryRecord event;
    while (this->historyActualSize + eventSize > EVENT_HISTORY_SIZE)
    {
        this->getEventAtOffset(0, &event);
        this->historyStartIndex = (this->historyStartIndex + EVENT_HEADER_SIZE + event.payloadSize) % EVENT_HISTORY_SIZE;
        this->historyActualSize -= EVENT_HEADER_SIZE + event.payloadSize;
    }

    // Write the event data. Time is little-endian value
    this->eventHistory[(this->historyStartIndex + this->historyActualSize) % EVENT_HISTORY_SIZE] = time;
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 1) % EVENT_HISTORY_SIZE] = time >> 8;
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 2) % EVENT_HISTORY_SIZE] = time >> 16;
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 3) % EVENT_HISTORY_SIZE] = time >> 24;
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 4) % EVENT_HISTORY_SIZE] = type;
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 5) % EVENT_HISTORY_SIZE] = payloadSize;
    // Write payload data
    for (int i = 0; i < payloadSize; ++i)
    {
        unsigned int arrayOffset = (this->historyStartIndex + this->historyActualSize + EVENT_HEADER_SIZE + i) % EVENT_HISTORY_SIZE;
        this->eventHistory[arrayOffset] = payload[i];
    }
    this->historyActualSize += eventSize;
}

bool EventHistory::getEvent(uint32_t withTime, EventHistoryRecord *eventOut) const
{
    // Iterate through events to find the specified one
    unsigned int offsetFromStart = 0;
    for (;;)
    {
        if (offsetFromStart < this->historyActualSize)
        {
            this->getEventAtOffset(offsetFromStart, eventOut);
            if (eventOut->time == withTime)
            {
                return true;
            }
            else
            {
                // Move index to the next event
                offsetFromStart += EVENT_HEADER_SIZE + eventOut->payloadSize;
            }
        }
        else
        {
            // No event at or after the specified one exists
            // This may happen due to bad request, or when time overflows
            // Use the first event if it exists, to handle overflow
            if (this->historyActualSize > 0)
            {
                this->getEventAtOffset(0, eventOut);
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}

void EventHistory::getEventAtOffset(unsigned int offsetFromStart, EventHistoryRecord *eventOut) const
{
    // Read time as little-endian value
    eventOut->time = ((uint32_t)this->eventHistory[(this->historyStartIndex + offsetFromStart) % EVENT_HISTORY_SIZE]);
    eventOut->time |= ((uint32_t)this->eventHistory[(this->historyStartIndex + offsetFromStart + 1) % EVENT_HISTORY_SIZE]) << 8;
    eventOut->time |= ((uint32_t)this->eventHistory[(this->historyStartIndex + offsetFromStart + 2) % EVENT_HISTORY_SIZE]) << 16;
    eventOut->time |= ((uint32_t)this->eventHistory[(this->historyStartIndex + offsetFromStart + 3) % EVENT_HISTORY_SIZE]) << 24;
    eventOut->type = this->eventHistory[(this->historyStartIndex + offsetFromStart + 4) % EVENT_HISTORY_SIZE];
    eventOut->payloadSize = this->eventHistory[(this->historyStartIndex + offsetFromStart + 5) % EVENT_HISTORY_SIZE];
    for (int i = 0; i < eventOut->payloadSize; ++i)
    {
        unsigned int arrayOffset = (this->historyStartIndex + offsetFromStart + EVENT_HEADER_SIZE + i) % EVENT_HISTORY_SIZE;
        eventOut->payload[i] = this->eventHistory[arrayOffset];
    }
}
