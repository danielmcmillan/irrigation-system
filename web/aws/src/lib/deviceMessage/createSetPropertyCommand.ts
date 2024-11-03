import { PublishCommand } from "@aws-sdk/client-iot-data-plane";

export function createSetPropertyCommand(
  deviceId: string,
  controllerId: number,
  propertyId: number,
  value: number
): PublishCommand {
  const payload = new DataView(new ArrayBuffer(4));
  payload.setUint8(0, controllerId);
  payload.setUint16(1, propertyId, true);
  // TODO handle value conversion (currently all mutable values are only 1 or 0)
  payload.setUint8(3, value);
  return new PublishCommand({
    topic: `icu-in/${deviceId}/setProperty`,
    payload: payload.buffer,
    qos: 1,
  });
}
