export interface DeviceError {
  component: number;
  code: number;
  text: string;
}

const errorTextDecoder = new TextDecoder();

export function getErrorFromData(payload: ArrayBuffer): DeviceError {
  const view = new DataView(payload);
  return {
    component: view.getUint8(0),
    code: view.getUint16(1, true),
    text: errorTextDecoder.decode(payload.slice(3)),
  };
}
