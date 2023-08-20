import {
  APIGatewayProxyResultV2,
  APIGatewayProxyWebsocketEventV2,
} from "aws-lambda";
import { IrrigationDataStore } from "./lib/store";

const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});

export async function handleWebSocketEvent(
  event: APIGatewayProxyWebsocketEventV2
): Promise<APIGatewayProxyResultV2> {
  console.info("WebSocket event", event);

  try {
    if (event.requestContext.eventType === "MESSAGE" && event.body) {
      const data = JSON.parse(event.body);
      if (data.action === "webPush/subscribe") {
        await store.addPushNotificationSubscription(data.subscription);
      } else if (data.action === "webPush/unsubscribe") {
        await store.removePushNotificationSubscription(data.subscription);
      } else {
        console.warn("Invalid WebSocket message", data);
      }
    }
  } catch (err) {
    console.error(err, event);
  }
  return {
    statusCode: 200,
  };
}
