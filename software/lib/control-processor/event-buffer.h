#ifndef _EVENT_BUFFER_H
#define _EVENT_BUFFER_H

#include "event-handler.h"

#define EVENT_BUFFER_CAPACITY 256
#define EVENT_HEADER_SIZE 4

namespace IrrigationSystem
{
    class EventBuffer : public EventHandler
    {
    public:
        EventBuffer();
        /**
         * Handle an incoming event by pushing it into the buffer.
         */
        void handleEvent(uint8_t type, uint8_t payloadSize, const uint8_t *payload) override;
        /**
         * If there is an event in the buffer, returns it's size.
         * Otherwise returns 0.
         */
        uint8_t peek() const;
        /**
         * If there is an event in the buffer, returns it's size, writes it's content to `eventOut` and removes it from the buffer.
         * Otherwise returns 0.
         */
        uint8_t pop(uint8_t *eventOut);
        /**
         * Returns whether there was a failure to handle an event.
         */
        bool checkBufferFullFault(bool clear);

    private:
        void pushData(uint8_t size, const uint8_t *data);

    public:
        uint8_t eventData[EVENT_BUFFER_CAPACITY];
        unsigned int dataStartIndex;
        unsigned int dataSize;
        uint16_t nextEventId;
        bool bufferFullFault;
    };
}
#endif
