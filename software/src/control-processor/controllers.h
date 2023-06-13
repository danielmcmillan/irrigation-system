#ifndef _CONTROLLER_NET_HOST_I2C_INTERFACE
#define _CONTROLLER_NET_HOST_I2C_INTERFACE
#include "controller-manager.h"
#include "stddef.h"

using namespace IrrigationSystem;

#define CONTROLLER_NET_HOST_I2C_SLAVE_ADDRESS 10
#define CONTROLLER_NET_HOST_I2C_PACKET_BUFFER_SIZE 32

enum class ControllersStatus
{
    Unconfigured,
    Initializing,
    Ready
};

class Controllers
{
public:
    Controllers(ControllerManager &controllers, EventHandler &eventHandler);
    void setup();
    bool loop();
    ControllersStatus getStatus() const;

    bool configStart();
    bool configAdd(const uint8_t *data, size_t length);
    bool configEnd();
    bool getPropertyValue(uint8_t controllerId, uint16_t propertyId, uint8_t *valuesOut);
    bool setPropertyValue(const uint8_t *data, size_t length);
    int runControllerCommand(const uint8_t *data, size_t length, uint8_t *responseOut, size_t *responseSizeOut);
    // void runPendingControllerCommand() const
    // bool getControllerCommandResult(const uint8_t **responseOut, size_t *responseSizeOut) const;

private:
    ControllerManager &controllers;
    ControllersStatus status;
    EventHandler &eventHandler;
    uint8_t lastUpdate;
};

#endif
