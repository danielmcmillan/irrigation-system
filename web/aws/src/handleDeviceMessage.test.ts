import { DynamoDBDocumentClient, QueryCommand, UpdateCommand } from "@aws-sdk/lib-dynamodb";
import { beforeEach, describe, expect, it } from "@jest/globals";
import { mockClient } from "aws-sdk-client-mock";
import { handleDeviceMessage } from "./handleDeviceMessage.js";
import { SQSClient } from "@aws-sdk/client-sqs";

const sqsMock = mockClient(SQSClient);
const dbMock = mockClient(DynamoDBDocumentClient);
const mockDeviceName = "dev1";
const mockDeviceNameBin = new TextEncoder().encode(mockDeviceName);

describe("handleDeviceMessage", () => {
  beforeEach(() => {
    dbMock.reset();
  });

  it("should update property state from an event", async () => {
    dbMock.on(QueryCommand).resolves({
      Items: [
        {
          pk: Uint8Array.from([0, 1]),
          sk: Uint8Array.from([1, ...mockDeviceNameBin, 0, 1, 4, 1, 0, 0]),
          val: Uint8Array.from([1, 0, 0, 0]),
          upd: 123,
          chg: 122,
        },
      ],
    });
    await handleDeviceMessage({
      data: "9wVIBAQLAmo=",
      time: 1689765899000,
      clientId: "all",
      deviceId: "icu-1",
      type: "event",
    });
    const params = dbMock.commandCalls(UpdateCommand).at(0)?.args.at(0)?.input;
    console.log(params);
    // expect(params?.Key).toEqual({
    //   pk: Uint8Array.from([1]),
    //   sk: new Uint8Array([...new TextEncoder().encode(mockDeviceName), 0, 0]),
    // });
  });
});
