export interface ScheduleEntry {
  propertyIds: string[];
  startTime: number;
  endTime: number;
}

export interface ScheduleStatus {
  nextEventTime?: number;
  lastEventTime?: number;
  active: boolean;
  aborted: boolean;
  pending: Array<{
    propertyId: string;
    value: boolean;
    since?: number;
  }>;
}
