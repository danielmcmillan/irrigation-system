import { Alert, BaseStyleProps } from "@aws-amplify/ui-react";
import { formatDistanceStrict } from "date-fns";
import { observer } from "mobx-react-lite";
import { ScheduleStatus } from "../irrigation/schedule";
import { IrrigationPropertyWithComponent } from "../irrigation/store";
import { useNow } from "./useNow";

export const ScheduleStatusAlert: React.FC<
  {
    status: ScheduleStatus | undefined;
    properties: IrrigationPropertyWithComponent[];
    showWhenInactive?: boolean;
  } & BaseStyleProps
> = observer(({ status, properties, showWhenInactive, ...alertProps }) => {
  const now = useNow(1000);
  const nextEventTime = status?.nextEventTime
    ? formatDistanceStrict(status.nextEventTime, now, { addSuffix: true })
    : undefined;
  const lastEventTime = status?.lastEventTime
    ? formatDistanceStrict(status.lastEventTime, now, { addSuffix: false })
    : undefined;

  let summary: string;
  if (
    !status ||
    (!status.active && !status.aborted && !status.nextEventTime && status.pending.length === 0)
  ) {
    if (showWhenInactive) {
      summary = "Schedule inactive";
    } else {
      return null;
    }
  } else if (status.aborted) {
    summary = "Schedule aborted due to failure";
  } else if (status.nextEventTime && !status.active) {
    summary = `Schedule starting ${nextEventTime}`;
  } else if (status.active && lastEventTime) {
    summary = `Schedule active, ${lastEventTime} remaining`;
  } else {
    summary = "Schedule stopping";
  }
  const pendingList = (status?.pending ?? [])
    .filter((pendingChange) => !pendingChange.since || now - pendingChange.since > 30000)
    .map((pendingChange) => {
      const property = properties.find((property) => property.id === pendingChange.propertyId);
      const name = property
        ? `${property.component?.name} ${property.name}`
        : pendingChange.propertyId;
      const duration = pendingChange.since
        ? formatDistanceStrict(now, pendingChange.since)
        : undefined;
      return (
        <span>
          {duration} for {name} to turn {pendingChange.value ? "on" : "off"}
        </span>
      );
    });

  return (
    <Alert
      variation={status?.aborted ? "error" : pendingList.length > 0 ? "warning" : "success"}
      {...alertProps}
    >
      {summary}
      {pendingList.length > 0 && (
        <div>
          Waiting for
          <ul style={{ margin: 0 }}>
            {pendingList.map((pending, index) => (
              <li key={index}>{pending}</li>
            ))}
          </ul>
        </div>
      )}
    </Alert>
  );
});
