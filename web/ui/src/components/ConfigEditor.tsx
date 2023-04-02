import React, { useCallback, useState } from "react";
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
} from "@aws-amplify/ui-react";
import { observer } from "mobx-react-lite";

interface ConfigEditorProps {
  configEntries: ConfigEntry[];
  loading: boolean;
  onUpdate: (updatedEntries: ConfigEntry[]) => void;
  onCancel: () => void;
  onSave: () => void;
}

export const ConfigEditor: React.FC<ConfigEditorProps> = observer(
  ({ configEntries, loading, onUpdate, onCancel, onSave }) => {
    const [editingEntry, setEditingEntry] = useState<ConfigEntry | null>(null);
    const controllerId = 4;
    const [type, setType] = useState<ConfigType>(ConfigType.RemoteUnitNode);
    const [remoteUnitId, setRemoteUnitId] = useState<number>(0);
    const [solenoidId, setSolenoidId] = useState<number>(0);
    const [nodeNumber, setNodeNumber] = useState<number>(1);
    const [solenoidNumber, setSolenoidNumber] = useState<number>(0);

    const startEditing = useCallback((entry: ConfigEntry | null) => {
      setEditingEntry(entry);
      if (entry) {
        setType(entry.type);
        setRemoteUnitId(entry.remoteUnitId);
        if (entry.type === ConfigType.RemoteUnitNode) {
          setNodeNumber(entry.nodeNumber);
        } else {
          setSolenoidId(entry.solenoidId);
          setSolenoidNumber(entry.solenoidNumber);
        }
      } else {
        setType(ConfigType.RemoteUnitNode);
        setRemoteUnitId(0);
        setSolenoidId(0);
        setNodeNumber(1);
        setSolenoidNumber(0);
      }
    }, []);

    const handleAddEntry = () => {
      const newEntry: ConfigEntry =
        type === ConfigType.RemoteUnitNode
          ? {
              controllerId,
              type,
              remoteUnitId,
              nodeNumber,
            }
          : {
              controllerId,
              type,
              solenoidId,
              remoteUnitId,
              solenoidNumber,
            };

      onUpdate([...configEntries, newEntry]);
    };

    const handleUpdateEntry = () => {
      if (!editingEntry) return;

      const updatedEntries = configEntries.map((entry) =>
        entry === editingEntry
          ? type === ConfigType.RemoteUnitNode
            ? {
                controllerId,
                type,
                remoteUnitId,
                nodeNumber,
              }
            : {
                controllerId,
                type,
                solenoidId,
                remoteUnitId,
                solenoidNumber,
              }
          : entry
      );

      onUpdate(updatedEntries);
      startEditing(null);
    };

    const handleRemoveEntry = (entryToRemove: ConfigEntry) => {
      const updatedEntries = configEntries.filter(
        (entry) => entry !== entryToRemove
      );
      onUpdate(updatedEntries);
    };

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

        <Flex direction="column">
          {configEntries.map((entry, index) => (
            <Flex key={index} direction="row" alignItems="center">
              <Flex direction="column" gap="0rem" flex={1}>
                <Text>
                  <strong>
                    {entry.type === ConfigType.RemoteUnitNode
                      ? "Remote Unit Node "
                      : "Remote Unit Solenoid "}
                  </strong>
                </Text>
                <Text>Remote Unit ID: {entry.remoteUnitId}</Text>
                {entry.type === ConfigType.RemoteUnitNode && (
                  <Text>Node Number: {(entry as any).nodeNumber}</Text>
                )}
                {entry.type === ConfigType.RemoteUnitSolenoid && (
                  <>
                    <Text>Solenoid ID: {entry.solenoidId}</Text>
                    <Text>
                      Solenoid Number: {(entry as any).solenoidNumber}
                    </Text>
                  </>
                )}
              </Flex>
              <ButtonGroup direction="column" gap="0.5rem">
                <Button
                  disabled={entry === editingEntry}
                  onClick={() => startEditing(entry)}
                >
                  Edit
                </Button>
                <Button onClick={() => handleRemoveEntry(entry)}>Remove</Button>
              </ButtonGroup>
            </Flex>
          ))}
        </Flex>

        <Heading level={3}>{editingEntry ? "Edit Entry" : "Add Entry"}</Heading>

        <Flex direction="column">
          <RadioGroupField
            label="Type"
            name="type"
            value={type.toString()}
            onChange={(e) =>
              setType(parseInt(e.target.value, 10) as ConfigType)
            }
          >
            <Radio value={ConfigType.RemoteUnitNode.toString()}>
              Remote Unit Node
            </Radio>
            <Radio value={ConfigType.RemoteUnitSolenoid.toString()}>
              Remote Unit Solenoid
            </Radio>
          </RadioGroupField>

          <TextField
            label="Controller ID"
            type="number"
            value={controllerId}
            disabled={true}
          />

          <StepperField
            label="Remote Unit ID"
            step={1}
            min={0}
            max={255}
            value={remoteUnitId}
            onStepChange={setRemoteUnitId}
          />

          {type === ConfigType.RemoteUnitSolenoid && (
            <StepperField
              label="Solenoid ID"
              step={1}
              min={0}
              max={255}
              value={solenoidId}
              onStepChange={setSolenoidId}
            />
          )}

          {type === ConfigType.RemoteUnitNode && (
            <StepperField
              label="Node Number"
              step={1}
              min={1}
              max={65535}
              value={nodeNumber}
              onStepChange={setNodeNumber}
            />
          )}

          {type === ConfigType.RemoteUnitSolenoid && (
            <StepperField
              label="Solenoid Number"
              step={1}
              min={0}
              max={255}
              value={solenoidNumber}
              onStepChange={setSolenoidNumber}
            />
          )}

          <Button onClick={editingEntry ? handleUpdateEntry : handleAddEntry}>
            {editingEntry ? "Update Entry" : "Add Entry"}
          </Button>
          {editingEntry && (
            <Button onClick={() => startEditing(null)}>Cancel editing</Button>
          )}
        </Flex>
      </Flex>
    );
  }
);
