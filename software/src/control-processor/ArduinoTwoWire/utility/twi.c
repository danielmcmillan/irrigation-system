/*
  twi.c - TWI/I2C library for Wiring & Arduino
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 2012 by Todd Krein (todd@krein.org) to implement repeated starts
  Modified 2020 by Greyson Christoforo (grey@christoforo.net) to implement timeouts
*/

#include <math.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <compat/twi.h>
#include "Arduino.h" // for digitalWrite and micros

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#include "pins_arduino.h"
#include "twi.h"

static volatile uint8_t twi_state;
static volatile uint8_t twi_slarw;
static volatile uint8_t twi_sendStop;   // should the transaction end with a stop
static volatile uint8_t twi_inRepStart; // in the middle of a repeated start

// twi_timeout_us > 0 prevents the code from getting stuck in various while loops here
// if twi_timeout_us == 0 then timeout checking is disabled (the previous Wire lib behavior)
// at some point in the future, the default twi_timeout_us value could become 25000
// and twi_do_reset_on_timeout could become true
// to conform to the SMBus standard
// http://smbus.org/specs/SMBus_3_1_20180319.pdf
static volatile uint32_t twi_timeout_us = 0ul;
static volatile bool twi_timed_out_flag = false;      // a timeout has been seen
static volatile bool twi_do_reset_on_timeout = false; // reset the TWI registers on timeout

static void (*twi_onSlaveTransmit)(uint8_t *, uint8_t, uint8_t *, uint8_t *);

static uint8_t twi_txBuffer[TWI_TX_BUFFER_LENGTH];
static volatile uint8_t twi_txBufferIndex;
static volatile uint8_t twi_txBufferLength;

static uint8_t twi_rxBuffer[TWI_RX_BUFFER_LENGTH];
static volatile uint8_t twi_rxBufferIndex;

static volatile uint8_t twi_error;

/*
 * Function twi_init
 * Desc     readys twi pins and sets twi bitrate
 * Input    none
 * Output   none
 */
void twi_init(void)
{
  // initialize state
  twi_state = TWI_READY;
  twi_sendStop = true; // default value
  twi_inRepStart = false;

  // activate internal pullups for twi.
  digitalWrite(SDA, 1);
  digitalWrite(SCL, 1);

  // initialize twi prescaler and bit rate
  cbi(TWSR, TWPS0);
  cbi(TWSR, TWPS1);
  TWBR = ((F_CPU / TWI_FREQ) - 16) / 2;

  /* twi bit rate formula from atmega128 manual pg 204
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  note: TWBR should be 10 or higher for master mode
  It is 72 for a 16mhz Wiring board with 100kHz TWI */

  // enable twi module, acks, and twi interrupt
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);
}

/*
 * Function twi_disable
 * Desc     disables twi pins
 * Input    none
 * Output   none
 */
void twi_disable(void)
{
  // disable twi module, acks, and twi interrupt
  TWCR &= ~(_BV(TWEN) | _BV(TWIE) | _BV(TWEA));

  // deactivate internal pullups for twi.
  digitalWrite(SDA, 0);
  digitalWrite(SCL, 0);
}

/*
 * Function twi_slaveInit
 * Desc     sets slave address and enables interrupt
 * Input    none
 * Output   none
 */
void twi_setAddress(uint8_t address)
{
  // set twi slave address (skip over TWGCE bit)
  TWAR = address << 1;
}

/*
 * Function twi_setClock
 * Desc     sets twi bit rate
 * Input    Clock Frequency
 * Output   none
 */
void twi_setFrequency(uint32_t frequency)
{
  TWBR = ((F_CPU / frequency) - 16) / 2;

  /* twi bit rate formula from atmega128 manual pg 204
  SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  note: TWBR should be 10 or higher for master mode
  It is 72 for a 16mhz Wiring board with 100kHz TWI */
}

/*
 * Function twi_readFrom
 * Desc     attempts to become twi bus master and read a
 *          series of bytes from a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes to read into array
 *          sendStop: Boolean indicating whether to send a stop at the end
 * Output   number of bytes read
 */
