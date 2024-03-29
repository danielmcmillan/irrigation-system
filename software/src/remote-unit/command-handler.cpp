#include "command-handler.h"
#include <Arduino.h>
#include <string.h>
#include "settings.h"

RemoteUnitCommandHandler::RemoteUnitCommandHandler(
    RemoteUnitConfig &config,
    RemoteUnitRfModule &rfModule,
    Solenoids &solenoids,
    RemoteUnitBattery &battery,
    RemoteUnitFaults &faults,
    RemoteUnitSensor &sensor,
    volatile unsigned long &counts)
    : config(config), rfModule(rfModule), solenoids(solenoids), battery(battery),
      faults(faults), sensor(sensor), counts(counts)
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
    *faultsOut = this->faults.getFaults();
    this->faults.clear();
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
    delay(600); // Give module some time before continuing with response
    return result;
}

int RemoteUnitCommandHandler::getTimer(uint32_t *timerOut) const
{
    *timerOut = this->counts;
    return 0;
}

int RemoteUnitCommandHandler::getSoftwareRevision(uint16_t *softwareRevisionOut) const
{
    *softwareRevisionOut = REMOTE_UNIT_REVISION;
    return 0;
}

RemoteUnitSensor::SensorReadingResult RemoteUnitCommandHandler::getSensorValue(uint8_t sensorId, uint16_t *valueOut) const
{
    if (sensorId & 0x80)
    {
        this->sensor.scheduleUpdate();
    }
    return this->sensor.getValue(sensorId & 0x7f, valueOut);
}
