#include "config.h"
#include <string.h>
#include "yl-800t.h"
#include <EEPROM.h>
#include "crc16.h"

const uint8_t *RemoteUnitConfig::getRaw() const
{
  return this->config;
}

const uint8_t *RemoteUnitConfig::setRaw(const uint8_t *newConfig)
{
  this->isChanged = memcmp(this->config, newConfig, REMOTE_UNIT_CONFIG_SIZE) != 0;
  memcpy(this->config, newConfig, REMOTE_UNIT_CONFIG_SIZE);
  return this->config;
}

int RemoteUnitConfig::load()
{
  for (uint8_t i = 0; i < REMOTE_UNIT_EEPROM_SIZE; ++i)
  {
    this->config[i] = EEPROM.read(i);
  }
  if (IrrigationSystem::CRC::crc16(this->config, REMOTE_UNIT_EEPROM_SIZE) == 0)
  {
    // EEPROM data looks good
    this->isChanged = false;
    return 0;
  }

  // EEPROM data is not valid, load default values
  uint32_t rfFreq = 434l * 1l << 14;          // 434 MHz
  this->config[0] = 122;                      // battery multiplier 0.0149
  this->config[1] = 96;                       // solenoid timeout ~840 seconds
  this->config[2] = rfFreq >> 16;             // Rf freq
  this->config[3] = rfFreq >> 8;              // Rf freq
  this->config[4] = rfFreq;                   // Rf freq
  this->config[5] = 5;                        // Rf power
  this->config[6] = YL_800T_BREATH_CYCLE_2S;  // Rf breath cycle
  this->config[7] = YL_800T_BREATH_TIME_32MS; // Rf breath time
  this->config[8] = 138;                      // Upper battery voltage threshold
  this->config[9] = 135;                      // Lower battery voltage threshold
  this->config[10] = 132;                     // Maintain battery voltage threshold
  this->config[11] = 105;                     // Minimum voltage for solenoid operation
  this->config[12] = 50;                      // Solenoid A on pulse width * 2 ms
  this->config[13] = 50;                      // Solenoid A off pulse width * 2 ms
  this->config[14] = 50;                      // Solenoid B on pulse width * 2 ms
  this->config[15] = 50;                      // Solenoid B off pulse width * 2 ms

  return 1;
}

int RemoteUnitConfig::persist()
{
  if (this->isChanged)
  {
    uint16_t crc = IrrigationSystem::CRC::crc16(this->config, REMOTE_UNIT_CONFIG_SIZE);
    for (uint8_t i = 0; i < REMOTE_UNIT_CONFIG_SIZE; ++i)
    {
      EEPROM.update(i, this->config[i]);
    }
    EEPROM.update(REMOTE_UNIT_CONFIG_SIZE, crc);
    EEPROM.update(REMOTE_UNIT_CONFIG_SIZE + 1, crc >> 8);
    this->isChanged = false;
  }
  return 0;
}

bool RemoteUnitConfig::getIsChanged() const
{
  return this->isChanged;
}

uint8_t RemoteUnitConfig::getBatteryCalibration() const
{
  return this->config[0];
}

uint8_t RemoteUnitConfig::getCommunicationTimeout() const
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

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdMaintain() const
{
  return this->config[10];
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdDisable() const
{
  return this->config[11];
}

uint8_t RemoteUnitConfig::getSolenoidAOnPulseWidth() const
{
  return this->config[12];
}

uint8_t RemoteUnitConfig::getSolenoidAOffPulseWidth() const
{
  return this->config[13];
}

uint8_t RemoteUnitConfig::getSolenoidBOnPulseWidth() const
{
  return this->config[14];
}

uint8_t RemoteUnitConfig::getSolenoidBOffPulseWidth() const
{
  return this->config[15];
}
