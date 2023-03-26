import { useCallback, useEffect, useMemo, useState } from "react";
import "./App.css";
import { IrrigationStore } from "../irrigation/store";
import { observer } from "mobx-react-lite";
import {
  Flex,
  Button,
  Collection,
  Card,
  useTheme,
  Badge,
  Text,
  Tabs,
  TabItem,
  CheckboxField,
  Table,
  TableRow,
  TableCell,
  TableBody,
  SwitchField,
  Loader,
  BadgeVariations,
} from "@aws-amplify/ui-react";
import {
  getLogLevelString,
  LogEntry,
  LogLevel,
  logLevels,
} from "../irrigation/log";

async function test(icu: IrrigationStore, type: number) {
  if (type == 0) {
    const configData = Uint8Array.from([
      // Remote unit
      6, 0x04, 0x01, 0x05, 0x01, 0x00,
      // Solenoid
      6, 0x04, 0x02, 0x09, 0x05, 0x00,
    ]);
    await icu.publish("icu-in/irrigation_test/setConfig", configData);
    console.log("Published setConfig");
  } else if (type == 1) {
    await icu.publish(
      "icu-in/irrigation_test/getProperties",
      new TextEncoder().encode(icu.clientId)
    );
    console.log("Published getProperties");
  } else if (type == 2) {
    await icu.publish(
      "icu-in/irrigation_test/getConfig",
      new TextEncoder().encode(icu.clientId)
    );
    console.log("Published getConfig");
  }
}

const LogEntryCard = ({ entry }: { entry: LogEntry }) => {
  const variation = (
    {
      [LogLevel.info]: "info",
      [LogLevel.warn]: "warning",
      [LogLevel.error]: "error",
    } as Record<LogLevel, BadgeVariations>
  )[entry.level];
  const detail = Object.entries(entry.detail)
    .map(([key, value]) => `${key}=${value}`)
    .join(" ");

  return (
    <Card variation="elevated">
      <Flex direction="row" wrap="wrap" alignItems="baseline">
        <Badge variation={variation}>{getLogLevelString(entry.level)}</Badge>
        <Text>{entry.time.toLocaleTimeString()}</Text>
        <Flex direction="column">
          <Text>{entry.summary}</Text>
          <Text>{detail}</Text>
        </Flex>
      </Flex>
    </Card>
  );
};

const LogEntries = observer(({ icu }: { icu: IrrigationStore }) => {
  const showStates = Object.fromEntries(
    logLevels.map((level) => [level, useState(true)])
  );
  const logs = useMemo(
    () => icu.log.filter((entry) => showStates[entry.level][0]).reverse(),
    [icu.log.at(-1)?.id, showStates[0][0], showStates[1][0], showStates[2][0]]
  );

  return (
    <Flex direction="column">
      <div />
      <Flex direction="row" justifyContent="space-evenly">
        {logLevels.map((level) => (
          <CheckboxField
            key={level}
            label={getLogLevelString(level)}
            checked={showStates[level][0]}
            name={level.toString()}
            value="yes"
            onChange={(e) => {
              showStates[level][1](e.target.checked);
            }}
          />
        ))}
        <Button size="small" onClick={() => icu.clearLog()}>
          Clear
        </Button>
      </Flex>
      <Collection type="list" direction="column" items={logs}>
        {(entry) => <LogEntryCard key={entry.id} entry={entry} />}
      </Collection>
    </Flex>
  );
});

const PropertyBooleanControl = ({
  desiredValue,
  onDesiredValueChange,
}: {
  desiredValue: boolean;
  onDesiredValueChange: (value: boolean) => unknown;
}) => {
  const [localValue, setLocalValue] = useState(desiredValue);
  const handleChange: React.ChangeEventHandler<HTMLInputElement> = useCallback(
    (e) => {
      const newValue: boolean = e.target.checked;
      if (desiredValue !== newValue) {
        onDesiredValueChange(newValue);
      }
      // Optimistically update the local value
      setLocalValue(newValue);
    },
    [setLocalValue, desiredValue, onDesiredValueChange]
  );
  const isChanging = desiredValue !== localValue;

  // Reset local switch state when desiredValue changes
  const [prevDesiredValue, setPrevDesiredValue] = useState(desiredValue);
  if (desiredValue !== prevDesiredValue) {
    setPrevDesiredValue(desiredValue);
    if (localValue !== desiredValue) {
      setLocalValue(desiredValue);
    }
  }

  return (
    <Flex direction="row" justifyContent="space-between">
      <SwitchField label="" isChecked={localValue} onChange={handleChange} />
      {isChanging && <Loader />}
    </Flex>
  );
};

