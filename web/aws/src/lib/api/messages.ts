import { DeviceStatus } from "../deviceStatus.js";
import { Alert, Device } from "./device.js";
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
export type InitialDeviceState = DeviceList;
export type SubscribeDeviceRequest = RequestMessage<"subscribe/device", { deviceIds: string[] }>;
export type SubscribeDeviceResponse = ResponseMessage<"subscribe/device", InitialDeviceState>;

// Change requests
export type SetPropertyRequest = RequestMessage<
  "set/property",
  { deviceId: string; propertyId: string; value: number }
>;
export type SetPropertyResponse = ResponseMessage<"set/property">;

// Incremental state changes
export interface DeviceUpdate {
  id: string;
  connected?: boolean;
  status?: DeviceStatus;
  lastUpdated?: number;
  properties?: Array<{
    id: string;
    lastUpdated?: number;
    lastChanged?: number;
    value?: number | boolean;
    desired?: {
      lastUpdated?: number;
      lastChanged?: number;
      value?: number | boolean;
    };
  }>;
  alerts?: Alert[];
}
export type DeviceUpdateEvent = ServerEventMessage<"update/device", DeviceUpdate>;

// Web Push
export type WebPushSubscribeRequest = RequestMessage<"webPush/subscribe", WebPushSubscription>;
export type WebPushUnsubscribeRequest = RequestMessage<"webPush/unsubscribe", WebPushSubscription>;
