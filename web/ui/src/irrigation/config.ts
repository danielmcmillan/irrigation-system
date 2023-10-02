import { ConfigIniParser } from "config-ini-parser";

export enum ConfigType {
  RemoteUnitNode = 0x01,
  RemoteUnitSolenoid = 0x02,
  RemoteUnitSensor = 0x03,
}
export interface ConfigEntryRemoteUnitNode {
  controllerId: number;
  type: ConfigType.RemoteUnitNode;
  remoteUnitId: number;
  nodeNumber: number;
}
export interface ConfigEntryRemoteUnitSolenoid {
  controllerId: number;
  type: ConfigType.RemoteUnitSolenoid;
  solenoidId: number;
  remoteUnitId: number;
  solenoidNumber: number;
}
export interface ConfigEntryRemoteUnitSensor {
  controllerId: number;
  type: ConfigType.RemoteUnitSensor;
  remoteUnitId: number;
}
export type ConfigEntry =
  | ConfigEntryRemoteUnitNode
  | ConfigEntryRemoteUnitSolenoid
  | ConfigEntryRemoteUnitSensor;

export function serializeConfigEntriesToBinary(entries: ConfigEntry[]): ArrayBuffer {
  const buffers: ArrayBuffer[] = [];

  for (const entry of entries) {
    let size: number;
    const buffer = new ArrayBuffer(6);
    const view = new DataView(buffer);

    view.setUint8(1, entry.controllerId);
    view.setUint8(2, entry.type);

    switch (entry.type) {
      case ConfigType.RemoteUnitNode: {
        size = 6;
        view.setUint8(3, entry.remoteUnitId);
        view.setUint16(4, entry.nodeNumber, true);
        break;
      }
      case ConfigType.RemoteUnitSolenoid: {
        size = 6;
        view.setUint8(3, entry.solenoidId);
        view.setUint8(4, entry.remoteUnitId);
        view.setUint8(5, entry.solenoidNumber);
        break;
      }
      case ConfigType.RemoteUnitSensor: {
        size = 4;
        view.setUint8(3, entry.remoteUnitId);
      }
    }

    view.setUint8(0, size);
    buffers.push(buffer.slice(0, size));
  }

  const totalLength = buffers.reduce((acc, val) => acc + val.byteLength, 0);
  const result = new Uint8Array(totalLength);
  let offset = 0;

  for (const buffer of buffers) {
    result.set(new Uint8Array(buffer), offset);
    offset += buffer.byteLength;
  }

  return result.buffer;
}

export function deserializeConfigEntriesFromBinary(data: ArrayBuffer): ConfigEntry[] {
  const entries: ConfigEntry[] = [];
  const view = new DataView(data);
  let idx = 0;

  while (idx < view.byteLength) {
    const length = view.getUint8(idx++);
    const controllerId = view.getUint8(idx++);
    const type = view.getUint8(idx++) as ConfigType;

    switch (type) {
      case ConfigType.RemoteUnitNode:
        if (length === 6) {
          const remoteUnitId = view.getUint8(idx++);
          const nodeNumber = view.getUint16(idx, true);
          idx += 2;
          entries.push({
            controllerId,
            type: ConfigType.RemoteUnitNode,
            remoteUnitId,
            nodeNumber,
          });
          break;
        }
      case ConfigType.RemoteUnitSolenoid:
        if (length === 6) {
          const solenoidId = view.getUint8(idx++);
          const remoteUnitId = view.getUint8(idx++);
          const solenoidNumber = view.getUint8(idx++);
          entries.push({
            controllerId,
            type: ConfigType.RemoteUnitSolenoid,
            solenoidId,
            remoteUnitId,
            solenoidNumber,
          });
          break;
        }
      case ConfigType.RemoteUnitSensor:
        if (length === 4 || length == 6) {
          const remoteUnitId = view.getUint8(idx++);
          entries.push({
            controllerId,
            type: ConfigType.RemoteUnitSensor,
            remoteUnitId,
          });
          if (length == 6) {
            console.warn("Workaround wrong config size");
            idx += 2;
          }
          break;
        }
      default:
        throw new Error(`Invalid config type: ${type} with length ${length}`);
    }
  }

  return entries;
}

export function serializeConfigEntriesToIni(entries: ConfigEntry[]): string {
  const ini = new ConfigIniParser("\n");
  ini.addSection("Zones");
  ini.addSection("Sensors");

  for (const entry of entries) {
    switch (entry.type) {
      case ConfigType.RemoteUnitNode: {
        // Ini format assumes Remote Unit ID and node number are equal.
        break;
      }
      case ConfigType.RemoteUnitSolenoid: {
        ini.set(
          "Zones",
          `Z${entry.solenoidId}`,
          `${entry.remoteUnitId}${String.fromCharCode(65 + entry.solenoidNumber)}`
        );
        break;
      }
      case ConfigType.RemoteUnitSensor: {
        ini.set("Sensors", `N${entry.remoteUnitId}`, "true");
        break;
      }
    }
  }

  return ini.stringify();
}

export function deserializeConfigEntriesFromIni(iniConfig: string): ConfigEntry[] {
  const ini = new ConfigIniParser("\n");
  ini.parse(iniConfig);

  const entries: ConfigEntry[] = [];
  const remoteUnitIds = new Set<number>();

  for (const [key, value] of ini.items("Zones")) {
    const solenoidId = Number(key.match(/^\s*[zZ]*(\d+)\s*$/)?.at(1));
    const matchNodeSolenoidSpecifier = value.match(/^\s*(\d+)\s*([a-zA-Z])?\s*$/);
    const remoteUnitId = Number(matchNodeSolenoidSpecifier?.at(1));
    const solenoidString = matchNodeSolenoidSpecifier?.at(2)?.toUpperCase() ?? "A";
    const solenoidNumber = solenoidString.charCodeAt(0) - 65;

    if (
      Number.isNaN(solenoidId) ||
      Number.isNaN(remoteUnitId) ||
      solenoidNumber < 0 ||
      solenoidNumber > 7
    ) {
      throw new Error(`Invalid zone config entry: ${key} = ${value}`);
    }

    entries.push({
      controllerId: 4,
      type: ConfigType.RemoteUnitSolenoid,
      solenoidId,
      remoteUnitId,
      solenoidNumber,
    });
    remoteUnitIds.add(remoteUnitId);
  }
  for (const remoteUnitId of remoteUnitIds) {
    entries.push({
      controllerId: 4,
      type: ConfigType.RemoteUnitNode,
      remoteUnitId,
      nodeNumber: Number.parseInt(remoteUnitId.toString(10), 16), // workaround for nodes being accidentally programmed with hex values
    });
  }
  for (const [key, value] of ini.items("Sensors")) {
    const remoteUnitId = Number(key.match(/^\s*[nN]*(\d+)\s*$/)?.at(1));
    entries.push({
      controllerId: 4,
      type: ConfigType.RemoteUnitSensor,
      remoteUnitId,
    });
  }

  return entries;
}
