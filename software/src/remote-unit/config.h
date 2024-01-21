#ifndef _REMOTE_UNIT_CONFIG_H
#define _REMOTE_UNIT_CONFIG_H
#include <inttypes.h>

#define REMOTE_UNIT_CONFIG_SIZE 16
#define REMOTE_UNIT_EEPROM_SIZE (1 + REMOTE_UNIT_CONFIG_SIZE + 2)
#define REMOTE_UNIT_RF_CONFIG_SIZE 6

class RemoteUnitConfig
{
  /**
   * The config data as it appears in the EEPROM.
   * First byte is the remote unit firmware revision which last wrote the data.
   * Last two bytes are a Modbus CRC-16 checksum.
   */
  uint8_t config[REMOTE_UNIT_EEPROM_SIZE] = {0};
  // Whether current config is different to the persisted config.
  bool isChanged = true;

public:
  /**
   * Get the configuration data.
   * Length of the return value is `REMOTE_UNIT_CONFIG_SIZE`.
   */
  const uint8_t *getRaw() const;
  /**
   * Sets the configuration data.
   * `newConfig` should have length `REMOTE_UNIT_CONFIG_SIZE`.
   */
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

  bool getIsChanged() const;

  uint8_t getBatteryCalibration() const;
  uint8_t getCommunicationTimeout() const;
  const uint8_t *getRfConfig() const;
  uint8_t getBatteryVoltageThresholdUpper() const;
  uint8_t getBatteryVoltageThresholdLower() const;
  uint8_t getBatteryVoltageThresholdMaintain() const;
  uint8_t getBatteryVoltageThresholdDisable() const;
  uint8_t getSolenoidAOnPulseWidth() const;
  uint8_t getSolenoidAOffPulseWidth() const;
  uint8_t getSolenoidBOnPulseWidth() const;
  uint8_t getSolenoidBOffPulseWidth() const;
  uint8_t getSensorUpdateInterval() const;
};
#endif
