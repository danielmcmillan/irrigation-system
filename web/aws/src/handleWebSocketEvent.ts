import {
  APIGatewayProxyResultV2,
  APIGatewayProxyWebsocketEventV2,
} from "aws-lambda";

export async function handleWebSocketEvent(
  event: APIGatewayProxyWebsocketEventV2
): Promise<APIGatewayProxyResultV2> {
  console.info("WebSocket event", event);
  return {
    statusCode: 200,
  };
}
