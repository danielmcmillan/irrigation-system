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
    this->historyStartIndex = 0;
    this->historyActualSize = 0;
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

bool EventHistory::getNextEvent(uint32_t afterTime, bool *timeMatched, EventHistoryRecord *eventOut) const
{
    *timeMatched = false;
    // Iterate through events to find the specified one
    unsigned int offsetFromStart = 0;
    for (;;)
    {
        if (offsetFromStart < this->historyActualSize)
        {
            // Get event at this index
            this->getEventAtOffset(offsetFromStart, eventOut);
            if (*timeMatched)
            {
                // The event matching requested time was found in previous iteration, so return this next event
                return true;
            }
            // Check for a match
            if (eventOut->time == afterTime)
            {
                *timeMatched = true;
            }
            // Move index to the next event
            offsetFromStart += EVENT_HEADER_SIZE + eventOut->payloadSize;
        }
        else if (*timeMatched)
        {
            // The event was found, there was no event after it
            // This happens when the requester is already up to date with the latest events
            return false;
        }
        else
        {
            // No event for the specified time exists
            // This may happen on initial request for first event, or when time overflows
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
