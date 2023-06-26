import { RawDeviceMessage, parseDeviceMessage } from "./message";
import { SQSClient, SendMessageCommand } from "@aws-sdk/client-sqs";

const sqs = new SQSClient({});

export async function handleDeviceMessage(
  event: RawDeviceMessage
): Promise<void> {
  const message = parseDeviceMessage(event);

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
