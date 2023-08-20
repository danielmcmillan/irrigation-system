import { clientsClaim } from "workbox-core";
import { cleanupOutdatedCaches, precacheAndRoute } from "workbox-precaching";

declare let self: ServiceWorkerGlobalScope;

// For auto update
self.skipWaiting();
clientsClaim();

cleanupOutdatedCaches();
precacheAndRoute(self.__WB_MANIFEST);

// Register event listener for the ‘push’ event.
self.addEventListener("push", function (event) {
  // Keep the service worker alive until the notification is created.
  event.waitUntil(
    (async () => {
      console.log("Got push event", event);
      const payload = event.data?.json();
      if (payload && payload.title && payload.message) {
      self.registration.showNotification("Hello ServiceWorker!", {
          body: payload.message,
          silent: payload.silent,
        });
      } else {
        self.registration.showNotification("Something went wrong", {
          body: "A notification was received but could not be understood",
      });
      }
    })()
  );
});
