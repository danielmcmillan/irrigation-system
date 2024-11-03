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
    this.vapidPublicKey = Uint8Array.from(atob(vapidPublicKeyString), (m) => m.codePointAt(0)!);
  }

  async getSubscription(): Promise<{ status: WebPushStatus; subscription?: PushSubscription }> {
    if (!("serviceWorker" in navigator)) {
      console.warn("Push notifications are not available");
      return { status: WebPushStatus.Unavailable };
    } else {
      const registration = await navigator.serviceWorker.getRegistration();
      if (!registration) {
        return { status: WebPushStatus.Unavailable };
      }
      const permissionState = await registration.pushManager.permissionState(
        this.subscriptionOptions
      );
      if (permissionState !== "granted") {
        return { status: WebPushStatus.NoPermission };
      }
      const subscription = await registration.pushManager.getSubscription();
      if (!subscription) {
        return { status: WebPushStatus.NotSubscribed };
      }
      if (subscription.expirationTime !== null && subscription.expirationTime < Date.now()) {
        return { status: WebPushStatus.SubscriptionExpired, subscription };
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
        return { status: WebPushStatus.NotSubscribed, subscription };
      }
      return { status: WebPushStatus.Active, subscription };
    }
  }

  /**
   * Subscribes to web push notifications.
   * Returns the subscription data for pushing notifications from the backend.
   */
  async subscribe(): Promise<object | undefined> {
    const { status, subscription } = await this.getSubscription();
    if (status === WebPushStatus.Unavailable) {
      return undefined;
    } else {
      const registration = await navigator.serviceWorker.ready;
      if (status === WebPushStatus.Active) {
        return subscription?.toJSON();
      }
      console.info("Subscribing for push notifications");
      const newSubscription = await registration.pushManager.subscribe(this.subscriptionOptions);
      return newSubscription.toJSON();
    }
  }

  async unsubscribe(): Promise<object | undefined> {
    const { status, subscription } = await this.getSubscription();
    if (status !== WebPushStatus.Unavailable) {
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
