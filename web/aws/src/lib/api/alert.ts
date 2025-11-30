export function getControllerErrorMessage(controllerId: number, errorData: ArrayBuffer): string {
  const view = new DataView(errorData);
  let message: string;
  if (controllerId == 2 && errorData.byteLength === 3) {
    const typeNum = view.getUint8(0);
    const codeNum = view.getUint16(1, true);
    const type =
      {
        0: "Start client",
        1: "Configure ID mappings",
        2: "Write",
        3: "Read",
      }[typeNum] ?? `Unknown (${typeNum})`;
    const code =
      ({
        5: "EIO: Input/output error",
        22: "EINVAL: Invalid argument",
        109: "ENOPROTOOPT: Protocol not available",
        111: "ECONNREFUSED: Connection refused",
        116: "ETIMEDOUT: Connection timed out",
        134: "ENOTSUP: Operation not supported",
        11235: "EMBXILFUN: Illegal function",
        11236: "EMBXILADD: Illegal data address",
        11237: "EMBXILVAL: Illegal data value",
        11238: "EMBXSFAIL: Slave device or server failure",
        11239: "EMBXACK: Acknowledge",
        11240: "EMBXSBUSY: Slave device or server is busy",
        11241: "EMBXNACK: Negative acknowledge",
        11242: "EMBXMEMPAR: Memory parity error",
        11244: "EMBXGPATH: Gateway path unavailable",
        11245: "EMBXGTAR: Target device failed to respond",
        11246: "EMBBADCRC: Invalid CRC",
        11247: "EMBBADDATA: Invalid data",
        11248: "EMBBADEXC: Invalid exception code",
        11250: "EMBMDATA: Too many data",
        11251: "EMBBADSLAVE: Response not from requested slave",
      }[codeNum] ?? "Unknown") + `(${codeNum})`;
    // 112345678-11234
    message = `VSD ${type} ${code}`;
  } else if (controllerId === 4) {
    message = "Remote Unit ";
    const typeNum = view.getUint8(0);
    const remoteUnitId = view.getUint8(1);
    if (remoteUnitId > 0) {
      message += `${remoteUnitId} `;
    }
    if ((typeNum & 0x80) === 0x80) {
      const faultNum = typeNum & 0x0f;
      message += "Fault ";
      message +=
        {
          0: "SolenoidTimeoutOccurred",
          1: "SolenoidNotActivated",
          2: "BatteryVoltageError",
          3: "ConfigReadFailed",
          4: "ConfigureRfModuleFailed",
          5: "SolenoidLowCurrent"
        }[faultNum] ?? `Unknown (${faultNum})`;
    } else {
      message +=
        {
          1: "Failed to configure RF module",
          2: "Failed to write to Serial",
          3: "Timeout waiting for response on Serial",
          4: "Response has invalid CRC",
          5: "Response includes invalid commands or data",
          6: "Response is valid but for unexpected node id",
          7: "Response is for unexpected commands",
        }[typeNum] ?? `Unknown (${typeNum})`;
    }
  } else {
    message = `Unknown controller ${controllerId} error: ${Buffer.from(errorData).toString("hex")}`;
  }
  return message;
}

export function getPropertyErrorMessage(
  controllerId: number,
  propertyId: number,
  errorData: ArrayBuffer
): string {
  let message: string;
  if (controllerId === 4 && (propertyId & 0xff00) === 0x0400) {
    message = `Failed to read from sensor. Error: ${Buffer.from(errorData).toString("hex")}`;
  } else {
    message = `Unknown property ${controllerId}/${propertyId} error: ${Buffer.from(
      errorData
    ).toString("hex")}`;
  }
  return message;
}
