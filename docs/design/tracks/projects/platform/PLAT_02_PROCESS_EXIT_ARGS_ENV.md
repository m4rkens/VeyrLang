# Process, Exit Code, Args, and Environment Probe

**Track:** Cross-platform systems  
**Status:** validation project card  
**Primary milestone pressure:** M1, M6-M8, M20

## Minimal project

A minimal executable/tooling probe that reads arguments/environment variables and returns observable exit codes in the same intended way on Windows and Linux.

## Forces in Veyr

- process entry contract pressure;
- argument vector design;
- environment access;
- exit-code semantics;
- `veyr run` behavior;
- test harness process execution.

## Must not accidentally force

- shell scripting language;
- async process management;
- job control/signals before they are actually needed.

## Platform requirement

This project must consider Windows and Linux from the start. macOS behavior should be documented as future/deferred when it cannot yet be implemented.

## Exit criteria

- `veyrc`/`veyr run` can observe exit codes predictably;
- argument/environment behavior is specified as far as needed;
- differences are not hidden as fake portability.
