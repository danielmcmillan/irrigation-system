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
import { observer } from "mobx-react-lite";
import React, { useMemo, useState } from "react";
import { IrrigationProperty } from "../irrigation/property";
import { DeviceComponentDefinition, PropertyHistoryItem } from "../irrigation/store";
import { Line } from "react-chartjs-2";
import {
  CategoryScale,
  Chart,
  Legend,
  LineElement,
  LinearScale,
  PointElement,
  Title,
  Tooltip,
} from "chart.js";

interface PropertyHistoryProps {
  property: IrrigationProperty;
  component: DeviceComponentDefinition | undefined;
  items: PropertyHistoryItem[] | undefined;
  onClose: () => void;
}

Chart.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

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
        source.map((h) => new Date(h.time * 1000).toLocaleString()),
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
                display: false,
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
