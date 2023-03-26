export enum LogLevel {
  info,
  warn,
  error,
}

export enum LogSource {
  ControlUnitEvent,
  ControlUnitError,
  LocalClient,
}

export interface LogEntry {
  time: Date;
  source: LogSource;
  level: LogLevel;
  summary: string;
  detail: Record<string, string>;
}

export const logLevels = [LogLevel.info, LogLevel.warn, LogLevel.error];

export function getLogLevelString(level: LogLevel) {
  return {
    [LogLevel.info]: "Info",
    [LogLevel.warn]: "Warning",
    [LogLevel.error]: "Error",
  }[level];
}
