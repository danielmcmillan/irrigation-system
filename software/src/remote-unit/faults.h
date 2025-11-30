#ifndef _REMOTE_UNIT_FAULTS_H
#define _REMOTE_UNIT_FAULTS_H
#include <stdint.h>

enum class RemoteUnitFault : uint8_t
{
  SolenoidTimeoutOccurred = 0,
  SolenoidNotActivated,
  BatteryVoltageError,
  ConfigReadFailed,
  ConfigureRfModuleFailed,
  SolenoidLowCurrent
};

class RemoteUnitFaults
{
  uint8_t faults = 0;

public:
  void setFault(RemoteUnitFault fault);
  uint8_t getFaults() const;
  void clear();
};

#endif
