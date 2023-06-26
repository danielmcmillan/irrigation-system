export enum DeviceEventType {
  GeneralInfo = 0x40,
  Started = 0x41,
  Configured = 0x42,
  Ready = 0x43,
  PropertyValueChanged = 0x48,
  PropertyDesiredValueChanged = 0x49,
  GeneralWarning = 0x80,
  GeneralError = 0xc0,
  ControllerError = 0xc1,
  PropertyError = 0xc2,
}

interface DeviceEventCommon {
  id: number;
  type: DeviceEventType;
}
export interface DeviceEventGeneral extends DeviceEventCommon {
  type:
    | DeviceEventType.GeneralInfo
    | DeviceEventType.GeneralWarning
    | DeviceEventType.GeneralError;
  data: ArrayBuffer;
}
export interface DeviceEventNotice extends DeviceEventCommon {
  type:
    | DeviceEventType.Started
    | DeviceEventType.Configured
    | DeviceEventType.Ready;
}
export interface DeviceEventPropertyValueChanged extends DeviceEventCommon {
  type: DeviceEventType.PropertyValueChanged;
  controllerId: number;
  propertyId: number;
  value: ArrayBuffer;
}
export interface DeviceEventPropertyDesiredValueChanged
  extends DeviceEventCommon {
  type: DeviceEventType.PropertyDesiredValueChanged;
  controllerId: number;
  propertyId: number;
  value: ArrayBuffer;
}
export interface DeviceEventControllerError extends DeviceEventCommon {
  type: DeviceEventType.ControllerError;
  controllerId: number;
  data: ArrayBuffer;
}
export interface DeviceEventPropertyError extends DeviceEventCommon {
  type: DeviceEventType.PropertyError;
  controllerId: number;
  propertyId: number;
  data: ArrayBuffer;
}
export type DeviceEvent =
  | DeviceEventGeneral
  | DeviceEventNotice
  | DeviceEventPropertyValueChanged
  | DeviceEventPropertyDesiredValueChanged
  | DeviceEventControllerError
  | DeviceEventPropertyError;

function getEventFromData(payload: ArrayBufferLike): DeviceEvent {
  const view = new DataView(payload);
  const id = view.getUint16(0, true);
  const type: DeviceEventType = view.getUint8(2);
  const payloadStart = 4;
  const payloadEnd = 4 + view.getUint8(3);
  switch (type) {
    case DeviceEventType.Started:
    case DeviceEventType.Configured:
    case DeviceEventType.Ready:
      return { id, type };
    case DeviceEventType.PropertyValueChanged:
    case DeviceEventType.PropertyDesiredValueChanged:
      return {
        id,
        type,
        controllerId: view.getUint8(payloadStart),
        propertyId: view.getUint16(payloadStart + 1, true),
        value: payload.slice(payloadStart + 3),
      };
    case DeviceEventType.ControllerError:
      return {
        id,
        type,
        controllerId: view.getUint8(payloadStart),
        data: payload.slice(payloadStart + 1),
      };
    case DeviceEventType.PropertyError:
      return {
        id,
        type,
        controllerId: view.getUint8(payloadStart),
        propertyId: view.getUint16(payloadStart + 1, true),
        data: payload.slice(payloadStart + 3),
      };
    case DeviceEventType.GeneralInfo:
    case DeviceEventType.GeneralWarning:
    case DeviceEventType.GeneralError:
    default:
      return {
        id,
        type,
        data: payload.slice(payloadStart, payloadEnd),
      };
  }
}

export function getEventsFromData(payload: ArrayBuffer): DeviceEvent[] {
  const view = new DataView(payload);
  const events: DeviceEvent[] = [];
  for (let i = 0; i < payload.byteLength; ) {
    const size = 4 + view.getUint8(i + 3);
    events.push(getEventFromData(payload.slice(i, i + size)));
    i += size;
  }
  return events;
}
