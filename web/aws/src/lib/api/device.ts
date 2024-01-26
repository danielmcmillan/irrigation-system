import { configureDeviceControllers } from "../deviceControllers/configureDeviceControllers.js";
import { getControllerDefinitions } from "../deviceControllers/definitions/getControllerDefinitions.js";
import {
  DeviceComponentDefinition,
  DevicePropertyDefinition,
  DevicePropertyValueType,
} from "../deviceControllers/types.js";
import { DeviceStatus } from "../deviceStatus.js";
import { DeviceState, PropertyState } from "../store.js";

export interface Property {
  /** String uniquely identifying this property */
  id: string;
  componentId: string;
  name: string;
  mutable?: boolean;
  unit?: string;
  lastUpdated?: number;
  lastChanged?: number;
  value?: number | boolean;
  desired?: {
    lastUpdated?: number;
    lastChanged?: number;
    value?: number | boolean;
  };
}

export interface Device {
  id: string;
  connected: boolean;
  status?: DeviceStatus;
  lastUpdated: number;
  components: DeviceComponentDefinition[];
  properties: Property[];
}

function getPropertyValue(
  definition: DevicePropertyDefinition,
  value: Uint8Array | undefined
): number | undefined {
  if (!value) {
    return undefined;
  }
  const view = new DataView(
    value.buffer.slice(value.byteOffset, value.byteOffset + value.byteLength)
  );

  let result: number | undefined;
  if (definition.format.type === DevicePropertyValueType.BooleanFlags) {
    // Value is 1 or 0 from the bit at specified index
    const bitIndex = definition.format.bitIndex ?? 0;
    const byteIndex = bitIndex % 8;
    const byte = byteIndex < view.byteLength ? view.getUint8(bitIndex % 8) : undefined;
    const bitInByte = (bitIndex / 8) | 0;
    if (byte !== undefined) {
      result = (byte & (1 << bitInByte)) === 0 ? 0 : 1;
    }
  } else if (definition.format.type === DevicePropertyValueType.UnsignedInt) {
    result = 0;
    for (let i = 0; i < view.byteLength; ++i) {
      result |= view.getUint8(i) >> (i * 8);
    }
  } else if (definition.format.type === DevicePropertyValueType.SignedInt) {
    if (view.byteLength === 1) {
      result = view.getInt8(0);
    } else if (view.byteLength === 2) {
      result = view.getInt16(0, true);
    } else if (view.byteLength === 4) {
      result = view.getInt32(0, true);
    } else {
      console.error(`Signed integer with size ${view.byteLength} is not supported`);
    }
  }
  if (result !== undefined) {
    return definition.format.mul !== undefined ? result * definition.format.mul : result;
  } else {
    console.warn(
      `Value for property ${definition.name} with type ${definition.format.type} could not be converted.`
    );
  }
}

function getProperty(
  definition: DevicePropertyDefinition,
  controllerId: number,
  state: PropertyState | undefined,
  desiredState: PropertyState | undefined
): Property {
  // Generate the unique string id for the property as base64(controller id + property id LE [+ bit index])
  const id = new Uint8Array(definition.format.bitIndex !== undefined ? 4 : 3);
  const idView = new DataView(id.buffer, id.byteOffset, id.byteOffset + id.byteLength);
  idView.setUint8(0, controllerId);
  idView.setUint16(1, definition.propertyId, true);
  if (definition.format.bitIndex !== undefined) {
    idView.setUint8(3, definition.format.bitIndex);
  }
  return {
    id: Buffer.from(id).toString("base64"),
    componentId: definition.componentId,
    name: definition.name,
    unit: definition.unit,
    mutable: definition.mutable,
    lastUpdated: state?.lastUpdated,
    lastChanged: state?.lastChanged,
    value: getPropertyValue(definition, state?.value),
    desired: definition.mutable
      ? {
          lastUpdated: desiredState?.lastUpdated,
          lastChanged: desiredState?.lastChanged,
          value: getPropertyValue(definition, desiredState?.value),
        }
      : undefined,
  };
}

function getDevice(deviceState: DeviceState, propertyState: PropertyState[]): Device {
  const controllers = getControllerDefinitions();
  if (deviceState.config) {
    configureDeviceControllers(controllers, deviceState.config);
  }
  const components: DeviceComponentDefinition[] = [];
  const properties: Property[] = [];
  for (const [controllerId, controller] of controllers.entries()) {
    components.push(...controller.getComponents());
    properties.push(
      ...controller.getProperties().map((propertyDefinition) => {
        const states = propertyState.filter(
          (property) =>
            property.controllerId === controllerId &&
            property.propertyId === propertyDefinition.propertyId
        );
        const [state, desiredState] = [
          states.find((s) => !s.isDesiredValue),
          states.find((s) => s.isDesiredValue),
        ];
        return getProperty(propertyDefinition, controllerId, state, desiredState);
      })
    );
  }
  return {
    id: deviceState.deviceId,
    connected: deviceState.connected ?? false,
    status: deviceState.status,
    lastUpdated: deviceState.lastUpdated,
    components,
    properties,
  };
}

export function getDevices(deviceState: DeviceState[], propertyState: PropertyState[]): Device[] {
  return deviceState.map((device) =>
    getDevice(
      device,
      propertyState.filter((p) => p.deviceId === device.deviceId)
    )
  );
}
