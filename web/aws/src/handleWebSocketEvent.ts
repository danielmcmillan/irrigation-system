import { APIGatewayProxyResultV2, APIGatewayProxyWebsocketEventV2 } from "aws-lambda";
import { getDevices, getPropertyId, getPropertyValue, parsePropertyId } from "./lib/api/device.js";
import {
  DeviceControllerCommandRequest,
  DeviceGetConfigRequest,
  DeviceGetConfigResponse,
  DeviceGetScheduleRequest,
  DeviceGetScheduleResponse,
  DeviceSetConfigRequest,
  DeviceSetConfigResponse,
  DeviceSetScheduleRequest,
  GetPropertyHistoryRequest,
  GetPropertyHistoryResponse,
  RequestMessage,
  ServerMessage,
  SetPropertyRequest,
  SetPropertyResponse,
  SubscribeDeviceRequest,
  SubscribeDeviceResponse,
  WebPushSubscribeRequest,
  WebPushUnsubscribeRequest,
} from "./lib/api/messages.js";
import { sendPushNotification } from "./lib/pushNotifications.js";
import { DeviceStateQueryType, IrrigationDataStore, ScheduleState } from "./lib/store.js";
import { IoTDataPlaneClient, PublishCommand } from "@aws-sdk/client-iot-data-plane";
import { getConfiguredDeviceControllerDefinitions } from "./lib/deviceControllers/configureDeviceControllers.js";

const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
// Expire client connections after the maximum connection time limit for API Gateway of 2 hours.
const WEB_SOCKET_CLIENT_TTL = 2.5 * 3600;

const iotData = new IoTDataPlaneClient({
  endpoint: process.env.IOT_ENDPOINT,
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
          await store.addPushNotificationSubscription(request.subscription);
          response = request;
        } else if (data.action === "webPush/unsubscribe") {
          const request = data as WebPushUnsubscribeRequest;
          await store.removePushNotificationSubscription(request.subscription);
          response = request;
        } else if (data.action === "webPush/test") {
          await sendPushNotification(
            {
              title: "Test notification",
              message: "This message is being sent to confirm that notifications are working",
            },
            store
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
          // TOOD - consistency issue for updates happening at subscription time?
          const { devices, properties } = await store.getDeviceState();
          const subscribeResponse: SubscribeDeviceResponse = {
            action: request.action,
            requestId: request.requestId,
            devices: getDevices(
              devices.filter((device) => request.deviceIds.includes(device.deviceId)),
              properties
            ),
          };
          response = subscribeResponse;
        } else if (data.action === "property/set") {
          const request = data as SetPropertyRequest;
          const { controllerId, propertyId } = parsePropertyId(request.propertyId);
          const payload = new DataView(new ArrayBuffer(4));
          payload.setUint8(0, controllerId);
          payload.setUint16(1, propertyId, true);
          // TODO handle value conversion (currently all mutable values are only 1 or 0)
          payload.setUint8(3, request.value);
          await iotData.send(
            new PublishCommand({
              topic: `icu-in/${request.deviceId}/setProperty`,
              payload: payload.buffer,
              qos: 1,
            })
          );
          const setPropertyResponse: SetPropertyResponse = {
            action: request.action,
            requestId: request.requestId,
          };
          response = setPropertyResponse;
        } else if (data.action === "propertyHistory/get") {
          const request = data as GetPropertyHistoryRequest;
          const { controllerId, propertyId, bitIndex } = parsePropertyId(request.propertyId);
          // Query values for the last 7 days
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
              propertyIds: entry.properties.map(([controllerId, propertyId]) =>
                getPropertyId(controllerId, propertyId, undefined)
              ),
              startTime: entry.startTime,
              endTime: entry.endTime,
              abortOnFailure: entry.abortOnFailure,
            })),
          };
          response = getScheduleResponse;
        } else if (data.action === "device/setSchedule") {
          const request = data as DeviceSetScheduleRequest;
          const schedule: ScheduleState = {
            deviceId: request.deviceId,
            entries: request.entries.map((entry) => ({
              properties: entry.propertyIds.map((propertyId) => {
                const parsed = parsePropertyId(propertyId);
                return [parsed.controllerId, parsed.propertyId];
              }),
              startTime: entry.startTime,
              endTime: entry.endTime,
              abortOnFailure: entry.abortOnFailure,
            })),
          };
          await store.setSchedule(schedule);
          response = { action: request.action, requestId: request.requestId };
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
