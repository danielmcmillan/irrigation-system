import {
  Alert,
  Badge,
  BadgeVariations,
  Button,
  ButtonGroup,
  Card,
  CheckboxField,
  Collection,
  Flex,
  Loader,
  Radio,
  RadioGroupField,
  SwitchField,
  TabItem,
  Table,
  TableBody,
  TableCell,
  TableRow,
  Tabs,
  Text,
  useTheme,
} from "@aws-amplify/ui-react";
import "@aws-amplify/ui-react/styles.css";
import { intlFormatDistance } from "date-fns";
import { runInAction } from "mobx";
import { observer } from "mobx-react-lite";
import React, { useCallback, useEffect, useMemo, useState } from "react";
import { ReadyState } from "react-use-websocket";
import { LogEntry, LogLevel, getLogLevelString, logLevels } from "../irrigation/log";
import { IrrigationProperty, PropertyType } from "../irrigation/property";
import { DeviceComponentDefinition, IrrigationStore } from "../irrigation/store";
import "./App.css";
import { IniConfigEditor } from "./IniConfigEditor";
import { RemoteUnitTool } from "./RemoteUnitTool";
import { Vacon100Tool } from "./Vacon100Tool";
import { PropertyHistory } from "./PropertyHistory";

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
  const showStates = Object.fromEntries(logLevels.map((level) => [level, useState(true)]));
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
  disabled,
  desiredValue,
  onDesiredValueChange,
}: {
  disabled?: boolean;
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
    <Flex direction="row" justifyContent="flex-start" position="relative" gap="2px">
      <SwitchField isDisabled={disabled} label="" isChecked={localValue} onChange={handleChange} />
      {isChanging && <Loader />}
    </Flex>
  );
};

const RelativeTimeText = ({
  time,
  ...props
}: { time: Date | undefined } & React.DetailedHTMLProps<
  React.HTMLAttributes<HTMLElement>,
  HTMLElement
>) => {
  const [text, setText] = useState("never");
  let timeout: number | undefined;
  useEffect(() => {
    const update = () => {
      if (!time) {
        return;
      }
      const now = new Date();
      const diff = now.valueOf() - time.valueOf();
      setText(diff < 60000 ? "now" : intlFormatDistance(time, now, { style: "narrow" }));
      setTimeout(update, 60000);
    };
    update();
    return () => {
      clearTimeout(timeout);
    };
  }, [time]);
  return <em {...props}>{text}</em>;
};

const PropertyControls = observer(
  ({ icu, openHistory }: { icu: IrrigationStore; openHistory: (propertyId: string) => void }) => {
    const { tokens } = useTheme();
    const [filter, setFilter] = useState("all");

    const groups = (() => {
      const properties: Record<
        string,
        (IrrigationProperty & { component?: DeviceComponentDefinition })[]
      > = {};
      for (const prop of icu.properties) {
        const component = icu.components.find((c) => c.id === prop.componentId);
        const type = prop.mutable ? "control" : "monitor";
        if (filter === "all" || filter === type) {
          const group = component?.typeName ?? "Properties";
          properties[group] ??= [];
          properties[group].push({ ...prop, component });
        }
      }
      const groups = Object.entries(properties).sort((a, b) => {
        const components = ["Node", "Sensor", "Zone", "Pump"];
        return components.indexOf(b[0]) - components.indexOf(a[0]);
      });
      return groups;
    })();

    return (
      <Flex direction="column">
        <div></div>
        <Flex direction="row" justifyContent="space-evenly">
          <RadioGroupField
            label=""
            name="filter"
            direction="row"
            value={filter}
            onChange={(e) => setFilter(e.target.value)}
          >
            <Radio value="all">All</Radio>
            <Radio value="control">Control</Radio>
            <Radio value="monitor">Monitor</Radio>
          </RadioGroupField>
        </Flex>
        <Table variation="bordered">
          <TableBody>
            {groups.flatMap(
              ([group, properties]): React.ReactNode => [
                <TableRow key={group} backgroundColor={tokens.colors.background.tertiary}>
                  <TableCell as="th" colSpan={3}>
                    {group}
                  </TableCell>
                </TableRow>,
                ...properties.map((prop): React.ReactNode => {
                  let value = prop.value;
                  if (value !== undefined && prop.type === PropertyType.Boolean) {
                    value = Boolean(value);
                  }
                  const desiredValue =
                    prop.desired?.value === undefined ? undefined : Boolean(prop.desired?.value);
                  const lastUpdated = prop.lastUpdated
                    ? new Date(prop.lastUpdated * 1000)
                    : undefined;
                  const lastChanged = prop.lastChanged
                    ? new Date(prop.lastChanged * 1000)
                    : undefined;
                  const relTooltip = `Changed at ${lastChanged?.toLocaleString()}. Updated at ${lastUpdated?.toLocaleString()}`;
                  const suffix = prop.unit ? ` ${prop.unit}` : "";
                  const valueText =
                    (value === undefined
                      ? "-"
                      : typeof value === "number"
                      ? (+value.toFixed(2)).toString()
                      : value.toString()) +
                    suffix +
                    " ";
                  return (
                    <TableRow key={prop.id}>
                      <TableCell as="th" overflow="hidden">
                        <Text whiteSpace="nowrap">
                          {prop.component?.name} {prop.name}
                        </Text>
                      </TableCell>
                      {!prop.mutable && (
                        <TableCell colSpan={2}>
                          <Text>
                            {valueText}
                            <RelativeTimeText
                              onClick={() => openHistory(prop.id)}
                              time={lastChanged}
                              title={relTooltip}
                              style={{ color: "gray", fontSize: "0.7em" }}
                            />
                          </Text>
                        </TableCell>
                      )}
                      {prop.mutable && (
                        <>
                          <TableCell
                            width="40%"
                            position="relative"
                            whiteSpace="nowrap"
                            overflow="hidden"
                            paddingRight="0"
                          >
                            <Flex direction="row" justifyContent="flex-start" gap="2px">
                              <Text whiteSpace="nowrap">
                                {valueText}
                                <RelativeTimeText
                                  onClick={() => openHistory(prop.id)}
                                  time={lastChanged}
                                  title={relTooltip}
                                  style={{ color: "gray", fontSize: "0.7em" }}
                                />
                              </Text>
                              {value !== desiredValue && <Loader />}
                            </Flex>
                          </TableCell>
                          <TableCell width="20%" overflow="hidden" paddingLeft="0" paddingRight="0">
                            <PropertyBooleanControl
                              disabled={!icu.ready}
                              desiredValue={desiredValue ?? false}
                              onDesiredValueChange={(value) =>
                                icu.requestSetProperty(prop.id, value ? 1 : 0)
                              }
                            />
                          </TableCell>
                        </>
                      )}
                    </TableRow>
                  );
                }),
              ]
            )}
          </TableBody>
        </Table>
      </Flex>
    );
  }
);

