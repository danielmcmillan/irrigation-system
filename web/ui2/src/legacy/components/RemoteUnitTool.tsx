import {
  Button,
  CheckboxField,
  Flex,
  Heading,
  ScrollView,
  SelectField,
  StepperField,
  Text,
  TextField,
} from "@aws-amplify/ui-react";
import React, { useMemo, useState } from "react";
import { binToHex, hexToBin, numberToHex } from "../irrigation/util";
import { ControllerCommandResult } from "../irrigation/store";
import { observer } from "mobx-react-lite";

interface RemoteUnitToolProps {
  onRunRequest: (commandData: ArrayBuffer) => void;
  onClose: () => void;
  results: ControllerCommandResult[];
}

const commandTypes = [
  {
    type: "getBattery",
    name: "Get battery",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x13];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x13;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 3) {
        return `${resultData.getUint16(1, true)} (raw value)`;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      return undefined;
    },
  },
  {
    type: "getNodeReceiveStrength",
    name: "Get node receive strength",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x14];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x14;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 2) {
        return `${resultData.getUint8(1) - 164} dBm (note -120=weak, -30=strong)`;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      return undefined;
    },
  },
  {
    type: "getReceiveStrength",
    name: "Get receive strength",
    isRemoteUnitCommand: false,
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x02];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength == 1 && request.getUint8(0) == 0x02;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 1) {
        return `${resultData.getUint8(0) - 164} dBm (note -120=weak, -30=strong)`;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      return undefined;
    },
  },
  {
    type: "getUpTime",
    name: "Get up time",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x16];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x16;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 5) {
        return `~${((resultData.getUint32(1, true) * 8.75) / 3600 / 24).toFixed(3)} days`;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      return undefined;
    },
  },
  {
    type: "getSensorValue",
    name: "Get sensor value",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x18, state === true ? 0xff : 0x00];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x18;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 4) {
        const value = resultData.getInt16(2, true);
        const success = (resultData.getUint8(1) & 0x80) > 0;
        const unread = (resultData.getUint8(1) & 0x40) > 0;
        const error = resultData.getUint8(1) & 0x3f;
        let result = value == 0x7fff ? "no value" : `${value * 0.1} kPa`;
        if (!success) {
          result += ` error ${error}`;
        }
        if (unread) {
          result += ` (unread)`;
        }
        return result;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      const requestRead = state === true;
      return (
        <CheckboxField
          name="requestRead"
          label="Request read"
          checked={requestRead}
          onChange={(e) => setState(e.target.checked)}
        />
      );
    },
  },
  {
    type: "getConfig",
    name: "Get config",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x15];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x15;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 17) {
        return `config`;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      return undefined;
    },
  },
  {
    type: "setConfig",
    name: "Set config",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return typeof state === "string" ? hexToBin("25" + state) : undefined;
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x25;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 17) {
        return `config`;
      }
    },
    initState(pastResults: ControllerCommandResult[]): unknown {
      const pastConfig = pastResults.find(
        (r) => r.request.byteLength >= 3 && new Uint8Array(r.request)[3] == 0x15
      );
      if (pastConfig && !pastConfig.responseCode && pastConfig.data) {
        return binToHex(pastConfig.data.slice(1));
      }
      return undefined;
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      const configData = typeof state === "string" ? state : "";
      const configBin = hexToBin(configData);
      const setConfigBin = (configBin: Uint8Array) => setState(binToHex(configBin));
      return (
        <>
          <TextField
            name="configData"
            label="Config"
            value={configData}
            onChange={(e) => setState(e.target.value)}
          />
          <StepperField
            name="configSignalStrength"
            label="Signal strength"
            min={1}
            max={7}
            defaultValue={7}
            value={configBin.byteLength >= 5 ? configBin[5] : undefined}
            onStepChange={(value) => {
              if (configBin.byteLength >= 5) {
                const newConfig = new Uint8Array(configBin);
                newConfig[5] = value;
                setConfigBin(newConfig);
              }
            }}
          />
          <StepperField
            name="configSensorInterval"
            label="Sensor read interval"
            min={0}
            max={15}
            defaultValue={0}
            value={
              configBin.byteLength >= 15
                ? (configBin[12] & 0x01) |
                  ((configBin[13] & 0x01) << 1) |
                  ((configBin[14] & 0x01) << 2) |
                  ((configBin[15] & 0x01) << 3)
                : undefined
            }
            onStepChange={(value) => {
              if (configBin.byteLength >= 15) {
                const newConfig = new Uint8Array(configBin);
                newConfig[12] = (newConfig[12] & 0xfe) | (value & 0x01);
                newConfig[13] = (newConfig[13] & 0xfe) | (value & 0x02 ? 0x01 : 0x00);
                newConfig[14] = (newConfig[14] & 0xfe) | (value & 0x04 ? 0x01 : 0x00);
                newConfig[15] = (newConfig[15] & 0xfe) | (value & 0x08 ? 0x01 : 0x00);
                setConfigBin(newConfig);
              }
            }}
          />
        </>
      );
    },
  },
  {
    type: "applyRfConfig",
    name: "Apply RF config",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x31];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x31;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 17) {
        return `config`;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      return undefined;
    },
  },
  {
    type: "persistConfig",
    name: "Persist config",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x30];
    },
    matchRequest(request: DataView): boolean {
      return request.byteLength >= 3 && request.getUint8(3) == 0x30;
    },
    resultString(resultData: DataView): string | undefined {
      if (resultData.byteLength === 17) {
        return `config`;
      }
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      return undefined;
    },
  },
  {
    type: "raw",
    name: "Raw command",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return typeof state === "string" ? hexToBin(state) : undefined;
    },
    matchRequest(request: DataView): boolean {
      return true;
    },
    resultString(resultData: DataView): string | undefined {
      return "Successful";
    },
    inputView(state: unknown, setState: (state: unknown) => void): React.ReactNode | undefined {
      const rawCommand = typeof state === "string" ? state : "";
      return (
        <TextField
          name="rawCommand"
          label="Raw Command"
          value={rawCommand}
          onChange={(e) => setState(e.target.value)}
        />
      );
    },
  },
];
type CommandType = typeof commandTypes extends readonly (infer ElementType)[] ? ElementType : never;

