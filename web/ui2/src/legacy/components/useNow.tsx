import { useState, useEffect } from "react";

export const useNow = (period = 1000) => {
  const [now, setNow] = useState(Date.now());
  useEffect(() => {
    const interval = setInterval(() => setNow(Date.now()), period);
    return () => clearInterval(interval);
  }, []);
  return now;
};
