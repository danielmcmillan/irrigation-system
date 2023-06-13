import os, sys
Import("env")

if env["PIOENV"] == "control-processor":
  path = "src/control-processor/settings.h"

if path is not None and not os.path.exists(path):
  print(f"ERROR: {path} does not exist", file=sys.stderr)
  sys.exit(1)
