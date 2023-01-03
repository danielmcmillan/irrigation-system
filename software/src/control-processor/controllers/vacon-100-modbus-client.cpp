#include "vacon-100-modbus-client.h"
#include <ModbusRTUClient.h>
#include <ArduinoRS485.h>
#include <errno.h>
#include "logging.h"

// IN: Control word
#define CONTROL_WORD_ADDRESS (2001 - 1)
#define CONTROL_WORD_ON 0x0001
#define CONTROL_WORD_FORCE 0x0100
// IN: Speed reference
#define SPEED_REF_ADDRESS (2003 - 1)
// IN: ID Map
#define ID_MAP_ADDRESS (10501 - 1)
// OUT: ID Map values
#define ID_MAP_VALUES_ADDRESS (10601 - 1)

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
        int result = ModbusRTUClient.begin(rs485, 0);
        ModbusRTUClient.setTimeout(2000);
        return result;
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

    int Vacon100Client::initIdMapping()
    {
        errno = 0;
        int result = ModbusRTUClient.beginTransmission(slaveId, HOLDING_REGISTERS, ID_MAP_ADDRESS, 20);
        if (result == 0)
        {
            return result;
        }
        // Write out the Application parameter ID for each value to be read from the corresponding ID Map registers
        ModbusRTUClient.write(864);   // status
        ModbusRTUClient.write(865);   // actualSpeed
        ModbusRTUClient.write(1);     // outputFrequency
        ModbusRTUClient.write(2);     // motorSpeed
        ModbusRTUClient.write(3);     // motorCurrent
        ModbusRTUClient.write(4);     // motorTorque
        ModbusRTUClient.write(5);     // motorPower
        ModbusRTUClient.write(6);     // motorVoltage
        ModbusRTUClient.write(7);     // dcLinkVoltage
        ModbusRTUClient.write(37);    // activeFaultCode
        ModbusRTUClient.write(15541); // feedbackPressure
        ModbusRTUClient.write(8);     // driveTemp
        ModbusRTUClient.write(9);     // motorTemp
        ModbusRTUClient.write(1054);  // energyUsed low
        ModbusRTUClient.write(1067);  // energyUsed high
        ModbusRTUClient.write(1772);  // runTime y
        ModbusRTUClient.write(1773);  // runTime d
        ModbusRTUClient.write(1774);  // runTime h
        ModbusRTUClient.write(1775);  // runTime m
        ModbusRTUClient.write(1776);  // runTime s
        return ModbusRTUClient.endTransmission();
    }

    int Vacon100Client::readInputRegisters(Vacon100Data *data)
    {
        LOG_INFO("Reading... ");
        errno = 0;
        int result = ModbusRTUClient.requestFrom(slaveId, INPUT_REGISTERS, ID_MAP_VALUES_ADDRESS, 20);
        if (result == 0)
        {
            return result;
        }
        // Read each value from the ID Map
        data->statusWord = ModbusRTUClient.read();
        data->actualSpeed = ModbusRTUClient.read();
        data->outputFrequency = ModbusRTUClient.read();
        data->motorSpeed = ModbusRTUClient.read();
        data->motorCurrent = ModbusRTUClient.read();
        data->motorTorque = ModbusRTUClient.read();
        data->motorPower = ModbusRTUClient.read();
        data->motorVoltage = ModbusRTUClient.read();
        data->dcLinkVoltage = ModbusRTUClient.read();
        data->activeFaultCode = ModbusRTUClient.read();
        data->feedbackPressure = ModbusRTUClient.read();
        data->driveTemp = ModbusRTUClient.read();
        data->motorTemp = ModbusRTUClient.read();
        data->energyUsed = (uint32_t)ModbusRTUClient.read();
        data->energyUsed += ((uint32_t)ModbusRTUClient.read()) << 16;
        data->runTime = ((uint32_t)ModbusRTUClient.read()) * 31536000;
        data->runTime += ((uint32_t)ModbusRTUClient.read()) * 86400;
        data->runTime += ((uint32_t)ModbusRTUClient.read()) * 3600;
        data->runTime += ((uint32_t)ModbusRTUClient.read()) * 60;
        data->runTime += (uint32_t)ModbusRTUClient.read();

        LOG_INFO("Read successful");
        return result;
    }

    uint16_t Vacon100Client::getErrorCode()
    {
        return errno;
    }

    void Vacon100Client::printError()
    {
        Serial.print(F("ERROR Vacon 100 communication failed: "));
        Serial.println(errno); // TODO errno ETIMEDOUT 116 was happening a lot, 11246 (Invalid CRC)
    }

    String Vacon100Data::toString()
    {
        char result[400] = {};
        sprintf_P(
            result,
            PSTR("Status word:\n- ready: %d\n- run: %d\n- direction: %d\n- fault: %d\n- alarm: %d\n"
                 "- atReference: %d\n- zeroSpeed: %d\n- fluxReady: %d\nProcess data:\n- actualSpeed: %d * 0.01%%\n"
                 "- outputFrequency: %d * 0.01 Hz\n- motorSpeed: %d rpm\n- motorCurrent: %d * 0.1 A\n- motorTorque: %d * 0.1%%\n"
                 "- motorPower: %d * 0.1%%\n- motorVoltage: %d * 0.1 V\n- dcLinkVoltage: %d V\n- activeFaultCode: %d\n"
                 "- pressure: %d\n- driveTemp: %d * 0.1 C\n- motorTemp: %d * 0.1%%\n- energyUsed: %d kWh\n- runTime: %d seconds\n"),
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
            this->activeFaultCode,
            this->feedbackPressure,
            this->driveTemp,
            this->motorTemp,
            this->energyUsed,
            this->runTime);
        return String(result);
    }
}
