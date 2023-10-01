export interface DeviceComponentDefinition {
  type: string;
  id: string;
  relatedIds?: string[];
}

export interface DevicePropertyDefinition {
  id: string; // base64(controller id + property id [+ bit index])
  componentId: string;
  name: string;
  mutable?: boolean;
}

export interface DeviceControllerDefinition {
  /**
   * Adds a controller configuration of the specified type.
   * Returns the length of configuration data read from `data`.
   */
  addConfig(type: number, data: ArrayBufferLike): void;

  getComponents(): DeviceComponentDefinition[];
  getProperties(): DevicePropertyDefinition[];
}

export interface DeviceControllerDefinitions {
  [controllerId: number]: DeviceControllerDefinition;
}
