import { handleDeviceMessage } from "./deviceListener/handler";
import { RawDeviceMessage } from "./deviceListener/message";

export async function handler(event: RawDeviceMessage): Promise<void> {
  return handleDeviceMessage(event);
}
