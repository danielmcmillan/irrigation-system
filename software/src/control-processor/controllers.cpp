#include "controllers.h"
#include <Arduino.h>
#include "logging.h"
#include "binary-util.h"

#define CONTROLLER_UPDATE_INTERVAL 3

Controllers::Controllers(ControllerManager &controllers, EventHandler &eventHandler)
    : controllers(controllers),
      status(ControllersStatus::Unconfigured),
      eventHandler(eventHandler),
      lastUpdate(0)
{
}

void Controllers::setup()
{
    controllers.setEventHandler(eventHandler);
    controllers.resetControllers();
    eventHandler.handleEvent(EventType::started, 0, nullptr); // todo move
}

bool Controllers::loop()
{
    uint8_t now = millis() >> 10;
    if ((uint8_t)(now - lastUpdate) < CONTROLLER_UPDATE_INTERVAL)
    {
        return true;
    }
    lastUpdate = now;

    if (status == ControllersStatus::Ready)
    {
        controllers.updateControllers();
        // runPendingControllerCommand();
    }
    else if (status == ControllersStatus::Initializing)
    {
        if (controllers.beginControllers())
        {
            status = ControllersStatus::Ready;
            eventHandler.handleEvent(EventType::ready, 0, nullptr);
        }
        else
        {
            delay(5000); // todo may want to avoid the delay
            return false;
        }
    }
    return true;
}

ControllersStatus Controllers::getStatus() const
{
    return status;
}

bool Controllers::configStart()
{
    if (status != ControllersStatus::Unconfigured)
    {
        status = ControllersStatus::Unconfigured;
        eventHandler.handleEvent(EventType::started, 0, nullptr); // todo move
    }
    controllers.resetControllers();
    return true;
}

bool Controllers::configAdd(const uint8_t *data, size_t length)
{
    uint8_t controllerId = data[0];
    uint8_t configType = data[1];
    const uint8_t *configData = &data[2];
    if (status != ControllersStatus::Unconfigured)
    {
        return false;
    }
    IrrigationSystem::Controller *controller = controllers.getController(controllerId);
    controller->configure(configType, configData);
    return true;
}

bool Controllers::configEnd()
{
    if (status != ControllersStatus::Unconfigured)
    {
        return false;
    }
    status = ControllersStatus::Initializing;
    eventHandler.handleEvent(EventType::configured, 0, nullptr); // todo move
    return true;
}

bool Controllers::getPropertyValue(uint8_t controllerId, uint16_t propertyId, uint8_t *valuesOut)
{
    if (status == ControllersStatus::Unconfigured)
    {
        return false;
    }
    IrrigationSystem::Controller *controller = controllers.getController(controllerId);
    const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
    unsigned int valueLength = definition.getPropertyLength(propertyId);
    bool readOnly = definition.getPropertyReadOnly(propertyId);

    uint32_t value = controller->getPropertyValue(propertyId);
    for (unsigned int i = 0; i < valueLength; ++i)
    {
        valuesOut[i] = value; // LSB
        value >>= 8;
    }

    if (!readOnly)
    {
        value = controller->getPropertyDesiredValue(propertyId);
        for (unsigned int i = 0; i < valueLength; ++i)
        {
            valuesOut[valueLength + i] = value; // LSB
            value >>= 8;
        }
    }
    return true;
}

bool Controllers::setPropertyValue(const uint8_t *data, size_t length)
{
    if (status == ControllersStatus::Unconfigured)
    {
        return false;
    }
    uint8_t controllerId = data[0];
    uint16_t propertyId = read16LE(&data[1]);
    const uint8_t *value = data + 3;

    IrrigationSystem::Controller *controller = controllers.getController(controllerId);
    const IrrigationSystem::ControllerDefinition &definition = controller->getDefinition();
    unsigned int valueLength = definition.getPropertyLength(propertyId);

    uint32_t desiredValue = 0;
    for (unsigned int i = 0; i < valueLength; ++i)
    {
        desiredValue |= value[i] << i * 8;
    }

    controller->setPropertyDesiredValue(propertyId, desiredValue);
    return true;
}

uint16_t Controllers::runControllerCommand(const uint8_t *data, size_t length, uint8_t *responseOut, size_t *responseSizeOut)
{
    uint8_t controllerId = data[0];
    IrrigationSystem::Controller *controller = controllers.getController(controllerId);
    if (controller != nullptr)
    {
        return controller->runCommand(data + 1, length - 1, responseOut, responseSizeOut);
    }
    return 0xffff;
}
