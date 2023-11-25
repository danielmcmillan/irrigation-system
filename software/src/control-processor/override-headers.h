// Note: add the build flag '-include "src/control-processor/override-headers.h"'
// ****** MUST RUN "PlatformIO: Clean" for changes to take affect! ******

// Reduce the maximum number of registers in a Modbus request
// #define MODBUS_MAX_READ_REGISTERS 20
// #define MODBUS_MAX_WRITE_REGISTERS 20

// #define DISABLE_LOGGING

#define INCLUDE_CONTROLLER_METADATA
#define HW_REV 2
