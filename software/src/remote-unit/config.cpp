#include "config.h"
#include <string.h>
#include "yl-800t.h"

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
  uint32_t rfFreq = 434l * 1l << 14;
  this->config[0] = 122;                      // battery multiplier * 2^13
  this->config[1] = 20;                       // solenoid timeout seconds / 16
  this->config[2] = rfFreq >> 16;             // Rf freq
  this->config[3] = rfFreq >> 8;              // Rf freq
  this->config[4] = rfFreq;                   // Rf freq
  this->config[5] = 5;                        // Rf power
  this->config[6] = YL_800T_BREATH_CYCLE_2S;  // Rf breath cycle
  this->config[7] = YL_800T_BREATH_TIME_32MS; // Rf breath time
  this->config[8] = 138;                      // Upper battery voltage threshold
  this->config[9] = 135;                      // Lower battery voltage threshold
  this->config[10] = 132;                     // Sleep battery voltage threshold
  this->config[11] = 4;                       // Inverse of half battery check frequency (1/2Hz)
  this->config[12] = 105;                     // Minimum voltage for solenoid operation
  this->config[13] = 50;                      // Solenoid A on pulse width / 500 seconds
  this->config[14] = 50;                      // Solenoid A off pulse width / 500 seconds
  this->config[15] = 50;                      // Solenoid B on pulse width / 500 seconds
  this->config[16] = 50;                      // Solenoid B off pulse width / 500 seconds
}

int RemoteUnitConfig::saveToEeprom()
{
  return 0;
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

uint8_t RemoteUnitConfig::getSolenoidAOnPulseWidth() const
{
  return this->config[13];
}

uint8_t RemoteUnitConfig::getSolenoidAOffPulseWidth() const
{
  return this->config[14];
}

uint8_t RemoteUnitConfig::getSolenoidBOnPulseWidth() const
{
  return this->config[15];
}

uint8_t RemoteUnitConfig::getSolenoidBOffPulseWidth() const
{
  return this->config[16];
}