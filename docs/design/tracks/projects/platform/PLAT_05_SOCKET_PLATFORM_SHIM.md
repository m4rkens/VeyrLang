# Socket Platform Shim Probe

**Track:** Cross-platform systems  
**Status:** validation project card  
**Primary milestone pressure:** M32-M34

## Minimal project

A minimal blocking TCP client/server or loopback probe that isolates Windows socket startup/cleanup differences from POSIX-style socket usage.

## Forces in Veyr

- platform modules for networking;
- resource cleanup;
- blocking IO errors;
- handle/socket distinction;
- future HTTP server foundation.

## Must not accidentally force

- async/await;
- TLS stack;
- production HTTP framework;
- global runtime.

## Platform requirement

This project must consider Windows and Linux from the start. macOS behavior should be documented as future/deferred when it cannot yet be implemented.

## Exit criteria

- minimal socket open/bind/connect/close path is represented portably;
- Windows-specific initialization is not leaked into every user program;
- Linux behavior remains simple and explicit.
