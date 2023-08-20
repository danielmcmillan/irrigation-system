export type KeyPartValue = number | boolean | string;
export enum KeyPartType {
  uint8,
  uint16le,
  uint32le,
  bool,
  utf8,
}
type KeyPartTypeForValue<V extends KeyPartValue> = V extends number
  ? KeyPartType.uint8 | KeyPartType.uint16le | KeyPartType.uint32le
  : V extends boolean
  ? KeyPartType.bool
  : V extends string
  ? KeyPartType.utf8
  : never;

interface StaticKeyPartDefinition<Value extends KeyPartValue> {
  value: Value;
  type: KeyPartTypeForValue<Value>;
}

type ValueOf<T> = T[keyof T];
type DynamicKeyPartDefinition<Key extends object> = ValueOf<{
  [Property in keyof Key]: {
    field: Property;
    type: KeyPartTypeForValue<Key[Property]>;
  };
}>;

export type KeyDefinition<Key extends object> = Array<
  | DynamicKeyPartDefinition<Key>
  | StaticKeyPartDefinition<number>
  | StaticKeyPartDefinition<string>
>;

const textEncoder = new TextEncoder();
const textDecoder = new TextDecoder();

export function buildBinaryKey<Key extends object>(
  definition: KeyDefinition<Key>,
  values: Key
): Uint8Array | undefined {
  const binParts: Uint8Array[] = [];
  let keyPartIndex = 0;
  for (const keyPartDefinition of definition) {
    const value =
      "value" in keyPartDefinition
        ? keyPartDefinition.value
        : values[keyPartDefinition.field];
    if (
      typeof value !==
      (keyPartDefinition.type === KeyPartType.utf8
        ? "string"
        : keyPartDefinition.type === KeyPartType.bool
        ? "boolean"
        : "number")
    ) {
      return undefined;
    }
    switch (keyPartDefinition.type) {
      case KeyPartType.uint8:
        binParts.push(new Uint8Array([value as number]));
        break;
      case KeyPartType.uint16le: {
        const binPart = new ArrayBuffer(2);
        new DataView(binPart).setUint16(0, value as number, true);
        binParts.push(new Uint8Array(binPart));
        break;
      }
      case KeyPartType.uint32le: {
        const binPart = new ArrayBuffer(4);
        new DataView(binPart).setUint32(0, value as number, true);
        binParts.push(new Uint8Array(binPart));
        break;
      }
      case KeyPartType.bool: {
        binParts.push(new Uint8Array([value ? 1 : 0]));
        break;
      }
      case KeyPartType.utf8:
        binParts.push(textEncoder.encode(value as string));
        binParts.push(new Uint8Array([0]));
        break;
    }
    ++keyPartIndex;
  }
  const result = new Uint8Array(
    binParts.reduce((acc, part) => acc + part.length, 0)
  );
  let offset = 0;
  for (const binPart of binParts) {
    result.set(binPart, offset);
    offset += binPart.length;
  }
  return result;
}

export function parseBinaryKey<Key extends object>(
  key: Uint8Array,
  definition: KeyDefinition<Key>
): Key | undefined {
  const result = {} as Key;
  let offset = 0;
  const view = new DataView(key.buffer, key.byteOffset, key.byteLength);
  for (const keyPartDefinition of definition) {
    let value: KeyPartValue;
    try {
      switch (keyPartDefinition.type) {
        case KeyPartType.uint8:
          value = view.getUint8(offset);
          offset += 1;
          break;
        case KeyPartType.uint16le:
          value = view.getUint16(offset, true);
          offset += 2;
          break;
        case KeyPartType.uint32le:
          value = view.getUint32(offset, true);
          offset += 4;
          break;
        case KeyPartType.bool:
          value = key[offset] > 0 ? true : false;
          offset += 1;
          break;
        case KeyPartType.utf8: {
          const end = key.indexOf(0, offset);
          if (end === -1) {
            return undefined;
          }
          value = textDecoder.decode(key.subarray(offset, end));
          offset = end + 1;
          break;
        }
        default:
          return undefined;
      }
    } catch (err) {
      if (err instanceof RangeError) {
        return undefined;
      }
      throw err;
    }
    if ("field" in keyPartDefinition) {
      result[keyPartDefinition.field] = value as Key[keyof Key];
    } else if (keyPartDefinition.value !== value) {
      return undefined;
    }
  }
  return offset <= key.length ? result : undefined;
}
