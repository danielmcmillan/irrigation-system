import { APIGatewayProxyResultV2, APIGatewayProxyWebsocketEventV2 } from "aws-lambda";
import { getDevices } from "./lib/api/device.js";
import {
  AllStateRequest,
  AllStateResponse,
  RequestMessage,
  ServerMessage,
  WebPushSubscribeRequest,
  WebPushUnsubscribeRequest,
} from "./lib/api/messages.js";
import { sendPushNotification } from "./lib/pushNotifications.js";
import { IrrigationDataStore } from "./lib/store.js";
import { WebSocketClient } from "./lib/webSocketClient.js";

const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
// Expire client connections after the maximum connection time limit for API Gateway of 2 hours.
const WEB_SOCKET_CLIENT_TTL = 2.5 * 3600;

export async function handleWebSocketEvent(
  event: APIGatewayProxyWebsocketEventV2
): Promise<APIGatewayProxyResultV2> {
  console.debug("WebSocket event", {
    type: event.requestContext.eventType,
    body: event.body,
  });
  const webSocketClient: WebSocketClient = {
    connectionId: event.requestContext.connectionId,
  };
  let response: ServerMessage | undefined;

  try {
    if (event.requestContext.eventType === "CONNECT") {
      await store.addWebSocketClient(webSocketClient, WEB_SOCKET_CLIENT_TTL);
    } else if (event.requestContext.eventType === "DISCONNECT") {
      await store.removeWebSocketClient(webSocketClient);
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
        } else if (data.action === "state/getAll") {
          const request = data as AllStateRequest;
          const { devices, properties } = await store.getDeviceState();
          const allStateResponse: AllStateResponse = {
            ...request,
            devices: getDevices(devices, properties),
            alerts: [],
          };
          response = allStateResponse;
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
