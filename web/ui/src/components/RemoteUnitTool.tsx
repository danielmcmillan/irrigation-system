import {
  Button,
  Flex,
  Heading,
  SelectField,
  StepperField,
  Text,
  TextField,
} from "@aws-amplify/ui-react";
import React, { useState } from "react";
import { binToHex, hexToBin } from "../irrigation/util";

interface RemoteUnitToolProps {
  onRunRequest: (commandData: ArrayBuffer) => void;
  onClose: () => void;
  result: ArrayBuffer | undefined;
}

type CommandType = "getBattery" | "getSignalStrength" | "getUpTime" | "raw";

function getCommand(
  type: CommandType,
  input?: Uint8Array
): Uint8Array | number[] | undefined {
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

export const RemoteUnitTool: React.FC<RemoteUnitToolProps> = ({
  onRunRequest,
  onClose,
  result,
}) => {
  const [nodeNumber, setNodeNumber] = useState(1);
  const [type, setType] = useState<CommandType | undefined>();
  const [resultType, setResultType] = useState<CommandType | undefined>();
  const [rawCommand, setRawCommand] = useState<string>("");

  const handleRunRequest = () => {
    const rawBinary = hexToBin(rawCommand);
    const command = type ? getCommand(type, rawBinary) : undefined;
    if (command) {
      const commandData = new Uint8Array(3 + command.length);
      commandData.set([1, nodeNumber, nodeNumber >> 8]);
      commandData.set(command, 3);
      onRunRequest(commandData);
      setResultType(type);
    }
  };

  const resultHex = result !== undefined ? `0x${binToHex(result)}` : undefined;
  let resultString = "No result";
  if (result !== undefined) {
    if (result.byteLength == 0) {
      resultString = "Failed";
    } else {
      const view = new DataView(result);
      switch (resultType) {
        case "getBattery":
          resultString = `Battery value: ${view.getUint16(1, true)}`;
          break;
        case "getSignalStrength":
          resultString = `Signal strength: ${
            view.getUint8(1) - 164
          } dBm (note -120=weak, -30=strong)`;
          break;
        case "getUpTime":
          resultString = `Up time: ~${
            (view.getUint32(1, true) * 8.75) / 3600 / 24
          } days`;
          break;
        case "raw":
          resultString = `Raw data: ${resultHex}`;
          break;
      }
    }
  }

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
        {resultString && <Text>{resultString}</Text>}
        {resultHex && <Text>{resultHex}</Text>}
      </Flex>
    </Flex>
  );
};
