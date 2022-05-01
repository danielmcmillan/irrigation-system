#include "command-handler.h"
#include <Arduino.h>
#include <string.h>

RemoteUnitCommandHandler::RemoteUnitCommandHandler(
    RemoteUnitConfig &config, RemoteUnitRfModule &rfModule, Solenoids &solenoids, RemoteUnitBattery &battery, RemoteUnitFaults &faults, volatile unsigned long &counts)
    : config(config), rfModule(rfModule), solenoids(solenoids), battery(battery), faults(faults), counts(counts)
{
}

int RemoteUnitCommandHandler::getSolenoidState(uint8_t *solenoidStateOut) const
{
    *solenoidStateOut = this->solenoids.getState();
    return 0;
}

int RemoteUnitCommandHandler::setSolenoidState(uint8_t state, uint8_t *newSolenoidStateOut) const
{
    if (battery.shouldDisable() && state != 0)
    {
        this->faults.setFault(RemoteUnitFault::SolenoidNotActivated);
        state = 0;
    }
    *newSolenoidStateOut = solenoids.setState(state);
    return 0;
}

int RemoteUnitCommandHandler::getBatteryVoltage(uint8_t *voltageOut) const
{
    *voltageOut = this->battery.getVoltage();
    return *voltageOut == 0;
}

int RemoteUnitCommandHandler::getBatteryRaw(uint16_t *batteryRawOut) const
{
    *batteryRawOut = this->battery.readRawVoltage();
    return *batteryRawOut == 0;
}

int RemoteUnitCommandHandler::getFaults(uint8_t *faultsOut) const
{
    *faultsOut = this->faults.getFaults();
    return 0;
}

int RemoteUnitCommandHandler::clearFaults(uint8_t *faultsOut) const
{
    this->faults.clear();
    *faultsOut = 0x00;
    return 0;
}

int RemoteUnitCommandHandler::getSignalStrength(uint8_t *signalStrengthOut) const
{
    return this->rfModule.readSignalStrength(signalStrengthOut);
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
    return 0;
}

int RemoteUnitCommandHandler::persistConfig(uint8_t *configDataOut) const
{
    memcpy(configDataOut, this->config.getRaw(), REMOTE_UNIT_CONFIG_SIZE);
    return this->config.persist();
}

int RemoteUnitCommandHandler::applyRfConfig(uint8_t *configDataOut) const
{
    memcpy(configDataOut, this->config.getRaw(), REMOTE_UNIT_CONFIG_SIZE);
    int result = this->rfModule.applyConfig();
    if (result)
    {
        faults.setFault(RemoteUnitFault::ConfigureRfModuleFailed);
    }
    return result;
}

int RemoteUnitCommandHandler::getTimer(uint32_t *timerOut) const
{
    *timerOut = this->counts;
    return 0;
}
