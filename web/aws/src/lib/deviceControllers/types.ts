export interface DeviceComponentDefinition {
  type: string;
  id: string;
  relatedIds?: string[];
  typeName: string;
  name: string;
}

export enum DevicePropertyValueType {
  BooleanFlags,
  UnsignedInt,
  SignedInt,
}

export interface DevicePropertyDefinition {
  propertyId: number;
  componentId: string;
  name: string;
  mutable?: boolean;
  unit?: string;
  format: {
    type: DevicePropertyValueType;
    /** Index into value for boolean flag properties. Default is 0. */
    bitIndex?: number;
    /** Value to multiply with the raw value. */
    mul?: number;
  };
}

export interface DeviceControllerDefinition {
  /**
   * Adds a controller configuration of the specified type.
   * Returns the length of configuration data read from `data`.
   */
  addConfig(type: number, data: Uint8Array): void;

  getComponents(): DeviceComponentDefinition[];
  getProperties(): DevicePropertyDefinition[];
}

export type DeviceControllerDefinitions = Map<number, DeviceControllerDefinition>;
