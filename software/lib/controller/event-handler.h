#ifndef _EVENT_HANDLER_H
#define _EVENT_HANDLER_H

#include <inttypes.h>

namespace IrrigationSystem
{
    namespace EventType
    {
        enum : uint8_t
        {
            generalInfo = 0x40,
            started = 0x41,
            configured = 0x42,
            ready = 0x43,
            propertyValueChanged = 0x48,
            propertyDesiredValueChanged = 0x49,
            generalWarning = 0x80,
            generalError = 0xc0,
            controllerError = 0xc1,
            propertyError = 0xc2
        };
    }

    class EventHandler
    {
    public:
        virtual void handleEvent(uint8_t type, uint8_t payloadSize, const uint8_t *payload) = 0;
        void handlePropertyValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value);
        void handlePropertyDesiredValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value);
        void handlePropertyError(uint8_t controllerId, uint16_t propertyId, uint8_t errorSize, uint32_t error);

    private:
        void handlePropertyLevelEvent(uint8_t type, uint8_t controllerId, uint16_t propertyId, uint8_t dataSize, uint32_t data);
    };
}

#endif
