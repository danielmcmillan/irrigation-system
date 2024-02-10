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
export type SubscribeDeviceRequest = RequestMessage<"device/subscribe", { deviceIds: string[] }>;
export type SubscribeDeviceResponse = ResponseMessage<"device/subscribe", InitialDeviceState>;

// Property requests
export type SetPropertyRequest = RequestMessage<
  "property/set",
  { deviceId: string; propertyId: string; value: number }
>;
export type SetPropertyResponse = ResponseMessage<"property/set">;
export type GetPropertyHistoryRequest = RequestMessage<
  "propertyHistory/get",
  { deviceId: string; propertyId: string }
>;
export type GetPropertyHistoryResponse = ResponseMessage<
  "propertyHistory/get",
  {
    values: Array<{
      time: number;
      value: number | undefined;
    }>;
  }
>;

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
export type DeviceUpdateEvent = ServerEventMessage<"device/update", DeviceUpdate>;

// Config
export type DeviceGetConfigRequest = RequestMessage<"device/getConfig", { deviceId: string }>;
export type DeviceGetConfigResponse = ResponseMessage<
  "device/getConfig",
  { config: string | undefined }
>;
export type DeviceSetConfigRequest = RequestMessage<
  "device/setConfig",
  { deviceId: string; config: string }
>;
export type DeviceSetConfigResponse = ResponseMessage<"device/setConfig">;

// Controller commands
export type DeviceControllerCommandRequest = RequestMessage<
  "device/controllerCommand",
  {
    deviceId: string;
    commandId: number;
    controllerId: number;
    data: string;
  }
>;
export type DeviceControllerCommandResultEvent = ServerEventMessage<
  "device/controllerCommandResult",
  {
    commandId: number;
    responseCode: number;
    data?: string;
  }
>;

// Web Push
export type WebPushSubscribeRequest = RequestMessage<"webPush/subscribe", WebPushSubscription>;
export type WebPushUnsubscribeRequest = RequestMessage<"webPush/unsubscribe", WebPushSubscription>;
