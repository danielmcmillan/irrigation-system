export enum WebPushStatus {
  Unavailable,
  NoPermission,
  NotSubscribed,
  SubscriptionExpired,
  Active,
}

export class WebPush {
  vapidPublicKey: Uint8Array;

  constructor({ vapidPublicKeyString }: { vapidPublicKeyString: string }) {
    this.vapidPublicKey = Uint8Array.from(
      atob(vapidPublicKeyString),
      (m) => m.codePointAt(0)!
    );
  }

  async getStatus() {
    if (!("serviceWorker" in navigator)) {
      console.warn("Push notifications are not available");
      return WebPushStatus.Unavailable;
    } else {
      const registration = await navigator.serviceWorker.ready;
      const permissionState = await registration.pushManager.permissionState(
        this.subscriptionOptions
      );
      console.log("permissionState", permissionState);
      if (permissionState !== "granted") {
        return WebPushStatus.NoPermission;
      }
      const subscription = await registration.pushManager.getSubscription();
      if (!subscription) {
        return WebPushStatus.NotSubscribed;
      }
      if (
        subscription.expirationTime !== null &&
        subscription.expirationTime < Date.now()
      ) {
        return WebPushStatus.SubscriptionExpired;
      }
      const subscriptionVapidKey = subscription.options.applicationServerKey
        ? new Uint8Array(subscription.options.applicationServerKey)
        : undefined;
      if (
        !subscriptionVapidKey ||
        subscriptionVapidKey.length !== this.vapidPublicKey.length ||
        subscriptionVapidKey.some((v, i) => this.vapidPublicKey[i] !== v)
      ) {
        // Subscription is for the wrong application
        await subscription.unsubscribe();
        return WebPushStatus.NotSubscribed;
      }
      return WebPushStatus.Active;
    }
  }

  /**
   * Subscribes to web push notifications.
   * Returns the subscription data for pushing notifications from the backend.
   */
  async subscribe(): Promise<object | undefined> {
    const status = await this.getStatus();
    if (status === WebPushStatus.Unavailable) {
      return undefined;
    } else {
      const registration = await navigator.serviceWorker.ready;
      if (status === WebPushStatus.Active) {
        const subscription = await registration.pushManager.getSubscription();
        return subscription?.toJSON();
      }
      console.info("Subscribing for push notifications");
      const subscription = await registration.pushManager.subscribe(
        this.subscriptionOptions
      );
      return subscription.toJSON();
    }
  }

  async unsubscribe(): Promise<object | undefined> {
    const status = await this.getStatus();
    if (status !== WebPushStatus.Unavailable) {
      const registration = await navigator.serviceWorker.ready;
      const subscription = await registration.pushManager.getSubscription();
      await subscription?.unsubscribe();
      return subscription?.toJSON();
    }
  }

  private get subscriptionOptions(): PushSubscriptionOptionsInit {
    return {
      userVisibleOnly: true,
      applicationServerKey: this.vapidPublicKey,
    };
  }
}
