#include <Arduino.h>
#include "solenoids.h"

Solenoids::Solenoids(const RemoteUnitConfig &config, const SolenoidDefinition (&definitions)[SOLENOID_COUNT])
    : config(config), definitions(definitions)
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
  const SolenoidDefinition &definition = this->definitions[index];
  uint8_t pulseWidth = index == 0 ? this->config.getSolenoidAOnPulseWidth() : this->config.getSolenoidBOnPulseWidth();
  digitalWrite(definition.positivePin, HIGH);
  digitalWrite(definition.negativePin, LOW);
  delay(pulseWidth * 2);
  digitalWrite(definition.positivePin, LOW);
}

void Solenoids::solenoidOff(uint8_t index)
{
  const SolenoidDefinition &definition = this->definitions[index];
  uint8_t pulseWidth = index == 0 ? this->config.getSolenoidAOffPulseWidth() : this->config.getSolenoidBOffPulseWidth();
  digitalWrite(definition.positivePin, LOW);
  digitalWrite(definition.negativePin, HIGH);
  delay(pulseWidth * 2);
  digitalWrite(definition.negativePin, LOW);
}
