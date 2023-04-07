import { binToHex, numberToHex } from "./util";
import { LogEntry, LogLevel, LogSource } from "./log";

export enum IrrigationEventType {
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

interface IrrigationEventCommon {
  id: number;
  type: IrrigationEventType;
}
export interface IrrigationEventGeneral extends IrrigationEventCommon {
  type:
    | IrrigationEventType.GeneralInfo
    | IrrigationEventType.GeneralWarning
    | IrrigationEventType.GeneralError;
  data: ArrayBuffer;
}
export interface IrrigationEventNotice extends IrrigationEventCommon {
  type:
    | IrrigationEventType.Started
    | IrrigationEventType.Configured
    | IrrigationEventType.Ready;
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
  data: ArrayBuffer;
}
export interface IrrigationEventPropertyError extends IrrigationEventCommon {
  type: IrrigationEventType.PropertyError;
  controllerId: number;
  propertyId: number;
  data: ArrayBuffer;
}
export type IrrigationEvent =
  | IrrigationEventGeneral
  | IrrigationEventNotice
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
    case IrrigationEventType.Ready:
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
        data: payload.slice(payloadStart + 1),
      };
    case IrrigationEventType.PropertyError:
      return {
        id,
        type,
        controllerId: view.getUint8(payloadStart),
        propertyId: view.getUint16(payloadStart + 1, true),
        data: payload.slice(payloadStart + 3),
      };
    case IrrigationEventType.GeneralInfo:
    case IrrigationEventType.GeneralWarning:
    case IrrigationEventType.GeneralError:
    default:
      return {
        id,
        type,
        data: payload.slice(payloadStart, payloadEnd),
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

function getControllerErrorDetail(
  controllerId: number,
  errorData: ArrayBuffer
): Record<string, unknown> {
  const view = new DataView(errorData);
  if (controllerId == 2 && errorData.byteLength === 3) {
    const typeNum = view.getUint8(0);
    const codeNum = view.getUint16(1, true);
    const type =
      {
        0: "Start client",
        1: "Configure ID mappings",
        2: "Write",
        3: "Read",
      }[typeNum] ?? `Unknown (${typeNum})`;
    const code =
      {
        5: "EIO: Input/output error",
        22: "EINVAL: Invalid argument",
        109: "ENOPROTOOPT: Protocol not available",
        111: "ECONNREFUSED: Connection refused",
        116: "ETIMEDOUT: Connection timed out",
        134: "ENOTSUP: Operation not supported",
        11235: "EMBXILFUN: Illegal function",
        11236: "EMBXILADD: Illegal data address",
        11237: "EMBXILVAL: Illegal data value",
        11238: "EMBXSFAIL: Slave device or server failure",
        11239: "EMBXACK: Acknowledge",
        11240: "EMBXSBUSY: Slave device or server is busy",
        11241: "EMBXNACK: Negative acknowledge",
        11242: "EMBXMEMPAR: Memory parity error",
        11244: "EMBXGPATH: Gateway path unavailable",
        11245: "EMBXGTAR: Target device failed to respond",
        11246: "EMBBADCRC: Invalid CRC",
        11247: "EMBBADDATA: Invalid data",
        11248: "EMBBADEXC: Invalid exception code",
        11250: "EMBMDATA: Too many data",
        11251: "EMBBADSLAVE: Response not from requested slave",
      }[codeNum] ?? `Unknown (${codeNum})`;
    // 112345678-11234
    return { type, code };
  } else if (controllerId === 4) {
    const typeNum = view.getUint8(0);
    const remoteUnitId = view.getUint8(1);
    let type = {
      1: "Failed to configure RF module",
      2: "Failed to write to Serial",
      3: "Timeout waiting for response on Serial",
      4: "Remote unit response has invalid CRC",
      5: "Remote unit response includes invalid commands or data",
      6: "Remote unit response is valid but for unexpected node id",
      7: "Remote unit response is for unexpected commands",
    }[typeNum];
    const result: Record<string, unknown> = {};
    if (remoteUnitId > 0) {
      result.remoteUnitId = remoteUnitId;
    }
    if ((typeNum & 0x80) === 0x80) {
      type = "Remote unit fault";
      result.faultNum = typeNum & 0x0f;
    }
    result.type = type ?? `Unknown (${typeNum})`;
    return result;
  }
  return {
    data: "0x" + binToHex(errorData),
  };
}

export function getEventDetail(
  event: IrrigationEvent
): Record<string, unknown> {
  switch (event.type) {
    case IrrigationEventType.Started:
    case IrrigationEventType.Configured:
    case IrrigationEventType.Ready:
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
        ...getControllerErrorDetail(event.controllerId, event.data),
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
    [IrrigationEventType.Ready]: LogLevel.info,
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
    level: levels[event.type] ?? LogLevel.warn,
    summary: `Event ${event.id
      .toString()
      .padStart(5, "0")} ${getEventTypeString(event.type)}`,
    detail: getEventDetail(event),
  };
}
