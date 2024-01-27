/** Convert binary buffer to hex string */
export function binToHex(bin: ArrayBufferLike): string {
  return new Uint8Array(bin).reduce((prev, cur) => prev + cur.toString(16).padStart(2, "0"), "");
}

/** Convert hex string to binary buffer */
export function hexToBin(hex: string): Uint8Array {
  return Uint8Array.from(hex.match(/.{1,2}/g)?.map((byte) => parseInt(byte, 16)) ?? []);
}

/** Convert number to hex string */
export function numberToHex(num: number, bytes: number): string {
  return num.toString(16).padStart(bytes * 2, "0");
}
