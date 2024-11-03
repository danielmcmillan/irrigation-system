import { SendMessageCommand, SQSClient } from "@aws-sdk/client-sqs";
import { PropertyState, ScheduleState } from "./store.js";

/** Time after setting a property until it is considered to have failed. */
const SET_TIME_LIMIT = 300000;
/** Time after setting a property during abort until giving up. */
const ABORT_TIME_LIMIT = 300000;
/** Time to wait between evaluations when some property change is pending. */
const EVALUATE_DELAY = 20000;
/** Time after an entry's end time before it will be cleaned up. */
const CLEAR_ENTRY_DELAY = 12 * 3600000;

export interface ScheduleMessageBody {
  deviceId: string;
  messageId: string;
  isRetry?: boolean;
}

export type EvaluateScheduleResult =
  | { abort: true }
  | {
      propertySetActions: Array<{ controllerId: number; propertyId: number; value: boolean }>;
      newPropertyScheduleStates: NonNullable<ScheduleState["state"]>;
      timeToNextEvaluation?: number;
      newEntries?: ScheduleState["entries"];
    };

export class IrrigationScheduleManager {
  private queueUrl: string;
  private sqs: SQSClient;

  constructor(config: { queueUrl: string; region?: string }) {
    this.queueUrl = config.queueUrl;
    this.sqs = new SQSClient({ region: config.region });
  }

  static generateMessageId() {
    return Math.floor(Math.random() * 2 ** 52)
      .toString(16)
      .padStart(13, "0");
  }

  async sendMessage(deviceId: string, messageId: string, delay: number, isRetry?: boolean) {
    const delaySeconds = Math.max(1, Math.ceil(Math.min(900000, delay) / 1000));
    const body: ScheduleMessageBody = { deviceId, messageId };
    if (isRetry !== undefined) {
      body.isRetry = isRetry;
    }
    console.info("Sending schedule message", { ...body, delaySeconds });
    await this.sqs.send(
      new SendMessageCommand({
        QueueUrl: this.queueUrl,
        DelaySeconds: delaySeconds,
        MessageBody: JSON.stringify(body),
      })
    );
  }

