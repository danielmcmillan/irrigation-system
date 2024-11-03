import {
  Accordion,
  Button,
  ButtonGroup,
  CheckboxField,
  Fieldset,
  Flex,
  Grid,
  Label,
  SliderField,
} from "@aws-amplify/ui-react";
import { formatDuration, formatRelative } from "date-fns";
import { observer } from "mobx-react-lite";
import { useEffect, useState } from "react";
import { ScheduleEntry } from "../irrigation/schedule";
import { IrrigationPropertyWithComponent } from "../irrigation/store";

export interface EntryEntryProps {
  properties: IrrigationPropertyWithComponent[];
  defaultEntry?: Partial<ScheduleEntry>;
  editing: boolean;
  onSave: (entry: ScheduleEntry) => void;
  onDelete: () => void;
  onCancel: () => void;
}

const defaultStartIn = 0;
const defaultRunTime = 900000;
const getDefaultEntry = (entry?: Partial<ScheduleEntry>): ScheduleEntry => ({
  startTime: Date.now() + defaultStartIn,
  endTime: Date.now() + defaultRunTime,
  propertyIds: [],
  ...entry,
});

const PropertySelector: React.FC<{
  selected: string[];
  onChange: (selected: string[]) => void;
  properties: IrrigationPropertyWithComponent[];
}> = observer(({ selected, onChange, properties }) => {
  const [expanded, setExpanded] = useState(selected.length === 0);
  const options = properties.map((p) => ({
    label: `${p.component?.name} ${p.name}`,
    value: p.id,
    checked: selected.includes(p.id),
  }));
  const summary =
    !expanded && selected.length > 0
      ? "Schedule " +
        options
          .filter((o) => o.checked)
          .map((o) => o.label)
          .join(", ")
      : "Select something to schedule";
  const handleChange: React.ChangeEventHandler<HTMLInputElement> = (e) => {
    const id = e.currentTarget.name;
    onChange(e.currentTarget.checked ? [...selected, id] : selected.filter((s) => s !== id));
  };
  const fields = (
    <Fieldset legend legendHidden>
      {options.map((o) => (
        <CheckboxField
          type="checkbox"
          key={o.value}
          name={o.value}
          label={o.label}
          checked={o.checked}
          onChange={handleChange}
        />
      ))}
    </Fieldset>
  );
  return (
    <Accordion
      value={expanded ? ["properties"] : []}
      onValueChange={(v) => setExpanded(v !== undefined && v.includes("properties"))}
      items={[
        {
          value: "properties",
          trigger: summary,
          content: fields,
        },
      ]}
    />
  );
});

const getDurationBaseTime = (relativeTo: number | undefined, timeOffset: number | undefined) =>
  (relativeTo ?? Date.now()) + (timeOffset ?? 0);
const calculateRelativeTime = (
  absoluteTime: number,
  relativeTo: number | undefined,
  timeOffset: number | undefined
) =>
  Math.max(
    0,
    Math.round((absoluteTime - getDurationBaseTime(relativeTo, timeOffset)) / 60000) * 60000
  );
