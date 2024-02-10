export interface CommandResultDeviceMessage {
  commandId: number;
  responseCode: number;
  data?: ArrayBuffer;
}

export function parseControllerCommandResult(payload: ArrayBuffer): CommandResultDeviceMessage {
  const view = new DataView(payload);
  return {
    commandId: view.getUint16(0, true),
    responseCode: view.getUint16(2, true),
    data: payload.byteLength > 4 ? payload.slice(4) : undefined,
  };
}
