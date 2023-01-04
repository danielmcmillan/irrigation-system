#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <unistd.h>
#include "modbus.h"

/**
 * Todo:
 * - make it usable in programatic test
 * - able to produce errors and faults
 * - more realistic values based on requested state
 */

uint16_t values[] = {
    0x0001, // FB Status word
    9000,   // FB Actual speed
    5000,   // Output frequency
    1000,   // Motor speed
    20,     // Motor current
    5000,   // Motor torque
    6000,   // Motor power
    2400,   // Motor voltage
    100,    // DC link voltage
    0,      // Active fault code
    123,    // Input pressure
    0xed15, // kWh low
    0x0001, // kWh high
    0,      // Motor run time years
    5,      // Motor run time days
    14,     // Motor run time hours
    34,     // Motor run time minutes
    12,     // Motor run time seconds
    200,    // Motor current 1 deci
    540,    // Drive heatsink temperature
    450     // Motor temperature
};

unsigned int valueCount = sizeof values / sizeof *values;

const uint16_t registerNumbers[] = {
    2101, // FB Status word
    2103, // FB Actual speed
    2104, // Output frequency
    2105, // Motor speed
    2106, // Motor current
    2107, // Motor torque
    2108, // Motor power
    2109, // Motor voltage
    2110, // DC link voltage
    2111, // Active fault code
    0,    // Input pressure
    0,    // kWh low
    0,    // kWh high
    0,    // Motor run time years
    0,    // Motor run time days
    0,    // Motor run time hours
    0,    // Motor run time minutes
    0,    // Motor run time seconds
    0,    // Motor current 1 deci
    0,    // Drive heatsink temperature
    0     // Motor temperature
};

const uint16_t applicationIds[] = {
    864,   // FB Status word
    865,   // FB Actual speed
    1,     // Output frequency
    2,     // Motor speed
    3,     // Motor current
    4,     // Motor torque
    5,     // Motor power
    6,     // Motor voltage
    7,     // DC link voltage
    37,    // Active fault code
    15541, // Input pressure
    1054,  // kWh low
    1067,  // kWh high
    1772,  // Motor run time years
    1773,  // Motor run time days
    1774,  // Motor run time hours
    1775,  // Motor run time minutes
    1776,  // Motor run time seconds
    45,    // Motor current 1 deci
    8,     // Drive heatsink temperature
    9      // Motor temperature
};

uint16_t totalRegisterCount = 16000;
uint16_t controlWordRegister = 2001;
uint16_t idMapRegister = 10501;
uint16_t idMapWordValueRegister = 10601;
uint16_t idMapRegisterCount = 30;

void update(modbus_mapping_t *mb_mapping)
{
    // Check control word for changes and update status word to match
    uint16_t controlWord = mb_mapping->tab_registers[controlWordRegister - 1];
    if (controlWord != 0xffff)
    {
        // Master has updated it from the initial value
        if ((controlWord & 1) == 1)
        {
            values[0] |= 0x0002;
        }
        else
        {
            values[0] &= ~0x0002;
        }
    }
    // Reset control word
    mb_mapping->tab_registers[controlWordRegister - 1] = 0xffff;

    for (unsigned int i = 0; i < valueCount; ++i)
    {
        // Copy value based on register numbers
        uint16_t registerNum = registerNumbers[i];
        if (registerNum > 0)
        {
            mb_mapping->tab_registers[registerNum - 1] = values[i];
        }
        // Copy value based on direct application IDs
        uint16_t directAppId = applicationIds[i];
        mb_mapping->tab_registers[directAppId - 1] = values[i];
        // Copy value based on ID map
        for (uint16_t r = 0; r < idMapRegisterCount; ++r)
        {
            uint16_t param = mb_mapping->tab_registers[idMapRegister + r - 1];
            if (param == applicationIds[i])
            {
                mb_mapping->tab_registers[idMapWordValueRegister + r - 1] = values[i];
            }
        }
    }

    // Copy holding registers to input registers
    memcpy(mb_mapping->tab_input_registers, mb_mapping->tab_registers, totalRegisterCount * sizeof *mb_mapping->tab_registers);
}

void vaconSimUsage(const char *argv0)
{
    std::cerr << "usage: " << argv0 << " vacon-sim --device <device>\n";
    exit(EXIT_FAILURE);
}

int vaconSim(int argc, char **argv)
{
    struct option options[] = {{"device", 1, nullptr, 'd'},
                               {0, 0, 0, 0}};
    const char *deviceStr = nullptr;
    while (true)
    {
        int option = getopt_long(argc, argv, "d:", options, 0);
        if (option == -1)
        {
            break;
        }
        switch (option)
        {
        case 'd':
            deviceStr = optarg;
            break;
        default /* '?' */:
            vaconSimUsage(argv[0]);
        }
    }
    if (!deviceStr)
    {
        vaconSimUsage(argv[0]);
        return 1;
    }
    if (access(deviceStr, F_OK) != 0)
    {
        std::cerr << "device " << deviceStr << " does not exist\n";
        return 2;
    }

    printf("Starting Modbus RTU slave\n");
    // Start Modbus RTU Slave/Server
    modbus_t *ctx = modbus_new_rtu(deviceStr, 9600, 'N', 8, 1);
    modbus_set_slave(ctx, 1);
    modbus_connect(ctx);
    // modbus_set_debug(ctx, TRUE);

    modbus_mapping_t *mb_mapping = modbus_mapping_new(0, 0, totalRegisterCount, totalRegisterCount);
    if (mb_mapping == nullptr)
    {
        fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    update(mb_mapping);
    for (;;)
    {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int rc = modbus_receive(ctx, query);
        if (rc > 0)
        {
            /* rc is the query size */
            modbus_reply(ctx, query, rc, mb_mapping);
            update(mb_mapping);

            std::cout << "Received query: 0x";
            for (unsigned i = 0; i < rc; ++i)
            {
                std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(query[i]);
            }
            std::cout << ".\n";
        }
        else if (rc == -1)
        {
            printf("Error: %s\n", modbus_strerror(errno));
        }
    }

    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);
    return 0;
}
