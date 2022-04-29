#include "faults.h"

void RemoteUnitFaults::setFault(RemoteUnitFault fault)
{
  this->faults |= 1 << (uint8_t)fault;
}

uint8_t RemoteUnitFaults::getFaults() const
{
  return this->faults;
}

void RemoteUnitFaults::clear()
{
  this->faults = 0;
}
