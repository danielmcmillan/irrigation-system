import { ConditionalCheckFailedException, DynamoDBClient } from "@aws-sdk/client-dynamodb";
import {
  DeleteCommand,
  DynamoDBDocumentClient,
  PutCommand,
  QueryCommand,
  QueryCommandInput,
  UpdateCommand,
} from "@aws-sdk/lib-dynamodb";
import { PushSubscription } from "web-push";
import { KeyDefinition, KeyPartType, buildBinaryKey, parseBinaryKey } from "./binaryKey.js";
import { DeviceStatus } from "./deviceStatus.js";
import { WebSocketClient } from "./webSocketClient.js";

export interface DeviceState {
  deviceId: string;
  lastUpdated: number;
  connected?: boolean;
  status?: DeviceStatus;
  config?: Uint8Array;
}

export interface PropertyState {
  deviceId: string;
  controllerId: number;
  propertyId: number;
  isDesiredValue: boolean;
  value: Uint8Array;
  lastUpdated: number;
  lastChanged: number;
  eventId?: number;
}

export enum DeviceStateQueryType {
  Device = 0,
  Properties = 1,
}

export interface ScheduleState {
  deviceId: string;
  entries: Array<{
    properties: Array<number>;
    startTime: number;
    endTime: number;
  }>;

  messageId?: string;
  abort?: boolean;
  state?: Array<{
    id: number;
    /** The latest value requested */
    set: boolean;
    /** The actual value seen after being requested */
    seen: boolean;
    setTime?: number;
    /** True when the time limit for the pending change has been reached */
    expired?: boolean;
  }>;
  lastEvaluated?: number;
}

const tableKeys: {
  devicePropertyState: {
    pk: KeyDefinition<{}>;
    skDeviceState: KeyDefinition<{ deviceId: string }>;
    skPropertyState: KeyDefinition<{
      deviceId: string;
      controllerId: number;
      propertyId: number;
      isDesiredValue: number;
    }>;
    partialSkType: KeyDefinition<{ type: number }>;
    partialSkDeviceId: KeyDefinition<{ type: number; deviceId: string }>;
  };
  deviceHistory: {
    pk: KeyDefinition<{
      deviceId: string;
    }>;
    sk: KeyDefinition<{ lastUpdated: number }>;
  };
  propertyHistory: {
    pk: KeyDefinition<{
      deviceId: string;
      controllerId: number;
      propertyId: number;
    }>;
    sk: KeyDefinition<{ lastUpdated: number }>;
  };
  schedule: {
    pk: KeyDefinition<{}>;
    sk: KeyDefinition<{ deviceId: string }>;
  };
  webSocketClient: {
    pk: KeyDefinition<{}>;
    sk: KeyDefinition<{ connectionId: string }>;
  };
  pushNotificationSubscription: {
    pk: KeyDefinition<{}>;
    sk: KeyDefinition<{ endpoint: string }>;
  };
} = {
  devicePropertyState: {
    pk: [
      { value: 0x00, type: KeyPartType.uint8 },
      { value: 0x01, type: KeyPartType.uint8 },
    ],
    skDeviceState: [
      { value: 0x00, type: KeyPartType.uint8 },
      { field: "deviceId", type: KeyPartType.utf8 },
    ],
    skPropertyState: [
      { value: 0x01, type: KeyPartType.uint8 },
      { field: "deviceId", type: KeyPartType.utf8 },
      { field: "controllerId", type: KeyPartType.uint8 },
      { field: "propertyId", type: KeyPartType.uint16le },
      { field: "isDesiredValue", type: KeyPartType.uint8 },
    ],
    partialSkType: [{ field: "type", type: KeyPartType.uint8 }],
    partialSkDeviceId: [
      { field: "type", type: KeyPartType.uint8 },
      { field: "deviceId", type: KeyPartType.utf8 },
    ],
  },
  deviceHistory: {
    pk: [
      { value: 0x00, type: KeyPartType.uint8 },
      { value: 0x02, type: KeyPartType.uint8 },
      { field: "deviceId", type: KeyPartType.utf8 },
    ],
    sk: [{ field: "lastUpdated", type: KeyPartType.uint32be }],
  },
  propertyHistory: {
    pk: [
      { value: 0x00, type: KeyPartType.uint8 },
      { value: 0x02, type: KeyPartType.uint8 },
      { field: "deviceId", type: KeyPartType.utf8 },
      { field: "controllerId", type: KeyPartType.uint8 },
      { field: "propertyId", type: KeyPartType.uint16le },
    ],
    sk: [{ field: "lastUpdated", type: KeyPartType.uint32be }],
  },
  schedule: {
    pk: [
      { value: 0x00, type: KeyPartType.uint8 },
      { value: 0x03, type: KeyPartType.uint8 },
    ],
    sk: [{ field: "deviceId", type: KeyPartType.utf8 }],
  },
  webSocketClient: {
    pk: [
      { value: 0x00, type: KeyPartType.uint8 },
      { value: 0x10, type: KeyPartType.uint8 },
    ],
    sk: [{ field: "connectionId", type: KeyPartType.utf8 }],
  },
  pushNotificationSubscription: {
    pk: [
      { value: 0x00, type: KeyPartType.uint8 },
      { value: 0x11, type: KeyPartType.uint8 },
    ],
    sk: [{ field: "endpoint", type: KeyPartType.utf8 }],
  },
};

