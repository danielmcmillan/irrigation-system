#ifndef _EVENT_HISTORY_H
#define _EVENT_HISTORY_H

#include "event-handler.h"

#define EVENT_HISTORY_SIZE 256
#define MAX_EVENT_SIZE 20
#define EVENT_HEADER_SIZE 6

struct EventHistoryRecord
{
    uint32_t time;
    uint8_t type;
    uint8_t payloadSize;
    uint8_t payload[MAX_EVENT_SIZE - EVENT_HEADER_SIZE];
};

class EventHistory : public IrrigationSystem::EventHandler
{
public:
    EventHistory();
    /**
     * Start handling events.
     * Time in events will be relative to when this was called.
     */
    void start();
    /**
     * Record a new event.
     * Older events will be removed to make room.
     */
    void handleEvent(uint8_t type, uint8_t payloadSize, const uint8_t *payload) override;
    /**
     * Get the event for the specified time.
     * If the event doesn't exist, returns the earliest event if any.
     * Returns whether any event was found.
     */
    bool getEvent(uint32_t withTime, EventHistoryRecord *eventOut) const;

private:
    uint8_t eventHistory[EVENT_HISTORY_SIZE];
    unsigned int historyStartIndex;
    unsigned int historyActualSize;

    void getEventAtOffset(unsigned int offsetFromStart, EventHistoryRecord *eventOut) const;

    // millis() at start time
    uint32_t startTime;
    uint32_t lastEventTime;
};

#endif
