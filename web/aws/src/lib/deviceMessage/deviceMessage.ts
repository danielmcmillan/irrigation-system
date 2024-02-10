import { CommandResultDeviceMessage, parseControllerCommandResult } from "./commandResult.js";
import { ErrorDeviceMessage, parseErrorDeviceMessage } from "./error.js";
import { DeviceEvent, parseEventDeviceMessage } from "./event.js";
import { PropertyDeviceMessage, parsePropertiesDeviceMessage } from "./properties.js";

/**
 * A device message as delivered by AWS IoT Rules engine
 */
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
  error?: ErrorDeviceMessage;
  properties?: PropertyDeviceMessage[];
  commandResult?: CommandResultDeviceMessage;
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
        result.events = parseEventDeviceMessage(buffer);
      } else if (input.type === "error") {
        result.error = parseErrorDeviceMessage(buffer);
      } else if (input.type === "properties") {
        result.properties = parsePropertiesDeviceMessage(buffer);
      } else if (input.type === "commandResult") {
        result.commandResult = parseControllerCommandResult(buffer);
      } else {
        result.data = buffer;
      }
    }
  } catch (err) {
    console.error("Error parsing message data", input.data, err);
  }
  return result;
}