uint8_t twi_readFrom(uint8_t address, uint8_t *data, uint8_t length, uint8_t sendStop)
{
  return 0;
}

/*
 * Function twi_writeTo
 * Desc     attempts to become twi bus master and write a
 *          series of bytes to a device on the bus
 * Input    address: 7bit i2c device address
 *          data: pointer to byte array
 *          length: number of bytes in array
 *          wait: boolean indicating to wait for write or not
 *          sendStop: boolean indicating whether or not to send a stop at the end
 * Output   0 .. success
 *          1 .. length to long for buffer
 *          2 .. address send, NACK received
 *          3 .. data send, NACK received
 *          4 .. other twi error (lost bus arbitration, bus error, ..)
 *          5 .. timeout
 */
uint8_t twi_writeTo(uint8_t address, uint8_t *data, uint8_t length, uint8_t wait, uint8_t sendStop)
{
  return 0;
}

/*
 * Function twi_attachSlaveTxEvent
 * Desc     sets function called before a slave write operation
 * Input    function: callback function to use
 * Output   none
 */
void twi_attachSlaveTxEvent(void (*function)(uint8_t *, uint8_t, uint8_t *, uint8_t *))
{
  twi_onSlaveTransmit = function;
}

/*
 * Function twi_reply
 * Desc     sends byte or readys receive line
 * Input    ack: byte indicating to ack or to nack
 * Output   none
 */
void twi_reply(uint8_t ack)
{
  // transmit master read ready signal, with or without ack
  if (ack)
  {
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
  }
  else
  {
    TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
  }
}

/*
 * Function twi_stop
 * Desc     relinquishes bus master status
 * Input    none
 * Output   none
 */
void twi_stop(void)
{
  // send stop condition
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT) | _BV(TWSTO);

  // wait for stop condition to be exectued on bus
  // TWINT is not set after a stop condition!
  // We cannot use micros() from an ISR, so approximate the timeout with cycle-counted delays
  const uint8_t us_per_loop = 8;
  uint32_t counter = (twi_timeout_us + us_per_loop - 1) / us_per_loop; // Round up
  while (TWCR & _BV(TWSTO))
  {
    if (twi_timeout_us > 0ul)
    {
      if (counter > 0ul)
      {
        _delay_us(10);
        counter--;
      }
      else
      {
        twi_handleTimeout(twi_do_reset_on_timeout);
        return;
      }
    }
  }

  // update twi state
  twi_state = TWI_READY;
}

/*
 * Function twi_releaseBus
 * Desc     releases bus control
 * Input    none
 * Output   none
 */
void twi_releaseBus(void)
{
  // release bus
  TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA) | _BV(TWINT);

  // update twi state
  twi_state = TWI_READY;
}

/*
 * Function twi_setTimeoutInMicros
 * Desc     set a timeout for while loops that twi might get stuck in
 * Input    timeout value in microseconds (0 means never time out)
 * Input    reset_with_timeout: true causes timeout events to reset twi
 * Output   none
 */
void twi_setTimeoutInMicros(uint32_t timeout, bool reset_with_timeout)
{
  twi_timed_out_flag = false;
  twi_timeout_us = timeout;
  twi_do_reset_on_timeout = reset_with_timeout;
}

/*
 * Function twi_handleTimeout
 * Desc     this gets called whenever a while loop here has lasted longer than
 *          twi_timeout_us microseconds. always sets twi_timed_out_flag
 * Input    reset: true causes this function to reset the twi hardware interface
 * Output   none
 */
void twi_handleTimeout(bool reset)
{
  twi_timed_out_flag = true;

  if (reset)
  {
    // remember bitrate and address settings
    uint8_t previous_TWBR = TWBR;
    uint8_t previous_TWAR = TWAR;

    // reset the interface
    twi_disable();
    twi_init();

    // reapply the previous register values
    TWAR = previous_TWAR;
    TWBR = previous_TWBR;
  }
}

/*
 * Function twi_manageTimeoutFlag
 * Desc     returns true if twi has seen a timeout
 *          optionally clears the timeout flag
 * Input    clear_flag: true if we should reset the hardware
 * Output   none
 */
