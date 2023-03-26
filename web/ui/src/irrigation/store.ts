import {
  action,
  computed,
  makeObservable,
  observable,
  runInAction,
} from "mobx";
import { PubSub } from "@aws-amplify/pubsub";
import { ZenObservable } from "zen-observable-ts/lib/types";
import { CONNECTION_STATE_CHANGE, ConnectionState } from "@aws-amplify/pubsub";
import { Hub } from "aws-amplify";
import {
  getEventsFromData,
  getLogFromEvent,
  IrrigationEventType,
} from "./event";
import { LogEntry, LogLevel } from "./log";
import { getErrorFromData, getLogFromError } from "./error";
import {
  getPropertiesFromData,
  getPropertyValue,
  IrrigationProperty,
  IrrigationPropertyValue,
} from "./property";

export enum MqttMessageType {
  Event = "event",
  Config = "config",
  Properties = "properties",
  Error = "error",
}

const maxLogEntries = 100;

export class IrrigationStore {
  private subscription: ZenObservable.Subscription | undefined;
  private static topicRegex =
    /^icu-out\/(?<clients>[^/]+)\/(?<deviceId>[^/]+)\/(?<messageType>.+)$/;

  connected: boolean = false;
  log: Array<LogEntry & { id: number }>;
  logId: number = 0;
  properties: IrrigationProperty[] = [];

  constructor(public readonly clientId: string) {
    this.log = [];
    makeObservable<
      this,
      "addLogEntries" | "updateProperties" | "updatePropertyValue"
    >(this, {
      connected: observable,
      log: observable,
      properties: observable,
      errorLogCount: computed,
      groupedProperties: computed,
      clearLog: action,
      addLogEntries: action,
      updateProperties: action,
      updatePropertyValue: action,
    });
    console.log("store", this);

    Hub.listen("pubsub", (data: any) => {
      const { payload } = data;
      if (payload.event === CONNECTION_STATE_CHANGE) {
        const connectionState = payload.data.connectionState as ConnectionState;
        console.log(`MQTT connection state updated: ${connectionState}`);
        runInAction(() => {
          this.connected = connectionState === ConnectionState.Connected;
        });
        if (connectionState === ConnectionState.Connected) {
          console.log(
            "Connection established, requesting current property state"
          );
          this.publish(
            "icu-in/irrigation_test/getProperties",
            new TextEncoder().encode(this.clientId)
          );
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
      "icu-out/all/#",
      `icu-out/${this.clientId}/#`,
    ]).subscribe({
      next: (msg: any) =>
        this.handleMessage(msg.value.topicSymbol, msg.value.bytes),
      error: (error) => console.error("MQTT subscription error", error),
      complete: () => console.log("MQTT subscription ended"),
    });
  }

  stop() {
    this.subscription?.unsubscribe();
    this.subscription = undefined;
  }

  clearLog() {
    this.log.splice(0, this.log.length);
  }

  get errorLogCount(): number {
    return this.log.reduce(
      (prev, cur) => (cur.level === LogLevel.error ? prev + 1 : prev),
      0
    );
  }

  get groupedProperties(): [string, IrrigationProperty[]][] {
    const properties: Record<string, IrrigationProperty[]> = {};
    for (const property of this.properties) {
      const group = property.objectName.split("|")[0];
      properties[group] ??= [];
      properties[group].push(property);
    }
    return Object.entries(properties);
  }

  get groupedWriteableProperties(): [string, IrrigationProperty[]][] {
    const properties: Record<string, IrrigationProperty[]> = {};
    for (const property of this.properties) {
      if (!property.isReadOnly) {
        const group = property.objectName.split("|")[0];
        properties[group] ??= [];
        properties[group].push(property);
      }
    }
    return Object.entries(properties);
  }

  requestPropertyValueUpdate(
    controllerId: number,
    propertyId: number,
    value: boolean
  ) {
    const payload = new ArrayBuffer(4);
    const view = new DataView(payload);
    view.setUint8(0, controllerId);
    view.setUint16(1, propertyId, true);
    view.setUint8(3, value ? 1 : 0);
    this.publish("icu-in/irrigation_test/setProperty", payload);
    // TODO notify if it fails
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

  private getProperty(
    controllerId: number,
    propertyId: number
  ): IrrigationProperty | undefined {
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
      console.warn(
        `Received event for unknown property ${controllerId}:${propertyId}`
      );
    }
  }

  private async handleMessage(
    topic: string,
    payload: Uint8Array
  ): Promise<unknown> {
    const buffer = payload.buffer.slice(
      payload.byteOffset,
      payload.byteOffset + payload.byteLength
    );
    const match = topic.match(IrrigationStore.topicRegex);
    if (match?.groups) {
      const { clients, deviceId, messageType } = match.groups;
      console.log(
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
          }
        }
      } else if (messageType === MqttMessageType.Error) {
        const error = getErrorFromData(buffer);
        this.addLogEntries([getLogFromError(error)]);
        console.log(getLogFromError(error), this.log);
      } else if (messageType === MqttMessageType.Properties) {
        const properties = getPropertiesFromData(buffer);
        this.updateProperties(properties);
      } else {
        console.log("MQTT message payload", payload);
      }
    } else {
      console.error(
        `Received MQTT message on unexpected topic ${topic}`,
        payload
      );
    }
    return;
  }
}
