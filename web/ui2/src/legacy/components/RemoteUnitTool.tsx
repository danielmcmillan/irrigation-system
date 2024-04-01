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
import React, { useState } from "react";
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
    matchCommand(cmd: number): boolean {
      return cmd === 0x13;
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
    type: "getSignalStrength",
    name: "Get signal strength",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x14];
    },
    matchCommand(cmd: number): boolean {
      return cmd === 0x14;
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
    type: "getUpTime",
    name: "Get up time",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x16];
    },
    matchCommand(cmd: number): boolean {
      return cmd === 0x16;
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
    type: "readSensor",
    name: "Read sensor",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return [0x18, state === true ? 0xff : 0x00];
    },
    matchCommand(cmd: number): boolean {
      return cmd === 0x18;
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
    type: "raw",
    name: "Raw command",
    getCommand(state: unknown): ArrayLike<number> | undefined {
      return typeof state === "string" ? hexToBin(state) : undefined;
    },
    matchCommand(cmd: number): boolean {
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

function getType(commandType: number): CommandType {
  const type = commandTypes.find((t) => t.matchCommand(commandType));
  return type ?? commandTypes.at(-1)!;
}

const RemoteUnitResult: React.FC<{ result: ControllerCommandResult }> = observer(({ result }) => {
  const resultHex = result.data !== undefined ? `0x${binToHex(result.data)}` : undefined;
  const request = new DataView(result.request);
  const commandType = request.getUint8(3);
  const requestType = getType(commandType);
  const node = request.getUint16(1, true);
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
      <td>
        {node} (0x{numberToHex(node, 2)})
      </td>
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

    const handleRunRequest = () => {
      const node = hexToBin(nodeNumber);
      const command = type ? type.getCommand(commandState) : undefined;
      if (command) {
        const commandData = new Uint8Array(3 + command.length);
        commandData.set([1, node[0], node[1]]);
        commandData.set(command, 3);
        onRunRequest(commandData.buffer);
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
              setCommandState(undefined);
              setType(commandTypes.find((t) => t.type === e.target.value)!);
            }}
          >
            {commandTypes.map((t) => (
              <option key={t.type} value={t.type}>
                {t.name}
              </option>
            ))}
          </SelectField>

          <TextField
            name="nodeNumber"
            label="Node Number (hex)"
            value={nodeNumber}
            onChange={(e) => setNodeNumber(e.target.value)}
          />
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
                {results.toReversed().map((result, i) => (
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
