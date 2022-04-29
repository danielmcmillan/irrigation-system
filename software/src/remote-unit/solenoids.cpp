#include <Arduino.h>
#include "solenoids.h"

Solenoids::Solenoids(const RemoteUnitConfig &config, const SolenoidDefinition (&definitions)[SOLENOID_COUNT])
    : config(config), definitions(definitions), state(0)
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
  state = 0;
}

void Solenoids::sleep()
{
  for (uint8_t i = 0; i < SOLENOID_COUNT; ++i)
  {
    const SolenoidDefinition &definition = this->definitions[i];
    digitalWrite(definition.positivePin, LOW);
    digitalWrite(definition.negativePin, LOW);
  }
  state = 0;
}

void Solenoids::solenoidOn(uint8_t index)
{
  const SolenoidDefinition &definition = this->definitions[index];
  uint8_t pulseWidth = index == 0 ? this->config.getSolenoidAOnPulseWidth() : this->config.getSolenoidBOnPulseWidth();
  digitalWrite(definition.positivePin, HIGH);
  digitalWrite(definition.negativePin, LOW);
  delay(pulseWidth * 2);
  digitalWrite(definition.positivePin, LOW);
  state |= 1 << index;
}

void Solenoids::solenoidOff(uint8_t index)
{
  const SolenoidDefinition &definition = this->definitions[index];
  uint8_t pulseWidth = index == 0 ? this->config.getSolenoidAOffPulseWidth() : this->config.getSolenoidBOffPulseWidth();
  digitalWrite(definition.positivePin, LOW);
  digitalWrite(definition.negativePin, HIGH);
  delay(pulseWidth * 2);
  digitalWrite(definition.negativePin, LOW);
  state &= ~(1 << index);
}

uint8_t Solenoids::getState() const
{
  return this->state;
}

uint8_t Solenoids::setState(uint8_t state)
{
  uint8_t toTurnOn = state & ~this->state;
  uint8_t toTurnOff = this->state & ~state;

  for (uint8_t i = 0; i < 8; ++i)
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

  this->state = state;
  return state;
}
