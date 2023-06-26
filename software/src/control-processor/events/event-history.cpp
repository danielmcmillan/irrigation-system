#include "event-history.h"
#include "binary-util.h"

using namespace IrrigationSystem;

EventHistory::EventHistory() : eventHistory{0},
                               historyStartIndex(0),
                               historyActualSize(0),
                               lastEventId(1)
{
}

void EventHistory::handleEvent(uint8_t type, uint8_t payloadSize, const uint8_t *payload)
{
    uint16_t eventId = this->lastEventId++;

    // Remove events from the start if it won't fit
    unsigned int eventSize = EVENT_HEADER_SIZE + payloadSize;
    EventHistoryRecord event;
    while (this->historyActualSize + eventSize > EVENT_HISTORY_SIZE)
    {
        this->getEventAtOffset(0, &event);
        this->historyStartIndex = (this->historyStartIndex + EVENT_HEADER_SIZE + event.payloadSize) % EVENT_HISTORY_SIZE;
        this->historyActualSize -= EVENT_HEADER_SIZE + event.payloadSize;
    }

    // Write the event data
    this->eventHistory[(this->historyStartIndex + this->historyActualSize) % EVENT_HISTORY_SIZE] = (uint8_t)eventId;
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 1) % EVENT_HISTORY_SIZE] = (uint8_t)(eventId >> 8);
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 2) % EVENT_HISTORY_SIZE] = type;
    this->eventHistory[(this->historyStartIndex + this->historyActualSize + 3) % EVENT_HISTORY_SIZE] = payloadSize;
    // Write payload data
    for (int i = 0; i < payloadSize; ++i)
    {
        unsigned int arrayOffset = (this->historyStartIndex + this->historyActualSize + EVENT_HEADER_SIZE + i) % EVENT_HISTORY_SIZE;
        this->eventHistory[arrayOffset] = payload[i];
    }
    this->historyActualSize += eventSize;
}

bool EventHistory::getNextEvent(uint16_t afterId, bool *idMatched, EventHistoryRecord *eventOut) const
{
    *idMatched = false;
    // Iterate through events to find the specified one
    unsigned int offsetFromStart = 0;
    for (;;)
    {
        if (offsetFromStart < this->historyActualSize)
        {
            // Get event at this index
            this->getEventAtOffset(offsetFromStart, eventOut);
            if (*idMatched)
            {
                // The event matching requested id was found in previous iteration, so return this next event
                return true;
            }
            // Check for a match
            if (eventOut->id == afterId)
            {
                *idMatched = true;
            }
            // Move index to the next event
            offsetFromStart += EVENT_HEADER_SIZE + eventOut->payloadSize;
        }
        else if (*idMatched)
        {
            // The event was found, there was no event after it
            // This happens when the requester is already up to date with the latest events
            return false;
        }
        else
        {
            // No event for the specified id exists
            // This may happen on initial request after startup, or when requester is behind and their last seen id is gone
            // Use the first event if it exists, to start at earliest event
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
    eventOut->id = read16LE(&this->eventHistory[(this->historyStartIndex + offsetFromStart) % EVENT_HISTORY_SIZE]);
    eventOut->type = this->eventHistory[(this->historyStartIndex + offsetFromStart + 2) % EVENT_HISTORY_SIZE];
    eventOut->payloadSize = this->eventHistory[(this->historyStartIndex + offsetFromStart + 3) % EVENT_HISTORY_SIZE];
    for (int i = 0; i < eventOut->payloadSize; ++i)
    {
        unsigned int arrayOffset = (this->historyStartIndex + offsetFromStart + EVENT_HEADER_SIZE + i) % EVENT_HISTORY_SIZE;
        eventOut->payload[i] = this->eventHistory[arrayOffset];
    }
}
