import React from "react";
import ReactDOM from "react-dom/client";
import "./index.css";
import "@aws-amplify/ui-react/styles.css";
import { configureAmplify } from "./amplify";
import App from "./components/App";
import { IrrigationStore } from "./irrigation/store";
import { config } from "./config";
// import "onsenui/css/onsenui.css";
// import "onsenui/css/onsen-css-components.css";

const clientId = configureAmplify(config);
const icu = new IrrigationStore(clientId);

const root = ReactDOM.createRoot(
  document.getElementById("root") as HTMLElement
);
root.render(
  <React.StrictMode>
    <App icu={icu} />
  </React.StrictMode>
);
