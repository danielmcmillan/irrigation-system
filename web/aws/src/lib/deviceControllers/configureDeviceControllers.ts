import { DeviceControllerDefinition } from "./types";

export interface DeviceConfig {
  controllerId: number;
  type: number;
  data: Uint8Array;
}

export function configureDeviceControllers(
  controllers: Record<number, DeviceControllerDefinition | undefined>,
  configData: Uint8Array
): void {
  let offset = 0;
  while (offset < configData.byteLength) {
    const length = configData[offset];
    const controllerId = configData[offset + 1];
    const type = configData[offset + 2];
    const controller = controllers[controllerId];
    if (controller) {
      controller.addConfig(type, configData.slice(offset + 2));
    } else {
      console.warn(
        `Failed to configure device controllers. Unknown controller id ${controllerId}.`
      );
    }
    offset += length;
  }
}
