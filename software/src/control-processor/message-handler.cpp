#include "message-handler.h"

ControlProcessorMessageHandler::ControlProcessorMessageHandler(ControllerManager &controllers)
    : controllers(controllers)
{
}

int ControlProcessorMessageHandler::propertyRead(uint8_t controllerId, uint16_t propertyId, uint8_t *valueOut, size_t *valueSizeOut) const
{
    IrrigationSystem::Controller *controller = this->controllers.getController(controllerId);
    const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
    unsigned int valueLength = definition.getPropertyLength(propertyId);
    bool readOnly = definition.getPropertyReadOnly(propertyId);

    uint32_t value = controller->getPropertyValue(propertyId);
    for (unsigned int i = 0; i < valueLength; ++i)
    {
        valueOut[i] = value; // LSB
        value >>= 8;
    }

    if (readOnly)
    {
        *valueSizeOut = valueLength;
    }
    else
    {
        value = controller->getPropertyDesiredValue(propertyId);
        for (unsigned int i = 0; i < valueLength; ++i)
        {
            valueOut[valueLength + i] = value; // LSB
            value >>= 8;
        }
        *valueSizeOut = valueLength * 2;
    }
    return 0;
}

int ControlProcessorMessageHandler::propertyWrite(uint8_t controllerId, uint16_t propertyId, const uint8_t *value) const
{
    IrrigationSystem::Controller *controller = controllers.getController(controllerId);
    const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
    unsigned int valueLength = definition.getPropertyLength(propertyId);

    uint32_t desiredValue = 0;
    for (unsigned int i = 0; i < valueLength; ++i)
    {
        desiredValue |= value[i] << i * 8;
    }

    controller->setPropertyDesiredValue(propertyId, desiredValue);
    return 0;
}
