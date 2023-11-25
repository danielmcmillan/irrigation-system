import {
  ConfigType,
  ConfigEntry,
  ConfigEntryRemoteUnitNode,
  ConfigEntryRemoteUnitSolenoid,
  serializeConfigEntriesToBinary,
  deserializeConfigEntriesFromBinary,
} from "./config";

describe("serializeConfigEntries and deserializeConfigEntries", () => {
  const testEntries: ConfigEntry[] = [
    {
      type: ConfigType.RemoteUnitNode,
      remoteUnitId: 2,
      nodeNumber: 3,
    },
    {
      type: ConfigType.RemoteUnitSolenoid,
      solenoidId: 5,
      remoteUnitId: 6,
      solenoidNumber: 7,
    },
  ];

  test("serializeConfigEntries and deserializeConfigEntries should be inverses", () => {
    const serialized = serializeConfigEntriesToBinary(testEntries);
    const deserialized = deserializeConfigEntriesFromBinary(serialized);

    expect(deserialized).toEqual(testEntries);
  });

  test("deserializeConfigEntries should return an empty array for an empty buffer", () => {
    const emptyBuffer = new ArrayBuffer(0);
    const deserialized = deserializeConfigEntriesFromBinary(emptyBuffer);

    expect(deserialized).toEqual([]);
  });

  test("deserializeConfigEntries should throw for an invalid entry length", () => {
    const invalidLengthData = new Uint8Array([0x07, 0x01, 0x01, 0x02, 0x03, 0x00, 0x00]).buffer;

    expect(() => {
      deserializeConfigEntriesFromBinary(invalidLengthData);
    }).toThrowError(`Invalid config type: ${0x0101} with length 7`);
  });

  test("deserializeConfigEntries should throw for an invalid config type", () => {
    const invalidTypeData = new Uint8Array([0x06, 0x01, 0x03, 0x02, 0x03, 0x00]).buffer;

    expect(() => {
      deserializeConfigEntriesFromBinary(invalidTypeData);
    }).toThrowError(`Invalid config type: ${0x0103} with length 6`);
  });
});
