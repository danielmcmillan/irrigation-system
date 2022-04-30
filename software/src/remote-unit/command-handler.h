#ifndef _REMOTE_UNIT_COMMANDS_H
#define _REMOTE_UNIT_COMMANDS_H
#include <inttypes.h>
#include "config.h"
#include "rf-module.h"
#include "solenoids.h"
#include "battery.h"
#include "faults.h"

class RemoteUnitCommandHandler
{
  RemoteUnitConfig &config;
  RemoteUnitRfModule &rfModule;
  Solenoids &solenoids;
  RemoteUnitBattery &battery;
  RemoteUnitFaults &faults;
  volatile unsigned long &counts;

public:
  RemoteUnitCommandHandler(RemoteUnitConfig &config, RemoteUnitRfModule &rfModule, Solenoids &solenoids, RemoteUnitBattery &battery, RemoteUnitFaults &faults, volatile unsigned long &counts);

  /**
   * Gets the state for all solenoids.
   *
   * Returns non-zero on failure.
   */

  int getSolenoidState(uint8_t *solenoidStateOut) const;

  /**
   * Sets the state of all solenoids.
   *
   * Returns non-zero on failure.
   */
  int setSolenoidState(uint8_t state, uint8_t *newSolenoidStateOut) const;

  /**
   * Retrieves the current battery voltage, in tenths of a volt.
   *
   * Returns non-zero on failure.
   */
  int getBatteryVoltage(uint8_t *voltageOut) const;

  /**
   * Retrieves the current uncalibrated battery voltage reading.
   *
   * Returns non-zero on failure.
   */
  int getBatteryRaw(uint16_t *batteryRawOut) const;

  /**
   * Gets the faults byte.
   *
   * Returns non-zero on failure.
   */
  int getFaults(uint8_t *faultsOut) const;

  /**
   * Clears all faults.
   *
   * Returns non-zero on failure.
   */
  int clearFaults(uint8_t *faultsOut) const;

  /**
   * Gets the reception strength in dB + 164
   *
   * Returns non-zero on failure.
   */
  int getSignalStrength(uint8_t *signalStrengthOut) const;

  /**
   * Gets the 13 byte config value.
   */
  int getConfig(uint8_t *configDataOut) const;

  /**
   * Sets the 13 byte config value.
   * The config is not persisted, so the change will revert after reset or a period of no communication.
   */
  int setConfig(const uint8_t *configData, uint8_t *configDataOut) const;

  /**
   * Persists the current config, so that it does not revert after reset or a period of no communication.
   */
  int persistConfig(uint8_t *configDataOut) const;

  /**
   * Applies the current config to the RF module.
   */
  int applyRfConfig(uint8_t *configDataOut) const;

  /**
   * Gets the approximate uptime in multiples of 8 seconds.
   */
  int getTimer(uint32_t *timerOut) const;
};
#endif
