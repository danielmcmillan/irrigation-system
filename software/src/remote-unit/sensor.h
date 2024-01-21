#ifndef _REMOTE_UNIT_SENSOR_H
#define _REMOTE_UNIT_SENSOR_H

#include <stdint.h>
#include <SoftwareSerial.h>
#include "config.h"

/**
 * Handles requests for sensor values.
 * Currently supports only RS485 water potential sensor.
 */
class RemoteUnitSensor
{
public:
  struct SensorReadingResult
  {
    bool success : 1;
    bool unread : 1;
    int8_t error : 6;
  };

private:
  const RemoteUnitConfig &config;
  const uint8_t txPin;
  const uint8_t reDePin;
  const uint8_t rs485EnablePin;
  SoftwareSerial ss;

  /** The number of counts when the last sensor update was performed. */
  unsigned long lastUpdateCounts;
  /** Whether the sensor was powered up in the previous interval and is now pending to be read from. */
  bool readPending;
  /** Results of the latest sensor reading. */
  SensorReadingResult lastResult;
  uint16_t sensorValue;

public:
  RemoteUnitSensor(
      const RemoteUnitConfig &config,
      uint8_t rxPin,
      uint8_t txPin,
      uint8_t reDePin,
      uint8_t rs485EnablePin);

  void setup();

  /**
   * Reads sensor values if they are due to be updated.
   *
   * Returns non-zero if there's an issue.
   */
  int update(unsigned long counts);

  /**
   * Get the latest sensor reading and result.
   */
  SensorReadingResult getValue(uint8_t sensor, uint16_t *valueOut);

  /** Schedule the sensor to be read on the next update. */
  void scheduleUpdate();

  /**
   * Read the current value of a sensor.
   *
   * Returns non-zero error code if it fails.
   */
  uint8_t readValue(uint8_t sensor, uint16_t *valueOut);

private:
  void rs485Off();
  void rs485On();
  void rs485Mode(bool tx);
  void rs485Setup();
};

#endif
