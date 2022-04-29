#include <inttypes.h>

#define REMOTE_UNIT_CONFIG_SIZE 15

class RemoteUnitConfig
{
  uint8_t config[REMOTE_UNIT_CONFIG_SIZE] = {0};

public:
  const uint8_t *getRaw() const;
  const uint8_t *setRaw(const uint8_t *newConfig);

  void loadFromEeprom();
  void saveToEeprom();

  uint8_t getBatteryCalibration() const;
  uint8_t getSolenoidTimeout() const;
  const uint8_t *getRfConfig() const;
  uint8_t getBatteryVoltageThresholdUpper() const;
  uint8_t getBatteryVoltageThresholdLower() const;
  uint8_t getBatteryVoltageThresholdSleep() const;
  uint8_t getBatteryVoltageThresholdDisable() const;
  uint8_t getBatteryVoltageCheckFrequency() const;
  uint8_t getSolenoidOnPulseWidth() const;
  uint8_t getSolenoidOffPulseWidth() const;
};
