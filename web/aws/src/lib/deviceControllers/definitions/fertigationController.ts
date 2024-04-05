import {
  DeviceComponentDefinition,
  DeviceControllerDefinition,
  DevicePropertyDefinition,
  DevicePropertyValueType,
} from "../types.js";

enum FertigationConfigType {
  Enabled = 0x01,
}

export class FertigationController implements DeviceControllerDefinition {
  enable: boolean = false;

  addConfig(type: FertigationConfigType, data: Uint8Array): void {
    switch (type) {
      case FertigationConfigType.Enabled:
        this.enable = true;
        return;
    }
    const exhaustiveAssertion: never = type;
  }

  getComponents(): DeviceComponentDefinition[] {
    if (!this.enable) {
      return [];
    }
    return [
      {
        type: "fertigation",
        id: "fertigation",
        typeName: "Fertigation",
        name: "Fertigation",
      },
    ];
  }

  getProperties(): DevicePropertyDefinition[] {
    if (!this.enable) {
      return [];
    }
    const properties: DevicePropertyDefinition[] = [
      {
        propertyId: 0x02,
        componentId: "fertigation",
        name: "Flow Rate",
        mutable: false,
        format: {
          type: DevicePropertyValueType.UnsignedInt,
          mul: 1 / 40000,
        },
        unit: "L/s",
      },
    ];
    return properties;
  }
}