const calculateAbsoluteTime = (
  relativeTime: number,
  relativeTo: number | undefined,
  timeOffset: number | undefined
) => getDurationBaseTime(relativeTo, timeOffset) + relativeTime;
export const TimeDurationInput: React.FC<{
  label: string;
  relativeTime?: number;
  onChange: (relativeTime: number) => void;
  /** The timestamp to calculate relativeTime based on if none is set. */
  absoluteTime: number;
  /** Base time for converting between relative and absolute time */
  relativeTo?: number;
  /** Offset to the base time */
  timeOffset?: number;
  min?: number;
  max?: number;
}> = observer(
  ({ label, relativeTime, onChange, absoluteTime, relativeTo, timeOffset, min, max }) => {
    const [calculatedRelativeTime, setCalculatedRelativeTime] = useState(
      () => relativeTime ?? calculateRelativeTime(absoluteTime, relativeTo, timeOffset)
    );
    const [calculatedAbsoluteTime, setCalculatedAbsoluteTime] = useState(() =>
      relativeTime ? calculateAbsoluteTime(relativeTime, relativeTo, timeOffset) : absoluteTime
    );
    useEffect(() => {
      const calculate = () => {
        if (relativeTime !== undefined) {
          setCalculatedAbsoluteTime(calculateAbsoluteTime(relativeTime, relativeTo, timeOffset));
        } else {
          setCalculatedRelativeTime(calculateRelativeTime(absoluteTime, relativeTo, timeOffset));
        }
      };
      const interval = setInterval(calculate, 1000);
      calculate();
      return () => clearInterval(interval);
    }, [relativeTime, absoluteTime, relativeTo, timeOffset]);

    const value = relativeTime ?? calculatedRelativeTime;
    const hours = Math.floor(value / 3600000);
    const minutes = (value % 3600000) / 60000;
    const formatValue = () => {
      const relative =
        value > 0
          ? formatDuration({
              hours: Math.floor(value / 3600000),
              minutes: (value % 3600000) / 60000,
            })
          : undefined;
      const absolute = formatRelative(calculatedAbsoluteTime, Date.now());
      return relative ? `${relative}, ${absolute}` : absolute;
    };

    return (
      <Fieldset
        legend={
          <Flex justifyContent="space-between">
            <span>{label}</span>
            <span style={{ fontWeight: "normal" }}>{formatValue()}</span>
          </Flex>
        }
      >
        <Grid rowGap="1em" columnGap="1em" templateColumns="min-content 1fr">
          <Label columnStart="1">Hours</Label>
          <SliderField
            columnStart="2"
            label="Hours"
            min={0}
            max={23}
            value={hours}
            onChange={(newHours) =>
              onChange(Math.max(min ?? 0, newHours * 3600000 + minutes * 60000))
            }
            labelHidden
          />
          <Label columnStart="1">Minutes</Label>
          <SliderField
            columnStart="2"
            label="Minutes"
            min={hours === 0 && min !== undefined ? min / 60000 : 0}
            max={59}
            value={minutes}
            onChange={(newMinutes) => onChange(hours * 3600000 + newMinutes * 60000)}
            labelHidden
          />
        </Grid>
      </Fieldset>
    );
  }
);

export const ScheduleEntryEditor: React.FC<EntryEntryProps> = observer(
  ({ properties, defaultEntry, editing, onSave, onDelete, onCancel }) => {
    const [entry, setEntry] = useState<ScheduleEntry>(getDefaultEntry(defaultEntry));
    // Relative schedule times. Won't be set when editing until a change is made
    const [relativeStartTime, setRelativeStartTime] = useState(
      editing ? undefined : defaultStartIn
    );
    const [runTime, setRunTime] = useState(
      defaultEntry?.endTime && defaultEntry?.startTime
        ? defaultEntry.endTime - defaultEntry.startTime
        : defaultRunTime
    );
    // useEffect(() => {
    //   setEntry(getDefaultEntry(defaultEntry));
    //   setRunTime(
    //     defaultEntry?.endTime && defaultEntry?.startTime
    //       ? defaultEntry.endTime - defaultEntry.startTime
    //       : defaultRunTime
    //   );
    // }, [defaultEntry]);
    // useEffect(() => {
    //   setRelativeStartTime(editing ? undefined : defaultStartIn);
    // }, [editing]);
    const handleSave = () => {
      const startTime =
        relativeStartTime === undefined ? entry.startTime : Date.now() + relativeStartTime;
      onSave({
        ...entry,
        startTime,
        endTime: startTime + runTime,
      });
    };
    const valid = entry.propertyIds.length > 0;
    return (
      <Flex direction="column" margin="1rem">
        <ButtonGroup direction="row">
          <Button flex={1} onClick={onCancel}>
            {editing ? "Close" : "Cancel"}
          </Button>
          <Button flex={1} onClick={handleSave} isDisabled={!valid}>
            {editing ? "Update" : "Add"}
          </Button>
        </ButtonGroup>
        <PropertySelector
          selected={entry.propertyIds}
          properties={properties}
          onChange={(ids) => setEntry((entry) => ({ ...entry, propertyIds: ids }))}
        />
        <TimeDurationInput
          label="Start in"
          relativeTime={relativeStartTime}
          onChange={setRelativeStartTime}
          absoluteTime={entry.startTime}
        />
        <TimeDurationInput
          label="Run for"
          relativeTime={runTime}
          onChange={setRunTime}
          absoluteTime={entry.endTime}
          relativeTo={relativeStartTime === undefined ? entry.startTime : undefined}
          timeOffset={relativeStartTime}
          min={60000}
        />
        {editing && (
          <Button onClick={onDelete} alignSelf="flex-start" variation="destructive">
            Cancel
          </Button>
        )}
      </Flex>
    );
  }
);
