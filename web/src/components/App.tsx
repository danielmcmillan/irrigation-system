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
import { HostName } from "./HostName";
import "./App.css";

interface Controller {
  name: string;
  properties: Record<number, Property>;
}
interface Property {
  name: string;
  readOnly: boolean;
}
const controllers: Record<number, Controller> = {
  0x02: {
    name: "vacon",
    properties: {
      0x01: {
        name: "available",
        readOnly: true,
      },
      0x02: {
        name: "motorOn",
        readOnly: false,
      },
      0x10: {
        name: "status",
        readOnly: true,
      },
      0x11: {
        name: "actualSpeed",
        readOnly: true,
      },
      0x12: {
        name: "outputFrequency",
        readOnly: true,
      },
      0x13: {
        name: "motorSpeed",
        readOnly: true,
      },
      0x14: {
        name: "motorCurrent",
        readOnly: true,
      },
      0x15: {
        name: "motorTorque",
        readOnly: true,
      },
      0x16: {
        name: "motorPower",
        readOnly: true,
      },
      0x17: {
        name: "motorVoltage",
        readOnly: true,
      },
      0x18: {
        name: "dcLinkVoltage",
        readOnly: true,
      },
      0x19: {
        name: "activeFaultCode",
        readOnly: true,
      },
      0x1a: {
        name: "feedbackPressure",
        readOnly: true,
      },
      0x1b: {
        name: "driveTemp",
        readOnly: true,
      },
      0x1c: {
        name: "motorTemp",
        readOnly: true,
      },
      0x1d: {
        name: "energyUsed",
        readOnly: true,
      },
      0x1e: {
        name: "runTime",
        readOnly: true,
      },
    },
  },
};
interface PropertyData extends Property {
  controllerId: number;
  propertyId: number;
  value?: number;
}

interface Event {
  id: number;
  type: number;
  controllerId?: string;
  propertyId?: string;
  value?: number;
  data?: string;
}

interface Data {
  loadingProperties: boolean;
  events: Event[];
  properties: PropertyData[];
}

const useData = (hostName: string): Data => {
  const [loadingProperties, setLoadingProperties] = useState(false);
  const [properties, setProperties] = useState<PropertyData[]>([]);
  const [events, setEvents] = useState<Event[]>([]);
  const prevEvent = useRef<string | undefined>();

  useEffect(() => {
    // If prevEvent is set and we can get it, then don't need to get properties
    // Set interval to refresh events

    const propertyMeta = Object.entries(controllers).flatMap(
      ([controllerId, controller]) =>
        Object.entries(controller.properties).map(
          ([propertyId, { name, readOnly }]): PropertyData => ({
            controllerId: Number(controllerId),
            propertyId: Number(propertyId),
            name,
            readOnly,
          })
        )
    );
    const newProperties: PropertyData[] = [];
    const work = async () => {
      if (!hostName) {
        return;
      }
      setLoadingProperties(true);
      for (const property of propertyMeta) {
        const url = `http://${hostName}/api/getProperty?controller=${property.controllerId.toString(
          16
        )}&id=${property.propertyId.toString(16)}`;
        const result = await fetch(url);
        if (result.ok) {
          const json = await result.json();
          property.value = json.value;
        } else {
          console.error(
            `Failed to get ${url}: ${result.status} ${await result.text()}`
          );
        }
        newProperties.push(property);
        setProperties(newProperties);
      }
      setLoadingProperties(false);
    };

    work().catch((err) => console.error(err));
  }, [hostName, setProperties, setLoadingProperties]);

  return {
    loadingProperties,
    events,
    properties,
  };
};

async function setProperty(
  hostName: string,
  controllerId: number,
  propertyId: number,
  value: number
) {
  const url = `http://${hostName}/api/setProperty?controller=${controllerId.toString(
    16
  )}&id=${propertyId.toString(16)}&value=${value.toString(16)}`;
  const result = await fetch(url, { method: "POST" });
  if (!result.ok) {
    console.error(
      `Failed to post ${url}: ${result.status} ${await result.text()}`
    );
  }
}

function PropertyRow(props: PropertyData & { onClick?: () => void }) {
  return (
    <div className="property-row">
      {props.name}: {props.value ?? "-"}
      {props.readOnly === false && <Button onClick={props.onClick}>⏻</Button>}
    </div>
  );
}

function PropertyList(props: { hostName: string; properties: PropertyData[] }) {
  return (
    <List
      dataSource={props.properties}
      renderHeader={() => <ListHeader>Properties</ListHeader>}
      renderRow={(row, index) => (
        <ListItem key={index}>
          <PropertyRow
            {...row}
            onClick={() =>
              setProperty(
                props.hostName,
                row.controllerId,
                row.propertyId,
                row.value ? 0 : 1
              )
            }
          />
        </ListItem>
      )}
    />
  );
}

function EventList(props: {}) {
  return (
    <List
      dataSource={[]}
      renderHeader={() => <ListHeader>Events</ListHeader>}
    />
  );
}

function App() {
  const [tabIndex, setTabIndex] = useState(0);
  const [hostName, setHostName] = useState(
    localStorage.getItem("hostName") ?? ""
  );
  const [hostOpen, setHostOpen] = useState(false);

  const data: Data = useData(hostName);

  return (
    <Tabbar
      onPreChange={({ index }) => setTabIndex(index)}
      activeIndex={tabIndex}
      renderTabs={(activeIndex, tabbar) => [
        {
          tab: <Tab label="Properties" icon="md-home" />,
          content: (
            <Page className="tab-page">
              {data.loadingProperties && <ProgressCircular indeterminate />}
              <PropertyList hostName={hostName} properties={data.properties} />
            </Page>
          ),
        },
        {
          tab: <Tab label="Events" icon="md-home" />,
          content: (
            <Page className="tab-page">
              <EventList />
            </Page>
          ),
        },
        {
          tab: <Tab label="Settings" icon="md-settings" />,
          content: (
            <Page className="tab-page">
              <div className="hostname-row">
                Hostname: {hostName}
                <Button modifier="quiet" onClick={() => setHostOpen(true)}>
                  Change
                </Button>
              </div>
              <Dialog
                onCancel={() => setHostOpen(false)}
                isOpen={hostOpen}
                isCancelable
              >
                <HostName
                  oldHostName={hostName}
                  onApply={(newHostName) => {
                    setHostName(newHostName);
                    localStorage.setItem("hostName", newHostName);
                    setHostOpen(false);
                  }}
                />
              </Dialog>
            </Page>
          ),
        },
      ]}
    />
  );
}

export default App;
