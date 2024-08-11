export interface DeviceSchedule {
  deviceId: string;
  entries: Array<{
    propertyIds: string[];
    startTime: number;
    endTime: number;
    abortOnFailure?: boolean;
  }>;
}