  static evaluateSchedule(
    schedule: ScheduleState,
    properties: PropertyState[],
    now: number
  ): EvaluateScheduleResult {
    console.debug("Enter evaluateSchedule", { schedule, properties, now });
    // for each property:
    // - run: should run - property is included in an entry where `start <= now < end`
    // - set: latest message was setProperty(1)
    // - seen: actual value seen after setProperty. Initially it is opposite of `set`.
    //
    // - run && (!set || (!seen && desired&value=0)) -> setProperty(1), set=true, seen=false
    // - run && set && !seen && desired&value=1 -> seen=true

    // - !run && (set || (seen && desired|value=1)) -> setProperty(0), set=false, seen=true
    // - !run && !set && seen && desired|value=0 -> seen=false
    //
    //  If there is no state change (retry), check time since `setTime`. If over threshold, set abort, raise error.

    // TODO: Logging and notifying results
    // TODO: maybe only update seen if some time has past since setTime (avoid evaluating based on stale property state if multiple evaluations happen to run in quick succession)
    //
    // - note: if there are 3 consecutive evaluations like this:
    //    - setProperty(0), set=false, seen=true
    //    - setProperty(1), set=true, seen=false
    //    - seen=true
    //   it is possible desired&value was 1 because the value was outdated after setProperty(0), so it may change after seen is set to true

    // Get schedule state of all relevant properties
    const scheduleStates = new Map<
      number,
      Partial<NonNullable<ScheduleState["state"]>[number] & { run: boolean }>
    >();
    if (!schedule.abort) {
      for (const entry of schedule.entries) {
        for (const num of entry.properties) {
          if (entry.startTime <= now && now < entry.endTime) {
            let property = scheduleStates.get(num);
            if (!property) {
              property = {};
              scheduleStates.set(num, property);
            }
            property.run = true;
          }
        }
      }
    }
    for (const state of schedule.state ?? []) {
      let property = scheduleStates.get(state.id);
      if (!property) {
        property = {};
        scheduleStates.set(state.id, property);
      }
      Object.assign(property, state);
    }

    const result: EvaluateScheduleResult = {
      propertySetActions: [],
      newPropertyScheduleStates: [],
    };

    for (const [id, scheduleState] of scheduleStates) {
      const newScheduleState: NonNullable<ScheduleState["state"]>[number] = {
        id,
        set: scheduleState.set ?? false,
        seen: scheduleState.seen ?? false,
      };
      if (scheduleState.setTime !== undefined) {
        newScheduleState.setTime = scheduleState.setTime;
      }
      const controllerId = id >> 16;
      const propertyId = id & 0xffff;
      const desiredValue =
        properties.find(
          (p) => p.controllerId === controllerId && p.propertyId === propertyId && p.isDesiredValue
        )?.value[0] ?? 0;
      const value =
        properties.find(
          (p) => p.controllerId === controllerId && p.propertyId === propertyId && !p.isDesiredValue
        )?.value[0] ?? 0;
      console.debug("Checking property against schedule state", {
        desiredValue,
        value,
        ...scheduleState,
      });
      if (scheduleState.run) {
        if (!scheduleState.set || (!scheduleState.seen && (desiredValue === 0 || value === 0))) {
          result.propertySetActions.push({
            controllerId,
            propertyId,
            value: true,
          });
          newScheduleState.set = true;
          newScheduleState.seen = false;
        } else if (desiredValue === 1 && value === 1) {
          newScheduleState.seen = true;
        }
      } else {
        if (scheduleState.set || (scheduleState.seen && (desiredValue === 1 || value === 1))) {
          result.propertySetActions.push({
            controllerId,
            propertyId,
            value: false,
          });
          newScheduleState.set = false;
          newScheduleState.seen = true;
        } else if (desiredValue === 0 && value === 0) {
          newScheduleState.seen = false;
        }
      }
      // Only need to keep state for active properties
      if (newScheduleState.set || newScheduleState.seen) {
        if (newScheduleState.set !== (scheduleState.set ?? false)) {
          // New change just applied
          newScheduleState.setTime = now;
        } else if (newScheduleState.set !== newScheduleState.seen) {
          // Previously applied change still pending
          if (
            !schedule.abort &&
            scheduleState.setTime !== undefined &&
            now - scheduleState.setTime > SET_TIME_LIMIT
          ) {
            // Start aborting
            return { abort: true };
          } else if (
            schedule.abort &&
            scheduleState.setTime !== undefined &&
            now - scheduleState.setTime > ABORT_TIME_LIMIT
          ) {
            // Give up aborting this property
            continue;
          }
        }
        result.newPropertyScheduleStates.push(newScheduleState);
      }
    }

    // Work out when to evaluate next
    if (result.newPropertyScheduleStates.some((p) => p.set !== p.seen)) {
      result.timeToNextEvaluation = EVALUATE_DELAY;
    } else {
      let nextTime: number | undefined;
      if (!schedule.abort) {
        nextTime = schedule.entries
          .flatMap((e) => [e.startTime, e.endTime])
          .filter((t) => t > now)
          .reduce<number | undefined>(
            (min, t) => (min === undefined ? t : Math.min(min, t)),
            undefined
          );
      }
      if (nextTime !== undefined) {
        result.timeToNextEvaluation = Math.max(EVALUATE_DELAY, nextTime - now);
      } else if (result.newPropertyScheduleStates.length > 0) {
        console.error(
          "Error: expected empty property schedule state since there are no pending entries or property changes",
          {
            result,
            schedule,
          }
        );
        throw new Error("expected empty property schedule state");
      } else {
        console.info("No more pending changes", { schedule });
      }
    }
    // Remove outdated entries
    if (schedule.entries.some((e) => e.endTime <= now - CLEAR_ENTRY_DELAY)) {
      result.newEntries = schedule.entries.filter((e) => e.endTime > now - CLEAR_ENTRY_DELAY);
    }
    return result;
  }
}
