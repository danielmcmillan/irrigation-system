import { DeviceControllerDefinitions } from "../types";
import { RemoteUnitController } from "./remoteUnitController";
import { Vacon100Controller } from "./vacon100Controller";

export const getControllerDefinitions = (): DeviceControllerDefinitions => ({
  0x02: new Vacon100Controller(),
  0x04: new RemoteUnitController(),
});
