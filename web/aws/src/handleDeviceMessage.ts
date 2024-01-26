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

const sqs = new SQSClient({});
const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
const historyTtl = 3600 * 24 * 90; // 90 days

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

async function updateStateStore(message: DeviceMessage): Promise<void> {
  const messageTime = (message.time / 1000) | 0;
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
    const lastChanged =
      existingProperty && Buffer.from(existingProperty.value).equals(Buffer.from(property.value))
        ? existingProperty.lastChanged
        : messageTime;
    // Property from previous event in same message
    const previousProperty = newPropertyStates.find(propertyPredicate);
    if (previousProperty) {
      previousProperty.value = property.value;
      previousProperty.lastChanged = Math.max(previousProperty.lastChanged, lastChanged);
    } else {
      newPropertyStates.push({
        ...property,
        deviceId: message.deviceId,
        lastUpdated: messageTime,
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
      lastUpdated: messageTime,
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
}

export async function handleDeviceMessage(inputEvent: RawDeviceMessage): Promise<void> {
  const message = parseDeviceMessage(inputEvent);

  const results = await Promise.allSettled([
    publishToSQS(message),
    sendNotifications(message),
    updateStateStore(message),
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
  // TODO send request for properties when needed and expire old properties
  // TODO forward events to websocket clients
}
