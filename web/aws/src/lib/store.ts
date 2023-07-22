import {
  ConditionalCheckFailedException,
  DynamoDBClient,
} from "@aws-sdk/client-dynamodb";
import {
  DynamoDBDocumentClient,
  PutCommand,
  QueryCommand,
  UpdateCommand,
} from "@aws-sdk/lib-dynamodb";
import { KeyPart, buildBinaryKey, parseBinaryKey } from "./binaryKey";

export enum DeviceStatus {
  Unconfigured = 0,
  Initializing = 1,
  Ready = 2,
}

export interface DeviceState {
  deviceId: string;
  lastUpdated: number;
  connected?: boolean;
  status?: DeviceStatus;
}

export interface PropertyState {
  deviceId: string;
  controllerId: number;
  propertyId: number;
  isDesiredValue: boolean;
  value: ArrayBuffer;
  lastUpdated: number;
  lastChanged: number;
  eventId?: number;
}

const deviceStateSkParts = [
  KeyPart.utf8, // device id
  KeyPart.uint8, // 0x00
];
const propertyStateSkParts = [
  KeyPart.utf8, // device id
  KeyPart.uint8, // 0x01
  KeyPart.uint8, // controller id
  KeyPart.uint16le, // property id
  KeyPart.uint8, // value=0, desired=1
];
const propertyHistoricalPkParts = [
  KeyPart.uint8, // 0x01
  KeyPart.utf8, // device id
  KeyPart.uint8, // controller id
  KeyPart.uint16le, // property id
];

export class IrrigationDataStore {
  private db: DynamoDBDocumentClient;
  private tableName: string;

  constructor(config: { tableName: string; region?: string }) {
    this.db = DynamoDBDocumentClient.from(
      new DynamoDBClient({ region: config.region })
    );
    this.tableName = config.tableName;
  }

  /** Get the state for all devices and their properties or the device with the specified id. */
  async getDeviceState(
    deviceId?: string
  ): Promise<{ devices: DeviceState[]; properties: PropertyState[] }> {
    let keyCondition = "pk = :pk";
    const values: Record<string, unknown> = {
      ":pk": Uint8Array.from([1]),
    };
    if (deviceId) {
      keyCondition += " AND begins_with(sk, :sk)";
      values[":sk"] = buildBinaryKey([KeyPart.utf8], [deviceId]);
    }

    const result = await this.db.send(
      new QueryCommand({
        TableName: this.tableName,
        KeyConditionExpression: keyCondition,
        ExpressionAttributeValues: values,
      })
    );
    const devices: DeviceState[] = [];
    const properties: PropertyState[] = [];
    for (const item of result.Items ?? []) {
      const devSkParts = parseBinaryKey(item.sk, deviceStateSkParts);
      if (!devSkParts) {
        throw new Error(`Invalid sk: ${item.sk}`);
      }
      const deviceId = devSkParts[0] as string;
      const isDevice = devSkParts[1] === 0;

      if (isDevice) {
        devices.push({
          deviceId,
          lastUpdated: item.upd,
          connected: item.con,
          status: item.sts,
        });
      } else {
        const propSkParts = parseBinaryKey(item.sk, propertyStateSkParts);
        if (!propSkParts) {
          throw new Error(`Invalid sk: ${item.sk}`);
        }
        const controllerId = propSkParts[2] as number;
        const propertyId = propSkParts[3] as number;
        const isDesiredValue = propSkParts[4] === 1;
        properties.push({
          deviceId,
          controllerId,
          propertyId,
          isDesiredValue,
          value: item.val,
          lastUpdated: item.upd,
          lastChanged: item.chg,
        });
      }
    }

    return { devices, properties };
  }

  /**
   * Update the state for a device.
   *
   * Only specified attributes are updated.
   */
  async updateDeviceState(state: DeviceState): Promise<void> {
    const names: Record<string, string> = {
      "#lu": "upd",
    };
    const values: Record<string, unknown> = {
      ":lu": state.lastUpdated,
    };
    const setExpressions = ["#lu = :lu"];
    if (state.connected !== undefined) {
      names["#cn"] = "con";
      values[":cn"] = state.connected;
      setExpressions.push("#cn = :cn");
    }
    if (state.status !== undefined) {
      names["#st"] = "sts";
      values[":st"] = state.status;
      setExpressions.push("#st = :st");
    }

    try {
      await this.db.send(
        new UpdateCommand({
          TableName: this.tableName,
          Key: {
            pk: new Uint8Array([1]),
            sk: buildBinaryKey(deviceStateSkParts, [state.deviceId, 0]),
          },
          ExpressionAttributeNames: names,
          ExpressionAttributeValues: values,
          UpdateExpression: `SET ${setExpressions.join(", ")}`,
          ConditionExpression: "attribute_not_exists(#lu) OR :lu >= #lu",
        })
      );
    } catch (err) {
      if (err instanceof ConditionalCheckFailedException) {
        // Table updated more recently, ignore
        console.warn(
          `Skipping update of device state for ${state.deviceId} at ${state.lastUpdated} since it was updated more recently.`
        );
      } else {
        throw err;
      }
    }
  }

  /**
   * Update the state for a property.
   */
  async updatePropertyState(state: PropertyState): Promise<void> {
    const names: Record<string, string> = {
      "#vl": "val",
      "#lu": "upd",
      "#lc": "chg",
      "#ex": "exp",
    };
    const values: Record<string, unknown> = {
      ":vl": new Uint8Array(state.value),
      ":lu": state.lastUpdated,
      ":lc": state.lastChanged,
    };
    let updateExpression = "REMOVE #ex SET #vl = :vl, #lu = :lu, #lc = :lc";

    if (state.eventId !== undefined) {
      names["#ev"] = "evtId";
      values[":ev"] = state.eventId;
      updateExpression += ", #ev = :ev";
    }

    try {
      await this.db.send(
        new UpdateCommand({
          TableName: this.tableName,
          Key: {
            pk: new Uint8Array([1]),
            sk: buildBinaryKey(propertyStateSkParts, [
              state.deviceId,
              1,
              state.controllerId,
              state.propertyId,
              state.isDesiredValue ? 1 : 0,
            ]),
          },
          ExpressionAttributeNames: names,
          ExpressionAttributeValues: values,
          UpdateExpression: updateExpression,
          ConditionExpression: "attribute_not_exists(#lu) OR :lu >= #lu",
        })
      );
    } catch (err) {
      if (err instanceof ConditionalCheckFailedException) {
        // Table updated more recently, ignore
        console.warn(
          `Skipping update of property state for ${state.deviceId}/${state.controllerId}/${state.propertyId} at ${state.lastUpdated} since it was updated more recently.`
        );
      } else {
        throw err;
      }
    }
  }

  async addPropertyHistory(state: PropertyState, ttl: number) {
    await this.db.send(
      new PutCommand({
        TableName: this.tableName,
        Item: {
          pk: buildBinaryKey(propertyHistoricalPkParts, [
            1,
            state.deviceId,
            state.controllerId,
            state.propertyId,
          ]),
          sk: Uint32Array.from([state.lastUpdated]),
          val: new Uint8Array(state.value),
          exp: state.lastUpdated + ttl,
        },
      })
    );
  }
}
