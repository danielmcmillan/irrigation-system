#include "event-handler.h"

void IrrigationSystem::EventHandler::handlePropertyValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value)
{
    uint8_t payload[] = {controllerId, propertyId, propertyId >> 8, value, value >> 8, value >> 16, value >> 24};
    this->handleEvent(EventType::propertyValueChanged, 3 + valueSize, payload);
}

void IrrigationSystem::EventHandler::handlePropertyDesiredValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value)
{
    uint8_t payload[] = {controllerId, propertyId, propertyId >> 8, value, value >> 8, value >> 16, value >> 24};
    this->handleEvent(EventType::propertyDesiredValueChanged, 3 + valueSize, payload);
}
