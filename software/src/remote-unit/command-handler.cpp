#include "command-handler.h"
#include <Arduino.h>
#include <string.h>

RemoteUnitCommandHandler::RemoteUnitCommandHandler(RemoteUnitConfig &config, Solenoids &solenoids, RemoteUnitBattery &battery)
    : config(config), solenoids(solenoids), battery(battery)
{
}

int RemoteUnitCommandHandler::getSolenoidState(uint8_t *solenoidStateOut) const
{
    *solenoidStateOut = solenoids.getState();
    return 0;
}

int RemoteUnitCommandHandler::setSolenoidState(uint8_t state, uint8_t *newSolenoidStateOut) const
{
    if (battery.shouldDisable())
    {
        state = 0;
    }
    *newSolenoidStateOut = solenoids.setState(state);
    return 0;
}

int RemoteUnitCommandHandler::getBatteryVoltage(uint8_t *voltageOut) const
{
    battery.update(millis());
    *voltageOut = battery.getVoltage();
    return *voltageOut == 0;
}

int RemoteUnitCommandHandler::getBatteryRaw(uint16_t *batteryRawOut) const
{
    *batteryRawOut = battery.readRawVoltage();
    return *batteryRawOut == 0;
}

int RemoteUnitCommandHandler::getFaults(uint8_t *faultsOut) const
{
    *faultsOut = 0x00;
    return 0;
}

int RemoteUnitCommandHandler::clearFaults(uint8_t *faultsOut) const
{
    *faultsOut = 0x00;
    return 0;
}

int RemoteUnitCommandHandler::getSignalStrength(uint8_t *signalStrengthOut) const
{
    *signalStrengthOut = 0x00;
    return 0;
}

int RemoteUnitCommandHandler::getConfig(uint8_t *configDataOut) const
{
    memcpy(configDataOut, this->config.getRaw(), REMOTE_UNIT_CONFIG_SIZE);
    return 0;
}

int RemoteUnitCommandHandler::setConfig(const uint8_t *configData, uint8_t *configDataOut) const
{
    this->config.setRaw(configData);
    memcpy(configDataOut, this->config.getRaw(), REMOTE_UNIT_CONFIG_SIZE);
    return this->config.saveToEeprom();
}
