#include "rf-module.h"
#include <Arduino.h>
#include "yl-800t.h"

#define RF_MODULE_RESPONSE_TIMEOUT 2000

RemoteUnitRfModule::RemoteUnitRfModule(uint16_t nodeId, const RemoteUnitConfig &config, uint8_t rfEnablePin)
    : nodeId(nodeId), config(config), rfEnablePin(rfEnablePin)
{
}

void RemoteUnitRfModule::setup() const
{
  pinMode(this->rfEnablePin, OUTPUT);
}

void RemoteUnitRfModule::sleep() const
{
  digitalWrite(this->rfEnablePin, HIGH);
}

void RemoteUnitRfModule::wake() const
{
  digitalWrite(this->rfEnablePin, LOW);
}

int RemoteUnitRfModule::applyConfig() const
{
  this->wake();
  Serial.setTimeout(RF_MODULE_RESPONSE_TIMEOUT);
  const uint8_t *rfConfig = config.getRfConfig();
  YL800TReadWriteAllParameters params = {
      .serialBaudRate = YL_800T_BAUD_RATE_9600,
      .serialParity = YL_800T_PARITY_NONE,
      .rfFrequency = (uint32_t)rfConfig[0] << 16 | (uint32_t)rfConfig[1] << 8 | rfConfig[2],
      .rfSpreadingFactor = YL_800T_RF_SPREADING_FACTOR_2048,
      .mode = YL_800T_RF_MODE_NODE,
      .rfBandwidth = YL_800T_RF_BANDWIDTH_125K,
      .nodeId = this->nodeId,
      .netId = 0,
      .rfTransmitPower = rfConfig[3],
      .breathCycle = rfConfig[4],
      .breathTime = rfConfig[5]};
  uint8_t message[25] = {0};
  uint8_t length = yl800tSendWriteAllParameters(&params, message);
  delay(100); // TODO why?
  Serial.write(message, length);
  Serial.flush();
  Serial.readBytes(message, 25);
  // TODO check response?
  return 0;
}

int RemoteUnitRfModule::readSignalStrength(uint8_t *signalStrengthOut) const
{
  // Wake up the RF module
  digitalWrite(this->rfEnablePin, LOW);
  Serial.setTimeout(RF_MODULE_RESPONSE_TIMEOUT);
  uint8_t message[16] = {0};
  uint8_t length = yl800tSendReadSignalStrength(message);
  Serial.write(message, length);

  size_t read = Serial.readBytes(message, 16);
  if (read == 0)
  {
    return -1;
  }
  return yl800tReceiveReadSignalStrength(message, signalStrengthOut);
}
