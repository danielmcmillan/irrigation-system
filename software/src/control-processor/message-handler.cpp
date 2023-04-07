#include "message-handler.h"
#include "binary-util.h"
#include <Arduino.h>

using namespace IrrigationSystem;

ControlProcessorMessageHandler::ControlProcessorMessageHandler(ControllerManager &controllers, ControlProcessorState &state, EventHistory &events)
    : controllers(controllers), state(state), events(events)
{
}

int IrrigationSystem::ControlProcessorMessageHandler::configStart() const
{
    state.status = ControlProcessorStatus::Unconfigured;
    controllers.resetControllers();
    events.handleEvent(EventType::started, 0, nullptr);
    return 0;
}

int IrrigationSystem::ControlProcessorMessageHandler::configAdd(uint8_t controllerId, uint8_t configType, const uint8_t *configData) const
{
    if (state.status != ControlProcessorStatus::Unconfigured)
    {
        return 1;
    }
    IrrigationSystem::Controller *controller = this->controllers.getController(controllerId);
    controller->configure(configType, configData);
    return 0;
}

int IrrigationSystem::ControlProcessorMessageHandler::configEnd() const
{
    if (state.status != ControlProcessorStatus::Unconfigured)
    {
        return 1;
    }
    state.status = ControlProcessorStatus::Initializing;
    events.handleEvent(EventType::configured, 0, nullptr);
    return 0;
}

int IrrigationSystem::ControlProcessorMessageHandler::getState(uint8_t *valueOut, size_t *valueSizeOut) const
{
    valueOut[0] = (uint8_t)state.status;
    *valueSizeOut = 1;
    return 0;
}

int ControlProcessorMessageHandler::propertyRead(uint8_t controllerId, uint16_t propertyId, uint8_t *valueOut, size_t *valueSizeOut) const
{
    if (state.status == ControlProcessorStatus::Unconfigured)
    {
        return 1;
    }
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
    if (state.status == ControlProcessorStatus::Unconfigured)
    {
        return 1;
    }
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

int ControlProcessorMessageHandler::eventGetNext(uint16_t afterId, uint8_t *resultIndicatorOut, uint8_t *eventOut, size_t *eventSizeOut) const
{
    bool timeMatched;
    EventHistoryRecord event;

    if (events.getNextEvent(afterId, &timeMatched, &event))
    {
        *resultIndicatorOut = timeMatched ? 0x01 : 0x02;
        *eventSizeOut = EVENT_HEADER_SIZE + event.payloadSize;
        // Write the event data
        write16LE(eventOut, event.id);
        eventOut[2] = event.type;
        eventOut[3] = event.payloadSize;
        // Write payload data
        for (int i = 0; i < event.payloadSize; ++i)
        {
            eventOut[EVENT_HEADER_SIZE + i] = event.payload[i];
        }
    }
    else
    {
        *resultIndicatorOut = 0x00;
        *eventSizeOut = 0;
    }

    return 0;
}

enum class ControllerCommandState
{
    None,
    Pending,
    Complete
} controllerCommandState;
uint16_t nextCommandControllerId;
uint8_t nextCommand[TWI_RX_BUFFER_LENGTH - 4];
size_t nextCommandSize;
// uint8_t nextCommandResult[20];
size_t nextCommandResultSize;
int nextCommandError;

int IrrigationSystem::ControlProcessorMessageHandler::runControllerCommand(uint16_t controllerId, const uint8_t *input, size_t inputSize) const
{
    if (inputSize > sizeof(nextCommand))
    {
        controllerCommandState = ControllerCommandState::None;
        return 1;
    }
    controllerCommandState = ControllerCommandState::Pending;
    nextCommandControllerId = controllerId;
    memcpy(nextCommand, input, inputSize);
    nextCommandSize = inputSize;
    return 0;
}

int IrrigationSystem::ControlProcessorMessageHandler::getControllerCommandResult(uint8_t *responseOut, size_t *responseSizeOut) const
{
    if (controllerCommandState != ControllerCommandState::Complete)
    {
        return 1;
    }
    if (nextCommandError != 0)
    {
        return 1 + nextCommandError;
    }
    memcpy(responseOut, nextCommand, nextCommandResultSize);
    *responseSizeOut = nextCommandResultSize;
    return 0;
}

void IrrigationSystem::ControlProcessorMessageHandler::runPendingControllerCommand() const
{
    if (controllerCommandState == ControllerCommandState::Pending)
    {
        nextCommandResultSize = 0;
        nextCommandError = controllers.getController(nextCommandControllerId)->runCommand(nextCommand, nextCommandSize, nextCommand, &nextCommandResultSize);
        controllerCommandState = ControllerCommandState::Complete;
    }
}
