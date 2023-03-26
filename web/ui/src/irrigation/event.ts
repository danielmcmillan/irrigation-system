import { binToHex, numberToHex } from "./util";
import { LogEntry, LogLevel, LogSource } from "./log";

export enum IrrigationEventType {
  GeneralInfo = 0x40,
  Started = 0x41,
  Configured = 0x42,
  PropertyValueChanged = 0x48,
  PropertyDesiredValueChanged = 0x49,
  GeneralWarning = 0x80,
  GeneralError = 0xc0,
  ControllerError = 0xc1,
  PropertyError = 0xc2,
}

interface IrrigationEventCommon {
  id: number;
  type: IrrigationEventType;
}
export interface IrrigationEventGeneral extends IrrigationEventCommon {
  type:
    | IrrigationEventType.GeneralInfo
    | IrrigationEventType.GeneralWarning
    | IrrigationEventType.GeneralError;
  data: Uint8Array;
}
export interface IrrigationEventStarted extends IrrigationEventCommon {
  type: IrrigationEventType.Started;
}
export interface IrrigationEventConfigured extends IrrigationEventCommon {
  type: IrrigationEventType.Configured;
}
export interface IrrigationEventPropertyValueChanged
  extends IrrigationEventCommon {
  type: IrrigationEventType.PropertyValueChanged;
  controllerId: number;
  propertyId: number;
  value: ArrayBuffer;
}
export interface IrrigationEventPropertyDesiredValueChanged
  extends IrrigationEventCommon {
  type: IrrigationEventType.PropertyDesiredValueChanged;
  controllerId: number;
  propertyId: number;
  value: ArrayBuffer;
}
export interface IrrigationEventControllerError extends IrrigationEventCommon {
  type: IrrigationEventType.ControllerError;
  controllerId: number;
  data: Uint8Array;
}
export interface IrrigationEventPropertyError extends IrrigationEventCommon {
  type: IrrigationEventType.PropertyError;
  controllerId: number;
  propertyId: number;
  data: Uint8Array;
}
export type IrrigationEvent =
  | IrrigationEventGeneral
  | IrrigationEventStarted
  | IrrigationEventConfigured
  | IrrigationEventPropertyValueChanged
  | IrrigationEventPropertyDesiredValueChanged
  | IrrigationEventControllerError
  | IrrigationEventPropertyError;

function getEventFromData(payload: ArrayBufferLike): IrrigationEvent {
  const view = new DataView(payload);
  const id = view.getUint16(0, true);
  const type: IrrigationEventType = view.getUint8(2);
  const payloadStart = 4;
  const payloadEnd = 4 + view.getUint8(3);
  switch (type) {
    case IrrigationEventType.Started:
    case IrrigationEventType.Configured:
      return { id, type };
    case IrrigationEventType.PropertyValueChanged:
    case IrrigationEventType.PropertyDesiredValueChanged:
      return {
        id,
        type,
        controllerId: view.getUint8(payloadStart),
        propertyId: view.getUint16(payloadStart + 1, true),
        value: payload.slice(payloadStart + 3),
      };
    case IrrigationEventType.ControllerError:
      return {
        id,
        type,
        controllerId: view.getUint8(payloadStart),
        data: new Uint8Array(payload.slice(payloadStart + 1)),
      };
    case IrrigationEventType.PropertyError:
      return {
        id,
        type,
        controllerId: view.getUint8(payloadStart),
        propertyId: view.getUint16(payloadStart + 1, true),
        data: new Uint8Array(payload.slice(payloadStart + 3)),
      };
    case IrrigationEventType.GeneralInfo:
    case IrrigationEventType.GeneralWarning:
    case IrrigationEventType.GeneralError:
    default:
      return {
        id,
        type,
        data: new Uint8Array(payload.slice(payloadStart, payloadEnd)),
      };
  }
}

export function getEventsFromData(payload: ArrayBufferLike): IrrigationEvent[] {
  const view = new DataView(payload);
  const events: IrrigationEvent[] = [];
  for (let i = 0; i < payload.byteLength; ) {
    const size = 4 + view.getUint8(i + 3);
    events.push(getEventFromData(payload.slice(i, i + size)));
    i += size;
  }
  return events;
}

function getEventTypeString(eventType: IrrigationEventType): string {
  return (
    IrrigationEventType[eventType] ?? `Unknown (0x${numberToHex(eventType, 1)})`
  );
}

export function getEventDetail(event: IrrigationEvent): Record<string, string> {
  switch (event.type) {
    case IrrigationEventType.Started:
    case IrrigationEventType.Configured:
      return {};
    case IrrigationEventType.PropertyValueChanged:
    case IrrigationEventType.PropertyDesiredValueChanged:
      return {
        controller: event.controllerId.toString(),
        property: "0x" + numberToHex(event.propertyId, 2),
        value: "0x" + binToHex(event.value),
      };
    case IrrigationEventType.ControllerError:
      return {
        controller: event.controllerId.toString(),
        data: "0x" + binToHex(event.data),
      };
    case IrrigationEventType.PropertyError:
      return {
        controller: event.controllerId.toString(),
        property: "0x" + numberToHex(event.propertyId, 2),
        data: "0x" + binToHex(event.data),
      };
    case IrrigationEventType.GeneralInfo:
    case IrrigationEventType.GeneralWarning:
    case IrrigationEventType.GeneralError:
    default:
      return {
        data: "0x" + binToHex(event.data),
      };
  }
}

export function getLogFromEvent(event: IrrigationEvent): LogEntry {
  const levels: Record<IrrigationEventType, LogLevel> = {
    [IrrigationEventType.GeneralInfo]: LogLevel.info,
    [IrrigationEventType.Started]: LogLevel.info,
    [IrrigationEventType.Configured]: LogLevel.info,
    [IrrigationEventType.PropertyValueChanged]: LogLevel.info,
    [IrrigationEventType.PropertyDesiredValueChanged]: LogLevel.info,
    [IrrigationEventType.GeneralWarning]: LogLevel.warn,
    [IrrigationEventType.GeneralError]: LogLevel.error,
    [IrrigationEventType.ControllerError]: LogLevel.error,
    [IrrigationEventType.PropertyError]: LogLevel.error,
  };
  return {
    time: new Date(),
    source: LogSource.ControlUnitEvent,
    level: levels[event.type],
    summary: `Event ${event.id
      .toString()
      .padStart(5, "0")} ${getEventTypeString(event.type)}`,
    detail: getEventDetail(event),
  };
}
