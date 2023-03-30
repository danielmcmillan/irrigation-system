import {
  ConfigType,
  ConfigEntry,
  ConfigEntryRemoteUnitNode,
  ConfigEntryRemoteUnitSolenoid,
  serializeConfigEntries,
  deserializeConfigEntries,
} from "./config";

describe("serializeConfigEntries and deserializeConfigEntries", () => {
  const testEntries: ConfigEntry[] = [
    {
      controllerId: 1,
      type: ConfigType.RemoteUnitNode,
      remoteUnitId: 2,
      nodeNumber: 3,
    },
    {
      controllerId: 4,
      type: ConfigType.RemoteUnitSolenoid,
      solenoidId: 5,
      remoteUnitId: 6,
      solenoidNumber: 7,
    },
  ];

  test("serializeConfigEntries and deserializeConfigEntries should be inverses", () => {
    const serialized = serializeConfigEntries(testEntries);
    const deserialized = deserializeConfigEntries(serialized);

    expect(deserialized).toEqual(testEntries);
  });

  test("deserializeConfigEntries should return an empty array for an empty buffer", () => {
    const emptyBuffer = new ArrayBuffer(0);
    const deserialized = deserializeConfigEntries(emptyBuffer);

    expect(deserialized).toEqual([]);
  });

  test("deserializeConfigEntries should throw for an invalid entry length", () => {
    const invalidLengthData = new Uint8Array([
      0x07, 0x01, 0x01, 0x02, 0x03, 0x00, 0x00,
    ]).buffer;

    expect(() => {
      deserializeConfigEntries(invalidLengthData);
    }).toThrowError("Invalid entry length: 7");
  });

  test("deserializeConfigEntries should throw for an invalid config type", () => {
    const invalidTypeData = new Uint8Array([0x06, 0x01, 0x03, 0x02, 0x03, 0x00])
      .buffer;

    expect(() => {
      deserializeConfigEntries(invalidTypeData);
    }).toThrowError("Invalid config type: 3");
  });
});
