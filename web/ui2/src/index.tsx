import React, { useCallback, useMemo } from "react";
import ReactDOM from "react-dom/client";
import { createBrowserRouter, redirect, RouterProvider, useLoaderData } from "react-router-dom";
import { ApiRequestSigner } from "./services/apiRequestSigner";
import { CognitoIdentityTokenProvider } from "./services/cognitoIdentityTokenProvider";
import "./index.css";
import { WebPush } from "./services/webPush";
import useWebSocket, { ReadyState } from "react-use-websocket";

const config = {
  region: import.meta.env.VITE_REGION,
  userPoolId: import.meta.env.VITE_USER_POOL_ID,
  identityPoolId: import.meta.env.VITE_IDENTITY_POOL_ID,
  mqttEndpoint: import.meta.env.VITE_MQTT_ENDPOINT,
  deviceId: import.meta.env.VITE_DEVICE_ID,
  cognitoClientId: import.meta.env.VITE_CLIENT_ID,
  cognitoDomain: import.meta.env.VITE_COGNITO_DOMAIN,
  apiEndpoint: import.meta.env.VITE_API_ENDPOINT,
  appUrl: import.meta.env.VITE_URL,
  vapidPublicKey: import.meta.env.VITE_VAPID_PUB_KEY,
};

const identityTokenProvider = new CognitoIdentityTokenProvider({
  clientId: config.cognitoClientId,
  loginDomain: config.cognitoDomain,
  redirectUri: `${config.appUrl}/auth`,
});
const apiRequestSigner = new ApiRequestSigner({
  region: config.region,
  identityTokenProvider,
  identityPoolId: config.identityPoolId,
  userPoolId: config.userPoolId,
  wsApiUrl: config.apiEndpoint,
});
const getWsUrl = () => apiRequestSigner.getWsApiUrl();
const webPush = new WebPush({ vapidPublicKeyString: config.vapidPublicKey });
async function webPushSubscribe(send: (msg: object) => void) {
  const subscription = await webPush.subscribe();
  if (subscription) {
    console.log("Subscribed");
    send({
      action: "webPush/subscribe",
      requestId: 1,
      subscription,
    });
  } else {
    console.log("Failed to subscribe");
  }
}
async function webPushUnsubscribe(send: (msg: object) => void) {
  const subscription = await webPush.unsubscribe();
  if (subscription) {
    console.log("Unsubscribed");
    send({
      action: "webPush/unsubscribe",
      requestId: 1,
      subscription,
    });
  }
}

const RootComponent = () => {
  const { readyState, lastJsonMessage, sendJsonMessage } = useWebSocket(getWsUrl, {
    retryOnError: true,
    shouldReconnect: () => true,
  });
  return (
    <div>
      <h2>WebSocket</h2>
      <ul>
        <li>Connection status: {ReadyState[readyState]}</li>
        <li>Last message: {JSON.stringify(lastJsonMessage)}</li>
      </ul>
      <h2>Websocket</h2>
      <button
        onClick={() => sendJsonMessage({ action: "subscribe/device", deviceIds: ["icu-test"] })}
      >
        Subscribe State
      </button>
      <button
        onClick={() =>
          sendJsonMessage({
            action: "set/property",
            deviceId: "icu-test",
            propertyId: "AgEA",
            value: 1,
          })
        }
      >
        Motor on
      </button>
      <button
        onClick={() =>
          sendJsonMessage({
            action: "set/property",
            deviceId: "icu-test",
            propertyId: "AgEA",
            value: 0,
          })
        }
      >
        Motor off
      </button>
      <h2>Web Push Notifications</h2>
      <button onClick={() => webPushSubscribe(sendJsonMessage)}>Subscribe</button>
      <button onClick={() => webPushUnsubscribe(sendJsonMessage)}>Unsubscribe</button>
      <button onClick={() => sendJsonMessage({ action: "webPush/test" })}>Test</button>
    </div>
  );
};

const router = createBrowserRouter([
  {
    path: "/",
    element: <RootComponent />,
  },
  {
    path: "/auth",
    element: <div />,
    loader: async ({ request }) => {
      const params = new URL(request.url).searchParams;
      if (!(await identityTokenProvider.handleAuthentication(params))) {
        throw new Error("Authentication failed");
      }
      return redirect("/");
    },
  },
]);

const root = ReactDOM.createRoot(document.getElementById("root") as HTMLElement);
root.render(
  <React.StrictMode>
    <RouterProvider router={router} />
  </React.StrictMode>
);
