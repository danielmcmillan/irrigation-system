import React, { useState } from "react";
import {
  View,
  Heading,
  Flex,
  RadioGroupField,
  Radio,
  Button,
  Text,
  StepperField,
} from "@aws-amplify/ui-react";
import { binToHex, numberToHex } from "../irrigation/util";

interface Vacon100ToolProps {
  onRunRequest: (address: number, value?: number) => void;
  onClose: () => void;
  result: ArrayBuffer | undefined;
}

export const Vacon100Tool: React.FC<Vacon100ToolProps> = ({
  onRunRequest,
  onClose,
  result,
}) => {
  const [isRead, setIsRead] = useState(true);
  const [address, setAddress] = useState<number>(0);
  const [value, setValue] = useState<number>(0);

  const handleRunRequest = () => {
    onRunRequest(address, isRead ? undefined : value);
  };

  const resultArray = result ? new Uint8Array(result) : undefined;
  const resultError =
    resultArray && resultArray.byteLength > 1 && resultArray[0] > 0
      ? result?.slice(1)
      : undefined;
  const resultValue =
    resultArray && resultArray.byteLength > 2 && resultArray[0] === 0
      ? resultArray[1] + (resultArray[2] << 8)
      : undefined;
  const successfulWrite =
    resultArray && resultArray.byteLength === 1 && resultArray[0] === 0;

  return (
    <Flex direction="column" margin="1rem">
      <Button onClick={onClose}>Close</Button>
      <Heading level={3}>Vacon100 Tool</Heading>

      <Flex direction="column" gap="1rem">
        <RadioGroupField
          name="mode"
          label="Mode"
          value={isRead ? "read" : "write"}
          onChange={(e) => setIsRead(e.target.value === "read")}
        >
          <Radio value="read">Read</Radio>
          <Radio value="write">Write</Radio>
        </RadioGroupField>

        <StepperField
          label="Address"
          type="number"
          min={0}
          max={65535}
          value={address}
          onStepChange={setAddress}
        />

        {!isRead && (
          <StepperField
            label="Value"
            type="number"
            min={0}
            max={65535}
            value={value}
            onStepChange={setValue}
          />
        )}

        <Button onClick={handleRunRequest}>Run Request</Button>

        {resultError !== undefined && (
          <Text>Error: {binToHex(resultError)}</Text>
        )}
        {resultValue !== undefined && (
          <Text>
            Read value: {resultValue} (0x{numberToHex(resultValue, 2)})
          </Text>
        )}
        {successfulWrite && <Text>Write successful</Text>}
      </Flex>
    </Flex>
  );
};
