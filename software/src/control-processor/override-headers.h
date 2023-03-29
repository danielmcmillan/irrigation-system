// Note: add the build flag '-include "src/control-processor/override-headers.h"'
// ****** MUST RUN "PlatformIO: Clean" for changes to take affect! ******

// Override the default TwoWire buffer sizes
#define TWI_TX_BUFFER_LENGTH 22
#define TWI_RX_BUFFER_LENGTH 24

// Reduce the maximum number of registers in a Modbus request
#define MODBUS_MAX_READ_REGISTERS 20
#define MODBUS_MAX_WRITE_REGISTERS 20

#define DISABLE_LOGGING
