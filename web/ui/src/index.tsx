import React from "react";
import ReactDOM from "react-dom/client";
import "./index.css";
import "@aws-amplify/ui-react/styles.css";
import { configureAmplify } from "./amplify";
import App from "./components/App";
import { IrrigationStore } from "./irrigation/store";

const config = {
  // @ts-ignore
  region: import.meta.env.VITE_REGION,
  // @ts-ignore
  identityPoolId: import.meta.env.VITE_IDENTITY_POOL_ID,
  // @ts-ignore
  mqttEndpoint: import.meta.env.VITE_MQTT_ENDPOINT,
  // @ts-ignore
  deviceId: import.meta.env.VITE_DEVICE_ID,
};

const clientId = configureAmplify(config);
const icu = new IrrigationStore(clientId, config.deviceId);

const root = ReactDOM.createRoot(
  document.getElementById("root") as HTMLElement
);
root.render(
  <React.StrictMode>
    <App icu={icu} />
  </React.StrictMode>
);