const App = observer(
  ({
    icu,
    reconnect,
    logout,
  }: {
    icu: IrrigationStore;
    reconnect: () => void;
    logout: () => void;
  }) => {
    const { tokens } = useTheme();
    const [tabIndex, setTabIndex] = useState(0);

    const [openPage, setOpenPage] = useState<
      "config" | "vaconTool" | "remoteUnitTool" | "history" | null
    >(null);
    const onClose = useCallback(() => setOpenPage(null), [setOpenPage]);
    const openPropertyHistory = useCallback(
      (propertyId: string) => {
        icu.requestPropertyHistory(propertyId);
        setOpenPage("history");
      },
      [setOpenPage]
    );

    if (openPage === "config") {
      return (
        <IniConfigEditor
          configIni={icu.configIni}
          loading={!icu.configLoaded}
          onUpdate={(configIni) =>
            runInAction(() => {
              icu.configIni = configIni;
            })
          }
          onCancel={() => setOpenPage(null)}
          onSave={() => {
            icu.requestSetConfig();
            setOpenPage(null);
          }}
        />
      );
    } else if (openPage === "vaconTool") {
      return (
        <Vacon100Tool
          onRunRequest={(address, value) => {
            const command =
              value === undefined
                ? new Uint8Array([1, address & 0xff, address >> 8])
                : new Uint8Array([2, address & 0xff, address >> 8, value & 0xff, value >> 8]);
            icu.requestControllerCommand(2, command.buffer);
          }}
          onClose={() => setOpenPage(null)}
          result={
            icu.controllerCommandResults.filter((result) => result.controllerId === 2).at(-1)?.data
          }
        />
      );
    } else if (openPage === "remoteUnitTool") {
      return (
        <RemoteUnitTool
          onRunRequest={(commandData) => {
            icu.requestControllerCommand(4, commandData);
          }}
          onClose={() => setOpenPage(null)}
          results={icu.controllerCommandResults.filter((result) => result.controllerId === 4)}
        />
      );
    } else if (openPage === "history" && icu.propertyHistory) {
      const property = icu.properties.find((p) => p.id === icu.propertyHistory?.propertyId);
      const component = icu.components.find((c) => c.id === property?.componentId);
      return (
        property && (
          <PropertyHistory
            property={property}
            component={component}
            onClose={onClose}
            items={icu.propertyHistory.items}
          />
        )
      );
    }

    return (
      <Tabs
        spacing="relative"
        backgroundColor={tokens.colors.background.primary}
        currentIndex={tabIndex}
        onChange={(index) => setTabIndex(Number(index))}
      >
        <TabItem title="Properties">
          <Alert variation={icu.ready ? "info" : "error"}>
            Browser: {ReadyState[icu.readyState]}.
            {icu.readyState === ReadyState.OPEN && (
              <> Controller: {icu.controllerConnected ? icu.controllerStatus : "Disconnected"}.</>
            )}
            {!icu.connectEnabled && <Button onClick={reconnect}>Reconnect</Button>}
          </Alert>
          <PropertyControls icu={icu} openHistory={openPropertyHistory} />
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
        <TabItem title="System">
          <ButtonGroup direction="column" margin="1rem">
            <Button
              onClick={() => {
                icu.requestConfig();
                setOpenPage("config");
              }}
            >
              Configure devices
            </Button>
            <Button
              onClick={() => {
                setOpenPage("vaconTool");
              }}
            >
              Vacon100 Tool
            </Button>
            <Button
              onClick={() => {
                setOpenPage("remoteUnitTool");
              }}
            >
              Remote Unit Tool
            </Button>
            <Button
              onClick={() => {
                const newDeviceId = prompt("Enter a new device id", icu.controlDeviceId);
                if (newDeviceId) {
                  localStorage.setItem("DEVICE_ID", newDeviceId);
                  location.reload();
                }
              }}
            >
              Device: {icu.controlDeviceId}
            </Button>
            <Button onClick={logout}>Logout</Button>
          </ButtonGroup>
        </TabItem>
      </Tabs>
    );
  }
);

export default App;
