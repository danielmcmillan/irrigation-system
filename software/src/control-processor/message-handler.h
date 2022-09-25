#ifndef _CONTROL_PROCESSOR_MESSAGE_HANDLER_H
#define _CONTROL_PROCESSOR_MESSAGE_HANDLER_H
#include <stdlib.h>
#include <inttypes.h>
#include "controller-manager.h"

using namespace IrrigationSystem;

class ControlProcessorMessageHandler
{
  ControllerManager &controllers;

public:
  ControlProcessorMessageHandler(ControllerManager &controllers);

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
};
#endif
