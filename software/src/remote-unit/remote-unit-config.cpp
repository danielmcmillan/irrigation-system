#include "remote-unit-config.h"
#include <string.h>

const uint8_t *RemoteUnitConfig::getRaw() const
{
  return this->config;
}

const uint8_t *RemoteUnitConfig::setRaw(const uint8_t *newConfig)
{
  memcpy(this->config, newConfig, REMOTE_UNIT_CONFIG_SIZE);
  return this->config;
}

void RemoteUnitConfig::loadFromEeprom()
{
  // TODO load data from eeprom. Handle case of missing config in eeprom.

  // Default values
  this->config[0] = 123; // battery multiplier * 2^13
  this->config[1] = 20;  // solenoid timeout seconds / 16
  this->config[2] = 0;   // Rf
  this->config[3] = 0;
  this->config[4] = 0;
  this->config[5] = 0;
  this->config[6] = 0;
  this->config[7] = 0;
  this->config[8] = 138;  // Upper battery voltage threshold
  this->config[9] = 135;  // Lower battery voltage threshold
  this->config[10] = 132; // Sleep battery voltage threshold
  this->config[11] = 4;   // Inverse of half battery check frequency (1/2Hz)
  this->config[12] = 105; // Minimum voltage for solenoid operation
  this->config[13] = 10;  // Solenoid off pulse width / 100 seconds
  this->config[14] = 10;  // Solenoid off pulse width / 100 seconds
}

void RemoteUnitConfig::saveToEeprom()
{
}

uint8_t RemoteUnitConfig::getBatteryCalibration() const
{
  return this->config[0];
}

uint8_t RemoteUnitConfig::getSolenoidTimeout() const
{
  return this->config[1];
}

const uint8_t *RemoteUnitConfig::getRfConfig() const
{
  return this->config + 2;
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdUpper() const
{
  return this->config[8];
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdLower() const
{
  return this->config[9];
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdSleep() const
{
  return this->config[10];
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdDisable() const
{
  return this->config[12];
}

uint8_t RemoteUnitConfig::getBatteryVoltageCheckFrequency() const
{
  return this->config[11];
}

uint8_t RemoteUnitConfig::getSolenoidOnPulseWidth() const
{
  return this->config[13];
}

uint8_t RemoteUnitConfig::getSolenoidOffPulseWidth() const
{
  return this->config[14];
}
