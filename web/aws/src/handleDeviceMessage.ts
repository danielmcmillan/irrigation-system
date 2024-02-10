import { SQSClient, SendMessageCommand } from "@aws-sdk/client-sqs";
import pLimit from "p-limit";
import { DeviceEventType } from "./lib/deviceMessage/event.js";
import {
  DeviceMessage,
  RawDeviceMessage,
  parseDeviceMessage,
} from "./lib/deviceMessage/deviceMessage.js";
import {
  DeviceState,
  DeviceStateQueryType,
  IrrigationDataStore,
  PropertyState,
} from "./lib/store.js";
import { sendPushNotification } from "./lib/pushNotifications.js";
import { DeviceStatus } from "./lib/deviceStatus.js";
import { DeviceUpdate, DeviceUpdateEvent } from "./lib/api/messages.js";
import { Alert, AlertSeverity, getProperties, getPropertyId } from "./lib/api/device.js";
import { getControllerDefinitions } from "./lib/deviceControllers/definitions/getControllerDefinitions.js";
import { configureDeviceControllers } from "./lib/deviceControllers/configureDeviceControllers.js";
import {
  ApiGatewayManagementApiClient,
  PostToConnectionCommand,
} from "@aws-sdk/client-apigatewaymanagementapi";
import { getControllerErrorMessage, getPropertyErrorMessage } from "./lib/api/alert.js";
import { WebSocketClient } from "./lib/webSocketClient.js";
import { IoTDataPlaneClient, PublishCommand } from "@aws-sdk/client-iot-data-plane";

const sqs = new SQSClient({});
const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
const historyTtl = 3600 * 24 * 90; // 90 days
const apigw = new ApiGatewayManagementApiClient({
  endpoint: process.env.API_GATEWAY_ENDPOINT,
});
const iotData = new IoTDataPlaneClient({
  endpoint: process.env.IOT_ENDPOINT,
});

async function publishToSQS(message: DeviceMessage): Promise<void> {
  if (["event", "error"].includes(message.type)) {
    await sqs.send(
      new SendMessageCommand({
        QueueUrl: process.env.STORAGE_QUEUE_URL,
        // Encode binary data as base64 strings
        MessageBody: JSON.stringify(message, (_key, value) =>
          value instanceof ArrayBuffer ? Buffer.from(value).toString("base64") : value
        ),
      })
    );
  }
}

async function sendNotifications(message: DeviceMessage): Promise<void> {
  if (message.type === "event" && message.events) {
    for (const event of message.events) {
      if (event.type === DeviceEventType.Started) {
        await sendPushNotification(
          {
            title: `Irrigation system ${message.deviceId} restarted`,
            message:
              "If the system is in use and the restart is not expected then check that it is still operating as expected.",
          },
          store
        );
      }
    }
  }
}

/** Get the public device update interface given the updated state. */
export function getDeviceUpdate(
  deviceState: DeviceState,
  propertyState: PropertyState[],
  deviceStateUpdated: boolean,
  alerts: Alert[]
): DeviceUpdate {
  const controllers = getControllerDefinitions();
  if (deviceState.config) {
    configureDeviceControllers(controllers, deviceState.config);
  }
  const propertyUpdates: DeviceUpdate["properties"] = [];
  for (const [controllerId, controller] of controllers.entries()) {
    for (const property of getProperties(
      deviceState.deviceId,
      controllerId,
      controller.getProperties(),
      propertyState,
      true
    )) {
      propertyUpdates.push({
        id: property.id,
        lastUpdated: property.lastUpdated,
        lastChanged: property.lastChanged,
        value: property.value,
        desired: property.desired,
      });
    }
  }
  return {
    id: deviceState.deviceId,
    connected: deviceState.connected,
    status: deviceState.status,
    lastUpdated: deviceStateUpdated ? deviceState.lastUpdated : undefined,
    properties: propertyUpdates,
    alerts: alerts.length > 0 ? alerts : undefined,
  };
}

