#ifndef _REMOTE_UNIT_BATTERY_H
#define _REMOTE_UNIT_BATTERY_H

#include <stdint.h>
#include "config.h"

class RemoteUnitBattery
{
  const RemoteUnitConfig &config;
  const uint8_t analogVoltageSensePin;
  const uint8_t chargeDisablePin;

  uint8_t lastVoltage;
  unsigned long lastUpdateCounts;

public:
  RemoteUnitBattery(const RemoteUnitConfig &config, uint8_t analogVoltageSensePin, uint8_t chargeDisablePin);

  void setup();

  /**
   * Read battery voltage and update charge state.
   *
   * Returns non-zero if there's an issue.
   */
  int update(unsigned long now);

  /**
   * Get the latest battery voltage reading in tenths of a volt.
   */
  uint8_t getVoltage() const;

  /**
   * Read the battery voltage and return the raw 10-bit reading without calibration applied.
   */
  uint16_t readRawVoltage();

  /**
   * Check whether the battery requires attention to maintain voltage.
   * If true, then update should continue being called at least as frequently as the configured check frequency.
   */
  bool shouldMaintain() const;

  /**
   * Check whether the critical low battery threshold is reached and solenoid control should be disabled.
   */
  bool shouldDisable() const;
};

#endif
