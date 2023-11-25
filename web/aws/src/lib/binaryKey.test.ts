import { describe, expect, it } from "@jest/globals";
import { KeyPartType, KeyDefinition, buildBinaryKey, parseBinaryKey } from "./binaryKey.js";

describe("binaryKey", () => {
  it("should parse binary key", () => {
    const definition: KeyDefinition<{
      x: number;
      y: string;
      z: number;
      a: number;
      b: boolean;
    }> = [
      { value: 100, type: KeyPartType.uint8 },
      { field: "x", type: KeyPartType.uint8 },
      { field: "y", type: KeyPartType.utf8 },
      { field: "z", type: KeyPartType.uint16le },
      { field: "a", type: KeyPartType.uint32le },
      { field: "b", type: KeyPartType.bool },
    ];
    const keyObj = { x: 123, y: "text", z: 1234, a: 101, b: true };
    const keyBin = buildBinaryKey(definition, keyObj)!;
    expect(keyBin[0]).toBe(100);
    expect(keyBin.byteLength).toBe(14);
    const result = parseBinaryKey(keyBin, definition);
    expect(result).toEqual(keyObj);
  });

  it("should not build binary key with invalid type", () => {
    const key = buildBinaryKey([{ field: "x", type: KeyPartType.uint8 }], {
      x: "1" as unknown as number,
    });
    expect(key).toBeUndefined();
  });

  it("should parse binary key with extra part", () => {
    const key = buildBinaryKey(
      [
        { field: "x", type: KeyPartType.uint8 },
        { field: "y", type: KeyPartType.uint16le },
        { field: "z", type: KeyPartType.uint8 },
      ],
      { x: 123, y: 1234, z: 23 }
    )!;
    const result = parseBinaryKey(key, [
      { field: "x", type: KeyPartType.uint8 },
      { field: "y", type: KeyPartType.uint16le },
    ]);
    expect(key.byteLength).toBe(4);
    expect(result).toEqual({ x: 123, y: 1234 });
  });

  it("should not parse binary key with missing part", () => {
    const key = buildBinaryKey(
      [
        { field: "x", type: KeyPartType.uint8 },
        { field: "x", type: KeyPartType.uint16le },
      ],
      { x: 1, y: 2 }
    )!;
    const result = parseBinaryKey(key, [
      { field: "x", type: KeyPartType.uint8 },
      { field: "y", type: KeyPartType.uint16le },
      { field: "z", type: KeyPartType.uint8 },
    ]);
    expect(key.byteLength).toBe(3);
    expect(result).toBeUndefined();
  });

  it("should not parse binary key with unmatched literal part", () => {
    const key = buildBinaryKey(
      [
        { value: 123, type: KeyPartType.uint8 },
        { field: "x", type: KeyPartType.uint8 },
      ],
      { x: 1 }
    )!;
    const result = parseBinaryKey(key, [
      { value: 124, type: KeyPartType.uint8 },
      { field: "x", type: KeyPartType.uint8 },
    ]);
    expect(key.byteLength).toBe(2);
    expect(result).toBeUndefined();
  });

  it("should not parse binary key with unterminated string", () => {
    const key = new TextEncoder().encode("text");
    const result = parseBinaryKey(key, [{ field: "x", type: KeyPartType.utf8 }]);
    expect(key.byteLength).toBe(4);
    expect(result).toBeUndefined();
  });
});
