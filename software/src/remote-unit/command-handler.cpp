#include "command-handler.h"
#include <string.h>

RemoteUnitCommandHandler::RemoteUnitCommandHandler(Solenoids &solenoids)
    : solenoids(solenoids)
{
}

int RemoteUnitCommandHandler::getSolenoidState(uint8_t *solenoidStateOut) const
{
    *solenoidStateOut = solenoids.getState();
    return 0;
}

int RemoteUnitCommandHandler::setSolenoidState(uint8_t state, uint8_t *newSolenoidStateOut) const
{
    *newSolenoidStateOut = solenoids.setState(state);
    return 0;
}

int RemoteUnitCommandHandler::getBatteryVoltage(uint8_t *voltageOut) const
{
    return 0;
}

int RemoteUnitCommandHandler::getBatteryRaw(uint16_t *batteryRawOut) const
{
    return 0;
}

int RemoteUnitCommandHandler::getFaults(uint8_t *faultsOut) const
{
    return 0;
}

int RemoteUnitCommandHandler::clearFaults(uint8_t *faultsOut) const
{
    *faultsOut = 0x00;
    return 0;
}

int RemoteUnitCommandHandler::getSignalStrength(uint8_t *signalStrengthOut) const
{
    return 0;
}

int RemoteUnitCommandHandler::getConfig(uint8_t *configDataOut) const
{
    return 0;
}

int RemoteUnitCommandHandler::setConfig(const uint8_t *configData, uint8_t *configDataOut) const
{
    memcpy(configDataOut, configData, 13);
    return 0;
}
