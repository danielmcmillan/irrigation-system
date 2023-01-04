#include "vacon-100-modbus-client.h"
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
    Vacon100Client::Vacon100Client(Stream &stream, int re, int de, int di) : rs485(stream, di, de, re),
                                                                             modbus(nullptr)
    {
    }

    int Vacon100Client::begin(uint8_t slaveId)
    {
        errno = 0;

        modbus = modbus_new_rtu(&rs485, 0, 0);
        if (modbus == nullptr)
        {
            return 0;
        }
        if (modbus_connect(modbus) != 0)
        {
            modbus_free(modbus);
            return 0;
        }
        modbus_set_error_recovery(modbus, MODBUS_ERROR_RECOVERY_PROTOCOL);
        modbus_set_response_timeout(modbus, 2000);
        modbus_set_slave(modbus, slaveId);

        return 1;
    }

    void Vacon100Client::end()
    {
        modbus_close(modbus);
        modbus_free(modbus);
        modbus = nullptr;
    }

    int Vacon100Client::setStart(bool start, bool force)
    {
        errno = 0;
        uint16_t value = (start ? CONTROL_WORD_ON : 0) | (force ? CONTROL_WORD_FORCE : 0);
        if (modbus_write_register(modbus, CONTROL_WORD_ADDRESS, value) < 0)
        {
            return 0;
        }

        return 1;
    }

    int Vacon100Client::setSpeed(uint16_t value)
    {
        errno = 0;
        if (modbus_write_register(modbus, SPEED_REF_ADDRESS, value) < 0)
        {
            return 0;
        }

        return 1;
    }

    int Vacon100Client::initIdMapping()
    {
        errno = 0;
        // Write out the Application parameter ID for each value to be read from the corresponding ID Map registers
        uint16_t values[] = {
            864,   // status
            865,   // actualSpeed
            1,     // outputFrequency
            2,     // motorSpeed
            3,     // motorCurrent
            4,     // motorTorque
            5,     // motorPower
            6,     // motorVoltage
            7,     // dcLinkVoltage
            37,    // activeFaultCode
            15541, // feedbackPressure
            8,     // driveTemp
            9,     // motorTemp
            1054,  // energyUsed low
            1067,  // energyUsed high
            1772,  // runTime y
            1773,  // runTime d
            1774,  // runTime h
            1775,  // runTime m
            1776   // runTime s
        };
        if (modbus_write_registers(modbus, ID_MAP_ADDRESS, 20, values) < 0)
        {
            return 0;
        }

        return 1;
    }

    int Vacon100Client::readInputRegisters(Vacon100Data *data)
    {
        LOG_INFO("Reading... ");
        errno = 0;
        uint16_t values[20];
        if (modbus_read_input_registers(modbus, ID_MAP_VALUES_ADDRESS, 20, values) < 0)
        {
            return 0;
        }

        // Save each value from the ID Map to data
        data->statusWord = values[0];
        data->actualSpeed = values[1];
        data->outputFrequency = values[2];
        data->motorSpeed = values[3];
        data->motorCurrent = values[4];
        data->motorTorque = values[5];
        data->motorPower = values[6];
        data->motorVoltage = values[7];
        data->dcLinkVoltage = values[8];
        data->activeFaultCode = values[9];
        data->feedbackPressure = values[10];
        data->driveTemp = values[11];
        data->motorTemp = values[12];
        data->energyUsed = (uint32_t)values[13] + ((uint32_t)values[14] << 16);
        data->runTime = ((uint32_t)values[15]) * 31536000 +
                        ((uint32_t)values[16]) * 86400 +
                        ((uint32_t)values[17]) * 3600 +
                        ((uint32_t)values[18]) * 60 +
                        (uint32_t)values[19];

        LOG_INFO("Read successful");
        return 1;
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
