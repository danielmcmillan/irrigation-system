#ifndef _REMOTE_UNIT_RF_MODULE_H
#define _REMOTE_UNIT_RF_MODULE_H

#include "config.h"

class RemoteUnitRfModule
{
  const uint16_t nodeId;
  const RemoteUnitConfig &config;
  const uint8_t rfEnablePin;

public:
  RemoteUnitRfModule(uint16_t nodeId, const RemoteUnitConfig &config, uint8_t rfEnablePin);

  void setup() const;

  void sleep() const;

  void wake() const;

  /**
   * Writes the current configuration values to the RF module.
   *
   * Returns non-zero on failure.
   */
  int applyConfig() const;

  /**
   * Reads the last signal strength from the RF module.
   *
   * Returns non-zero on failure.
   */
  int readSignalStrength(uint8_t *signalStrengthOut) const;
};

#endif
