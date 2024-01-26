export interface ErrorDeviceMessage {
  component: number;
  code: number;
  text: string;
}

const errorTextDecoder = new TextDecoder();

/**
 * Parse an "error" device message.
 * This type of message contains one `ErrorDeviceMessage`.
 */
export function parseErrorDeviceMessage(payload: ArrayBuffer): ErrorDeviceMessage {
  const view = new DataView(payload);
  return {
    component: view.getUint8(0),
    code: view.getUint16(1, true),
    text: errorTextDecoder.decode(payload.slice(3)),
  };
}
