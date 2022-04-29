#ifndef _REMOTE_UNIT_CONFIG_H
#define _REMOTE_UNIT_CONFIG_H
#include <inttypes.h>

#define REMOTE_UNIT_CONFIG_SIZE 17

class RemoteUnitConfig
{
  uint8_t config[REMOTE_UNIT_CONFIG_SIZE] = {0};

public:
  const uint8_t *getRaw() const;
  const uint8_t *setRaw(const uint8_t *newConfig);

  /**
   * Load the previously persisted config.
   */
  void loadFromEeprom();

  /**
   * Persist the current config.
   *
   * Returns non-zero on failure.
   */
  int saveToEeprom();

  uint8_t getBatteryCalibration() const;
  uint8_t getSolenoidTimeout() const;
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
