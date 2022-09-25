#ifndef _BINARY_UTIL_H
#define _BINARY_UTIL_H
#include <inttypes.h>

namespace IrrigationSystem
{
  inline uint16_t read16LE(const uint8_t *data)
  {
    return (uint16_t)data[0] | (uint16_t)data[1] << 8;
  }

  inline void write16LE(uint8_t *data, uint16_t value)
  {
    data[0] = (uint8_t)value;
    data[1] = (uint8_t)(value >> 8);
  }
}

#endif
