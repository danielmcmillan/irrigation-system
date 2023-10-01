import { configureDeviceControllers } from "../deviceControllers/configureDeviceControllers";
import { getControllerDefinitions } from "../deviceControllers/definitions/getControllerDefinitions";
import { DeviceComponentDefinition, DevicePropertyDefinition } from "../deviceControllers/types";
import { DeviceStatus } from "../deviceStatus";
import { DeviceState, PropertyState } from "../store";

export interface Device {
  id: string;
  connected: boolean;
  status?: DeviceStatus;
  lastUpdated: number;
  components: DeviceComponentDefinition[];
  properties: Array<
    DevicePropertyDefinition & {
      lastUpdated?: number;
      lastChanged?: number;
      value?: number | boolean;
      desiredValue?: number | boolean;
    }
  >;
}

function getDevice(deviceState: DeviceState, propertyState: PropertyState[]): Device {
  const controllers = getControllerDefinitions();
  if (deviceState.config) {
    configureDeviceControllers(controllers, deviceState.config);
  }
  return {
    id: deviceState.deviceId,
    connected: deviceState.connected ?? false,
    status: deviceState.status,
    lastUpdated: deviceState.lastUpdated,
    components: [],
    properties: [],
  };
}

export function getDevices(deviceState: DeviceState[], propertyState: PropertyState[]): Device[] {
  return deviceState.map((device) =>
    getDevice(
      device,
      propertyState.filter((p) => p.deviceId === device.deviceId)
    )
  );
}
