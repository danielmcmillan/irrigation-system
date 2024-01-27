import {
  DeviceComponentDefinition,
  DeviceControllerDefinition,
  DevicePropertyDefinition,
  DevicePropertyValueType,
} from "../types.js";

enum RemoteUnitConfigType {
  RemoteUnit = 0x01,
  Solenoid = 0x02,
  Sensor = 0x03,
}

export class RemoteUnitController implements DeviceControllerDefinition {
  remoteUnits: { id: number; nodeId: number; hasSensor?: boolean }[] = [];
  solenoids: { id: number; remoteUnitId: number; numberAtRemoteUnit: number }[] = [];

  addConfig(type: RemoteUnitConfigType, data: Uint8Array): void {
    const view = new DataView(data.buffer);
    switch (type) {
      case RemoteUnitConfigType.RemoteUnit:
        this.remoteUnits.push({
          id: view.getUint8(0),
          nodeId: view.getUint16(1, true),
        });
        return;
      case RemoteUnitConfigType.Solenoid:
        this.solenoids.push({
          id: view.getUint8(0),
          remoteUnitId: view.getUint8(1),
          numberAtRemoteUnit: view.getUint8(2),
        });
        return;
      case RemoteUnitConfigType.Sensor:
        const remoteUnit = this.remoteUnits.find(
          (remoteUnit) => remoteUnit.id === view.getUint8(0)
        );
        if (remoteUnit) {
          remoteUnit.hasSensor = true;
        }
        return;
    }
    const exhaustiveAssertion: never = type;
  }

  getComponents(): DeviceComponentDefinition[] {
    return [
      ...this.remoteUnits.map(
        (remoteUnit): DeviceComponentDefinition => ({
          type: "rmu",
          id: `rmu_${remoteUnit.id}`,
          typeName: "Node",
          name: `${remoteUnit.id}`,
        })
      ),
      ...this.remoteUnits
        .filter((remoteUnit) => remoteUnit.hasSensor)
        .map((remoteUnit) => ({
          type: "sensor",
          id: `snr_${remoteUnit.id}`,
          typeName: "Sensor",
          name: `${remoteUnit.id}`,
        })),
      ...this.solenoids.map(
        (solenoid): DeviceComponentDefinition => ({
          type: "zone",
          id: `zn_${solenoid.id}`,
          typeName: "Zone",
          name: `${solenoid.id}`,
        })
      ),
    ];
  }

  getProperties(): DevicePropertyDefinition[] {
    return [
      ...this.remoteUnits.map(
        (remoteUnit): DevicePropertyDefinition => ({
          name: "Available",
          componentId: `rmu_${remoteUnit.id}`,
          propertyId: (1 << 8) | remoteUnit.id,
          format: {
            type: DevicePropertyValueType.BooleanFlags,
          },
        })
      ),
      ...this.remoteUnits.map(
        (remoteUnit): DevicePropertyDefinition => ({
          name: "Voltage",
          componentId: `rmu_${remoteUnit.id}`,
          propertyId: (2 << 8) | remoteUnit.id,
          unit: "V",
          format: {
            type: DevicePropertyValueType.UnsignedInt,
            mul: 0.1,
          },
        })
      ),
      ...this.remoteUnits
        .filter((remoteUnit) => remoteUnit.hasSensor)
        .map(
          (remoteUnit): DevicePropertyDefinition => ({
            name: "Moisture",
            componentId: `snr_${remoteUnit.id}`,
            propertyId: (4 << 8) | remoteUnit.id,
            unit: "kPa",
            format: {
              type: DevicePropertyValueType.SignedInt,
              mul: 0.1,
            },
          })
        ),
      ...this.solenoids.map(
        (solenoid): DevicePropertyDefinition => ({
          name: "Open",
          componentId: `zn_${solenoid.id}`,
          propertyId: (3 << 8) | solenoid.id,
          mutable: true,
          format: {
            type: DevicePropertyValueType.BooleanFlags,
          },
        })
      ),
    ];
  }
}
