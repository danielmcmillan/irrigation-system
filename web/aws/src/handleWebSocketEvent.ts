import { APIGatewayProxyResultV2, APIGatewayProxyWebsocketEventV2 } from "aws-lambda";
import { IrrigationDataStore } from "./lib/store";
import { sendPushNotification } from "./lib/pushNotifications";

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

export async function handleWebSocketEvent(
  event: APIGatewayProxyWebsocketEventV2
): Promise<APIGatewayProxyResultV2> {
  console.info("WebSocket event", {
    type: event.requestContext.eventType,
    body: event.body,
  });
  let response: ResponseMessage | undefined;

  try {
    if (event.requestContext.eventType === "MESSAGE" && event.body) {
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
