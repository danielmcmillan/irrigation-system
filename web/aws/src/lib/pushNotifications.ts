import webpush from "web-push";
import { IrrigationDataStore } from "./store";
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
 * Send a push notification to all subscriptions.
 * Removes subscriptions that no longer exist.
 */
export async function sendPushNotification(
  notification: PushNotification,
  store: IrrigationDataStore
) {
  const payload = JSON.stringify(notification);
  const subscriptions = await store.listPushNotificationSubscriptions();
  await Promise.all(
    subscriptions.map(async (subscription) => {
      const result = await webpush.sendNotification(subscription, payload);
      if ([404, 410].includes(result.statusCode)) {
        console.warn(
          "Dropping web notification subscription since it no longer exists",
          result
        );
        await store.removePushNotificationSubscription(subscription);
      } else if (result.statusCode > 299) {
        console.error("Failed to push web notification", result);
      }
    })
  );
}
