export type IrrigationPropertyValue = number | boolean;

export enum PropertyType {
  Number,
  Boolean,
}

export interface IrrigationProperty {
  /** String uniquely identifying this property */
  id: string;
  componentId: string;
  name: string;
  mutable?: boolean;
  type?: PropertyType;
  unit?: string;
  lastUpdated?: number;
  lastChanged?: number;
  value?: IrrigationPropertyValue;
  desired?: {
    lastUpdated?: number;
    lastChanged?: number;
    value?: number | boolean;
  };
}
