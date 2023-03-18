import React, { useEffect, useRef, useState } from "react";
import ons from "onsenui";
import {
  Button,
  Dialog,
  Page,
  List,
  ListHeader,
  ListItem,
  ProgressCircular,
  Tabbar,
  Tab,
} from "react-onsenui";
import "./App.css";
// import { mqtt, iot, auth } from "aws-iot-device-sdk-v2";
import { mqtt, iot, auth } from "aws-crt/dist.browser/browser";
import { fromCognitoIdentityPool } from "@aws-sdk/credential-provider-cognito-identity";
import { CognitoIdentityClient } from "@aws-sdk/client-cognito-identity";

interface Event {
  id: number;
  type: number;
  controllerId?: string;
  propertyId?: string;
  value?: number;
  data?: string;
}

function PropertyList(props: {}) {
  return (
    <List
      dataSource={["x"]}
      renderHeader={() => <ListHeader>Properties</ListHeader>}
      renderRow={(row, index) => <ListItem key={index}>{row}</ListItem>}
    />
  );
}

async function connectWebsocket() {
  const region = "ap-southeast-2";
  const provider = fromCognitoIdentityPool({
    client: new CognitoIdentityClient({ region }),
    identityPoolId: "",
  });
  class IotCredentialsProvider extends auth.CredentialsProvider {
    credentials: auth.AWSCredentials | undefined;
    getCredentials(): auth.AWSCredentials | undefined {
      return this.credentials;
    }
    async refresh() {
      const credentials = await provider();
      this.credentials = {
        aws_access_id: credentials.accessKeyId,
        aws_secret_key: credentials.secretAccessKey,
        aws_region: region,
        aws_sts_token: credentials.sessionToken,
      };
    }
  }
  const iotProvider = new IotCredentialsProvider();
  await iotProvider.refresh();
  return new Promise<mqtt.MqttClientConnection>((resolve, reject) => {
    let config =
      iot.AwsIotMqttConnectionConfigBuilder.new_builder_for_websocket()
        .with_clean_session(true)
        .with_client_id(`pub_sub_sample(${new Date()})`)
        .with_endpoint("")
        .with_credential_provider(iotProvider)
        .with_use_websockets()
        .with_keep_alive_seconds(30)
        .build();

    console.log("Connecting websocket...");
    const client = new mqtt.MqttClient();

    const connection = client.new_connection(config);
    connection.on("connect", (session_present) => {
      resolve(connection);
    });
    connection.on("interrupt", (error) => {
      console.log(`Connection interrupted: error=${error}`);
    });
    connection.on("resume", (return_code, session_present) => {
      console.log(
        `Resumed: rc: ${return_code} existing session: ${session_present}`
      );
    });
    connection.on("disconnect", () => {
      console.log("Disconnected");
    });
    connection.on("error", (error) => {
      reject(error);
    });
    connection.connect();
  });
}
let connection: mqtt.MqttClientConnection | undefined = undefined;
async function test() {
  if (!connection) {
    connection = await connectWebsocket();
    console.log("Got connection");
    await connection.subscribe(
      "test/sub",
      0,
      (
        topic: string,
        payload: ArrayBuffer,
        dup: boolean,
        qos: mqtt.QoS,
        retain: boolean
      ) => {
        console.log(
          `Got message on ${topic}: ${new TextDecoder().decode(payload)}`
        );
      }
    );
  }
  // const pub = await connection.publish("test/hello", "Hello there!", 0, false);
  const configData = Uint8Array.from([
    // Remote unit
    6, 0x04, 0x01, 0x05, 0x01, 0x00,
    // Solenoid
    6, 0x04, 0x02, 0x09, 0x05, 0x00,
  ]);
  const pub = await connection.publish(
    "icu-in/irrigation_test/config",
    configData,
    0,
    false
  );
  console.log("Published", pub);
  // await connection.disconnect();
}

function App() {
  const [tabIndex, setTabIndex] = useState(0);

  return (
    <Tabbar
      onPreChange={({ index }) => setTabIndex(index)}
      activeIndex={tabIndex}
      renderTabs={(activeIndex, tabbar) => [
        {
          tab: <Tab key="Properties" label="Properties" icon="md-home" />,
          content: (
            <Page key="Properties" className="tab-page">
              <PropertyList />
            </Page>
          ),
        },
        {
          tab: <Tab key="Events" label="Events" icon="md-home" />,
          content: <Page key="Events" className="tab-page"></Page>,
        },
        {
          tab: <Tab key="Settings" label="Settings" icon="md-settings" />,
          content: (
            <Page key="Settings" className="tab-page">
              <Button onClick={test}>Test</Button>
            </Page>
          ),
        },
      ]}
    />
  );
}

export default App;
