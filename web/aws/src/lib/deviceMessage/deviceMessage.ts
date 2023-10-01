import { DeviceError, getErrorFromData } from "./deviceError";
import { DeviceEvent, getEventsFromData } from "./deviceEvent";
import { DeviceProperty, getPropertiesFromData } from "./deviceProperty";

export interface RawDeviceMessage {
  data?: string;
  time: number;
  clientId?: string;
  deviceId: string;
  type: string;
}

export interface DeviceMessage {
  time: number;
  clientId?: string;
  deviceId: string;
  type: string;
  events?: DeviceEvent[];
  error?: DeviceError;
  properties?: DeviceProperty[];
  data?: ArrayBuffer;
}

/**
 * Parse information from the specified raw message received from the irrigation device.
 * If the message type is unsupported or parsing fails, the message will include the original data.
 */
export function parseDeviceMessage(input: RawDeviceMessage): DeviceMessage {
  const result: DeviceMessage = {
    time: input.time,
    clientId: input.clientId,
    deviceId: input.deviceId,
    type: input.type,
  };

  try {
    if (input.data) {
      const data = Buffer.from(input.data, "base64");
      const buffer = data.buffer.slice(data.byteOffset, data.byteOffset + data.byteLength);
      if (input.type === "event") {
        result.events = getEventsFromData(buffer);
      } else if (input.type === "error") {
        result.error = getErrorFromData(buffer);
      } else if (input.type === "properties") {
        result.properties = getPropertiesFromData(buffer);
      } else {
        result.data = buffer;
      }
    }
  } catch (err) {
    console.error("Error parsing message data", input.data, err);
  }
  return result;
}
