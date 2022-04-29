#ifndef _REMOTE_UNIT_SOLENOIDS_H
#define _REMOTE_UNIT_SOLENOIDS_H
#include "config.h"

#define SOLENOID_COUNT 2
#define SOLENOID_FORCE_FLAG 0x80

struct SolenoidDefinition
{
  uint8_t positivePin;
  uint8_t negativePin;
};

class Solenoids
{
  const RemoteUnitConfig &config;
  const SolenoidDefinition (&definitions)[SOLENOID_COUNT];
  uint8_t state;

public:
  Solenoids(const RemoteUnitConfig &config, const SolenoidDefinition (&definitions)[SOLENOID_COUNT]);
  void setup();
  void sleep();
  void solenoidOn(uint8_t index);
  void solenoidOff(uint8_t index);

  /**
   * Get the current state of all solenoids.
   *
   * Each bit represents the state of a solenoid, starting from solenoid 0 in the least significant bit.
   * The most significant bit SOLENOID_FORCE_FLAG indicates that the actual state of solenoids is unknown.
   */
  uint8_t getState() const;

  /**
   * Sets the current state of all solenoids.
   *
   * Normally solenoids are only activated when their state has changed.
   * If the most significant bit SOLENOID_FORCE_FLAG is set in the current state or the requested state,
   * then current state of solenoids is assumed to be unknown and all solenoids are activated.
   */
  uint8_t setState(uint8_t state);
};
#endif
