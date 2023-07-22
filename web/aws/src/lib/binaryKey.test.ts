import { describe, expect, it } from "@jest/globals";
import { KeyPart, buildBinaryKey, parseBinaryKey } from "./binaryKey";

describe("binaryKey", () => {
  it("should parse binary key", () => {
    const parts = [KeyPart.uint8, KeyPart.utf8, KeyPart.uint16le];
    const key = buildBinaryKey(parts, [123, "text", 1234])!;
    const result = parseBinaryKey(key, parts);
    expect(key.byteLength).toBe(8);
    expect(result).toEqual([123, "text", 1234]);
  });

  it("should not build binary key with invalid type", () => {
    const key = buildBinaryKey([KeyPart.uint8], ["1"]);
    expect(key).toBeUndefined();
  });

  it("should parse binary key with extra part", () => {
    const key = buildBinaryKey(
      [KeyPart.uint8, KeyPart.uint16le, KeyPart.uint8],
      [123, 1234, 23]
    )!;
    const result = parseBinaryKey(key, [KeyPart.uint8, KeyPart.uint16le]);
    expect(key.byteLength).toBe(4);
    expect(result).toEqual([123, 1234]);
  });

  it("should not parse binary key with missing part", () => {
    const key = buildBinaryKey([KeyPart.uint8, KeyPart.uint16le], [1, 2])!;
    const result = parseBinaryKey(key, [
      KeyPart.uint8,
      KeyPart.uint16le,
      KeyPart.uint8,
    ]);
    expect(key.byteLength).toBe(3);
    expect(result).toBeUndefined();
  });

  it("should not parse binary key with unterminated string", () => {
    const key = new TextEncoder().encode("text");
    const result = parseBinaryKey(key, [KeyPart.utf8]);
    expect(key.byteLength).toBe(4);
    expect(result).toBeUndefined();
  });
});
