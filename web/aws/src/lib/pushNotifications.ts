import webpush, { WebPushError } from "web-push";
import { IrrigationDataStore } from "./store.js";
webpush.setVapidDetails(
  process.env.VAPID_SUBJECT!,
  process.env.VAPID_PUBLIC_KEY!,
  process.env.VAPID_PRIVATE_KEY!
);

export interface PushNotification {
  title: string;
  message: string;
  silent?: boolean;
}

/**
 * Send a push notification to all subscriptions, or subscription for the specified endpoint or deviceId.
 * Removes subscriptions that no longer exist.
 */
export async function sendPushNotification(
  notification: PushNotification,
  store: IrrigationDataStore,
  filter?: { deviceId?: string; endpoint?: string }
) {
  const payload = JSON.stringify(notification);
  const subscriptions = await store.listPushNotificationSubscriptions(filter?.endpoint);
  await Promise.all(
    subscriptions
      .filter(
        (subscription) =>
          filter?.deviceId === undefined || subscription.deviceId === filter?.deviceId
      )
      .map(async (subscription) => {
        try {
          await webpush.sendNotification(subscription, payload);
        } catch (err) {
          if (err instanceof WebPushError) {
            if ([404, 410].includes(err.statusCode)) {
              console.warn("Dropping web notification subscription since it no longer exists", err);
              await store.removePushNotificationSubscription(subscription);
            } else {
              console.error("Failed to push web notification", err);
            }
          } else {
            throw err;
          }
        }
      })
  );
}
