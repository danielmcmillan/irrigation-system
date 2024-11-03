import { SQSEvent } from "aws-lambda";
import { IrrigationScheduleManager, ScheduleMessageBody } from "./lib/schedule.js";
import { DeviceStateQueryType, IrrigationDataStore, ScheduleState } from "./lib/store.js";
import { IoTDataPlaneClient } from "@aws-sdk/client-iot-data-plane";
import { createSetPropertyCommand } from "./lib/deviceMessage/createSetPropertyCommand.js";

const store = new IrrigationDataStore({
  tableName: process.env.DYNAMODB_TABLE_NAME!,
});
const iotData = new IoTDataPlaneClient({
  endpoint: process.env.IOT_ENDPOINT,
});
const scheduleManager = new IrrigationScheduleManager({
  queueUrl: process.env.SCHEDULE_QUEUE_URL!,
});

export async function handleScheduleMessage(event: SQSEvent): Promise<void> {
  console.log("handleScheduleMessage", ...event.Records);
  const now = Date.now();
  const { deviceId, messageId, isRetry } = JSON.parse(
    event.Records[0].body!
  ) as ScheduleMessageBody;
  const [schedule, { properties }] = await Promise.all([
    store.getSchedule(deviceId),
    store.getDeviceState(DeviceStateQueryType.Properties, deviceId),
  ]);
  if (schedule.messageId === messageId) {
    let aborting = false;
    let result = IrrigationScheduleManager.evaluateSchedule(schedule, properties, now);
    console.debug("Evaluated schedule", result);
    // Re-evaluate for abort
    if (!schedule.abort && "abort" in result && result.abort) {
      aborting = true;
      result = IrrigationScheduleManager.evaluateSchedule(
        { ...schedule, abort: true },
        properties,
        now
      );
      console.debug("Re-evaluated schedule", result);
    }
    if (!("abort" in result)) {
      const newScheduleState: Partial<ScheduleState> = {
        state: result.newPropertyScheduleStates,
      };
      if (aborting) {
        newScheduleState.abort = true;
      }
      if (result.newEntries !== undefined) {
        newScheduleState.entries = result.newEntries;
      }
      // Send message for next evaluation
      // Uses a new messageId, so it can't be processed until schedule is updated
      // Must be completed before updating store, because if store is updated without sending message there will be no more evaluations
      if (result.timeToNextEvaluation !== undefined) {
        newScheduleState.messageId = IrrigationScheduleManager.generateMessageId();
        await scheduleManager.sendMessage(
          deviceId,
          newScheduleState.messageId,
          result.timeToNextEvaluation
        );
      }
      // Update schedule state
      // Must be completed before sending device message to avoid untracked property updates.
      // If messages fail after updating store, the next evaluation would always set the same properties again.
      await store.updateSchedule(deviceId, newScheduleState);
      for (const action of result.propertySetActions) {
        await iotData.send(
          createSetPropertyCommand(
            deviceId,
            action.controllerId,
            action.propertyId,
            action.value ? 1 : 0
          )
        );
      }
    }
  } else {
    const timestamp = +event.Records[0].attributes.SentTimestamp;
    const age = Number.isFinite(timestamp) ? now - timestamp : undefined;
    console.info("aborting due to messageId mismatch", {
      deviceId,
      wanted: schedule.messageId,
      got: messageId,
      age,
      isRetry,
    });
    if (!isRetry && (age === undefined || age < 30000)) {
      // Could be due to unseen update to schedule, so retry is necessary
      scheduleManager.sendMessage(deviceId, messageId, 30000, true);
    }
  }
}
