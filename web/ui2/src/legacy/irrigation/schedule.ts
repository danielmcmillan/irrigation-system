export interface ScheduleEntry {
  id?: string;
  propertyIds: string[];
  startTime: number;
  endTime: number;
  abortOnFailure?: boolean;
}
