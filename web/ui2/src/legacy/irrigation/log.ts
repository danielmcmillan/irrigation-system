export enum LogLevel {
  info,
  warn,
  error,
  critical,
}

export enum LogSource {
  ControlUnitEvent,
  ControlUnitError,
  LocalClient,
}

export interface LogEntry {
  time: Date;
  level: LogLevel;
  summary: string;
  detail: Record<string, unknown>;
}

export const logLevels = [LogLevel.info, LogLevel.warn, LogLevel.error, LogLevel.critical];

export function getLogLevelString(level: LogLevel) {
  return {
    [LogLevel.info]: "Info",
    [LogLevel.warn]: "Warning",
    [LogLevel.error]: "Error",
    [LogLevel.critical]: "Critical",
  }[level];
}
