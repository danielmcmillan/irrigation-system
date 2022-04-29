#ifndef _REMOTE_UNIT_SERIAL_INTERFACE_H
#define _REMOTE_UNIT_SERIAL_INTERFACE_H
#define REMOTE_UNIT_NO_DATA -1
#define REMOTE_UNIT_INVALID_PACKET_CRC -2
#define REMOTE_UNIT_INVALID_PACKET_COMMAND -3
#define REMOTE_UNIT_INVALID_NODE_ID -4
#define REMOTE_UNIT_INVALID_PACKET_RESPONSE_TOO_LARGE -5
#define REMOTE_UNIT_WRITE_FAILURE -6

/**
 * Receive a packet over the Serial interface and handle the encoded commands.
 *
 * Returns a value less than 0 on failure.
 */
int receivePacket(uint16_t nodeId);
#endif
