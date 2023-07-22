import { DeviceEventType } from "./lib/deviceMessage/deviceEvent";
import {
  DeviceMessage,
  RawDeviceMessage,
  parseDeviceMessage,
} from "./lib/deviceMessage/deviceMessage";
import { SQSClient, SendMessageCommand } from "@aws-sdk/client-sqs";
import {
  DeviceState,
  DeviceStatus,
  IrrigationDataStore,
  PropertyState,
} from "./lib/store";
import pLimit from "p-limit";

const sqs = new SQSClient({});
const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
const propertyHistoryTtl = 3600 * 24 * 14; // 14 days

async function publishToSQS(message: DeviceMessage): Promise<void> {
  if (["event", "error"].includes(message.type)) {
    await sqs.send(
      new SendMessageCommand({
        QueueUrl: process.env.STORAGE_QUEUE_URL,
        // Encode binary data as base64 strings
        MessageBody: JSON.stringify(message, (_key, value) =>
          value instanceof ArrayBuffer
            ? Buffer.from(value).toString("base64")
            : value
        ),
      })
    );
  }
}

async function updateStateStore(message: DeviceMessage): Promise<void> {
  if (!["event", "properties"].includes(message.type)) {
    return;
  }
  const messageTime = (message.time / 1000) | 0;
  let newDeviceState: Pick<DeviceState, "connected" | "status"> | undefined;
  const newPropertyStates: PropertyState[] = [];
  const { properties } = await store.getDeviceState(message.deviceId);

  const addProperty = (
    property: Pick<
      PropertyState,
      "controllerId" | "propertyId" | "isDesiredValue" | "value"
    >
  ) => {
    const propertyPredicate = (p: PropertyState) =>
      p.controllerId === property.controllerId &&
      p.propertyId === property.propertyId &&
      (p.isDesiredValue ?? false) === (property.isDesiredValue ?? false);
    // Property already in table
    const existingProperty = properties.find(propertyPredicate);
    const lastChanged =
      existingProperty &&
      Buffer.from(existingProperty.value).equals(Buffer.from(property.value))
        ? existingProperty.lastChanged
        : messageTime;
    // Property from previous event in same message
    const previousProperty = newPropertyStates.find(propertyPredicate);
    if (previousProperty) {
      previousProperty.value = property.value;
      previousProperty.lastChanged = Math.max(
        previousProperty.lastChanged,
        lastChanged
      );
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
        const isDesiredValue =
          event.type === DeviceEventType.PropertyDesiredValueChanged;
        addProperty({
          controllerId: event.controllerId,
          propertyId: event.propertyId,
          isDesiredValue,
          value: event.value,
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
        value: property.value,
      });
      if (property.desiredValue !== undefined) {
        addProperty({
          controllerId: property.controllerId,
          propertyId: property.propertyId,
          isDesiredValue: true,
          value: property.desiredValue,
        });
      }
    }
  }
  const promises: Promise<unknown>[] = [];
  const limit = pLimit(5);
  if (newDeviceState) {
    promises.push(
      limit(() =>
        store.updateDeviceState({
          deviceId: message.deviceId,
          lastUpdated: messageTime,
          ...newDeviceState,
        })
      )
    );
  }
  for (const prop of newPropertyStates) {
    promises.push(limit(() => store.updatePropertyState(prop)));
    promises.push(
      limit(() => store.addPropertyHistory(prop, propertyHistoryTtl))
    );
  }
  await Promise.all(promises);
}

export async function handleDeviceMessage(
  inputEvent: RawDeviceMessage
): Promise<void> {
  const message = parseDeviceMessage(inputEvent);

  await Promise.all([
    publishToSQS(message),
    (async () => {
      try {
        await updateStateStore(message);
      } catch (err) {
        console.error(
          "Failed to update state store",
          { inputEvent, message },
          err
        );
      }
    })(),
  ]);
  // TODO handle IoT connection events
  // TODO send request for properties when needed and expire old properties
  // TODO forward events to websocket clients
}
