import { ScheduleState } from "../store.js";
import { getPropertyId } from "./device.js";

export interface DeviceSchedule {
  deviceId: string;
  entries: Array<{
    propertyIds: string[];
    startTime: number;
    endTime: number;
  }>;
}

export interface ScheduleStatus {
  deviceId: string;
  /** Timestamp for the next scheduled change (next start/end time after now) */
  nextEventTime?: number;
  /** Timestamp for the final scheduled change (latest end time) */
  lastEventTime?: number;
  /** There is an active schedule entry (start <= now < end) */
  active: boolean;
  /** Scheduling is aborted */
  aborted: boolean;
  /** Properties waiting to change to the desired state */
  pending: Array<{
    propertyId: string;
    value: boolean;
    since?: number;
  }>;
}

export function getScheduleStatus(state: ScheduleState): ScheduleStatus {
  const now = state.lastEvaluated ?? Date.now();
  const eventTimes = state.entries
    .flatMap((e) => [e.startTime, e.endTime])
    .filter((t) => t > now)
    .sort((a, b) => a - b);
  return {
    deviceId: state.deviceId,
    nextEventTime: eventTimes.at(0),
    lastEventTime: eventTimes.at(-1),
    active: state.entries.some((e) => e.startTime <= now && now < e.endTime),
    aborted: state.abort ?? false,
    pending: (state.state ?? [])
      .filter((s) => s.set !== s.seen)
      .map((s) => ({
        propertyId: getPropertyId(s.id >> 16, s.id & 0xffff, undefined),
        value: s.set,
        since: s.setTime,
      })),
  };
}
