import {
  DynamoDBDocumentClient,
  PutCommand,
  QueryCommand,
  UpdateCommand,
} from "@aws-sdk/lib-dynamodb";
import { beforeEach, describe, expect, it } from "@jest/globals";
import { mockClient } from "aws-sdk-client-mock";
import { DeviceStateQueryType, IrrigationDataStore, PropertyState } from "./store.js";
import { DeviceStatus } from "./deviceStatus.js";

const dbMock = mockClient(DynamoDBDocumentClient);
const mockDeviceName = "dev1";
const mockDeviceNameBin = new TextEncoder().encode(mockDeviceName);
const store = new IrrigationDataStore({ tableName: "table" });

describe("store", () => {
  const property: PropertyState = {
    deviceId: mockDeviceName,
    controllerId: 4,
    propertyId: 321,
    isDesiredValue: false,
    value: Uint8Array.from([3, 5]),
    lastUpdated: 999,
    lastChanged: 320,
    eventId: 1234,
  };

  beforeEach(() => {
    dbMock.reset();
  });

  it("should query device state", async () => {
    dbMock.on(QueryCommand).resolves({
      Items: [
        {
          pk: Uint8Array.from([0, 1]),
          sk: new Uint8Array([0, ...mockDeviceNameBin, 0]),
          con: true,
          sts: DeviceStatus.Ready,
          upd: 1234,
        },
      ],
    });
    const { devices, properties } = await store.getDeviceState(DeviceStateQueryType.Device);
    const params = dbMock.commandCalls(QueryCommand).at(0)?.args.at(0)?.input;
    expect(params?.KeyConditionExpression).toBe("pk = :pk AND begins_with(sk, :sk)");
    expect(params?.ExpressionAttributeValues).toEqual({
      ":pk": Uint8Array.from([0, 1]),
      ":sk": new Uint8Array([0]),
    });
    expect(devices.length).toBe(1);
    expect(properties.length).toBe(0);
  });

  it("should query property state by device id", async () => {
    dbMock.on(QueryCommand).resolves({});
    await store.getDeviceState(DeviceStateQueryType.Properties, mockDeviceName);
    const params = dbMock.commandCalls(QueryCommand).at(0)?.args.at(0)?.input;
    expect(params?.KeyConditionExpression).toBe("pk = :pk AND begins_with(sk, :sk)");
    expect(params?.ExpressionAttributeValues).toEqual({
      ":pk": Uint8Array.from([0, 1]),
      ":sk": new Uint8Array([1, ...mockDeviceNameBin, 0]),
    });
  });

  it("should update device state", async () => {
    dbMock.on(UpdateCommand).resolves({});
    await store.updateDeviceState({
      deviceId: mockDeviceName,
      lastUpdated: 1000,
      connected: true,
      status: DeviceStatus.Ready,
    });
    const params = dbMock.commandCalls(UpdateCommand).at(0)?.args.at(0)?.input;
    expect(params?.Key).toEqual({
      pk: Uint8Array.from([0, 1]),
      sk: new Uint8Array([0, ...mockDeviceNameBin, 0]),
    });
    expect(params?.UpdateExpression).toEqual(
      "SET " + ["#lu = :lu", "#cn = :cn", "#st = :st"].join(", ")
    );
    expect(params?.ExpressionAttributeNames).toEqual({
      "#lu": "upd",
      "#cn": "con",
      "#st": "sts",
    });
    expect(params?.ExpressionAttributeValues).toEqual({
      ":lu": 1000,
      ":cn": true,
      ":st": DeviceStatus.Ready,
    });
  });

  it("should update property state", async () => {
    dbMock.on(UpdateCommand).resolves({});

    await store.updatePropertyState(property);
    const params = dbMock.commandCalls(UpdateCommand).at(0)?.args.at(0)?.input;
    expect(params?.UpdateExpression).toEqual(
      "REMOVE #ex SET " + ["#vl = :vl", "#lu = :lu", "#lc = :lc", "#ev = :ev"].join(", ")
    );
    expect(params?.ExpressionAttributeNames).toEqual({
      "#vl": "val",
      "#lu": "upd",
      "#lc": "chg",
      "#ev": "evtId",
      "#ex": "exp",
    });
    expect(params?.ExpressionAttributeValues).toEqual({
      ":vl": property.value,
      ":lu": 999,
      ":lc": 320,
      ":ev": 1234,
    });
  });

  it("should add property historical record", async () => {
    dbMock.on(PutCommand).resolves({});
    await store.addPropertyHistory(property, 3600);
    const params = dbMock.commandCalls(PutCommand).at(0)?.args.at(0)?.input;
    expect(params).toEqual({
      TableName: "table",
      Item: {
        pk: new Uint8Array([0, 2, ...mockDeviceNameBin, 0, 4, 321 & 0xff, (321 >> 8) & 0xff]),
        sk: new Uint8Array(Uint32Array.from([999]).buffer),
        val: property.value,
        exp: property.lastUpdated + 3600,
      },
    });
  });
});
