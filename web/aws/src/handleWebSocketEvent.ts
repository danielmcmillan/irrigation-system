import { IoTDataPlaneClient, PublishCommand } from "@aws-sdk/client-iot-data-plane";
import { APIGatewayProxyResultV2, APIGatewayProxyWebsocketEventV2 } from "aws-lambda";
import { getDevices, getPropertyId, getPropertyValue, parsePropertyId } from "./lib/api/device.js";
import {
  DeviceControllerCommandRequest,
  DeviceGetConfigRequest,
  DeviceGetConfigResponse,
  DeviceGetScheduleRequest,
  DeviceGetScheduleResponse,
  DeviceSetConfigRequest,
  DeviceSetScheduleRequest,
  DeviceSetScheduleResponse,
  GetPropertyHistoryRequest,
  GetPropertyHistoryResponse,
  RequestMessage,
  ServerMessage,
  SetPropertyRequest,
  SetPropertyResponse,
  SubscribeDeviceRequest,
  SubscribeDeviceResponse,
  WebPushSubscribeRequest,
  WebPushTestRequest,
  WebPushUnsubscribeRequest,
} from "./lib/api/messages.js";
import { getScheduleStatus } from "./lib/api/schedule.js";
import { getConfiguredDeviceControllerDefinitions } from "./lib/deviceControllers/configureDeviceControllers.js";
import { createSetPropertyCommand } from "./lib/deviceMessage/createSetPropertyCommand.js";
import { sendPushNotification } from "./lib/pushNotifications.js";
import { IrrigationScheduleManager } from "./lib/schedule.js";
import { DeviceStateQueryType, IrrigationDataStore, ScheduleState } from "./lib/store.js";

const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
// Expire client connections after the maximum connection time limit for API Gateway of 2 hours.
const WEB_SOCKET_CLIENT_TTL = 2.5 * 3600;

const iotData = new IoTDataPlaneClient({
  endpoint: process.env.IOT_ENDPOINT,
});

const scheduleManager = new IrrigationScheduleManager({
  queueUrl: process.env.SCHEDULE_QUEUE_URL!,
});

