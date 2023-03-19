// Note: add the build flag '-include "src/control-processor/override-headers.h"'

// Override the default TwoWire buffer sizes
#define TWI_TX_BUFFER_LENGTH 20
#define TWI_RX_BUFFER_LENGTH 10

// Reduce the maximum number of registers in a Modbus request
#define MODBUS_MAX_READ_REGISTERS 20
#define MODBUS_MAX_WRITE_REGISTERS 20
