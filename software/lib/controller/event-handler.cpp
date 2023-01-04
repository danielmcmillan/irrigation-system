#include "event-handler.h"

void IrrigationSystem::EventHandler::handlePropertyValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value)
{
    uint8_t payload[] = {controllerId, (uint8_t)propertyId, (uint8_t)(propertyId >> 8), (uint8_t)value, (uint8_t)(value >> 8), (uint8_t)(value >> 16), (uint8_t)(value >> 24)};
    this->handleEvent(EventType::propertyValueChanged, 3 + valueSize, payload);
}

void IrrigationSystem::EventHandler::handlePropertyDesiredValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value)
{
    uint8_t payload[] = {controllerId, (uint8_t)propertyId, (uint8_t)(propertyId >> 8), (uint8_t)value, (uint8_t)(value >> 8), (uint8_t)(value >> 16), (uint8_t)(value >> 24)};
    this->handleEvent(EventType::propertyDesiredValueChanged, 3 + valueSize, payload);
}