bool twi_manageTimeoutFlag(bool clear_flag)
{
  bool flag = twi_timed_out_flag;
  if (clear_flag)
  {
    twi_timed_out_flag = false;
  }
  return (flag);
}

ISR(TWI_vect)
{
  switch (TW_STATUS)
  {
  // All Master
  case TW_START:     // sent start condition
  case TW_REP_START: // sent repeated start condition
    // copy device address and r/w bit to output register and ack
    TWDR = twi_slarw;
    twi_reply(1);
    break;

  // Master Transmitter
  case TW_MT_SLA_ACK:   // slave receiver acked address
  case TW_MT_DATA_ACK:  // slave receiver acked data
  case TW_MT_SLA_NACK:  // address sent, nack received
  case TW_MT_DATA_NACK: // data sent, nack received
  case TW_MT_ARB_LOST:  // lost bus arbitration
    break;

  // Master Receiver
  case TW_MR_DATA_ACK:  // data received, ack sent
  case TW_MR_SLA_ACK:   // address sent, ack received
  case TW_MR_DATA_NACK: // data received, nack sent
  case TW_MR_SLA_NACK:  // address sent, nack received
    break;
  // TW_MR_ARB_LOST handled by TW_MT_ARB_LOST case

  // Slave Receiver
  case TW_SR_SLA_ACK:            // addressed, returned ack
  case TW_SR_GCALL_ACK:          // addressed generally, returned ack
  case TW_SR_ARB_LOST_SLA_ACK:   // lost arbitration, returned ack
  case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration, returned ack
    // enter slave receiver mode
    twi_state = TWI_SRX;
    // indicate that rx buffer can be overwritten and ack
    twi_rxBufferIndex = 0;
    twi_reply(1);
    break;
  case TW_SR_DATA_ACK:       // data received, returned ack
  case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
    // if there is still room in the rx buffer
    if (twi_rxBufferIndex < TWI_RX_BUFFER_LENGTH)
    {
      // put byte in buffer and ack
      twi_rxBuffer[twi_rxBufferIndex++] = TWDR;
      twi_reply(1);
    }
    else
    {
      // otherwise nack
      twi_reply(0);
    }
    break;
  case TW_SR_STOP: // stop or repeated start condition received
    // ack future responses and leave slave receiver state
    twi_releaseBus();
    break;
  case TW_SR_DATA_NACK:       // data received, returned nack
  case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
    // nack back at master
    twi_reply(0);
    break;

  // Slave Transmitter
  case TW_ST_SLA_ACK:          // addressed, returned ack
  case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
    // enter slave transmitter mode
    twi_state = TWI_STX;
    // ready the tx buffer index for iteration
    twi_txBufferIndex = 0;
    // set tx buffer length to be zero, to verify if user changes it
    twi_txBufferLength = 0;
    // request for txBuffer to be filled and length to be set
    // note: user must use provided pointers to do this
    uint8_t txLength = 0;
    twi_onSlaveTransmit(twi_rxBuffer, twi_rxBufferIndex, twi_txBuffer, &txLength);
    twi_txBufferLength = txLength;
    // if they didn't change buffer & length, initialize it
    if (0 == twi_txBufferLength)
    {
      twi_txBufferLength = 1;
      twi_txBuffer[0] = 0x00;
    }
    __attribute__((fallthrough));
    // transmit first byte from buffer, fall
  case TW_ST_DATA_ACK: // byte sent, ack returned
    // copy data to output register
    TWDR = twi_txBuffer[twi_txBufferIndex++];
    // if there is more to send, ack, otherwise nack
    if (twi_txBufferIndex < twi_txBufferLength)
    {
      twi_reply(1);
    }
    else
    {
      twi_reply(0);
    }
    break;
  case TW_ST_DATA_NACK: // received nack, we are done
  case TW_ST_LAST_DATA: // received ack, but we are done already!
    // ack future responses
    twi_reply(1);
    // leave slave receiver state
    twi_state = TWI_READY;
    break;

  // All
  case TW_NO_INFO: // no state information
    break;
  case TW_BUS_ERROR: // bus error, illegal stop/start
    twi_error = TW_BUS_ERROR;
    twi_stop();
    break;
  }
}
