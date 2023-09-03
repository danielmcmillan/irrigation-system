import { APIGatewayProxyResultV2, APIGatewayProxyWebsocketEventV2 } from "aws-lambda";
import { IrrigationDataStore } from "./lib/store";
import { sendPushNotification } from "./lib/pushNotifications";
import { WebSocketClient } from "./lib/webSocketClient";

interface ResponseMessage {
  action: string;
  requestId: number;
  error?: {
    message: string;
  };
}

const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
// Expire client connections after the maximum connection time limit for API Gateway of 2 hours.
const WEB_SOCKET_CLIENT_TTL = 2.5 * 3600;

export async function handleWebSocketEvent(
  event: APIGatewayProxyWebsocketEventV2
): Promise<APIGatewayProxyResultV2> {
  console.info("WebSocket event", {
    type: event.requestContext.eventType,
    body: event.body,
  });
  const webSocketClient: WebSocketClient = {
    connectionId: event.requestContext.connectionId,
  };
  let response: ResponseMessage | undefined;

  try {
    if (event.requestContext.eventType === "CONNECT") {
      await store.addWebSocketClient(webSocketClient, WEB_SOCKET_CLIENT_TTL);
    } else if (event.requestContext.eventType === "DISCONNECT") {
      await store.removeWebSocketClient(webSocketClient);
    } else if (event.requestContext.eventType === "MESSAGE" && event.body) {
      const data = JSON.parse(event.body);
      response = {
        action: data.action,
        requestId: data.requestId,
        error: undefined,
      };
      if (data.action === "webPush/subscribe") {
        await store.addPushNotificationSubscription(data.subscription);
      } else if (data.action === "webPush/unsubscribe") {
        await store.removePushNotificationSubscription(data.subscription);
      } else if (data.action === "webPush/test") {
        await sendPushNotification(
          {
            title: "Test notification",
            message: "This message is being sent to confirm that notifications are working",
          },
          store
        );
      } else {
        response.error = { message: "Invalid WebSocket message" };
      }
    }
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
