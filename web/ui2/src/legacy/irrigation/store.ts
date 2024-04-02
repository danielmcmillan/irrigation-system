import { action, computed, makeObservable, observable, runInAction } from "mobx";
import { LogEntry, LogLevel } from "./log";
import { IrrigationProperty } from "./property";
import { ReadyState } from "react-use-websocket";
import {
  deserializeConfigEntriesFromBinary,
  deserializeConfigEntriesFromIni,
  serializeConfigEntriesToBinary,
  serializeConfigEntriesToIni,
} from "./config";
import { arrayBufferToBase64, base64ToArrayBuffer } from "./util";

export enum ControllerStatus {
  Unconfigured = "Unconfigured",
  Initialising = "Initialising...",
  Ready = "Ready",
}

export interface ControllerCommandResult {
  id: number;
  time: number;
  controllerId: number;
  request: ArrayBuffer;
  responseCode?: number;
  data?: ArrayBuffer;
}

export interface DeviceComponentDefinition {
  type: string;
  id: string;
  relatedIds?: string[];
  typeName: string;
  name: string;
}

export interface Alert {
  time: number;
  severity: LogLevel;
  /** Id of the property if the alert is specific to a property. */
  propertyId?: string;
  message: string;
}

export interface PropertyHistoryItem {
  time: number;
  value: number | undefined;
}

const maxLogEntries = 100;

export class IrrigationStore {
  log: Array<LogEntry & { id: number }>;
  logId: number = 0;
  components: DeviceComponentDefinition[] = [];
  properties: IrrigationProperty[] = [];
  configIni: string = "";
  configLoaded = false;
  controllerCommandResults: ControllerCommandResult[] = [];
  readyState: ReadyState = ReadyState.CLOSED;
  connectEnabled: boolean = true;
  controllerConnected: boolean = false;
  controllerStatus: ControllerStatus = ControllerStatus.Unconfigured;
  propertyHistory:
    | undefined
    | {
        requestId: number;
        propertyId: string;
        items?: PropertyHistoryItem[];
      };
  private sendJsonMessage: ((message: object) => void) | undefined;

  constructor(public readonly controlDeviceId: string) {
    this.log = [];
    makeObservable<this, "addLogEntries" | "updateProperties">(this, {
      log: observable,
      components: observable,
      properties: observable,
      configIni: observable,
      configLoaded: observable,
      controllerCommandResults: observable,
      readyState: observable,
      connectEnabled: observable,
      setConnectEnabled: action,
      controllerConnected: observable,
      controllerStatus: observable,
      propertyHistory: observable,
      ready: computed,
      errorLogCount: computed,
      setReadyState: action,
      clearLog: action,
      addLogEntries: action,
      updateProperties: action,
    });
  }

  public setSendJsonMessage(sendJsonMessage: (message: object) => void) {
    this.sendJsonMessage = sendJsonMessage;
  }

  get ready(): boolean {
    return (
      this.readyState === ReadyState.OPEN &&
      this.controllerConnected &&
      this.controllerStatus === ControllerStatus.Ready
    );
  }

  setReadyState(readyState: ReadyState) {
    this.readyState = readyState;
  }

  setConnectEnabled(connectEnabled: boolean) {
    this.connectEnabled = connectEnabled;
  }

  clearLog() {
    this.log.splice(0, this.log.length);
  }

  get errorLogCount(): number {
    return this.log.reduce((prev, cur) => (cur.level === LogLevel.error ? prev + 1 : prev), 0);
  }

  requestSetProperty(propertyId: string, value: number) {
    // TODO notify if publish fails
    this.sendJsonMessage?.({
      action: "property/set",
      deviceId: this.controlDeviceId,
      propertyId,
      value,
    });
  }

  requestConfig() {
    runInAction(() => {
      this.configIni = "";
      this.configLoaded = false;
    });
    this.sendJsonMessage?.({
      action: "device/getConfig",
      deviceId: this.controlDeviceId,
    });
  }

  requestSetConfig() {
    if (this.configLoaded) {
      this.sendJsonMessage?.({
        action: "device/setConfig",
        deviceId: this.controlDeviceId,
        config: arrayBufferToBase64(
          serializeConfigEntriesToBinary(deserializeConfigEntriesFromIni(this.configIni))
        ),
      });
    }
  }

  requestControllerCommand(controllerId: number, commandData: ArrayBuffer) {
    const commandId = this.controllerCommandResults.length;
    runInAction(() => {
      this.controllerCommandResults.push({
        id: commandId,
        time: Date.now(),
        controllerId,
        request: commandData,
      });
    });
    this.sendJsonMessage?.({
      action: "device/controllerCommand",
      commandId,
      deviceId: this.controlDeviceId,
      controllerId,
      data: arrayBufferToBase64(commandData),
    });
  }

