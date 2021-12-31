#ifndef VACON100_MODBUS
#define VACON100_MODBUS
#include <Arduino.h>
#include <ArduinoRS485.h>

namespace Vacon100Modbus
{
    /** Data from the Vacon100 input registers. */
    struct Data
    {
        /** Flags indicating current status. See `Vacon100StatusWordMask`. */
        uint16_t statusWord;
        /** The speed percentage in 0.01% increments */
        uint16_t actualSpeed;
        /** The output frequency in 0.01 Hz increments */
        uint16_t outputFrequency;
        /** The motor speed in rpm */
        uint16_t motorSpeed;
        /** The motor current in 0.1 A increments */
        uint16_t motorCurrent;
        /** The motor torque in 0.1% increments */
        uint16_t motorTorque;
        /** The motor power in 0.1% increments */
        uint16_t motorPower;
        /** The motor voltage in 0.1 V increments */
        uint16_t motorVoltage;
        /** The DC link voltage in Volts */
        uint16_t dcLinkVoltage;
        /** The active fault code */
        uint16_t activeFaultCode;

        String toString();
    };

    /** Masks for bits in the status word. */
    namespace StatusWordMask
    {
        extern uint16_t ready;
        extern uint16_t run;
        extern uint16_t direction;
        extern uint16_t fault;
        extern uint16_t alarm;
        extern uint16_t atReference;
        extern uint16_t zeroSpeed;
        extern uint16_t fluxReady;
    }

    /**
     * Client for fieldbus control of Vacon 100 using Modbus RTU over RS485.
     */
    class Client
    {
    public:
        Client(Stream &stream, int re, int de, int di);

        /** Set the slave address of the Vacon100 to something other than the default 1. */
        void setSlaveId(int slaveAddress);

        /**
         * Start the client.
         * TODO: error. Returns 0 on failure.
         */
        int begin();

        /**
         * Stop the client.
         */
        void end();

        /**
         * Sends a start or stop request by setting the control word register.
         * Specify force as true to force fieldbus control place.
         * TODO: error. Returns 0 on failure.
         **/
        int setStart(bool start, bool force = false);

        /**
         * Set the FB speed reference register.
         * Value range is 0 - 10,000, where 10,000 represents 100%
         * TODO: error. Returns 0 on failure.
         */
        int setSpeed(uint16_t value);

        /**
         * Read the current state from input registers into the provided Vacon100Data object.
         * TODO: error. Returns 0 on failure.
         */
        int readInputRegisters(Data *result);

    private:
        int slaveId;
        RS485Class rs485;
    };
}
#endif
