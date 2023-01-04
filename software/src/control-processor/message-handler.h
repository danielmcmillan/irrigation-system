#ifndef _CONTROL_PROCESSOR_MESSAGE_HANDLER_H
#define _CONTROL_PROCESSOR_MESSAGE_HANDLER_H
#include <stdlib.h>
#include <inttypes.h>
#include "controller-manager.h"
#include "events/event-history.h"

namespace IrrigationSystem
{
  class ControlProcessorMessageHandler
  {
    ControllerManager &controllers;
    EventHistory &events;

  public:
    ControlProcessorMessageHandler(ControllerManager &controllers, EventHistory &events);

    /**
     * Read the value of a property.
     * If the property is not read-only, then the value also includes the current desired value.
     * Returns 0.
     */
    int propertyRead(uint8_t controllerId, uint16_t propertyId, uint8_t *valueOut, size_t *valueSizeOut) const;

    /**
     * Update the desired value of a property.
     * Returns 0.
     */
    int propertyWrite(uint8_t controllerId, uint16_t propertyId, const uint8_t *value) const;

    /**
     * Get the next event.
     * Returns 0.
     */
    int eventGetNext(uint16_t afterId, uint8_t *resultIndicatorOut, uint8_t *eventOut, size_t *eventSizeOut) const;
  };
}
#endif
