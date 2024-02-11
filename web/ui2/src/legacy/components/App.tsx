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
import { runInAction } from "mobx";
import { observer } from "mobx-react-lite";
import { useCallback, useMemo, useState } from "react";
import { ReadyState } from "react-use-websocket";
import { LogEntry, LogLevel, getLogLevelString, logLevels } from "../irrigation/log";
import { IrrigationProperty, PropertyType } from "../irrigation/property";
import { DeviceComponentDefinition, IrrigationStore } from "../irrigation/store";
import "./App.css";
import { IniConfigEditor } from "./IniConfigEditor";
import { RemoteUnitTool } from "./RemoteUnitTool";
import { Vacon100Tool } from "./Vacon100Tool";

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
    <Flex direction="row" justifyContent="space-between">
      <SwitchField isDisabled={disabled} label="" isChecked={localValue} onChange={handleChange} />
      {isChanging && <Loader />}
    </Flex>
  );
};

const PropertyControls = observer(({ icu }: { icu: IrrigationStore }) => {
  const { tokens } = useTheme();
  const [filter, setFilter] = useState("all");
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
  const groups = Object.entries(properties);

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
          {groups.flatMap(([group, properties]) => [
            <TableRow key={group} backgroundColor={tokens.colors.background.tertiary}>
              <TableCell as="th" colSpan={3}>
                {group}
              </TableCell>
            </TableRow>,
            ...properties.map((prop) => {
              let value = prop.value;
              if (value !== undefined && prop.type === PropertyType.Boolean) {
                value = Boolean(value);
              }
              const desiredValue =
                prop.desired?.value === undefined ? undefined : Boolean(prop.desired?.value);
              return (
                <TableRow key={prop.id}>
                  <TableCell as="th">
                    <Text>
                      {prop.component?.name} {prop.name}
                    </Text>
                  </TableCell>
                  {!prop.mutable && (
                    <TableCell colSpan={2}>
                      {(() => {
                        let suffix = "";
                        if (
                          typeof value === "number" &&
                          prop.name === "Sensor" &&
                          value >= 0x7ff0 &&
                          value <= 0x7fff
                        ) {
                          value = value & 0x000f;
                          suffix = " (no value)";
                        } else if (prop.unit) {
                          suffix = " " + prop.unit;
                        }
                        let text =
                          value === undefined
                            ? "-"
                            : (typeof value === "number"
                                ? (+value.toFixed(2)).toString()
                                : value.toString()) + suffix;
                        // const relText =
                        //   prop.lastChanged !== undefined
                        //     ? intlFormatDistance(prop.lastChanged * 1000, new Date())
                        //     : undefined;
                        return <Text>{text}</Text>;
                      })()}
                    </TableCell>
                  )}
                  {prop.mutable && (
                    <>
                      <TableCell width="27%">
                        <Flex direction="row" justifyContent="space-between">
                          {value?.toString()}
                          {value !== desiredValue && <Loader />}
                        </Flex>
                      </TableCell>
                      <TableCell width="33%">
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
          ])}
        </TableBody>
      </Table>
    </Flex>
  );
});

const App = observer(({ icu }: { icu: IrrigationStore }) => {
  const { tokens } = useTheme();
  const [tabIndex, setTabIndex] = useState(0);

  // useEffect(() => {
  //   icu.start();
  //   return () => {
  //     icu.stop();
  //   };
  // }, []);

  const [openPage, setOpenPage] = useState<"config" | "vaconTool" | "remoteUnitTool" | null>(null);

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
          Browser: {ReadyState[icu.readyState]}. Controller: {icu.controllerStatus}
        </Alert>
        <PropertyControls icu={icu} />
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
        </ButtonGroup>
      </TabItem>
    </Tabs>
  );
});

export default App;