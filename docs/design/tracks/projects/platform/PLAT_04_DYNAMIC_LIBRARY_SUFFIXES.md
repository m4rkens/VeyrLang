# Dynamic Library Suffix and Loader Probe

**Track:** Cross-platform systems  
**Status:** validation project card  
**Primary milestone pressure:** M21-M23, M26-M31

## Minimal project

A small dynamic-loading probe that opens a platform-native shared library and resolves one exported symbol through a safe wrapper.

## Forces in Veyr

- `.dll`, `.so`, and future `.dylib` naming;
- native handle wrappers;
- `Result`-based load errors;
- function-pointer representation;
- plugin/GPU loader pressure.

## Must not accidentally force

- stable Veyr language ABI;
- full plugin registry;
- cross-platform ABI pretending all loaders are identical.

## Platform requirement

This project must consider Windows and Linux from the start. macOS behavior should be documented as future/deferred when it cannot yet be implemented.

## Exit criteria

- Windows/Linux dynamic loading works through platform modules;
- macOS `.dylib`/framework implications are documented;
- errors are surfaced clearly.
