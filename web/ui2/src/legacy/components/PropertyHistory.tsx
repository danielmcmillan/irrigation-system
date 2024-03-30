import {
  Button,
  CheckboxField,
  Flex,
  Heading,
  Loader,
  Table,
  TableBody,
  TableCell,
  TableHead,
  TableRow,
} from "@aws-amplify/ui-react";
import {
  Chart,
  Legend,
  LineElement,
  LinearScale,
  PointElement,
  TimeScale,
  Title,
  Tooltip,
} from "chart.js";
import "chartjs-adapter-luxon";
import { observer } from "mobx-react-lite";
import React, { useMemo, useState } from "react";
import { Line } from "react-chartjs-2";
import { IrrigationProperty } from "../irrigation/property";
import { DeviceComponentDefinition, PropertyHistoryItem } from "../irrigation/store";

interface PropertyHistoryProps {
  property: IrrigationProperty;
  component: DeviceComponentDefinition | undefined;
  items: PropertyHistoryItem[] | undefined;
  onClose: () => void;
}

Chart.register(LinearScale, TimeScale, PointElement, LineElement, Title, Tooltip, Legend);

export const PropertyHistory: React.FC<PropertyHistoryProps> = observer(
  ({ property, component, items, onClose }) => {
    const [showChangesOnly, setShowChangesOnly] = useState(true);
    const historyItems = useMemo(
      () =>
        showChangesOnly
          ? (items ?? []).filter((item, i, a) => i >= a.length - 1 || item.value !== a[i + 1].value)
          : items,
      [items, showChangesOnly]
    );
    const [labels, data] = useMemo(() => {
      const source = (items ?? []).toReversed();
      return [
        source.map((h) => h.time * 1000),
        // source.map((h) => new Date(h.time * 1000).toLocaleString()),
        source.map((h) => h.value),
      ];
    }, [items]);
    return (
      <Flex direction="column" margin="1rem">
        <Button onClick={onClose}>Close!</Button>
        <Heading level={3}>
          {component?.typeName} {component?.name} {property.name} History
        </Heading>

        <Line
          style={{ maxHeight: "50vh" }}
          options={{
            responsive: true,

            plugins: {
              legend: {
                display: false,
              },
            },
            scales: {
              x: {
                type: "time",
                max: Date.now(),
                time: {
                  unit: "day",
                },
              },
            },
          }}
          data={{
            labels,
            datasets: [
              {
                data,
                borderColor: "blue",
              },
            ],
          }}
        />

        <Flex direction="column" gap="1rem">
          <CheckboxField
            name="onlyChanges"
            label="Only Changes"
            checked={showChangesOnly}
            onChange={(event) => {
              setShowChangesOnly(event.target.checked);
            }}
          />
          {!items && <Loader />}
          <Table>
            <TableHead>
              <TableRow>
                <TableCell>Time</TableCell>
                <TableCell>Value</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {historyItems?.map((item) => {
                return (
                  <TableRow key={item.time}>
                    <TableCell>{new Date(item.time * 1000).toLocaleString()}</TableCell>
                    <TableCell>
                      {item.value !== undefined ? +item.value.toFixed(2) : null}
                    </TableCell>
                  </TableRow>
                );
              })}
            </TableBody>
          </Table>
        </Flex>
      </Flex>
    );
  }
);
