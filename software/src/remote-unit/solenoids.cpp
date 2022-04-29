#include <Arduino.h>
#include "solenoids.h"

Solenoids::Solenoids(const RemoteUnitConfig &config, const SolenoidDefinition (&definitions)[SOLENOID_COUNT])
    : config(config), definitions(definitions), state(SOLENOID_FORCE_FLAG)
{
}

void Solenoids::setup()
{
  for (uint8_t i = 0; i < SOLENOID_COUNT; ++i)
  {
    const SolenoidDefinition &definition = this->definitions[i];
    pinMode(definition.positivePin, OUTPUT);
    pinMode(definition.negativePin, OUTPUT);
    digitalWrite(definition.positivePin, LOW);
    digitalWrite(definition.negativePin, LOW);
  }
}

void Solenoids::sleep()
{
  for (uint8_t i = 0; i < SOLENOID_COUNT; ++i)
  {
    const SolenoidDefinition &definition = this->definitions[i];
    digitalWrite(definition.positivePin, LOW);
    digitalWrite(definition.negativePin, LOW);
  }
}

void Solenoids::solenoidOn(uint8_t index)
{
  if (index < SOLENOID_COUNT)
  {
    const SolenoidDefinition &definition = this->definitions[index];
    uint8_t pulseWidth = index == 0 ? this->config.getSolenoidAOnPulseWidth() : this->config.getSolenoidBOnPulseWidth();
    digitalWrite(definition.positivePin, HIGH);
    digitalWrite(definition.negativePin, LOW);
    delay(pulseWidth * 2);
    digitalWrite(definition.positivePin, LOW);
  }
  this->state |= 1 << index;
}

void Solenoids::solenoidOff(uint8_t index)
{
  if (index < SOLENOID_COUNT)
  {
    const SolenoidDefinition &definition = this->definitions[index];
    uint8_t pulseWidth = index == 0 ? this->config.getSolenoidAOffPulseWidth() : this->config.getSolenoidBOffPulseWidth();
    digitalWrite(definition.positivePin, LOW);
    digitalWrite(definition.negativePin, HIGH);
    delay(pulseWidth * 2);
    digitalWrite(definition.negativePin, LOW);
  }
  this->state &= ~(1 << index);
}

uint8_t Solenoids::getState() const
{
  return this->state;
}

uint8_t Solenoids::setState(uint8_t state)
{
  // SOLENOID_FORCE_FLAG bit of state indicates solenoid states should be ignored
  // This is set as initial state to indicate solenoids states are unknown on startup
  // It can be set in input state to request forcing solenoid activation on demand
  if (((this->state | state) & SOLENOID_FORCE_FLAG) != 0)
  {
    // Assume current state is opposite of desired to activate all solenoids regardless of current state
    // Also clear most significant bit since state of all solenoids will now be known
    this->state = (~state) & ~SOLENOID_FORCE_FLAG;
  }
  uint8_t toTurnOn = state & ~this->state;
  uint8_t toTurnOff = this->state & ~state;

  for (uint8_t i = 0; i < 7; ++i)
  {
    if ((toTurnOn & (1 << i)) != 0)
    {
      this->solenoidOn(i);
    }
    if ((toTurnOff & (1 << i)) != 0)
    {
      this->solenoidOff(i);
    }
  }

  return this->state;
}
