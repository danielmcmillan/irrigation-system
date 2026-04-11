import { Badge, Button, Collection, Flex, Heading, Loader } from "@aws-amplify/ui-react";
import AddScheduleIcon from "@mui/icons-material/MoreTime";
import { format, formatDuration } from "date-fns";
import { observer } from "mobx-react-lite";
import { ScheduleEntry, ScheduleStatus } from "../irrigation/schedule";
import { IrrigationPropertyWithComponent } from "../irrigation/store";
import { ScheduleStatusAlert } from "./ScheduleStatusAlert";
import { useNow } from "./useNow";

export interface ScheduleProps {
  title?: string | React.ReactNode;
  entries: ScheduleEntry[];
  loading: boolean;
  onClose: () => void;
  onEditEntry: (index: number) => void;
  properties: IrrigationPropertyWithComponent[];
  status: ScheduleStatus | undefined;
  relativeOnly?: boolean;
}

export const ScheduleList: React.FC<ScheduleProps> = observer(
  ({ title, entries, loading, onClose, onEditEntry, properties, status, relativeOnly }) => {
    let now = useNow();
    if (relativeOnly) {
      now = 0;
    }
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
      <Flex direction="column" padding="1em">
        <Flex direction="row" alignItems="center">
          <Button onClick={onClose}>Close</Button>
          <Heading level={4}>{title}</Heading>
        </Flex>
        {status && (
          <ScheduleStatusAlert
            status={status}
            properties={properties}
            showWhenInactive
            marginLeft="-1em"
            marginRight="-1em"
          />
        )}
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
                  return !relativeOnly ? "now" : "immediately";
                }
                const durationText = formatDuration({
                  hours: Math.floor(relativeTime / 3600000),
                  minutes: Math.round((relativeTime % 3600000) / 60000),
                  seconds: relativeTime < 30000 ? Math.round(relativeTime / 1000) : undefined,
                });
                if (relativeOnly) {
                  return `after ${durationText}`;
                }
                return time < now ? `${durationText} ago` : `in ${durationText}`;
              },
            );
            const runTime = entry.endTime - entry.startTime;
            const runTimeText = formatDuration({
              hours: Math.floor(runTime / 3600000),
              minutes: Math.round((runTime % 3600000) / 60000),
            });
            const [startTimeText, endTimeText] = [entry.startTime, entry.endTime].map((time) => {
              return format(time, started || pending ? "hh:mm:ss a" : "hh:mm a");
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
                {relativeOnly && (
                  <>
                    <span>Starts {relativeStartText}</span>
                    <span>For {runTimeText}</span>
                  </>
                )}
                {!relativeOnly && pending && (
                  <span>
                    Starting {relativeStartText} ({startTimeText})
                  </span>
                )}
                {!relativeOnly && started && (
                  <Flex alignItems="flex-start">
                    <Badge variation={"success"}>Started</Badge>
                    <span>
                      {relativeStartText} ({startTimeText})
                    </span>
                  </Flex>
                )}
                {!relativeOnly && aborted && (
                  <Flex alignItems="flex-start">
                    <Badge variation={"error"}>Aborted</Badge>
                    <span>
                      {startTimeText} to {endTimeText}
                    </span>
                  </Flex>
                )}
                {!relativeOnly && pending && (
                  <span>
                    For {runTimeText} ({endTimeText})
                  </span>
                )}
                {!relativeOnly && started && (
                  <span>
                    Ending {relativeEndText} ({endTimeText})
                  </span>
                )}
                {!relativeOnly && stopped && (
                  <span>
                    Ended {relativeEndText} ({startTimeText} to {endTimeText})
                  </span>
                )}
                {entry.propertyIds
                  .map((id) => {
                    const p = properties.find((p) => p.id === id);
                    return p ? `${p.component?.name} ${p.name}` : id;
                  })
                  .join(", ")}
              </Button>
            );
          }}
        </Collection>
      </Flex>
    );
  },
);
