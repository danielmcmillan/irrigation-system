import {
  DeviceComponentDefinition,
  DeviceControllerDefinition,
  DevicePropertyDefinition,
} from "../types";

export class Vacon100Controller implements DeviceControllerDefinition {
  addConfig(type: number, data: ArrayBufferLike): void {}

  getComponents(): DeviceComponentDefinition[] {
    return [];
  }

  getProperties(): DevicePropertyDefinition[] {
    return [];
  }
}
