import { DeviceControllerDefinitions } from "./types.js";

export interface DeviceConfig {
  controllerId: number;
  type: number;
  data: Uint8Array;
}

export function configureDeviceControllers(
  controllers: DeviceControllerDefinitions,
  configData: Uint8Array
): void {
  let offset = 0;
  while (offset < configData.byteLength) {
    const length = configData[offset];
    const controllerId = configData[offset + 1];
    const type = configData[offset + 2];
    const controller = controllers.get(controllerId);
    if (controller) {
      controller.addConfig(type, configData.slice(offset + 3));
    } else {
      console.warn(`Error configuring device controllers. Unknown controller id ${controllerId}.`);
    }
    offset += length;
  }
}
