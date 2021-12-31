#include "vacon100-modbus.h"
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>

// IN: Control word
#define CONTROL_WORD_ADDRESS 2001
#define CONTROL_WORD_ON 0x0001
#define CONTROL_WORD_FORCE 0x0100
// IN: Speed reference
#define SPEED_REF_ADDRESS 2003
// OUT: Status word
#define STATUS_WORD_ADDRESS 2101

namespace Vacon100Modbus
{
    namespace StatusWordMask
    {
        uint16_t ready = 1;
        uint16_t run = 1 << 1;
        uint16_t direction = 1 << 2;
        uint16_t fault = 1 << 3;
        uint16_t alarm = 1 << 4;
        uint16_t atReference = 1 << 5;
        uint16_t zeroSpeed = 1 << 6;
        uint16_t fluxReady = 1 << 7;
    }

    Client::Client(Stream &stream, int re, int de, int di) : slaveId(1),
                                                             rs485(stream, di, de, re)
    {
    }

    void Client::setSlaveId(int slaveAddress)
    {
        this->slaveId = slaveAddress;
    }

    int Client::begin()
    {
        return ModbusRTUClient.begin(rs485, 0);
    }

    void Client::end()
    {
        ModbusRTUClient.end();
    }

    int Client::setStart(bool start, bool force)
    {
        uint16_t value = (start ? CONTROL_WORD_ON : 0) | (force ? CONTROL_WORD_FORCE : 0);
        return ModbusRTUClient.holdingRegisterWrite(slaveId, CONTROL_WORD_ADDRESS, value);
    }

    int Client::setSpeed(uint16_t value)
    {
        return ModbusRTUClient.holdingRegisterWrite(slaveId, SPEED_REF_ADDRESS, value);
    }

    int Client::readInputRegisters(Data *data)
    {
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

    String Data::toString()
    {
        char result[400] = {};
        sprintf_P(
            result,
            PSTR("Status word:\n- ready: %d\n- run: %d\n- direction: %d\n- fault: %d\n- alarm: %d\n"
                 "- atReference: %d\n- zeroSpeed: %d\n- fluxReady: %d\nProcess data:\n- actualSpeed: %d * 0.01%%\n"
                 "- outputFrequency: %d * 0.01 Hz\n- motorSpeed: %d rpm\n- motorCurrent: %d * 0.1 A\n- motorTorque: %d * 0.1%%\n"
                 "- motorPower: %d * 0.1%%\n- motorVoltage: %d * 0.1 V\n- dcLinkVoltage: %d V\n- activeFaultCode: %d\n"),
            (this->statusWord & StatusWordMask::ready) > 0,
            (this->statusWord & StatusWordMask::run) > 0,
            (this->statusWord & StatusWordMask::direction) > 0,
            (this->statusWord & StatusWordMask::fault) > 0,
            (this->statusWord & StatusWordMask::alarm) > 0,
            (this->statusWord & StatusWordMask::atReference) > 0,
            (this->statusWord & StatusWordMask::zeroSpeed) > 0,
            (this->statusWord & StatusWordMask::fluxReady) > 0,
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
