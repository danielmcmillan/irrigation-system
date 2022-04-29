#ifndef _REMOTE_UNIT_COMMANDS_H
#define _REMOTE_UNIT_COMMANDS_H
#include <inttypes.h>
#include "solenoids.h"

class RemoteUnitCommandHandler
{
  Solenoids &solenoids;

public:
  RemoteUnitCommandHandler(Solenoids &solenoids);

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
   */
  int setConfig(const uint8_t *configData, uint8_t *configDataOut) const;
};
#endif