async function updateStateStore(message: DeviceMessage): Promise<void> {
  const messageTimeSeconds = (message.time / 1000) | 0;
  let newDeviceState: Pick<DeviceState, "connected" | "status" | "config"> | undefined;
  const newPropertyStates: PropertyState[] = [];
  let properties: PropertyState[] | undefined;

  if (["event", "properties"].includes(message.type)) {
    ({ properties } = await store.getDeviceState(
      DeviceStateQueryType.Properties,
      message.deviceId
    ));
  } else if (!["connected", "disconnected", "config"].includes(message.type)) {
    return;
  }

  const addProperty = (
    property: Pick<PropertyState, "controllerId" | "propertyId" | "isDesiredValue" | "value">
  ) => {
    const propertyPredicate = (p: PropertyState) =>
      p.controllerId === property.controllerId &&
      p.propertyId === property.propertyId &&
      (p.isDesiredValue ?? false) === (property.isDesiredValue ?? false);
    // Property already in table
    const existingProperty = properties?.find(propertyPredicate);
    const isValueUnchanged =
      existingProperty && Buffer.from(existingProperty.value).equals(Buffer.from(property.value));
    // We can ignore an update for a "property" message if the value didn't change
    const isValueUpdated = message.type !== "property" || !isValueUnchanged;
    const lastChanged = isValueUnchanged ? existingProperty.lastChanged : messageTimeSeconds;
    // Property from previous event in same message
    const previousProperty = newPropertyStates.find(propertyPredicate);
    if (previousProperty) {
      previousProperty.value = property.value;
      previousProperty.lastChanged = Math.max(previousProperty.lastChanged, lastChanged);
    } else if (isValueUpdated) {
      newPropertyStates.push({
        ...property,
        deviceId: message.deviceId,
        lastUpdated: messageTimeSeconds,
        lastChanged,
      });
    }
  };
  if (message.type === "event" && message.events) {
    for (const event of message.events) {
      if (
        event.type === DeviceEventType.PropertyValueChanged ||
        event.type === DeviceEventType.PropertyDesiredValueChanged
      ) {
        const isDesiredValue = event.type === DeviceEventType.PropertyDesiredValueChanged;
        addProperty({
          controllerId: event.controllerId,
          propertyId: event.propertyId,
          isDesiredValue,
          value: new Uint8Array(event.value),
        });
      } else if (event.type === DeviceEventType.Started) {
        newDeviceState = { status: DeviceStatus.Unconfigured };
      } else if (event.type === DeviceEventType.Configured) {
        newDeviceState = { status: DeviceStatus.Initializing };
      } else if (event.type === DeviceEventType.Ready) {
        newDeviceState = { status: DeviceStatus.Ready };
      }
    }
  } else if (message.type === "properties" && message.properties) {
    for (const property of message.properties) {
      addProperty({
        controllerId: property.controllerId,
        propertyId: property.propertyId,
        isDesiredValue: false,
        value: new Uint8Array(property.value),
      });
      if (property.desiredValue !== undefined) {
        addProperty({
          controllerId: property.controllerId,
          propertyId: property.propertyId,
          isDesiredValue: true,
          value: new Uint8Array(property.desiredValue),
        });
      }
    }
  } else if (message.type === "connected") {
    newDeviceState = { connected: true };
  } else if (message.type === "disconnected") {
    newDeviceState = { connected: false };
  } else if (message.type === "config") {
    newDeviceState = { config: message.data ? new Uint8Array(message.data) : undefined };
  }
  const promises: Promise<unknown>[] = [];
  const limit = pLimit(5);
  if (newDeviceState) {
    const deviceState = {
      deviceId: message.deviceId,
      lastUpdated: messageTimeSeconds,
      ...newDeviceState,
    };
    promises.push(limit(() => store.updateDeviceState(deviceState)));
    promises.push(limit(() => store.addDeviceHistory(deviceState, historyTtl)));
  }
  for (const prop of newPropertyStates) {
    promises.push(limit(() => store.updatePropertyState(prop)));
    promises.push(limit(() => store.addPropertyHistory(prop, historyTtl)));
  }
  await Promise.all(promises);

  // TODO separate function to build alerts from events/errors
  const alerts: Alert[] = [];
  if (message.type === "event" && message.events) {
    for (const event of message.events) {
      switch (event.type) {
        case DeviceEventType.Started:
          alerts.push({
            time: messageTimeSeconds,
            severity: AlertSeverity.Critical,
            message: "Irrigation system restarted",
          });
          break;
        case DeviceEventType.GeneralError:
          alerts.push({
            time: messageTimeSeconds,
            severity: AlertSeverity.Error,
            message: Buffer.from(event.data).toString("hex"),
          });
          break;
        case DeviceEventType.GeneralWarning:
          alerts.push({
            time: messageTimeSeconds,
            severity: AlertSeverity.Warning,
            message: Buffer.from(event.data).toString("hex"),
          });
          break;
        case DeviceEventType.GeneralInfo:
          alerts.push({
            time: messageTimeSeconds,
            severity: AlertSeverity.Info,
            message: Buffer.from(event.data).toString("hex"),
          });
          break;
        case DeviceEventType.ControllerError:
          alerts.push({
            time: messageTimeSeconds,
            severity: AlertSeverity.Error,
            message: getControllerErrorMessage(event.controllerId, event.data),
          });
          break;
        case DeviceEventType.PropertyError:
          alerts.push({
            time: messageTimeSeconds,
            severity: AlertSeverity.Error,
            message: getPropertyErrorMessage(event.controllerId, event.propertyId, event.data),
            propertyId: getPropertyId(event.controllerId, event.propertyId, undefined), // TODO doesn't support bit flag properties
          });
          break;
      }
    }
  } else if (message.type === "error" && message.error) {
    alerts.push({
      time: messageTimeSeconds,
      severity: AlertSeverity.Error,
      message: `${["Wifi", "Mqtt", "Config", "Controller", "Update"][message.error.component]} (${
        message.error.code
      }): ${message.error.text}`,
    });
  }
  // TODO persist alerts in store

  // Send events to subscribed websocket clients
  // Todo: depends on list of device/property updates but should be decoupled from the store updates
  let clients: WebSocketClient[] | undefined;
  if (newDeviceState || newPropertyStates.length > 0 || alerts.length > 0) {
    // Todo: don't run queries sequentially
    const {
      devices: [deviceState],
    } = await store.getDeviceState(DeviceStateQueryType.Device, message.deviceId);
    clients = (await store.listWebSocketClients()).filter((client) =>
      client.deviceIds?.includes(message.deviceId)
    );
    console.debug(
      `Sending update for device ${deviceState?.deviceId} to ${clients.length} websocket clients`
    );
    if (!deviceState || clients.length === 0) {
      return;
    }
    const event: DeviceUpdateEvent = {
      type: "device/update",
      ...getDeviceUpdate(
        {
          deviceId: message.deviceId,
          config: deviceState.config,
          lastUpdated: messageTimeSeconds,
          ...newDeviceState,
        },
        newPropertyStates,
        newDeviceState !== undefined,
        alerts
      ),
    };
    await Promise.all(
      clients.map(async (client) =>
        apigw.send(
          new PostToConnectionCommand({
            ConnectionId: client.connectionId,
            Data: JSON.stringify(event),
          })
        )
      )
    );
  }
}

async function requestDeviceState(message: DeviceMessage): Promise<void> {
  // TODO maybe just have the device automatically send state on startup
  if (
    message.type === "event" &&
    message.events?.some((event) => event.type === DeviceEventType.Ready)
  ) {
    await Promise.all([
      iotData.send(
        new PublishCommand({
          topic: `icu-in/${message.deviceId}/getConfig`,
          payload: "iot",
          qos: 1,
        })
      ),
      iotData.send(
        new PublishCommand({
          topic: `icu-in/${message.deviceId}/getProperties`,
          payload: "iot",
          qos: 1,
        })
      ),
    ]);
  }
}

export async function handleDeviceMessage(inputEvent: RawDeviceMessage): Promise<void> {
  const message = parseDeviceMessage(inputEvent);

  const results = await Promise.allSettled([
    publishToSQS(message),
    sendNotifications(message),
    updateStateStore(message),
    requestDeviceState(message),
  ]);
  for (const result of results) {
    if (result.status === "rejected") {
      console.error(
        "An error ocurred when handling message",
        { inputEvent, message },
        result.reason
      );
    }
  }
  // TODO expire old properties
}
