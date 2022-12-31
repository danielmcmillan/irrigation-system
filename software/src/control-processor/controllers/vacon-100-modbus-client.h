#ifndef VACON100_MODBUS_H
#define VACON100_MODBUS_H
#include <Arduino.h>
#include <ArduinoRS485.h>

namespace IrrigationSystem
{
    /** Data from the Vacon100 input registers. */
    struct Vacon100Data
    {
        /** Flags indicating current status. See `Vacon100StatusWordMask`. */
        uint8_t statusWord;
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
        /** The pressure reading used as the PID feedback signal */
        uint16_t feedbackPressure;
        /** The temperature of the drive heat sink, in 0.1 C increments */
        uint16_t driveTemp;
        /** The calculated temperature of the motor as a percentage of the nominal working temperature */
        uint16_t motorTemp;
        /** The total energy used in kWh */
        uint32_t energyUsed;
        /** The total motor runtime in seconds */
        uint32_t runTime;

        String toString();
    };

    /** Masks for bits in the status word. */
    namespace Vacon100StatusWordMask
    {
        enum : uint16_t
        {
            ready = 1,
            run = 1 << 1,
            direction = 1 << 2,
            fault = 1 << 3,
            alarm = 1 << 4,
            atReference = 1 << 5,
            zeroSpeed = 1 << 6,
            fluxReady = 1 << 7,
        };
    }

    /**
     * Client for fieldbus control of Vacon 100 using Modbus RTU over RS485.
     */
    class Vacon100Client
    {
    public:
        Vacon100Client(Stream &stream, int re, int de, int di);

        /** Set the slave address of the Vacon 100 to something other than the default 1. */
        void setSlaveId(int slaveAddress);

        /**
         * Start the client.
         * Returns 0 on failure and sets errno.
         */
        int begin();

        /**
         * Stop the client.
         */
        void end();

        /**
         * Sends a start or stop request by setting the control word register.
         * Specify force as true to force fieldbus control place.
         * Returns 0 on failure and sets errno.
         **/
        int setStart(bool start, bool force = false);

        /**
         * Set the FB speed reference register.
         * Value range is 0 - 10,000, where 10,000 represents 100%
         * Returns 0 on failure and sets errno.
         */
        int setSpeed(uint16_t value);

        /**
         * Set up the ID mapping.
         * This must be performed at least once for readInputRegisters to work.
         * Returns 0 on failure and sets errno.
         */
        int initIdMapping();

        /**
         * Read the current state from input registers in the ID mapping into the provided Vacon100Data object.
         * Returns 0 on failure and sets errno.
         */
        int readInputRegisters(Vacon100Data *result);

        void printError();

    private:
        int slaveId;
        RS485Class rs485;
    };
}
#endif
