export interface DeviceSchedule {
  deviceId: string;
  entries: Array<{
    /**
     * Unique identifier for the entry.
     * If empty, it represents a new entry.
     */
    id?: string;
    propertyIds: string[];
    startTime: number;
    endTime: number;
    abortOnFailure?: boolean;
  }>;
}
