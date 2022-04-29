#ifndef _REMOTE_UNIT_COMMANDS_H
#define _REMOTE_UNIT_COMMANDS_H
#include <inttypes.h>
/**
 * Gets the state for all solenoids.
 *
 * Returns non-zero on failure.
 */

int getSolenoidState(uint8_t *solenoidStateOut);

/**
 * Sets the state of all solenoids.
 *
 * Returns non-zero on failure.
 */
int setSolenoidState(uint8_t state, uint8_t *newSolenoidStateOut);

/**
 * Retrieves the current battery voltage, in tenths of a volt.
 *
 * Returns non-zero on failure.
 */
int getBatteryVoltage(uint8_t *voltageOut);

/**
 * Retrieves the current uncalibrated battery voltage reading.
 *
 * Returns non-zero on failure.
 */
int getBatteryRaw(uint16_t *batteryRawOut);

/**
 * Gets the faults byte.
 *
 * Returns non-zero on failure.
 */
int getFaults(uint8_t *faultsOut);

/**
 * Clears all faults.
 *
 * Returns non-zero on failure.
 */
int clearFaults(uint8_t *faultsOut);

/**
 * Gets the reception strength in dB + 164
 *
 * Returns non-zero on failure.
 */
int getSignalStrength(uint8_t *signalStrengthOut);

/**
 * Gets the 13 byte config value.
 */
int getConfig(uint8_t *configDataOut);

/**
 * Sets the 13 byte config value.
 */
int setConfig(const uint8_t *configData, uint8_t *configDataOut);
#endif
