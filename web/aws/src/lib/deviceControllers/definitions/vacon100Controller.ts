import {
  DeviceComponentDefinition,
  DeviceControllerDefinition,
  DevicePropertyDefinition,
  DevicePropertyValueType,
} from "../types.js";

enum Vacon100ConfigType {
  EnableModbus = 0x01,
}

export class Vacon100Controller implements DeviceControllerDefinition {
  enableModbus: boolean = false;

  addConfig(type: Vacon100ConfigType, data: Uint8Array): void {
    switch (type) {
      case Vacon100ConfigType.EnableModbus:
        this.enableModbus = true;
        return;
    }
    const exhaustiveAssertion: never = type;
  }

  getComponents(): DeviceComponentDefinition[] {
    return [
      {
        type: "vsd",
        id: "vsd",
      },
    ];
  }

  getProperties(): DevicePropertyDefinition[] {
    const properties: DevicePropertyDefinition[] = [
      {
        propertyId: 0x01,
        componentId: "vsd",
        name: "Run",
        mutable: true,
        format: {
          type: DevicePropertyValueType.BooleanFlags,
        },
      },
    ];
    if (this.enableModbus) {
      properties.push({
        propertyId: 0x02,
        componentId: "vsd",
        name: "Available",
        format: {
          type: DevicePropertyValueType.BooleanFlags,
        },
      });
      // TODO add other modbus properties
    }
    return properties;
  }
}
