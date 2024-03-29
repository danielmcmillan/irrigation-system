import { action, computed, makeObservable, observable, runInAction } from "mobx";
import { PubSub } from "@aws-amplify/pubsub";
import { ZenObservable } from "zen-observable-ts/lib/types";
import { CONNECTION_STATE_CHANGE, ConnectionState } from "@aws-amplify/pubsub";
import { Hub } from "aws-amplify";
import { getEventsFromData, getLogFromEvent, IrrigationEventType } from "./event";
import { LogEntry, LogLevel } from "./log";
import { getErrorFromData, getLogFromError } from "./error";
import { getPropertiesFromData, getPropertyValue, IrrigationProperty } from "./property";
import {
  ConfigEntry,
  deserializeConfigEntriesFromBinary,
  deserializeConfigEntriesFromIni,
  serializeConfigEntriesToBinary,
  serializeConfigEntriesToIni,
} from "./config";
import { binToHex } from "./util";

export enum MqttMessageType {
  Event = "event",
  Config = "config",
  Properties = "properties",
  Error = "error",
  CommandResult = "commandResult",
}

export enum ControllerStatus {
  Ready = "Ready",
  LoadingProperties = "Loading Properties...",
  Unconfigured = "Starting...",
  Unknown = "Unknown",
}

export interface ControllerCommandResult {
  id: number;
  time: number;
  controllerId: number;
  request: ArrayBuffer;
  responseCode?: number;
  data?: ArrayBuffer;
}

const maxLogEntries = 100;

export class IrrigationStore {
  private subscription: ZenObservable.Subscription | undefined;
  private static topicRegex =
    /^icu-out\/(?<clients>[^/]+)\/(?<deviceId>[^/]+)\/(?<messageType>.+)$/;

  log: Array<LogEntry & { id: number }>;
  logId: number = 0;
  connectionState: ConnectionState = ConnectionState.Disconnected;
  properties: IrrigationProperty[] = [];
  configIni: string = "";
  configLoaded = false;
  controllerCommandResults: ControllerCommandResult[] = [];
  controllerStatus: ControllerStatus = ControllerStatus.Unknown;

  constructor(public readonly clientId: string, public readonly controlDeviceId: string) {
    this.log = [];
    makeObservable<this, "addLogEntries" | "updateProperties" | "updatePropertyValue">(this, {
      connectionState: observable,
      log: observable,
      properties: observable,
      configIni: observable,
      configLoaded: observable,
      controllerCommandResults: observable,
      controllerStatus: observable,
      ready: computed,
      errorLogCount: computed,
      clearLog: action,
      addLogEntries: action,
      updateProperties: action,
      updatePropertyValue: action,
    });

    Hub.listen("pubsub", (data: any) => {
      const { payload } = data;
      if (payload.event === CONNECTION_STATE_CHANGE) {
        runInAction(() => {
          this.connectionState = payload.data.connectionState as ConnectionState;
          if (this.connectionState !== ConnectionState.Connected) {
            this.controllerStatus = ControllerStatus.Unknown;
          }
        });
        if (this.connectionState === ConnectionState.Connected) {
          this.requestProperties();
        }
      }
    });
  }

  start() {
    if (this.subscription?.closed === false) {
      // Already have active subscription
      return;
    }
    this.subscription = PubSub.subscribe([
      `icu-out/all/${this.controlDeviceId}/#`,
      `icu-out/${this.clientId}/${this.controlDeviceId}/#`,
    ]).subscribe({
      next: (msg: any) => this.handleMessage(msg.value.topicSymbol, msg.value.bytes),
      error: (error) => console.error("MQTT subscription error", error),
      complete: () => console.log("MQTT subscription ended"),
    });
  }

  stop() {
    this.subscription?.unsubscribe();
    this.subscription = undefined;
  }

  get ready(): boolean {
    return (
      this.connectionState === ConnectionState.Connected &&
      this.controllerStatus === ControllerStatus.Ready
    );
  }

  clearLog() {
    this.log.splice(0, this.log.length);
  }

  get errorLogCount(): number {
    return this.log.reduce((prev, cur) => (cur.level === LogLevel.error ? prev + 1 : prev), 0);
  }

  requestSetProperty(controllerId: number, propertyId: number, value: boolean) {
    const payload = new ArrayBuffer(4);
    const view = new DataView(payload);
    view.setUint8(0, controllerId);
    view.setUint16(1, propertyId, true);
    view.setUint8(3, value ? 1 : 0);
    this.publish(`icu-in/${this.controlDeviceId}/setProperty`, payload);
    // TODO notify if publish fails
  }

  requestConfig() {
    runInAction(() => {
      this.configIni = "";
      this.configLoaded = false;
    });
    return this.publish(
      `icu-in/${this.controlDeviceId}/getConfig`,
      new TextEncoder().encode(this.clientId)
    );
  }