export class IrrigationDataStore {
  private db: DynamoDBDocumentClient;
  private tableName: string;

  constructor(config: { tableName: string; region?: string }) {
    this.db = DynamoDBDocumentClient.from(new DynamoDBClient({ region: config.region }), {
      marshallOptions: { removeUndefinedValues: true },
    });
    this.tableName = config.tableName;
  }

  /** Get the state for all devices and their properties or the device with the specified id. */
  async getDeviceState(
    type?: DeviceStateQueryType,
    deviceId?: string
  ): Promise<{ devices: DeviceState[]; properties: PropertyState[] }> {
    let keyCondition = "pk = :pk";
    const values: Record<string, unknown> = {
      ":pk": buildBinaryKey(tableKeys.devicePropertyState.pk, {}),
    };
    if (type !== undefined) {
      keyCondition += " AND begins_with(sk, :sk)";
      values[":sk"] =
        deviceId === undefined
          ? buildBinaryKey(tableKeys.devicePropertyState.partialSkType, {
              type,
            })
          : buildBinaryKey(tableKeys.devicePropertyState.partialSkDeviceId, {
              type,
              deviceId,
            });
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
      const devSkParts = parseBinaryKey(item.sk, tableKeys.devicePropertyState.partialSkDeviceId);
      if (!devSkParts) {
        throw new Error(`Invalid sk: ${item.sk}`);
      }
      const { type, deviceId } = devSkParts;

      if (type == DeviceStateQueryType.Device) {
        devices.push({
          deviceId,
          lastUpdated: item.upd,
          connected: item.con,
          status: item.sts,
          config: item.cfg,
        });
      } else {
        const propSkParts = parseBinaryKey(item.sk, tableKeys.devicePropertyState.skPropertyState);
        if (!propSkParts) {
          throw new Error(`Invalid sk: ${item.sk}`);
        }
        const { controllerId, propertyId, isDesiredValue } = propSkParts;
        properties.push({
          deviceId,
          controllerId,
          propertyId,
          isDesiredValue: isDesiredValue === 1,
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
    if (state.config !== undefined) {
      names["#cf"] = "cfg";
      values[":cf"] = new Uint8Array(state.config);
      setExpressions.push("#cf = :cf");
    }

    try {
      await this.db.send(
        new UpdateCommand({
          TableName: this.tableName,
          Key: {
            pk: buildBinaryKey(tableKeys.devicePropertyState.pk, {}),
            sk: buildBinaryKey(tableKeys.devicePropertyState.skDeviceState, {
              deviceId: state.deviceId,
            }),
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
   * Record a log of the current state of a device.
   */
  async addDeviceHistory(state: DeviceState, ttl: number) {
    if (state.connected === undefined) {
      // Only track updates to connected state in history
      return;
    }
    await this.db.send(
      new PutCommand({
        TableName: this.tableName,
        Item: {
          pk: buildBinaryKey(tableKeys.deviceHistory.pk, state),
          sk: buildBinaryKey(tableKeys.deviceHistory.sk, state),
          con: state.connected,
          exp: state.lastUpdated + ttl,
        },
      })
    );
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
      ":vl": state.value,
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
            pk: buildBinaryKey(tableKeys.devicePropertyState.pk, {}),
            sk: buildBinaryKey(tableKeys.devicePropertyState.skPropertyState, {
              deviceId: state.deviceId,
              controllerId: state.controllerId,
              propertyId: state.propertyId,
              isDesiredValue: state.isDesiredValue ? 1 : 0,
            }),
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

  /**
   * Record a log of the current state of a property.
   */
  async addPropertyHistory(state: PropertyState, ttl: number) {
    await this.db.send(
      new PutCommand({
        TableName: this.tableName,
        Item: {
          pk: buildBinaryKey(tableKeys.propertyHistory.pk, state),
          sk: buildBinaryKey(tableKeys.propertyHistory.sk, state),
          val: new Uint8Array(state.value),
          exp: state.lastUpdated + ttl,
        },
      })
    );
  }

  /**
   * Get a list of historical property values.
   */
  async getPropertyHistory(
    property: {
      deviceId: string;
      controllerId: number;
      propertyId: number;
    },
    from: number,
    to: number
  ): Promise<Array<{ time: number; value: Uint8Array }>> {
    const result = await this.db.send(
      new QueryCommand({
        TableName: this.tableName,
        KeyConditionExpression: "pk = :pk AND sk BETWEEN :from AND :to",
        ScanIndexForward: false,
        ExpressionAttributeValues: {
          ":pk": buildBinaryKey(tableKeys.propertyHistory.pk, property),
          ":from": buildBinaryKey(tableKeys.propertyHistory.sk, { lastUpdated: from }),
          ":to": buildBinaryKey(tableKeys.propertyHistory.sk, { lastUpdated: to }),
        },
      })
    );
    return (result.Items ?? []).map((item) => {
      const skParts = parseBinaryKey(item.sk, tableKeys.propertyHistory.sk);
      if (!skParts) {
        throw new Error(`Invalid property history sk: ${item.sk}`);
      }
      return {
        time: skParts.lastUpdated,
        value: item.val,
      };
    });
  }

  /**
   * Record a connected WebSocket client.
   */
  async addWebSocketClient(client: WebSocketClient, ttl: number): Promise<void> {
    const { connectionId: _connectionId, ...data } = client;
    await this.db.send(
      new PutCommand({
        TableName: this.tableName,
        Item: {
          pk: buildBinaryKey(tableKeys.webSocketClient.pk, client),
          sk: buildBinaryKey(tableKeys.webSocketClient.sk, client),
          ...data,
          exp: ((Date.now() / 1000) | 0) + ttl,
        },
      })
    );
  }

  /**
   * Remove a WebSocket client.
   */
  async removeWebSocketClient(client: WebSocketClient): Promise<void> {
    await this.db.send(
      new DeleteCommand({
        TableName: this.tableName,
        Key: {
          pk: buildBinaryKey(tableKeys.webSocketClient.pk, client),
          sk: buildBinaryKey(tableKeys.webSocketClient.sk, client),
        },
      })
    );
  }

  /**
   * List all connected WebSocket clients.
   */
  async listWebSocketClients(): Promise<WebSocketClient[]> {
    const result = await this.db.send(
      new QueryCommand({
        TableName: this.tableName,
        KeyConditionExpression: "pk = :pk",
        ExpressionAttributeValues: {
          ":pk": buildBinaryKey(tableKeys.webSocketClient.pk, {}),
        },
      })
    );
    return (result.Items ?? []).map((item) => {
      const skParts = parseBinaryKey(item.sk, tableKeys.webSocketClient.sk);
      if (!skParts) {
        throw new Error(`Invalid WebSocket client sk: ${item.sk}`);
      }
      return {
        connectionId: skParts.connectionId,
        deviceIds: item.deviceIds ?? [],
      };
    });
  }

  /**
   * Record a subscribed push notification client.
   */
  async addPushNotificationSubscription(
    subscription: PushSubscription,
    deviceId: string
  ): Promise<void> {
    const { endpoint: _endpoint, ...data } = subscription;
    await this.db.send(
      new PutCommand({
        TableName: this.tableName,
        Item: {
          pk: buildBinaryKey(tableKeys.pushNotificationSubscription.pk, subscription),
          sk: buildBinaryKey(tableKeys.pushNotificationSubscription.sk, subscription),
          ...data,
          deviceId,
        },
      })
    );
  }

  /**
   * Remove a subscribed push notification client.
   */
  async removePushNotificationSubscription(subscription: PushSubscription): Promise<void> {
    await this.db.send(
      new DeleteCommand({
        TableName: this.tableName,
        Key: {
          pk: buildBinaryKey(tableKeys.pushNotificationSubscription.pk, subscription),
          sk: buildBinaryKey(tableKeys.pushNotificationSubscription.sk, subscription),
        },
      })
    );
  }

  /**
   * List all subscribed push notification clients.
   * @param endpoint If specified, only list subscriptions for this endpoint.
   */
  async listPushNotificationSubscriptions(
    endpoint?: string
  ): Promise<Array<PushSubscription & { deviceId: string }>> {
    const queryCommandInput: QueryCommandInput = {
      TableName: this.tableName,
      KeyConditionExpression: "pk = :pk",
      ExpressionAttributeValues: {
        ":pk": buildBinaryKey(tableKeys.pushNotificationSubscription.pk, {}),
      },
    };
    if (endpoint !== undefined) {
      queryCommandInput.KeyConditionExpression += " AND sk = :sk";
      queryCommandInput.ExpressionAttributeValues![":sk"] = buildBinaryKey(
        tableKeys.pushNotificationSubscription.sk,
        { endpoint }
      );
    }
    const result = await this.db.send(new QueryCommand(queryCommandInput));
    return (result.Items ?? []).map((item) => {
      const skParts = parseBinaryKey(item.sk, tableKeys.pushNotificationSubscription.sk);
      if (!skParts) {
        throw new Error(`Invalid push subscription sk: ${item.sk}`);
      }
      return {
        endpoint: skParts.endpoint,
        keys: item.keys,
        deviceId: item.deviceId,
      };
    });
  }

  /**
   * Get the scheduled property changes for a device.
   */
  async getSchedule(deviceId: string): Promise<ScheduleState> {
    const result = await this.db.send(
      new QueryCommand({
        TableName: this.tableName,
        KeyConditionExpression: "pk = :pk AND sk = :sk",
        ExpressionAttributeValues: {
          ":pk": buildBinaryKey(tableKeys.schedule.pk, {}),
          ":sk": buildBinaryKey(tableKeys.schedule.sk, { deviceId }),
        },
      })
    );
    const item = result.Items?.[0];
    return {
      deviceId,
      entries: item?.e ?? [],
      state: item?.s ?? [],
      abort: item?.a ?? false,
      messageId: item?.m,
      lastEvaluated: item?.t,
    };
  }

  /**
   * Set the scheduled property changes for a device.
   */
  async updateSchedule(deviceId: string, schedule: Partial<ScheduleState>): Promise<void> {
    const names: Record<string, string> = {};
    const values: Record<string, unknown> = {};
    const updateExpressions: string[] = [];

    if (schedule.entries) {
      names["#e"] = "e";
      values[":e"] = schedule.entries;
      updateExpressions.push("#e = :e");
    }
    if (schedule.abort !== undefined) {
      names["#a"] = "a";
      values[":a"] = schedule.abort;
      updateExpressions.push("#a = :a");
    }
    if (schedule.messageId !== undefined) {
      names["#m"] = "m";
      values[":m"] = schedule.messageId;
      updateExpressions.push("#m = :m");
    }
    if (schedule.state) {
      names["#s"] = "s";
      values[":s"] = schedule.state;
      updateExpressions.push("#s = :s");
    }
    if (schedule.lastEvaluated !== undefined) {
      names["#t"] = "t";
      values[":t"] = schedule.lastEvaluated;
      updateExpressions.push("#t = :t");
    }
    const updateExpression = `SET ${updateExpressions.join(", ")}`;

    await this.db.send(
      new UpdateCommand({
        TableName: this.tableName,
        Key: {
          pk: buildBinaryKey(tableKeys.schedule.pk, {}),
          sk: buildBinaryKey(tableKeys.schedule.sk, { deviceId }),
        },
        ExpressionAttributeNames: names,
        ExpressionAttributeValues: values,
        UpdateExpression: updateExpression,
      })
    );
  }
}
