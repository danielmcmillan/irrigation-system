#ifndef _CRC16_H
#define _CRC16_H
#include <Arduino.h>

namespace IrrigationSystem
{
    namespace CRC
    {
        /**
         * The initial CRC value used is 0xffff.
         */
        static const uint16_t crc16Init = 0xffff;
        /**
         * Update a CRC value based on the next byte of data.
         * Algorithm and polynomial are CRC-16/MODBUS.
         * Uses polynomial 0x8005 with input and output reflection.
         * It should be called initially with `crc16Init` and the first byte of data.
         */
        uint16_t crc16Update(uint16_t crc, uint8_t data);
        /**
         * Calculates a CRC-16/MODBUS value for a given set of data.
         */
        uint16_t crc16(const uint8_t *data, uint16_t length);
    }
}

#endif