  requestSetConfig() {
    if (this.configLoaded) {
      this.publish(
        `icu-in/${this.controlDeviceId}/setConfig`,
        serializeConfigEntriesToBinary(deserializeConfigEntriesFromIni(this.configIni))
      );
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
    const payload = new Uint8Array(3 + commandData.byteLength);
    const dataView = new DataView(payload.buffer);

    dataView.setUint16(0, commandId, true);
    dataView.setUint8(2, controllerId);
    payload.set(new Uint8Array(commandData), 3);
    this.publish(`icu-in/${this.controlDeviceId}/command`, payload.buffer);
  }

  private requestProperties(): Promise<unknown> {
    const topic = `icu-in/${this.controlDeviceId}/getProperties`;
    console.log(`Requesting current property state with topic "${topic}"`);
    return this.publish(topic, new TextEncoder().encode(this.clientId));
  }

  async publish(topic: string, payload: ArrayBufferLike): Promise<unknown> {
    return PubSub.publish(topic, payload as any);
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

  private getProperty(controllerId: number, propertyId: number): IrrigationProperty | undefined {
    return this.properties.find(
      (p) => p.controllerId === controllerId && p.propertyId === propertyId
    );
  }

  private updateProperties(properties: IrrigationProperty[]) {
    for (const prop of properties) {
      const existing = this.getProperty(prop.controllerId, prop.propertyId);
      if (existing) {
        Object.assign(existing, prop);
      } else {
        this.properties.push(prop);
      }
    }
    this.controllerStatus = ControllerStatus.Ready;
  }

  private updatePropertyValue(
    controllerId: number,
    propertyId: number,
    value: ArrayBuffer,
    desired = false
  ) {
    const prop = this.getProperty(controllerId, propertyId);
    if (prop) {
      const newValue = getPropertyValue(value, prop.format);
      if (desired) {
        prop.desiredValue = newValue;
      } else {
        prop.value = newValue;
      }
    } else {
      console.warn(`Received event for unknown property ${controllerId}:${propertyId}`);
    }
  }

  private async handleMessage(topic: string, payload: Uint8Array): Promise<unknown> {
    const buffer = payload.buffer.slice(
      payload.byteOffset,
      payload.byteOffset + payload.byteLength
    );
    const match = topic.match(IrrigationStore.topicRegex);
    if (match?.groups) {
      const { clients, deviceId, messageType } = match.groups;
      console.debug(
        `MQTT message type ${messageType} for client ${clients} received from ${deviceId}`
      );
      if (messageType === MqttMessageType.Event) {
        const events = getEventsFromData(buffer);
        this.addLogEntries(events.map(getLogFromEvent));
        for (const event of events) {
          if (
            event.type === IrrigationEventType.PropertyValueChanged ||
            event.type === IrrigationEventType.PropertyDesiredValueChanged
          ) {
            this.updatePropertyValue(
              event.controllerId,
              event.propertyId,
              event.value,
              event.type === IrrigationEventType.PropertyDesiredValueChanged
            );
          } else if (event.type === IrrigationEventType.Started) {
            runInAction(() => {
              this.controllerStatus = ControllerStatus.Unconfigured;
            });
          } else if (event.type === IrrigationEventType.Configured) {
            await this.requestProperties();
            runInAction(() => {
              this.controllerStatus = ControllerStatus.LoadingProperties;
            });
          }
        }
      } else if (messageType === MqttMessageType.Error) {
        const error = getErrorFromData(buffer);
        this.addLogEntries([getLogFromError(error)]);
      } else if (messageType === MqttMessageType.Properties) {
        const properties = getPropertiesFromData(buffer);
        this.updateProperties(properties);
      } else if (messageType === MqttMessageType.Config) {
        const configIni = serializeConfigEntriesToIni(deserializeConfigEntriesFromBinary(buffer));
        runInAction(() => {
          this.configIni = configIni;
          this.configLoaded = true;
        });
      } else if (messageType === MqttMessageType.CommandResult) {
        if (buffer.byteLength < 4) {
          console.error("Received command result with invalid length", buffer);
        } else {
          const dataView = new DataView(buffer);
          const commandId = dataView.getUint16(0, true);
          const responseCode = dataView.getUint16(2, true);
          if (this.controllerCommandResults[commandId]) {
            runInAction(() => {
              this.controllerCommandResults[commandId].responseCode = responseCode;
              if (responseCode == 0) {
                this.controllerCommandResults[commandId].data = buffer.slice(4);
              }
            });
          } else {
            console.error(`Received command result for unknown command ID ${commandId}`, buffer);
          }
        }
      } else {
        console.warn(`MQTT message payload for unknown type ${messageType}`, payload);
      }
    } else {
      console.error(`Received MQTT message on unexpected topic ${topic}`, payload);
    }
    return;
  }
}
