#include "config.h"
#include <string.h>
#include "yl-800t.h"
#include <EEPROM.h>
#include "crc16.h"
#include "settings.h"

const uint8_t *RemoteUnitConfig::getRaw() const
{
  return this->config + 1;
}

const uint8_t *RemoteUnitConfig::setRaw(const uint8_t *newConfig)
{
  this->config[0] = REMOTE_UNIT_REVISION;
  this->isChanged = memcmp(this->config + 1, newConfig, REMOTE_UNIT_CONFIG_SIZE) != 0;
  memcpy(this->config + 1, newConfig, REMOTE_UNIT_CONFIG_SIZE);
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
  uint32_t rfFreq = 434l * 1l << 14; // 434 MHz
  this->config[0] = REMOTE_UNIT_REVISION;
  this->config[1] = 122;                      // battery multiplier 0.0149
  this->config[2] = 112;                      // solenoid timeout ~15 minutes
  this->config[3] = rfFreq >> 16;             // Rf freq
  this->config[4] = rfFreq >> 8;              // Rf freq
  this->config[5] = rfFreq;                   // Rf freq
  this->config[6] = 5;                        // Rf power
  this->config[7] = YL_800T_BREATH_CYCLE_2S;  // Rf breath cycle
  this->config[8] = YL_800T_BREATH_TIME_32MS; // Rf breath time
  this->config[9] = 138;                      // Upper battery voltage threshold
  this->config[10] = 135;                     // Lower battery voltage threshold
  this->config[11] = 132;                     // Maintain battery voltage threshold
  this->config[12] = 105;                     // Minimum voltage for solenoid operation
  this->config[13] = 50;                      // Solenoid A on pulse width * 2 ms
  this->config[14] = 50;                      // Solenoid A off pulse width * 2 ms
  this->config[15] = 50;                      // Solenoid B on pulse width * 2 ms
  this->config[16] = 50;                      // Solenoid B off pulse width * 2 ms
  uint8_t sensorUpdateInterval = 0;           // Sensor update interval in multiples of 64 timer counts or 0 to disable
  this->config[13] |= (sensorUpdateInterval & 0x01) > 0;
  this->config[14] |= (sensorUpdateInterval & 0x02) > 0;
  this->config[15] |= (sensorUpdateInterval & 0x04) > 0;
  this->config[16] |= (sensorUpdateInterval & 0x08) > 0;

  return 1;
}

int RemoteUnitConfig::persist()
{
  if (this->isChanged)
  {
    uint16_t crc = IrrigationSystem::CRC::crc16(this->config, REMOTE_UNIT_EEPROM_SIZE - 2);
    for (uint8_t i = 0; i < REMOTE_UNIT_EEPROM_SIZE - 2; ++i)
    {
      EEPROM.update(i, this->config[i]);
    }
    EEPROM.update(REMOTE_UNIT_EEPROM_SIZE - 2, crc);
    EEPROM.update(REMOTE_UNIT_EEPROM_SIZE - 1, crc >> 8);
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
  return this->config[1];
}

uint8_t RemoteUnitConfig::getCommunicationTimeout() const
{
  return this->config[2];
}

const uint8_t *RemoteUnitConfig::getRfConfig() const
{
  return this->config + 3;
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdUpper() const
{
  return this->config[9];
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdLower() const
{
  return this->config[10];
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdMaintain() const
{
  return this->config[11];
}

uint8_t RemoteUnitConfig::getBatteryVoltageThresholdDisable() const
{
  return this->config[12];
}

uint8_t RemoteUnitConfig::getSolenoidAOnPulseWidth() const
{
  return this->config[13] & ~1;
}

uint8_t RemoteUnitConfig::getSolenoidAOffPulseWidth() const
{
  return this->config[14] & ~1;
}

uint8_t RemoteUnitConfig::getSolenoidBOnPulseWidth() const
{
  return this->config[15] & ~1;
}

uint8_t RemoteUnitConfig::getSolenoidBOffPulseWidth() const
{
  return this->config[16] & ~1;
}

uint8_t RemoteUnitConfig::getSensorUpdateInterval() const
{
  return (this->config[13] & 1) + (this->config[14] & 1) * 0x02 + (this->config[15] & 1) * 0x04 + (this->config[16] & 1) * 0x08;
}