export async function handleWebSocketEvent(
  event: APIGatewayProxyWebsocketEventV2
): Promise<APIGatewayProxyResultV2> {
  console.debug("WebSocket event", {
    type: event.requestContext.eventType,
    body: event.body,
  });
  const connectionId = event.requestContext.connectionId;
  let response: ServerMessage | undefined;

  try {
    if (event.requestContext.eventType === "CONNECT") {
    } else if (event.requestContext.eventType === "DISCONNECT") {
      await store.removeWebSocketClient({ connectionId });
    } else if (event.requestContext.eventType === "MESSAGE" && event.body) {
      const data: RequestMessage = JSON.parse(event.body);
      try {
        if (data.action === "webPush/subscribe") {
          const request = data as WebPushSubscribeRequest;
          await store.addPushNotificationSubscription(request.subscription, request.deviceId);
          response = request;
        } else if (data.action === "webPush/unsubscribe") {
          const request = data as WebPushUnsubscribeRequest;
          await store.removePushNotificationSubscription(request.subscription);
          response = request;
        } else if (data.action === "webPush/test") {
          const request = data as WebPushTestRequest;
          await sendPushNotification(
            {
              title: "Irrigation system test notification",
              message: "Notification received successfully!",
            },
            store,
            { endpoint: request.subscription.endpoint, deviceId: request.deviceId }
          );
          response = data;
        } else if (data.action === "device/subscribe") {
          const request = data as SubscribeDeviceRequest;
          // Subscribe the client to updates for the requested devices
          await store.addWebSocketClient(
            { connectionId, deviceIds: request.deviceIds },
            WEB_SOCKET_CLIENT_TTL
          );
          // Respond with the current state of the requested devices
          // TODO - consistency issue for updates happening at subscription time?
          const [{ devices, properties }, ...scheduleStates] = await Promise.all([
            store.getDeviceState(),
            ...request.deviceIds.map((id) => store.getSchedule(id)),
          ]);
          const subscribeResponse: SubscribeDeviceResponse = {
            action: request.action,
            requestId: request.requestId,
            devices: getDevices(
              devices.filter((device) => request.deviceIds.includes(device.deviceId)),
              properties
            ),
            scheduleStatus: scheduleStates.map((state) => getScheduleStatus(state)),
          };
          response = subscribeResponse;
        } else if (data.action === "property/set") {
          const request = data as SetPropertyRequest;
          const { controllerId, propertyId } = parsePropertyId(request.propertyId);
          await iotData.send(
            createSetPropertyCommand(request.deviceId, controllerId, propertyId, request.value)
          );
          const setPropertyResponse: SetPropertyResponse = {
            action: request.action,
            requestId: request.requestId,
          };
          response = setPropertyResponse;
        } else if (data.action === "propertyHistory/get") {
          const request = data as GetPropertyHistoryRequest;
          const { controllerId, propertyId, bitIndex } = parsePropertyId(request.propertyId);
          // Query values for the last 14 days
          const [
            {
              devices: [device],
            },
            values,
          ] = await Promise.all([
            store.getDeviceState(DeviceStateQueryType.Device, request.deviceId),
            store.getPropertyHistory(
              {
                deviceId: request.deviceId,
                controllerId,
                propertyId,
              },
              Date.now() / 1000 - 14 * 24 * 3600,
              Date.now() / 1000
            ),
          ]);
          const controllerDefinitions = getConfiguredDeviceControllerDefinitions(device?.config);
          const propertyDefinition = controllerDefinitions
            .get(controllerId)
            ?.getProperties()
            .find(
              (property) =>
                property.propertyId === propertyId && property.format.bitIndex === bitIndex
            );
          if (propertyDefinition) {
            const propertyHistoryResponse: GetPropertyHistoryResponse = {
              action: request.action,
              requestId: request.requestId,
              values: values.map((value) => ({
                time: value.time,
                value: getPropertyValue(propertyDefinition, value.value),
              })),
            };
            response = propertyHistoryResponse;
          } else {
            response = {
              action: request.action,
              requestId: request.requestId,
              error: {
                message: `Unknown property with id ${request.propertyId} for device ${request.deviceId}`,
              },
            };
          }
        } else if (data.action === "device/getConfig") {
          const request = data as DeviceGetConfigRequest;
          const {
            devices: [device],
          } = await store.getDeviceState(DeviceStateQueryType.Device, request.deviceId);
          if (device) {
            const getConfigResponse: DeviceGetConfigResponse = {
              action: request.action,
              requestId: request.requestId,
              config: device.config ? Buffer.from(device.config).toString("base64") : undefined,
            };
            response = getConfigResponse;
          } else {
            response = {
              action: request.action,
              requestId: request.requestId,
              error: {
                message: `Unknown device ${request.deviceId}`,
              },
            };
          }
        } else if (data.action === "device/setConfig") {
          const request = data as DeviceSetConfigRequest;
          const {
            devices: [device],
          } = await store.getDeviceState(DeviceStateQueryType.Device, request.deviceId);
          if (device) {
            await iotData.send(
              new PublishCommand({
                topic: `icu-in/${request.deviceId}/setConfig`,
                payload: Buffer.from(request.config, "base64"),
                qos: 1,
              })
            );
            response = { action: request.action, requestId: request.requestId };
          } else {
            response = {
              action: request.action,
              requestId: request.requestId,
              error: {
                message: `Unknown device ${request.deviceId}`,
              },
            };
          }
        } else if (data.action === "device/controllerCommand") {
          const request = data as DeviceControllerCommandRequest;
          const commandData = Buffer.from(request.data, "base64");
          const payload = new ArrayBuffer(3 + commandData.byteLength);
          const view = new DataView(payload);
          view.setUint16(0, request.commandId, true);
          view.setUint8(2, request.controllerId);
          new Uint8Array(payload).set(commandData, 3);
          await iotData.send(
            new PublishCommand({
              topic: `icu-in/${request.deviceId}/command`,
              payload,
              qos: 1,
            })
          );
          response = { action: request.action, requestId: request.requestId };
        } else if (data.action === "device/getSchedule") {
          const request = data as DeviceGetScheduleRequest;
          const schedule = await store.getSchedule(request.deviceId);
          const getScheduleResponse: DeviceGetScheduleResponse = {
            action: request.action,
            deviceId: request.deviceId,
            requestId: request.requestId,
            entries: schedule.entries.map((entry) => ({
              propertyIds: entry.properties.map((num) =>
                getPropertyId(num >> 16, num & 0xffff, undefined)
              ),
              startTime: entry.startTime,
              endTime: entry.endTime,
            })),
          };
          response = getScheduleResponse;
        } else if (data.action === "device/setSchedule") {
          const request = data as DeviceSetScheduleRequest;
          const entries: ScheduleState["entries"] = request.entries.map((entry) => ({
            properties: entry.propertyIds.map((propertyId) => {
              const parsed = parsePropertyId(propertyId);
              return (parsed.controllerId << 16) | parsed.propertyId;
            }),
            startTime: entry.startTime,
            endTime: entry.endTime,
          }));
          const messageId = IrrigationScheduleManager.generateMessageId();
          const schedule: Partial<ScheduleState> = {
            entries,
            abort: false,
            messageId,
          };
          await Promise.all([
            store.updateSchedule(request.deviceId, schedule),
            scheduleManager.sendMessage(request.deviceId, messageId, 1),
          ]);
          const setScheduleResponse: DeviceSetScheduleResponse = {
            action: request.action,
            requestId: request.requestId,
            deviceId: request.deviceId,
            entries: request.entries,
          };
          response = setScheduleResponse;
        } else {
          response = {
            action: data.action,
            requestId: data.requestId,
            error: { message: "Invalid WebSocket message" },
          };
        }
      } catch (err) {
        console.error("Error handling WebSocket message", err);
        response = {
          action: data.action,
          requestId: data.requestId,
          error: { message: "An unexpected error occurred." },
        };
      }
    }
    console.debug("ws response", response);
    return {
      statusCode: 200,
      body: response ? JSON.stringify(response) : undefined,
    };
  } catch (err) {
    console.error(err, event);
    return {
      statusCode: 500,
    };
  }
}
