#ifndef _REMOTE_UNIT_SENSOR_H
#define _REMOTE_UNIT_SENSOR_H

#include <stdint.h>
#include <SoftwareSerial.h>

/**
 * Handles requests for sensor values.
 * Currently supports only RS485 water potential sensor.
 */
class RemoteUnitSensor
{
  const uint8_t txPin;
  const uint8_t reDePin;
  const uint8_t rs485EnablePin;
  SoftwareSerial ss;

public:
  RemoteUnitSensor(
      uint8_t rxPin,
      uint8_t txPin,
      uint8_t reDePin,
      uint8_t rs485EnablePin);

  void setup();

  /**
   * Read the current value of a sensor.
   *
   * Returns non-zero if there's an issue.
   */
  int readValue(uint8_t sensor, uint16_t *valueOut);

private:
  void rs485Off();
  void rs485On();
  void rs485Mode(bool tx);
  void rs485Setup();
};

#endif