const PropertyControls = observer(({ icu }: { icu: IrrigationStore }) => {
  const groups = icu.groupedWriteableProperties;
  const { tokens } = useTheme();

  return (
    <Table variation="bordered">
      <TableBody>
        {groups.flatMap(([group, properties]) => [
          <TableRow
            key={group}
            backgroundColor={tokens.colors.background.tertiary}
          >
            <TableCell as="th" colSpan={3}>
              {group}
            </TableCell>
          </TableRow>,
          ...properties.map((prop) => {
            const value = Array.isArray(prop.value) ? prop.value[0] : false;
            const desiredValue = Array.isArray(prop.desiredValue)
              ? prop.desiredValue[0]
              : false;
            return (
              <TableRow key={`${prop.controllerId}${prop.propertyId}`}>
                <TableCell as="th">
                  {prop.propertyName.split("|").map((name) => (
                    <Text key={name}>
                      {prop.objectName.split("|").slice(1).join(" ")} {name}
                    </Text>
                  ))}
                </TableCell>
                <TableCell width="27%">
                  <Flex direction="row" justifyContent="space-between">
                    {value.toString()}
                    {value !== desiredValue && <Loader />}
                  </Flex>
                </TableCell>
                <TableCell width="33%">
                  <PropertyBooleanControl
                    desiredValue={desiredValue}
                    onDesiredValueChange={(value) =>
                      icu.requestPropertyValueUpdate(
                        prop.controllerId,
                        prop.propertyId,
                        value
                      )
                    }
                  />
                </TableCell>
              </TableRow>
            );
          }),
        ])}
      </TableBody>
    </Table>
  );
});

const PropertyMonitoring = observer(({ icu }: { icu: IrrigationStore }) => {
  const groups = icu.groupedProperties;
  const { tokens } = useTheme();

  return (
    <Table variation="bordered">
      <TableBody>
        {groups.flatMap(([group, properties]) => [
          <TableRow
            key={group}
            backgroundColor={tokens.colors.background.tertiary}
          >
            <TableCell as="th" colSpan={2}>
              {group}
            </TableCell>
          </TableRow>,
          ...properties.map((prop) => (
            <TableRow key={`${prop.controllerId}${prop.propertyId}`}>
              <TableCell as="th">
                {prop.propertyName.split("|").map((name, index) => (
                  <Text key={index}>
                    {prop.objectName.split("|").slice(1).join(" ")} {name}
                  </Text>
                ))}
              </TableCell>
              <TableCell>
                {(Array.isArray(prop.value) ? prop.value : [prop.value]).map(
                  (value, index) => (
                    <Text key={index}>
                      {value.toString()} {prop.format.unit}
                    </Text>
                  )
                )}
              </TableCell>
            </TableRow>
          )),
        ])}
      </TableBody>
    </Table>
  );
});

const App = observer(({ icu }: { icu: IrrigationStore }) => {
  const { tokens } = useTheme();

  useEffect(() => {
    icu.start();
    return () => {
      icu.stop();
    };
  }, []);

  return (
    <Tabs spacing="relative" backgroundColor={tokens.colors.background.primary}>
      <TabItem title="Control">
        <PropertyControls icu={icu} />
      </TabItem>
      <TabItem title="Monitor">
        <PropertyMonitoring icu={icu} />
      </TabItem>
      <TabItem
        title={
          <Flex direction="row" gap="0rem" justifyContent="center">
            Log
            {icu.errorLogCount > 0 && (
              <>
                {" "}
                <Badge size="small" variation="error">
                  {icu.errorLogCount}
                </Badge>
              </>
            )}
          </Flex>
        }
      >
        <LogEntries icu={icu} />
      </TabItem>
      <TabItem title="Config">
        <Flex
          direction="column"
          backgroundColor={tokens.colors.background.primary}
        >
          <p>
            Connection status: {icu.connected ? "connected" : "disconnected"}
          </p>
          <Button onClick={() => test(icu, 2)}>Get Config</Button>
          <Button onClick={() => test(icu, 0)}>Set Config</Button>
          <Button onClick={() => test(icu, 1)}>Get Properties</Button>
        </Flex>
      </TabItem>
    </Tabs>
  );
});

export default App;
