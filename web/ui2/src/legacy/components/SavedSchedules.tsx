import {
  Button,
  Card,
  Collection,
  Flex,
  Heading,
  Loader,
  SelectField,
} from "@aws-amplify/ui-react";
import CreateIcon from "@mui/icons-material/Add";
import EditIcon from "@mui/icons-material/Edit";
import StartIcon from "@mui/icons-material/ScheduleSend";
import { observer } from "mobx-react-lite";
import { useState } from "react";
import { SavedSchedule } from "../irrigation/schedule";

export interface SavedSchedulesProps {
  savedSchedules: SavedSchedule[];
  loading: boolean;
  onClose: () => void;
  onEdit: (index: number) => void;
  onNew: () => void;
  onStart: (index: number, after?: { time?: number; next?: boolean }) => void;
  isScheduleActive?: boolean;
}

export const SavedScheduleRow: React.FC<{
  schedule: SavedSchedule;
  onStart: (after?: { time?: number; next?: boolean }) => void;
  onEdit: () => void;
  isScheduleActive?: boolean;
}> = observer(({ schedule, onStart, onEdit, isScheduleActive }) => {
  const [timeAfter, setTimeAfter] = useState<number>(0);
  const empty = schedule.entries.length === 0;
  return (
    <Card variation="elevated">
      <Flex direction="row" wrap="wrap" justifyContent="flex-end">
        <span style={{ flexGrow: 1 }}>{schedule.name}</span>
        <Flex direction="row" alignItems="center" justifyContent="flex-end">
          {!empty && (
            <>
              <SelectField
                value={`${timeAfter}`}
                onChange={(e) => setTimeAfter(+e.target.value)}
                label="Start in"
                labelHidden
                shrink={0}
              >
                <option value={0}>Now</option>
                <option value={-1}>Next</option>
                <option value={60000}>1 minute</option>
                <option value={300000}>5 minutes</option>
                <option value={900000}>15 minutes</option>
                <option value={3600000}>1 hour</option>
              </SelectField>
              <Button
                variation="primary"
                onClick={() => onStart(timeAfter === -1 ? { next: true } : { time: timeAfter })}
                shrink={0}
              >
                <StartIcon />
              </Button>
            </>
          )}
          <Button variation="menu" onClick={() => onEdit()}>
            <EditIcon />
          </Button>
        </Flex>
      </Flex>
    </Card>
  );
});

export const SavedSchedules: React.FC<SavedSchedulesProps> = observer(
  ({ savedSchedules, loading, onClose, onEdit, onNew, onStart, isScheduleActive }) => {
    return (
      <Flex direction="column" padding="1em">
        <Flex direction="row" alignItems="center">
          <Button onClick={onClose}>Close</Button>
          <Heading level={4}>Saved schedules</Heading>
        </Flex>
        {loading && <Loader alignSelf="center" />}
        <Collection
          items={savedSchedules}
          searchNoResultsFound="Press New to create a saved schedule"
          type="list"
          direction="column"
        >
          {(schedule, index) => {
            return (
              <SavedScheduleRow
                key={index}
                schedule={schedule}
                isScheduleActive={isScheduleActive}
                onEdit={() => onEdit(index)}
                onStart={(after) => onStart(index, after)}
              />
            );
          }}
        </Collection>
        {!loading && (
          <Button onClick={onNew} gap="0.2em">
            Create
            <CreateIcon fontSize="small" />
          </Button>
        )}
      </Flex>
    );
  },
);
