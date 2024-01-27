import { action, computed, makeObservable, observable, runInAction } from "mobx";
import { LogEntry, LogLevel } from "./log";
import { IrrigationProperty } from "./property";
import { ReadyState } from "react-use-websocket";

export enum ControllerStatus {
  Disconnected = "Disconnected",
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
  controllerStatus: ControllerStatus = ControllerStatus.Disconnected;
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
      controllerStatus: observable,
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

  // start() {
  //   if (this.subscription?.closed === false) {
  //     // Already have active subscription
  //     return;
  //   }
  //   this.subscription = PubSub.subscribe([
  //     `icu-out/all/${this.controlDeviceId}/#`,
  //     `icu-out/${this.clientId}/${this.controlDeviceId}/#`,
  //   ]).subscribe({
  //     next: (msg: any) => this.handleMessage(msg.value.topicSymbol, msg.value.bytes),
  //     error: (error) => console.error("MQTT subscription error", error),
  //     complete: () => console.log("MQTT subscription ended"),
  //   });
  // }

  // stop() {
  //   this.subscription?.unsubscribe();
  //   this.subscription = undefined;
  // }

  get ready(): boolean {
    return this.readyState === ReadyState.OPEN && this.controllerStatus === ControllerStatus.Ready;
  }

  setReadyState(readyState: ReadyState) {
    this.readyState = readyState;
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
      action: "set/property",
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
    // TODO
    // return this.publish(
    //   `icu-in/${this.controlDeviceId}/getConfig`,
    //   new TextEncoder().encode(this.clientId)
    // );
  }

  requestSetConfig() {
    if (this.configLoaded) {
      // TODO
      // this.publish(
      //   `icu-in/${this.controlDeviceId}/setConfig`,
      //   serializeConfigEntriesToBinary(deserializeConfigEntriesFromIni(this.configIni))
      // );
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
    // TODO
    // const payload = new Uint8Array(3 + commandData.byteLength);
    // const dataView = new DataView(payload.buffer);

    // dataView.setUint16(0, commandId, true);
    // dataView.setUint8(2, controllerId);
    // payload.set(new Uint8Array(commandData), 3);
    // this.publish(`icu-in/${this.controlDeviceId}/command`, payload.buffer);
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

  // private updatePropertyValue(
  //   controllerId: number,
  //   propertyId: string,
  //   value: ArrayBuffer,
  //   desired = false
  // ) {
  //   const prop = this.getProperty(controllerId, propertyId);
  //   if (prop) {
  //     const newValue = getPropertyValue(value, prop.format);
  //     if (desired) {
  //       prop.desiredValue = newValue;
  //     } else {
  //       prop.value = newValue;
  //     }
  //   } else {
  //     console.warn(`Received event for unknown property ${controllerId}:${propertyId}`);
  //   }
  // }

  public async handleJsonMessage(message: any): Promise<void> {
    let device: any;
    if (message.action === "subscribe/device") {
      device = message.devices[0];
    } else if (message.type === "update/device") {
      device = message;
    }
    if (device) {
      console.debug("Legacy: handling new device state", device);
      let newStatus: ControllerStatus | undefined;
      if (device.connected === false) {
        newStatus = ControllerStatus.Disconnected;
      } else if (device.status === 0) {
        newStatus = ControllerStatus.Unconfigured;
      } else if (device.status === 1) {
        newStatus = ControllerStatus.Initialising;
      } else if (device.status === 2) {
        newStatus = ControllerStatus.Ready;
      } else if (device.connected === true) {
        newStatus = ControllerStatus.Unconfigured;
      }
      if (newStatus !== undefined) {
        runInAction(() => {
          this.controllerStatus = newStatus!;
        });
      }
      if (device.properties) {
        this.updateProperties(device.properties);
      }
      if (device.components) {
        runInAction(() => {
          this.components = device.components;
        });
      }
    }

    //   const events = getEventsFromData(buffer);
    //   this.addLogEntries(events.map(getLogFromEvent));
    //   for (const event of events) {
    //     if (
    //       event.type === IrrigationEventType.PropertyValueChanged ||
    //       event.type === IrrigationEventType.PropertyDesiredValueChanged
    //     ) {
    //       this.updatePropertyValue(
    //         event.controllerId,
    //         event.propertyId,
    //         event.value,
    //         event.type === IrrigationEventType.PropertyDesiredValueChanged
    //       );
    //     } else if (event.type === IrrigationEventType.Started) {
    //       runInAction(() => {
    //         this.controllerStatus = ControllerStatus.Unconfigured;
    //       });
    //     } else if (event.type === IrrigationEventType.Configured) {
    //       await this.requestProperties();
    //       runInAction(() => {
    //         this.controllerStatus = ControllerStatus.LoadingProperties;
    //       });
    //     }
    //   }
    // } else if (messageType === MqttMessageType.Error) {
    //   const error = getErrorFromData(buffer);
    //   this.addLogEntries([getLogFromError(error)]);
    // } else if (messageType === MqttMessageType.Properties) {
    //   const properties = getPropertiesFromData(buffer);
    //   this.updateProperties(properties);
    // } else if (messageType === MqttMessageType.Config) {
    //   const configIni = serializeConfigEntriesToIni(deserializeConfigEntriesFromBinary(buffer));
    //   runInAction(() => {
    //     this.configIni = configIni;
    //     this.configLoaded = true;
    //   });
    // } else if (messageType === MqttMessageType.CommandResult) {
    //   if (buffer.byteLength < 4) {
    //     console.error("Received command result with invalid length", buffer);
    //   } else {
    //     const dataView = new DataView(buffer);
    //     const commandId = dataView.getUint16(0, true);
    //     const responseCode = dataView.getUint16(2, true);
    //     if (this.controllerCommandResults[commandId]) {
    //       runInAction(() => {
    //         this.controllerCommandResults[commandId].responseCode = responseCode;
    //         if (responseCode == 0) {
    //           this.controllerCommandResults[commandId].data = buffer.slice(4);
    //         }
    //       });
    //     } else {
    //       console.error(`Received command result for unknown command ID ${commandId}`, buffer);
    //     }
    //   }
    // } else {
    //   console.warn(`MQTT message payload for unknown type ${messageType}`, payload);
    // }
  }
}
