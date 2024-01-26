import { DeviceControllerDefinition, DeviceControllerDefinitions } from "../types.js";
import { RemoteUnitController } from "./remoteUnitController.js";
import { Vacon100Controller } from "./vacon100Controller.js";

export const getControllerDefinitions = (): DeviceControllerDefinitions =>
  new Map<number, DeviceControllerDefinition>([
    [0x02, new Vacon100Controller()],
    [0x04, new RemoteUnitController()],
  ]);
