export interface DeviceProperty {
  controllerId: number;
  propertyId: number;
  isReadOnly: boolean;
  value: ArrayBuffer;
  desiredValue?: ArrayBuffer;
}

function getPropertyFromData(payload: ArrayBufferLike): {
  property: DeviceProperty;
  bytesLength: number;
} {
  const view = new DataView(payload);
  const isReadOnly = view.getUint8(3) != 0;
  const valueLength = view.getUint8(4);
  const value = payload.slice(5, 5 + valueLength);
  const desiredValue = isReadOnly
    ? undefined
    : payload.slice(5 + valueLength, 5 + valueLength * 2);

  const objectNameOffset = 5 + valueLength * (isReadOnly ? 1 : 2);
  const objectNameLength = view.getUint8(objectNameOffset);
  const propertyNameOffset = objectNameOffset + 1 + objectNameLength;
  const propertyNameLength = view.getUint8(propertyNameOffset);
  const formatOffset = propertyNameOffset + 1 + propertyNameLength;
  const formatBytesLength =
    view.getUint8(formatOffset) === 0
      ? 3 + view.getUint8(formatOffset + 2)
      : 4 + view.getUint8(formatOffset + 3);

  const property: DeviceProperty = {
    controllerId: view.getUint8(0),
    propertyId: view.getUint16(1, true),
    isReadOnly,
    value,
    desiredValue,
  };
  return {
    bytesLength: formatOffset + formatBytesLength,
    property,
  };
}

export function getPropertiesFromData(
  payload: ArrayBufferLike
): DeviceProperty[] {
  const properties: DeviceProperty[] = [];
  for (let i = 0; i < payload.byteLength; ) {
    const { property, bytesLength } = getPropertyFromData(payload.slice(i));
    properties.push(property);
    i += bytesLength;
  }
  return properties;
}
