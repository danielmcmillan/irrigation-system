#include "event-handler.h"

void IrrigationSystem::EventHandler::handlePropertyValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value)
{
    handlePropertyLevelEvent(EventType::propertyValueChanged, controllerId, propertyId, valueSize, value);
}

void IrrigationSystem::EventHandler::handlePropertyDesiredValueChanged(uint8_t controllerId, uint16_t propertyId, uint8_t valueSize, uint32_t value)
{
    handlePropertyLevelEvent(EventType::propertyDesiredValueChanged, controllerId, propertyId, valueSize, value);
}

void IrrigationSystem::EventHandler::handlePropertyError(uint8_t controllerId, uint16_t propertyId, uint8_t errorSize, uint32_t error)
{
    handlePropertyLevelEvent(EventType::propertyError, controllerId, propertyId, errorSize, error);
}

void IrrigationSystem::EventHandler::handlePropertyLevelEvent(uint8_t type, uint8_t controllerId, uint16_t propertyId, uint8_t dataSize, uint32_t data)
{
    uint8_t payload[] = {controllerId, (uint8_t)propertyId, (uint8_t)(propertyId >> 8), (uint8_t)data, (uint8_t)(data >> 8), (uint8_t)(data >> 16), (uint8_t)(data >> 24)};
    this->handleEvent(type, 3 + dataSize, payload);
}
