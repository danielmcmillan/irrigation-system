import React from "react";
import ReactDOM from "react-dom/client";
import {
  createBrowserRouter,
  redirect,
  RouterProvider,
  useLoaderData,
} from "react-router-dom";
import { ApiRequestSigner } from "./services/apiRequestSigner";
import { CognitoIdentityTokenProvider } from "./services/cognitoIdentityTokenProvider";
import "./index.css";
import { WebPush } from "./services/webPush";

const config = {
  region: import.meta.env.VITE_REGION,
  userPoolId: import.meta.env.VITE_USER_POOL_ID,
  identityPoolId: import.meta.env.VITE_IDENTITY_POOL_ID,
  mqttEndpoint: import.meta.env.VITE_MQTT_ENDPOINT,
  deviceId: import.meta.env.VITE_DEVICE_ID,
  cognitoClientId: import.meta.env.VITE_CLIENT_ID,
  cognitoDomain: import.meta.env.VITE_COGNITO_DOMAIN,
  apiEndpoint: import.meta.env.VITE_API_ENDPOINT,
  authRedirectUri: `http://${import.meta.env.VITE_HOSTNAME}/auth`,
  vapidPublicKey: import.meta.env.VITE_VAPID_PUB_KEY,
};

const identityTokenProvider = new CognitoIdentityTokenProvider({
  clientId: config.cognitoClientId,
  loginDomain: config.cognitoDomain,
  redirectUri: config.authRedirectUri,
});
const apiRequestSigner = new ApiRequestSigner({
  region: config.region,
  identityTokenProvider,
  identityPoolId: config.identityPoolId,
  userPoolId: config.userPoolId,
  wsApiUrl: config.apiEndpoint,
});
const webPush = new WebPush({ vapidPublicKeyString: config.vapidPublicKey });
async function webPushSubscribe(ws: WebSocket) {
  const subscription = await webPush.subscribe();
  ws.send(
    JSON.stringify({
      action: "webPush/subscribe",
      subscription,
    })
  );
}
async function webPushUnsubscribe(ws: WebSocket) {
  const subscription = await webPush.unsubscribe();
  if (subscription) {
    ws.send(
      JSON.stringify({
        action: "webPush/unsubscribe",
        subscription,
      })
    );
  }
}

const RootComponent = () => {
  const ws = useLoaderData() as WebSocket;
  return (
    <div>
      Hello world!{" "}
      <button onClick={() => webPushSubscribe(ws)}>Subscribe</button>
      <button onClick={() => webPushUnsubscribe(ws)}>Unsubscribe</button>
      <button onClick={() => ws.send(JSON.stringify({ msg: "send" }))}>
        Send
      </button>
    </div>
  );
};

const router = createBrowserRouter([
  {
    path: "/",
    element: <RootComponent />,
    loader: async () => {
      const apiUrl = await apiRequestSigner.getWsApiUrl();
      const ws = new WebSocket(apiUrl);
      await new Promise<Event>((resolve, reject) => {
        ws.addEventListener("open", (e) => {
          console.log("ws open", e);
          resolve(e);
        });
        ws.addEventListener("error", (e) => {
          console.error("ws error", e);
          reject(e);
        });
        ws.addEventListener("close", (e) => console.info("ws close", e));
        ws.addEventListener("message", (e) => console.info("ws message", e));
      });
      return ws;
    },
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

const root = ReactDOM.createRoot(
  document.getElementById("root") as HTMLElement
);
root.render(
  <React.StrictMode>
    <RouterProvider router={router} />
  </React.StrictMode>
);
