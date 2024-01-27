import React, { useCallback, useEffect, useRef, useState } from "react";
import { ConfigEntry, ConfigType } from "../irrigation/config";
import {
  Heading,
  Button,
  Flex,
  RadioGroupField,
  Radio,
  TextField,
  Loader,
  ButtonGroup,
  Text,
  StepperField,
  TextAreaField,
} from "@aws-amplify/ui-react";
import { observer } from "mobx-react-lite";

interface ConfigEditorProps {
  configIni: string;
  loading: boolean;
  onUpdate: (configIni: string) => void;
  onCancel: () => void;
  onSave: () => void;
}

export const IniConfigEditor: React.FC<ConfigEditorProps> = observer(
  ({ configIni, loading, onUpdate, onCancel, onSave }) => {
    const [textRows, setTextRows] = useState<number>(3);

    useEffect(() => {
      setTextRows(Math.max(3, configIni.split("\n").length + 1));
    }, [configIni]);

    return (
      <Flex direction="column" margin="1rem">
        <ButtonGroup direction="row">
          <Button flex={1} onClick={onCancel}>
            Cancel
          </Button>
          <Button flex={1} disabled={loading} onClick={onSave}>
            Save
          </Button>
        </ButtonGroup>
        <Text>Note: Pressing save will cause the controller to reset.</Text>
        {loading && <Loader />}
        <TextAreaField
          rows={textRows}
          label="Config"
          labelHidden={true}
          isDisabled={loading}
          autoComplete="off"
          value={configIni}
          onChange={(e) => onUpdate(e.target.value)}
        />
      </Flex>
    );
  }
);
