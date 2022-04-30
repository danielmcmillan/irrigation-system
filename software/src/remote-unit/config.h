#ifndef _REMOTE_UNIT_CONFIG_H
#define _REMOTE_UNIT_CONFIG_H
#include <inttypes.h>

#define REMOTE_UNIT_CONFIG_SIZE 17
#define REMOTE_UNIT_EEPROM_SIZE REMOTE_UNIT_CONFIG_SIZE + 2
#define REMOTE_UNIT_RF_CONFIG_SIZE 6

class RemoteUnitConfig
{
  uint8_t config[REMOTE_UNIT_EEPROM_SIZE] = {0};
  // Whether current config is different to the persisted config.
  bool isChanged = true;

public:
  const uint8_t *getRaw() const;
  const uint8_t *setRaw(const uint8_t *newConfig);

  /**
   * Load the previously persisted config if it has not been loaded or has changed.
   *
   * Loads defaults and returns non-zero on error.
   */
  int load();

  /**
   * Persist the current config.
   * Config is written to EEPROM.
   *
   * Returns non-zero on failure.
   */
  int persist();

  uint8_t getBatteryCalibration() const;
  uint8_t getCommunicationTimeout() const;
  const uint8_t *getRfConfig() const;
  uint8_t getBatteryVoltageThresholdUpper() const;
  uint8_t getBatteryVoltageThresholdLower() const;
  uint8_t getBatteryVoltageThresholdSleep() const;
  uint8_t getBatteryVoltageThresholdDisable() const;
  uint8_t getBatteryVoltageCheckFrequency() const;
  uint8_t getSolenoidAOnPulseWidth() const;
  uint8_t getSolenoidAOffPulseWidth() const;
  uint8_t getSolenoidBOnPulseWidth() const;
  uint8_t getSolenoidBOffPulseWidth() const;
};
#endif
