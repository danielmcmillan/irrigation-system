export enum ConfigType {
  RemoteUnitNode = 0x01,
  RemoteUnitSolenoid = 0x02,
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
export type ConfigEntry =
  | ConfigEntryRemoteUnitNode
  | ConfigEntryRemoteUnitSolenoid;

export function serializeConfigEntries(entries: ConfigEntry[]): ArrayBuffer {
  const buffers: ArrayBuffer[] = [];

  for (const entry of entries) {
    const buffer = new ArrayBuffer(6);
    const view = new DataView(buffer);

    view.setUint8(0, 6);
    view.setUint8(1, entry.controllerId);
    view.setUint8(2, entry.type);

    switch (entry.type) {
      case ConfigType.RemoteUnitNode: {
        view.setUint8(3, entry.remoteUnitId);
        view.setUint16(4, entry.nodeNumber, true);
        break;
      }
      case ConfigType.RemoteUnitSolenoid: {
        view.setUint8(3, entry.solenoidId);
        view.setUint8(4, entry.remoteUnitId);
        view.setUint8(5, entry.solenoidNumber);
        break;
      }
    }

    buffers.push(buffer);
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

export function deserializeConfigEntries(data: ArrayBuffer): ConfigEntry[] {
  const entries: ConfigEntry[] = [];
  const view = new DataView(data);
  let idx = 0;

  while (idx < view.byteLength) {
    const length = view.getUint8(idx++);
    const controllerId = view.getUint8(idx++);
    const type = view.getUint8(idx++) as ConfigType;

    if (length === 6) {
      switch (type) {
        case ConfigType.RemoteUnitNode: {
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
        case ConfigType.RemoteUnitSolenoid: {
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
        default:
          throw new Error(`Invalid config type: ${type}`);
      }
    } else {
      throw new Error(`Invalid entry length: ${length}`);
    }
  }

  return entries;
}
