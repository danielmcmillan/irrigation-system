import { getConfiguredDeviceControllerDefinitions } from "../deviceControllers/configureDeviceControllers.js";
import {
  DeviceComponentDefinition,
  DevicePropertyDefinition,
  DevicePropertyValueType,
} from "../deviceControllers/types.js";
import { DeviceStatus } from "../deviceStatus.js";
import { DeviceState, PropertyState } from "../store.js";

export enum PropertyType {
  Number,
  Boolean,
}

export interface Property {
  /** String uniquely identifying this property */
  id: string;
  componentId: string;
  name: string;
  mutable?: boolean;
  unit?: string;
  type?: PropertyType;
  lastUpdated?: number;
  lastChanged?: number;
  value?: number | boolean;
  desired?: {
    lastUpdated?: number;
    lastChanged?: number;
    value?: number | boolean;
  };
}

export enum AlertSeverity {
  Info,
  Warning,
  Error,
  Critical,
}
export interface Alert {
  time: number;
  severity: AlertSeverity;
  /** Id of the property if the alert is specific to a property. */
  propertyId?: string;
  message: string;
}

export interface Device {
  id: string;
  connected: boolean;
  status?: DeviceStatus;
  lastUpdated: number;
  components: DeviceComponentDefinition[];
  properties: Property[];
  alerts: Alert[];
}

export function parsePropertyId(propertyId: string): {
  controllerId: number;
  propertyId: number;
  bitIndex?: number;
} {
  const data = Buffer.from(propertyId, "base64");
  const view = new DataView(data.buffer, data.byteOffset, data.byteLength);
  return {
    controllerId: view.getUint8(0),
    propertyId: view.getUint16(1, true),
    bitIndex: data.byteLength > 3 ? view.getUint8(3) : undefined,
  };
}

export function getPropertyId(
  controllerId: number,
  propertyId: number,
  bitIndex: number | undefined
): string {
  // Generate the unique string id for the property as base64(controller id + property id LE [+ bit index])
  const id = new Uint8Array(bitIndex !== undefined ? 4 : 3);
  const idView = new DataView(id.buffer, id.byteOffset, id.byteLength);
  idView.setUint8(0, controllerId);
  idView.setUint16(1, propertyId, true);
  if (bitIndex !== undefined) {
    idView.setUint8(3, bitIndex);
  }
  return Buffer.from(id).toString("base64");
}

export function getPropertyValue(
  definition: DevicePropertyDefinition,
  value: Uint8Array | undefined
): number | undefined {
  if (!value) {
    return undefined;
  }
  const view = new DataView(value.buffer, value.byteOffset, value.byteLength);

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
  controllerId: number,
  definition: DevicePropertyDefinition,
  state: PropertyState | undefined,
  desiredState: PropertyState | undefined
): Property {
  return {
    id: getPropertyId(controllerId, definition.propertyId, definition.format.bitIndex),
    componentId: definition.componentId,
    name: definition.name,
    type:
      definition.format.type === DevicePropertyValueType.BooleanFlags
        ? PropertyType.Boolean
        : PropertyType.Number,
    unit: definition.unit,
    mutable: definition.mutable,
    lastUpdated: state?.lastUpdated,
    lastChanged: state?.lastChanged,
    value: getPropertyValue(definition, state?.value),
    desired:
      definition.mutable && desiredState
        ? {
            lastUpdated: desiredState?.lastUpdated,
            lastChanged: desiredState?.lastChanged,
            value: getPropertyValue(definition, desiredState?.value),
          }
        : undefined,
  };
}

/**
 * Combines the property definitions and state to get a list of properties with the public interface.
 * When skipStateless is enabled, only properties with state provided are returned.
 */
export function getProperties(
  deviceId: string,
  controllerId: number,
  definitions: DevicePropertyDefinition[],
  propertyState: PropertyState[],
  skipStateless = false
): Property[] {
  const properties: Property[] = [];
  for (const propertyDefinition of definitions) {
    const states = propertyState.filter(
      (property) =>
        property.deviceId === deviceId &&
        property.controllerId === controllerId &&
        property.propertyId === propertyDefinition.propertyId
    );
    if (!skipStateless || states.length > 0) {
      const [state, desiredState] = [
        states.find((s) => !s.isDesiredValue),
        states.find((s) => s.isDesiredValue),
      ];
      properties.push(getProperty(controllerId, propertyDefinition, state, desiredState));
    }
  }
  return properties;
}

export function getDevice(deviceState: DeviceState, propertyState: PropertyState[]): Device {
  const controllers = getConfiguredDeviceControllerDefinitions(deviceState.config);
  const components: DeviceComponentDefinition[] = [];
  const properties: Property[] = [];
  for (const [controllerId, controller] of controllers.entries()) {
    components.push(...controller.getComponents());
    properties.push(
      ...getProperties(
        deviceState.deviceId,
        controllerId,
        controller.getProperties(),
        propertyState
      )
    );
  }
  return {
    id: deviceState.deviceId,
    connected: deviceState.connected ?? false,
    status: deviceState.status,
    lastUpdated: deviceState.lastUpdated,
    components,
    properties,
    alerts: [],
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