  requestPropertyHistory(propertyId: string) {
    const requestId = (Math.random() * 2 ** 31) | 0;
    runInAction(() => {
      this.propertyHistory = {
        propertyId,
        requestId,
      };
    });
    this.sendJsonMessage?.({
      action: "propertyHistory/get",
      requestId,
      deviceId: this.controlDeviceId,
      propertyId,
    });
  }

  private addLogEntries(entries: LogEntry[]) {
    for (const entry of entries) {
      this.log.push({
        id: ++this.logId,
        ...entry,
      });
    }
    if (this.log.length > maxLogEntries) {
      this.log.splice(0, this.log.length - maxLogEntries);
    }
  }

  private getProperty(id: string): IrrigationProperty | undefined {
    return this.properties.find((p) => p.id === id);
  }

  private updateProperties(properties: IrrigationProperty[]) {
    for (const prop of properties) {
      const existing = this.getProperty(prop.id);
      if (existing) {
        Object.assign(existing, prop);
      } else {
        this.properties.push(prop);
      }
    }
  }

  public async handleJsonMessage(message: any): Promise<void> {
    let device: any;
    if (message.error) {
      console.error("Received error message", message);
      return;
    }
    if (message.action === "device/subscribe") {
      device = message.devices[0];
    } else if (message.type === "device/update") {
      device = message;
    }
    if (device) {
      console.debug("Legacy: handling new device state", device);
      if (device.connected !== undefined) {
        runInAction(() => {
          this.controllerConnected = device.connected;
        });
      }
      let newStatus: ControllerStatus | undefined;
      if (device.status === 0) {
        newStatus = ControllerStatus.Unconfigured;
      } else if (device.status === 1) {
        newStatus = ControllerStatus.Initialising;
      } else if (device.status === 2) {
        newStatus = ControllerStatus.Ready;
      }
      if (newStatus !== undefined) {
        runInAction(() => {
          this.controllerStatus = newStatus!;
        });
      }
      if (device.properties) {
        this.updateProperties(device.properties);
        if (!device.components) {
          this.addLogEntries(
            device.properties.flatMap((property: IrrigationProperty) => {
              const { name, componentId } = this.getProperty(property.id) ?? property;
              const component = this.components.find((c) => c.id === componentId);
              return (
                [
                  [true, property.desired],
                  [false, property],
                ] as const
              )
                .filter(([_, values]) => values?.value !== undefined)
                .map(([desired, values]) => ({
                  time: new Date(values!.lastUpdated! * 1000),
                  level: LogLevel.info,
                  summary: `Property ${desired ? "desired " : ""}value updated: ${
                    component?.typeName
                  } ${component?.name} ${name} = ${values!.value}`,
                  detail: {
                    lastChanged:
                      values!.lastChanged && new Date(values!.lastChanged * 1000).toLocaleString(),
                  },
                }));
            })
          );
        }
      }
      if (device.components) {
        runInAction(() => {
          this.components = device.components;
        });
      }
      if (device.alerts) {
        this.addLogEntries(
          device.alerts.map((alert: Alert) => {
            const property = alert.propertyId ? this.getProperty(alert.propertyId) : undefined;
            const component = property
              ? this.components.find((c) => c.id === property.componentId)
              : undefined;
            return {
              time: new Date(alert.time * 1000),
              level: alert.severity,
              summary: alert.message,
              detail: {
                property: property?.name,
                component: component ? `${component.typeName} ${component.name}` : undefined,
              },
            };
          })
        );
      }
    }

    if (message.action === "device/getConfig" && !message.error) {
      const configIni = serializeConfigEntriesToIni(
        deserializeConfigEntriesFromBinary(base64ToArrayBuffer(message.config))
      );
      runInAction(() => {
        this.configIni = configIni;
        this.configLoaded = true;
      });
    }

    if (message.type === "device/controllerCommandResult") {
      const { commandId, responseCode, data } = message;
      if (this.controllerCommandResults[commandId]) {
        runInAction(() => {
          this.controllerCommandResults[commandId].responseCode = responseCode;
          this.controllerCommandResults[commandId].data = data
            ? base64ToArrayBuffer(data)
            : undefined;
        });
      } else {
        console.error(
          `Received controller command result for unknown command ID ${commandId}`,
          message
        );
      }
    }

    if (message.action === "propertyHistory/get") {
      if (this.propertyHistory && message.requestId === this.propertyHistory.requestId) {
        const history = this.propertyHistory;
        runInAction(() => {
          history.items = message.values;
        });
      } else {
        console.error("Received unexpected property history message", {
          message,
          state: this.propertyHistory,
        });
      }
    }
  }
}
