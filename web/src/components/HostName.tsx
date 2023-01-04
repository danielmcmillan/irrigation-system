import React, { useState } from "react";
import { Button, Dialog, Page, Toolbar, Input } from "react-onsenui";
import "./HostName.css";

interface HostNameProps {
  oldHostName: string;
  onApply: (hostName: string) => unknown;
}

export function HostName(props: HostNameProps) {
  const [newHostName, setNewHostName] = useState(props.oldHostName);

  return (
    <div className="HostName">
      <div>Enter hostname</div>
      <Input
        value={newHostName}
        onChange={(event) => setNewHostName(event.target.value)}
        modifier="underbar"
      />
      <Button onClick={() => props.onApply(newHostName)}>Ok</Button>
    </div>
  );
}
