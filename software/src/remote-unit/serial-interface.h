#ifndef _REMOTE_UNIT_SERIAL_INTERFACE_H
#define _REMOTE_UNIT_SERIAL_INTERFACE_H

#include "command-handler.h"
#include "remote-unit-packet.h"

using namespace IrrigationSystem;

class RemoteUnitSerialInterface
{
  const uint16_t nodeId;
  const RemoteUnitCommandHandler &commands;

public:
  enum class Result : uint8_t
  {
    success = 0,
    noData,
    invalidPacketCrc,
    invalidPacketCommand,
    invalidNodeId,
    invalidPacketResponseTooLarge,
    writeFailure
  };

  RemoteUnitSerialInterface(uint16_t nodeId, const RemoteUnitCommandHandler &commands);
  /**
   * Receive a packet over the Serial interface and handle the encoded commands.
   *
   * Returns a non-zero value on failure.
   */
  Result receivePacket(unsigned long timeout) const;

private:
  void handleCommand(RemoteUnitPacket::RemoteUnitCommand command, const uint8_t *data, uint8_t *responseData) const;

  RemoteUnitSerialInterface::Result handlePacket(const uint8_t *packet, size_t size) const;
};
#endif
