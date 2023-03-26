export enum IrrigationPropertyValueType {
  BooleanFlags = 0,
  UnsignedInt = 1,
  SignedInt = 2,
}

export interface IrrigationPropertyBooleanFormat {
  type: IrrigationPropertyValueType.BooleanFlags;
  booleanCount: number;
  unit?: string;
}
export interface IrrigationPropertyNumericFormat {
  type:
    | IrrigationPropertyValueType.SignedInt
    | IrrigationPropertyValueType.UnsignedInt;
  mulBase: number;
  mulExponent: number;
  unit?: string;
}
export type IrrigationPropertyFormat =
  | IrrigationPropertyBooleanFormat
  | IrrigationPropertyNumericFormat;

export type IrrigationPropertyValue = number | boolean[];

export interface IrrigationProperty {
  controllerId: number;
  propertyId: number;
  isReadOnly: boolean;
  value: IrrigationPropertyValue;
  desiredValue?: IrrigationPropertyValue;
  objectName: string;
  propertyName: string;
  format: IrrigationPropertyFormat;
}

const propertyTextDecoder = new TextDecoder();

function getPropertyFormatFromData(payload: ArrayBufferLike): {
  format: IrrigationPropertyFormat;
  bytesLength: number;
} {
  const view = new DataView(payload);
  const type: IrrigationPropertyValueType = view.getUint8(0);
  if (type === IrrigationPropertyValueType.BooleanFlags) {
    return {
      bytesLength: 3 + view.getUint8(2),
      format: {
        type,
        booleanCount: view.getUint8(1),
      },
    };
  } else {
    const unitLength = view.getUint8(3);
    return {
      bytesLength: 4 + unitLength,
      format: {
        type,
        mulBase: view.getUint8(1),
        mulExponent: view.getInt8(2),
        unit:
          unitLength > 0
            ? propertyTextDecoder.decode(payload.slice(4, 4 + unitLength))
            : undefined,
      },
    };
  }
}

export function getPropertyValue(
  valueBytes: ArrayBufferLike,
  format: IrrigationPropertyFormat
): IrrigationPropertyValue {
  const view = new DataView(valueBytes);
  let valueNum: number;
  if (format.type === IrrigationPropertyValueType.SignedInt) {
    if (view.byteLength === 1) {
      valueNum = view.getInt8(0);
    } else if (view.byteLength === 2) {
      valueNum = view.getInt16(0, true);
    } else if (view.byteLength === 4) {
      valueNum = view.getInt32(0, true);
    } else {
      throw new Error(
        `Signed integer with size ${valueBytes.byteLength} is not supported`
      );
    }
  } else {
    valueNum = 0;
    // Arbitrary length little endian unsigned value
    for (let i = 0; i < view.byteLength; ++i) {
      valueNum += view.getUint8(i) << (i * 8);
    }
  }
  if (format.type === IrrigationPropertyValueType.BooleanFlags) {
    return Array(format.booleanCount)
      .fill(0)
      .map((_, bitIndex) => (valueNum & (1 << bitIndex)) > 0);
  } else {
    return valueNum * format.mulBase ** format.mulExponent;
  }
}

function getPropertyFromData(payload: ArrayBufferLike): {
  property: IrrigationProperty;
  bytesLength: number;
} {
  const view = new DataView(payload);
  const isReadOnly = view.getUint8(3) != 0;
  const valueLength = view.getUint8(4);
  const valueBytes = payload.slice(5, 5 + valueLength);
  const desiredValueBytes = isReadOnly
    ? undefined
    : payload.slice(5 + valueLength, 5 + valueLength * 2);

  const objectNameOffset = 5 + valueLength * (isReadOnly ? 1 : 2);
  const objectNameLength = view.getUint8(objectNameOffset);
  const propertyNameOffset = objectNameOffset + 1 + objectNameLength;
  const propertyNameLength = view.getUint8(propertyNameOffset);
  const formatOffset = propertyNameOffset + 1 + propertyNameLength;
  const { format, bytesLength: formatBytesLength } = getPropertyFormatFromData(
    payload.slice(formatOffset)
  );

  const property: IrrigationProperty = {
    controllerId: view.getUint8(0),
    propertyId: view.getUint16(1, true),
    isReadOnly,
    value: getPropertyValue(valueBytes, format),
    desiredValue:
      desiredValueBytes === undefined
        ? undefined
        : getPropertyValue(desiredValueBytes, format),
    objectName: propertyTextDecoder.decode(
      payload.slice(objectNameOffset + 1, propertyNameOffset)
    ),
    propertyName: propertyTextDecoder.decode(
      payload.slice(propertyNameOffset + 1, formatOffset)
    ),
    format,
  };
  return {
    bytesLength: formatOffset + formatBytesLength,
    property,
  };
}

export function getPropertiesFromData(
  payload: ArrayBufferLike
): IrrigationProperty[] {
  const properties: IrrigationProperty[] = [];
  for (let i = 0; i < payload.byteLength; ) {
    const { property, bytesLength } = getPropertyFromData(payload.slice(i));
    properties.push(property);
    i += bytesLength;
  }
  return properties;
}
