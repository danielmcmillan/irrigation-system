#include "vacon-100-modbus-client.h"
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <errno.h>

// IN: Control word
#define CONTROL_WORD_ADDRESS (2001 - 1)
#define CONTROL_WORD_ON 0x0001
#define CONTROL_WORD_FORCE 0x0100
// IN: Speed reference
#define SPEED_REF_ADDRESS (2003 - 1)
// OUT: Status word
#define STATUS_WORD_ADDRESS (2101 - 1)

namespace IrrigationSystem
{
    Vacon100Client::Vacon100Client(Stream &stream, int re, int de, int di) : slaveId(1),
                                                                             rs485(stream, di, de, re)
    {
    }

    void Vacon100Client::setSlaveId(int slaveAddress)
    {
        this->slaveId = slaveAddress;
    }

    int Vacon100Client::begin()
    {
        errno = 0;
        return ModbusRTUClient.begin(rs485, 0);
    }

    void Vacon100Client::end()
    {
        ModbusRTUClient.end();
    }

    int Vacon100Client::setStart(bool start, bool force)
    {
        errno = 0;
        uint16_t value = (start ? CONTROL_WORD_ON : 0) | (force ? CONTROL_WORD_FORCE : 0);
        return ModbusRTUClient.holdingRegisterWrite(slaveId, CONTROL_WORD_ADDRESS, value);
    }

    int Vacon100Client::setSpeed(uint16_t value)
    {
        errno = 0;
        return ModbusRTUClient.holdingRegisterWrite(slaveId, SPEED_REF_ADDRESS, value);
    }

    int Vacon100Client::readInputRegisters(Vacon100Data *data)
    {
        errno = 0;
        int result = ModbusRTUClient.requestFrom(slaveId, INPUT_REGISTERS, STATUS_WORD_ADDRESS, 11);
        if (result != 0)
        {
            data->statusWord = ModbusRTUClient.read();
            ModbusRTUClient.read(); // skip reserved register
            data->actualSpeed = ModbusRTUClient.read();
            data->outputFrequency = ModbusRTUClient.read();
            data->motorSpeed = ModbusRTUClient.read();
            data->motorCurrent = ModbusRTUClient.read();
            data->motorTorque = ModbusRTUClient.read();
            data->motorPower = ModbusRTUClient.read();
            data->motorVoltage = ModbusRTUClient.read();
            data->dcLinkVoltage = ModbusRTUClient.read();
            data->activeFaultCode = ModbusRTUClient.read();
        }
        return result;
    }

    String Vacon100Data::toString()
    {
        char result[400] = {};
        sprintf_P(
            result,
            PSTR("Status word:\n- ready: %d\n- run: %d\n- direction: %d\n- fault: %d\n- alarm: %d\n"
                 "- atReference: %d\n- zeroSpeed: %d\n- fluxReady: %d\nProcess data:\n- actualSpeed: %d * 0.01%%\n"
                 "- outputFrequency: %d * 0.01 Hz\n- motorSpeed: %d rpm\n- motorCurrent: %d * 0.1 A\n- motorTorque: %d * 0.1%%\n"
                 "- motorPower: %d * 0.1%%\n- motorVoltage: %d * 0.1 V\n- dcLinkVoltage: %d V\n- activeFaultCode: %d\n"),
            (this->statusWord & Vacon100StatusWordMask::ready) > 0,
            (this->statusWord & Vacon100StatusWordMask::run) > 0,
            (this->statusWord & Vacon100StatusWordMask::direction) > 0,
            (this->statusWord & Vacon100StatusWordMask::fault) > 0,
            (this->statusWord & Vacon100StatusWordMask::alarm) > 0,
            (this->statusWord & Vacon100StatusWordMask::atReference) > 0,
            (this->statusWord & Vacon100StatusWordMask::zeroSpeed) > 0,
            (this->statusWord & Vacon100StatusWordMask::fluxReady) > 0,
            this->actualSpeed,
            this->outputFrequency,
            this->motorSpeed,
            this->motorCurrent,
            this->motorTorque,
            this->motorPower,
            this->motorVoltage,
            this->dcLinkVoltage,
            this->activeFaultCode);
        return String(result);
    }
}