function getType(request: DataView): CommandType {
  const type = commandTypes.find((t) => t.matchRequest(request));
  return type ?? commandTypes.at(-1)!;
}

const RemoteUnitResult: React.FC<{ result: ControllerCommandResult }> = observer(({ result }) => {
  const resultHex = result.data !== undefined ? `0x${binToHex(result.data)}` : undefined;
  const request = new DataView(result.request);
  const requestType = getType(request);
  const node = requestType.isRemoteUnitCommand !== false ? request.getUint16(1, true) : 0;
  const requestData = binToHex(result.request.slice(3));
  let resultString = "Pending...";
  if (result.responseCode) {
    resultString = `Error: ${result.responseCode}`;
  } else if (result.data) {
    const view = new DataView(result.data);
    resultString = requestType.resultString(view) ?? "Invalid";
  }
  return (
    <tr>
      <td>{result.id}</td>
      <td>{new Date(result.time).toLocaleTimeString()}</td>
      <td>{requestType.type}</td>
      <td>0x{numberToHex(node, 2)}</td>
      <td>0x{requestData}</td>
      <td>{resultString}</td>
      <td>{resultHex ?? "-"}</td>
    </tr>
  );
});

export const RemoteUnitTool: React.FC<RemoteUnitToolProps> = observer(
  ({ onRunRequest, onClose, results }) => {
    const [nodeNumber, setNodeNumber] = useState("01");
    const [type, setType] = useState<CommandType>(commandTypes[0]);
    const [commandState, setCommandState] = useState<unknown>(undefined);
    const resultsDescending = useMemo(() => results.toReversed(), [results]);

    const handleRunRequest = () => {
      const node = hexToBin(nodeNumber);
      const command = type ? type.getCommand(commandState) : undefined;
      if (command) {
        if (type.isRemoteUnitCommand !== false) {
          const commandData = new Uint8Array(3 + command.length);
          commandData.set([1, node[0], node[1]]);
          commandData.set(command, 3);
          onRunRequest(commandData.buffer);
        } else {
          const commandData = new Uint8Array(command.length);
          commandData.set(command, 0);
          onRunRequest(commandData.buffer);
        }
      }
    };

    return (
      <Flex direction="column" margin="1rem">
        <Button onClick={onClose}>Close</Button>
        <Heading level={3}>Remote Unit Tool</Heading>

        <Flex direction="column" gap="1rem">
          <SelectField
            name="type"
            label="Type"
            value={type.type}
            onChange={(e) => {
              const type = commandTypes.find((t) => t.type === e.target.value)!;
              setCommandState(type.initState?.(resultsDescending));
              setType(type);
            }}
          >
            {commandTypes.map((t) => (
              <option key={t.type} value={t.type}>
                {t.name}
              </option>
            ))}
          </SelectField>

          {type.isRemoteUnitCommand !== false && (
            <TextField
              name="nodeNumber"
              label="Node Number (hex)"
              value={nodeNumber}
              onChange={(e) => setNodeNumber(e.target.value)}
            />
          )}
          {type.inputView(commandState, setCommandState)}
          <Button onClick={handleRunRequest}>Run Request</Button>
          <ScrollView>
            <table style={{ textAlign: "left" }}>
              <thead>
                <tr>
                  <th></th>
                  <th>Time</th>
                  <th>Type</th>
                  <th>Node</th>
                  <th>Request data</th>
                  <th>Response</th>
                  <th>Response data</th>
                </tr>
              </thead>
              <tbody>
                {resultsDescending.map((result, i) => (
                  <RemoteUnitResult key={i} result={result} />
                ))}
              </tbody>
            </table>
          </ScrollView>
        </Flex>
      </Flex>
    );
  }
);
