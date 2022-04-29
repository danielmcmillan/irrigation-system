#ifndef _REMOTE_UNIT_SOLENOIDS_H
#define _REMOTE_UNIT_SOLENOIDS_H
#include "config.h"

#define SOLENOID_COUNT 2

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
  uint8_t getState() const;
  uint8_t setState(uint8_t state);
};
#endif
