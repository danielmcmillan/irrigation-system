import {
  DeviceComponentDefinition,
  DeviceControllerDefinition,
  DevicePropertyDefinition,
} from "../types";

enum RemoteUnitConfigType {
  Node = 0x01,
  Solenoid = 0x02,
}

export class RemoteUnitController implements DeviceControllerDefinition {
  addConfig(type: RemoteUnitConfigType, data: ArrayBufferLike): void {
    switch (type) {
      case RemoteUnitConfigType.Node:
        return;
      case RemoteUnitConfigType.Solenoid:
        return;
    }
    const exhaustiveAssertion: never = type;
  }

  getComponents(): DeviceComponentDefinition[] {
    return [];
  }

  getProperties(): DevicePropertyDefinition[] {
    return [];
  }
}
