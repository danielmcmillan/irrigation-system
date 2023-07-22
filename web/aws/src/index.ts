import { handleDeviceMessage } from "./handleDeviceMessage";
import { RawDeviceMessage } from "./lib/deviceMessage/deviceMessage";

export async function handler(event: RawDeviceMessage): Promise<void> {
  return handleDeviceMessage(event);
}
