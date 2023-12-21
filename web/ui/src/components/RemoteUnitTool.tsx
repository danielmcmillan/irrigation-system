import {
  Button,
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

type CommandType = "getBattery" | "getSignalStrength" | "getUpTime" | "raw";

function getCommand(type: CommandType, input?: Uint8Array): Uint8Array | number[] | undefined {
  switch (type) {
    case "getBattery":
      return [0x13];
    case "getSignalStrength":
      return [0x14];
    case "getUpTime":
      return [0x16];
    case "raw":
      return input;
    default:
      return undefined;
  }
}

function getType(commandType: number): CommandType {
  switch (commandType) {
    case 0x13:
      return "getBattery";
    case 0x14:
      return "getSignalStrength";
    case 0x16:
      return "getUpTime";
    default:
      return "raw";
  }
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
    switch (requestType) {
      case "getBattery":
        if (result.data.byteLength === 3) {
          resultString = `${view.getUint16(1, true)} (raw value)`;
          break;
        }
      case "getSignalStrength":
        if (result.data.byteLength === 2) {
          resultString = `${view.getUint8(1) - 164} dBm (note -120=weak, -30=strong)`;
          break;
        }
      case "getUpTime":
        if (result.data.byteLength === 5) {
          resultString = `~${((view.getUint32(1, true) * 8.75) / 3600 / 24).toFixed(3)} days`;
          break;
        }
      case "raw":
        resultString = "Successful";
        break;
    }
  }
  return (
    <tr>
      <td>{result.id}</td>
      <td>{new Date(result.time).toLocaleTimeString()}</td>
      <td>{requestType}</td>
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
    const [nodeNumber, setNodeNumber] = useState(1);
    const [type, setType] = useState<CommandType>("getBattery");
    const [rawCommand, setRawCommand] = useState<string>("");

    const handleRunRequest = () => {
      const rawBinary = hexToBin(rawCommand);
      const command = type ? getCommand(type, rawBinary) : undefined;
      if (command) {
        const commandData = new Uint8Array(3 + command.length);
        commandData.set([1, nodeNumber, nodeNumber >> 8]);
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
            value={type}
            onChange={(e) => setType(e.target.value as CommandType)}
          >
            <option value="getBattery">Get battery</option>
            <option value="getSignalStrength">Get signal strength</option>
            <option value="getUpTime">Get up time</option>
            <option value="raw">Raw command</option>
          </SelectField>

          <StepperField
            label="Node Number"
            min={1}
            max={65535}
            value={nodeNumber}
            onStepChange={setNodeNumber}
          />

          {type === "raw" && (
            <TextField
              name="rawCommand"
              label="Raw Command"
              value={rawCommand}
              onChange={(e) => setRawCommand(e.target.value)}
            />
          )}

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
