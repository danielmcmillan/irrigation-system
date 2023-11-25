import { DeviceStatus } from "../deviceStatus.js";
import { Alert } from "./alert.js";
import { Device } from "./device.js";
import { WebPushSubscription } from "./webPush.js";

export type RequestMessage<Action extends string = string, Params extends object = {}> = Params & {
  action: Action;
  requestId: number;
};
export type ResponseMessage<
  Action extends string = string,
  Params extends object = {}
> = RequestMessage<Action, Params> & {
  error?: {
    message: string;
  };
};
export type ServerEventMessage<
  Type extends string = string,
  Params extends object = {}
> = Params & {
  type: Type;
};

export type ServerMessage = ResponseMessage | ServerEventMessage | undefined;

// Full state
export interface DeviceList {
  devices: Device[];
}
export interface AlertList {
  alerts: Alert[];
}
export type AllState = DeviceList & AlertList;
export type AllStateRequest = RequestMessage<"state/getAll", {}>;
export type AllStateResponse = ResponseMessage<"state/getAll", AllState>;

// Incremental state changes
export interface DeviceStateChange {
  devices: Array<{
    id: string;
    connected?: boolean;
    status?: DeviceStatus;
    lastUpdated: number;
  }>;
  properties: Array<{
    id: string;
    value?: number | boolean;
    desiredValue?: number | boolean;
    lastUpdated: number;
    lastChanged?: number;
  }>;
}
export type StateChangeEvent = ServerEventMessage<"state/change", DeviceStateChange>;
// Updated details for one or more devices
export type DevicesEvent = ServerEventMessage<"state/devices", DeviceList>;
// New alerts
export type AlertsEvent = ServerEventMessage<"state/alerts", AlertList>;

// Web Push
export type WebPushSubscribeRequest = RequestMessage<"webPush/subscribe", WebPushSubscription>;
export type WebPushUnsubscribeRequest = RequestMessage<"webPush/unsubscribe", WebPushSubscription>;
