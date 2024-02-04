import { LogEntry, LogLevel, LogSource } from "./log";
import { numberToHex } from "./util";

export enum ErrorComponent {
  Wifi,
  Mqtt,
  Config,
  Controller,
  Update,
}

export interface IrrigationError {
  component: ErrorComponent;
  code: number;
  text: string;
}

const errorTextDecoder = new TextDecoder();

export function getErrorFromData(payload: ArrayBufferLike): IrrigationError {
  const view = new DataView(payload);
  return {
    component: view.getUint8(0),
    code: view.getUint16(1, true),
    text: errorTextDecoder.decode(payload.slice(3)),
  };
}

export function getLogFromError(error: IrrigationError): LogEntry {
  return {
    time: new Date(),
    source: LogSource.ControlUnitError,
    level: LogLevel.error,
    summary: `Error [${ErrorComponent[error.component]}]`,
    detail: {
      code: "0x" + numberToHex(error.code, 2),
      text: error.text,
    },
  };
}
