import { Badge, Button, Collection, Flex, Loader } from "@aws-amplify/ui-react";
import AddScheduleIcon from "@mui/icons-material/MoreTime";
import { format, formatDuration } from "date-fns";
import { observer } from "mobx-react-lite";
import { ScheduleEntry, ScheduleStatus } from "../irrigation/schedule";
import { IrrigationPropertyWithComponent } from "../irrigation/store";
import { ScheduleStatusAlert } from "./ScheduleStatusAlert";
import { useNow } from "./useNow";

export interface ScheduleProps {
  entries: ScheduleEntry[];
  loading: boolean;
  onClose: () => void;
  onEditEntry: (index: number) => void;
  properties: IrrigationPropertyWithComponent[];
  status: ScheduleStatus | undefined;
}

export const ScheduleList: React.FC<ScheduleProps> = observer(
  ({ entries, loading, onClose, onEditEntry, properties, status }) => {
    const now = useNow();
    const sortedEntries = entries
      .map((entry, index) => ({ ...entry, index }))
      .sort((a, b) => {
        const startA = a.startTime < now ? 0 : a.startTime;
        const startB = b.startTime < now ? 0 : b.startTime;
        if (startA === startB) {
          return b.endTime - a.endTime;
        } else {
          return startB - startA;
        }
      });
    return (
      <Flex direction="column" margin="1em">
        <Button onClick={onClose}>Close</Button>
        <ScheduleStatusAlert
          status={status}
          properties={properties}
          showWhenInactive
          marginLeft="-1em"
          marginRight="-1em"
        />
        {loading && <Loader alignSelf="center" />}
        {!loading && (
          <Button onClick={() => onEditEntry(-1)} gap="0.2em">
            New
            <AddScheduleIcon fontSize="small" />
          </Button>
        )}
        <Collection
          items={sortedEntries}
          searchNoResultsFound="Press New to start a schedule"
          type="list"
          direction="column"
        >
          {(entry) => {
            const stopped = entry.endTime <= now;
            const aborted = status?.aborted === true && !stopped;
            const started = entry.startTime <= now && !stopped && !aborted;
            const pending = !started && !stopped && !aborted;
            const [relativeStartText, relativeEndText] = [entry.startTime, entry.endTime].map(
              (time) => {
                const relativeTime = Math.abs(time - now);
                if (relativeTime < 5000) {
                  return "now";
                }
                const durationText = formatDuration({
                  hours: Math.floor(relativeTime / 3600000),
                  minutes: Math.round((relativeTime % 3600000) / 60000),
                  seconds: relativeTime < 30000 ? Math.round(relativeTime / 1000) : undefined,
                });
                return time < now ? `${durationText} ago` : `in ${durationText}`;
              }
            );
            const runTime = entry.endTime - entry.startTime;
            const runTimeText = formatDuration({
              hours: Math.floor(runTime / 3600000),
              minutes: Math.round((runTime % 3600000) / 60000),
            });
            const [startTimeText, endTimeText] = [entry.startTime, entry.endTime].map((time) => {
              return format(time, "hh:mm:ss a");
            });
            return (
              <Button
                key={entry.index}
                disabled={stopped}
                onClick={stopped ? undefined : () => onEditEntry(entry.index)}
                direction="column"
                alignItems="flex-start"
                gap="0.5em"
              >
                {pending && (
                  <span>
                    Starting {relativeStartText} ({startTimeText})
                  </span>
                )}
                {started && (
                  <Flex alignItems="flex-start">
                    <Badge variation={"success"}>Started</Badge>
                    <span>
                      {relativeStartText} ({startTimeText})
                    </span>
                  </Flex>
                )}
                {aborted && <Badge variation={"error"}>Aborted</Badge>}
                {stopped && (
                  <span>
                    Ended {relativeEndText} ({endTimeText})
                  </span>
                )}
                {entry.propertyIds
                  .map((id) => {
                    const p = properties.find((p) => p.id === id);
                    return p ? `${p.component?.name} ${p.name}` : id;
                  })
                  .join(", ")}
                {pending && (
                  <span>
                    For {runTimeText} ({endTimeText})
                  </span>
                )}
                {(started || aborted) && (
                  <span>
                    Ending {relativeEndText} ({endTimeText})
                  </span>
                )}
              </Button>
            );
          }}
        </Collection>
      </Flex>
    );
  }
);
