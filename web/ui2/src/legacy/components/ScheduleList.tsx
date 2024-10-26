import {
  Button,
  Flex,
  Heading,
  Loader,
  Table,
  TableBody,
  TableCell,
  TableRow,
} from "@aws-amplify/ui-react";
import EditIcon from "@mui/icons-material/Edit";
import AddScheduleIcon from "@mui/icons-material/MoreTime";
import { format } from "date-fns";
import { observer } from "mobx-react-lite";
import { ScheduleEntry } from "../irrigation/schedule";

export interface ScheduleProps {
  entries: ScheduleEntry[];
  loading: boolean;
  onClose: () => void;
  onEditEntry: (index: number) => void;
}

export const ScheduleList: React.FC<ScheduleProps> = observer(
  ({ entries, loading, onClose, onEditEntry }) => {
    return (
      <Flex direction="column" margin="1rem">
        <Button onClick={onClose}>Close</Button>
        <Heading level={3}>Scheduled Changes</Heading>
        {loading && <Loader alignSelf="center" />}
        <Table variation="bordered">
          <TableBody>
            {entries.map((entry, index) => (
              <TableRow key={index}>
                <TableCell>
                  Id: {entry.id ?? "none"}
                  <br />
                  {entry.propertyIds.join(", ")}
                  <br />
                  {format(entry.startTime, "yyyy-MM-dd hh:mm:ss")}
                  <br />
                  {format(entry.endTime, "yyyy-MM-dd hh:mm:ss")}
                </TableCell>
                <TableCell>
                  <Button size="small" variation="link" disabled={loading}>
                    <EditIcon onClick={() => onEditEntry(index)} fontSize="small" />
                  </Button>
                </TableCell>
              </TableRow>
            ))}
          </TableBody>
        </Table>
        {!loading && (
          <Button onClick={() => onEditEntry(-1)} gap="0.2em">
            New
            <AddScheduleIcon fontSize="small" />
          </Button>
        )}
      </Flex>
    );
  }
);
