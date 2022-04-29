#include "commands.h"
#include <string.h>

int getSolenoidState(uint8_t *solenoidStateOut)
{
    *solenoidStateOut = 0x01;
    return 0;
}

int setSolenoidState(uint8_t state, uint8_t *newSolenoidStateOut)
{
    *newSolenoidStateOut = state;
    return 0;
}

int getBatteryVoltage(uint8_t *voltageOut)
{
    return 0;
}

int getBatteryRaw(uint16_t *batteryRawOut)
{
    return 0;
}

int getFaults(uint8_t *faultsOut)
{
    return 0;
}

int clearFaults(uint8_t *faultsOut)
{
    *faultsOut = 0x00;
    return 0;
}

int getSignalStrength(uint8_t *signalStrengthOut)
{
    return 0;
}

int getConfig(uint8_t *configDataOut)
{
    return 0;
}

int setConfig(const uint8_t *configData, uint8_t *configDataOut)
{
    memcpy(configDataOut, configData, 13);
    return 0;
}
