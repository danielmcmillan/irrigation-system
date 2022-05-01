#include <Arduino.h>
#include "battery.h"
#include <stdlib.h>

#define VOLTAGE_READING_COUNT 15
#define MIN_EXPECTED_VOLTAGE 90
#define MAX_EXPECTED_VOLTAGE 150
#define NON_MAINTAIN_CHECK_COUNTS 7 // ~1 minute

int vCompare(const void *elem1, const void *elem2)
{
  int f = *((uint16_t *)elem1);
  int s = *((uint16_t *)elem2);
  return (f > s) - (f < s);
}

RemoteUnitBattery::RemoteUnitBattery(const RemoteUnitConfig &config, uint8_t analogVoltageSensePin, uint8_t chargeDisablePin)
    : config(config), analogVoltageSensePin(analogVoltageSensePin), chargeDisablePin(chargeDisablePin),
      lastVoltage(0), lastUpdateCounts(0)
{
}

void RemoteUnitBattery::setup()
{
  analogReference(INTERNAL);
  // pinMode(this->analogVoltageSensePin, INPUT);
  pinMode(this->chargeDisablePin, OUTPUT);
  // Enable charge initially
  digitalWrite(this->chargeDisablePin, LOW);
}

int RemoteUnitBattery::update(unsigned long counts)
{
  // Update at each update when maintaining, otherwise after NON_MAINTAIN_CHECK_COUNTS counts
  if (this->shouldMaintain() || this->lastUpdateCounts == 0 || (counts - this->lastUpdateCounts) > NON_MAINTAIN_CHECK_COUNTS)
  {
    this->lastUpdateCounts = counts;
    uint16_t raw = this->readRawVoltage();
    this->lastVoltage = ((uint32_t)raw * (uint32_t)this->config.getBatteryCalibration() * 10) >> 13;

    // Update charge state
    if (this->lastVoltage < this->config.getBatteryVoltageThresholdLower())
    {
      digitalWrite(this->chargeDisablePin, LOW);
    }
    else if (this->lastVoltage > this->config.getBatteryVoltageThresholdUpper())
    {
      digitalWrite(this->chargeDisablePin, HIGH);
    }
  }
  // Return non-zero when voltage is outside expected range
  return this->lastVoltage < MIN_EXPECTED_VOLTAGE || this->lastVoltage > MAX_EXPECTED_VOLTAGE;
}

uint16_t RemoteUnitBattery::readRawVoltage() const
{
  // Ignore the first reading
  analogRead(this->analogVoltageSensePin);
  // Take multiple readings
  uint16_t readings[VOLTAGE_READING_COUNT];
  for (uint8_t i = 0; i < VOLTAGE_READING_COUNT; ++i)
  {
    readings[i] = analogRead(this->analogVoltageSensePin);
  }
  // Sum all except the lowest 2 and highest 2
  qsort(readings, VOLTAGE_READING_COUNT, 2, vCompare);
  uint16_t sum = 0;
  for (uint8_t i = 2; i < VOLTAGE_READING_COUNT - 2; ++i)
  {
    sum += readings[i];
  }
  // Return the average
  return sum / (VOLTAGE_READING_COUNT - 4);
}

uint8_t RemoteUnitBattery::getVoltage() const
{
  return this->lastVoltage;
}

bool RemoteUnitBattery::shouldMaintain() const
{
  return this->lastVoltage >= this->config.getBatteryVoltageThresholdMaintain();
}

bool RemoteUnitBattery::shouldDisable() const
{
  return this->lastVoltage < this->config.getBatteryVoltageThresholdDisable();
}
