import { APIGatewayProxyResultV2, APIGatewayProxyWebsocketEventV2 } from "aws-lambda";
import { getDevices, parsePropertyId } from "./lib/api/device.js";
import {
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
import { IrrigationDataStore } from "./lib/store.js";
import { IoTDataPlaneClient, PublishCommand } from "@aws-sdk/client-iot-data-plane";

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
        } else if (data.action === "subscribe/device") {
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
        } else if (data.action === "set/property") {
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
