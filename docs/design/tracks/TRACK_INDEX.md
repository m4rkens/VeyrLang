# Veyr Track Index

**Status:** roadmap navigation.

Veyr uses tracks to keep future project pressure visible without letting future features invade early milestones.

## Track families

- `cli`: small command-line tools that force basic language usability.
- `memory`: ownership, allocation, Drop/defer, unsafe boundary.
- `modules`: libraries, targets, packages, plugins.
- `ffi`: C/native ABI and dynamic loading.
- `platform`: Windows/Linux portability, future macOS readiness, OS behavior probes.
- `graphics`: windowing, OpenGL, Vulkan, and cross-API GPU layer.
- `concurrency`: threads, atomics, jobs, parallel IO.
- `network`: sockets and server pressure.
- `security`: Sysmon/Splunk/MITRE/Atomic/Sigma-like tooling.
- `code-analysis`: tokenizers, AST/CFG/DDG, vulnerability pattern scanning.
- `simracing`: telemetry, calibration, binary protocols, hardware-tooling pressure.
- `tooling`: formatter, LSP, test runner, package manager, bindgen.
- `audio`: app/audio/plugin/metadata tooling pressure.
- `self-host`: mini language and Veyr compiler subset in Veyr.

Project cards:

```text
docs/design/tracks/projects/
```

## Rule

A future feature proposal should cite at least one project card or create a new card before implementation planning.
