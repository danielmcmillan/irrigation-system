import React from "react";
import ReactDOM from "react-dom/client";
import {
  createBrowserRouter,
  redirect,
  RouterProvider,
  useLoaderData,
} from "react-router-dom";
import { ApiRequestSigner } from "./aws/apiRequestSigner";
import { CognitoIdentityTokenProvider } from "./aws/cognitoIdentityTokenProvider";
import "./index.css";

const config = {
  // @ts-ignore
  region: import.meta.env.VITE_REGION,
  // @ts-ignore
  userPoolId: import.meta.env.VITE_USER_POOL_ID,
  // @ts-ignore
  identityPoolId: import.meta.env.VITE_IDENTITY_POOL_ID,
  // @ts-ignore
  mqttEndpoint: import.meta.env.VITE_MQTT_ENDPOINT,
  // @ts-ignore
  deviceId: import.meta.env.VITE_DEVICE_ID,
  // @ts-ignore
  cognitoClientId: import.meta.env.VITE_CLIENT_ID,
  // @ts-ignore
  cognitoDomain: import.meta.env.VITE_COGNITO_DOMAIN,
  // @ts-ignore
  apiEndpoint: import.meta.env.VITE_API_ENDPOINT,
  // @ts-ignore
  authRedirectUri: `http://${import.meta.env.VITE_HOSTNAME}/auth`,
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

const RootComponent = () => {
  const data = useLoaderData() as string;
  return <div>Hello world! id token: {data}</div>;
};

const router = createBrowserRouter([
  {
    path: "/",
    element: <RootComponent />,
    loader: async () => {
      // return (await identityTokenProvider.getIdToken()) ?? null;
      const apiUrl = await apiRequestSigner.getWsApiUrl();
      const ws = new WebSocket(apiUrl);
      ws.addEventListener("open", (e) => {
        console.log(e);
        ws.send("Hello world!");
        ws.close();
      });
      ws.addEventListener("error", (ev) => console.error("ws error", ev));
      ws.addEventListener("close", (ev) => console.info("ws close", ev));
      ws.addEventListener("message", (ev) => console.info("ws message", ev));
      return apiUrl.toString();
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
