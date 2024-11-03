import { Alert, AlertVariations, BaseStyleProps } from "@aws-amplify/ui-react";
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
  let variation: AlertVariations = "info";
  if (
    !showWhenInactive &&
    (!status ||
      (!status.active &&
        !(status.aborted && status.lastEventTime && now - status.lastEventTime > 86400000) &&
        !status.nextEventTime &&
        status.pending.length === 0))
  ) {
    return null;
  } else if (status?.aborted) {
    summary = "Schedule aborted due to failure";
    variation = "error";
  } else if (status?.nextEventTime && !status.active) {
    summary = `Schedule starting ${nextEventTime}`;
  } else if (status?.active && lastEventTime) {
    summary = `Schedule active, ${lastEventTime} remaining`;
    variation = "success";
  } else if (status?.pending && status.pending.length > 0) {
    summary = "Schedule stopping";
  } else {
    summary = "Schedule inactive";
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
  if (pendingList.length > 0 && variation !== "error") {
    variation = "warning";
  }

  return (
    <Alert variation={variation} {...alertProps}>
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
