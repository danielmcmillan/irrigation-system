export enum KeyPart {
  uint8,
  uint16le,
  utf8,
}
export type KeyPartValue = number | string;

const textEncoder = new TextEncoder();
const textDecoder = new TextDecoder();

export function buildBinaryKey(
  keyParts: KeyPart[],
  values: KeyPartValue[]
): Uint8Array | undefined {
  const binParts: Uint8Array[] = [];
  let keyPartIndex = 0;
  for (const keyPart of keyParts) {
    const value = values[keyPartIndex];
    if (typeof value !== (keyPart === KeyPart.utf8 ? "string" : "number")) {
      return undefined;
    }
    switch (keyPart) {
      case KeyPart.uint8:
        binParts.push(new Uint8Array([value as number]));
        break;
      case KeyPart.uint16le:
        binParts.push(
          new Uint8Array([
            value as number & 0xff,
            ((value as number) >> 8) & 0xff,
          ])
        );
        break;
      case KeyPart.utf8:
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

export function parseBinaryKey(
  key: Uint8Array,
  keyParts: KeyPart[]
): KeyPartValue[] | undefined {
  const result: KeyPartValue[] = [];
  let offset = 0;
  for (const keyPartType of keyParts) {
    switch (keyPartType) {
      case KeyPart.uint8:
        result.push(key[offset]);
        offset++;
        break;
      case KeyPart.uint16le:
        result.push(key[offset] | (key[offset + 1] << 8));
        offset += 2;
        break;
      case KeyPart.utf8:
        const end = key.indexOf(0, offset);
        if (end === -1) {
          return undefined;
        }
        result.push(textDecoder.decode(key.subarray(offset, end)));
        offset = end + 1;
        break;
    }
  }
  return offset <= key.length ? result : undefined;
}
