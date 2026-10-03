# Cross-Platform Systems Track

**Status:** mandatory pressure track  
**Primary platforms:** Windows + Linux  
**Future platform:** macOS

This track exists because Veyr must not become a language/toolchain that only works cleanly on the developer's current machine. Cross-platform support is not a late cosmetic feature; it affects path handling, process execution, diagnostics, dynamic libraries, linking, modules, build profiles, standard-library design, windowing, graphics APIs, networking, and testing.

## Track law

Windows and Linux are mandatory Tier 1 platforms. macOS is a Tier 2 future bring-up target that must not be structurally blocked.

A language or tooling feature that touches OS behavior must either:

1. define a portable common model;
2. define explicit platform-specific modules; or
3. stay deferred until the platform behavior is understood.

It must not silently assume one OS is the default truth.

## Validation projects

Project cards live in:

```text
docs/design/tracks/projects/platform/
```

Recommended order:

1. `PLAT_01_PATHS_FILESYSTEM_CASES.md`
2. `PLAT_02_PROCESS_EXIT_ARGS_ENV.md`
3. `PLAT_03_TERMINAL_DIAGNOSTICS.md`
4. `PLAT_04_DYNAMIC_LIBRARY_SUFFIXES.md`
5. `PLAT_05_SOCKET_PLATFORM_SHIM.md`
6. `PLAT_06_CI_PLATFORM_MATRIX.md`
7. `PLAT_07_MACOS_FEASIBILITY_NOTES.md`

## Capabilities forced

- structured path representation;
- deterministic source identity across path spellings;
- executable/shared-library suffix handling;
- process spawn/exit-code model;
- environment variable access;
- terminal diagnostics and color policy;
- platform-specific native library linkage;
- dynamic library handle wrappers;
- sockets and thread primitives behind explicit modules;
- CI/platform test gating;
- project manifest platform conditionals.

## What this track must not force too early

- full cross-compilation matrix before the compiler can build useful programs;
- macOS implementation before Windows/Linux parity exists;
- universal GUI abstraction before native window probes;
- cross-API renderer before backend-specific OpenGL/Vulkan probes;
- textual preprocessor-style platform syntax.

## Relationship to graphics

Graphics platforms split into separate layers:

```text
OS window/event loop
-> native window handle
-> API surface/context
-> backend-specific wrapper
-> cross-API GPU core
```

This makes Windows/Linux mandatory early and keeps macOS/Metal future work possible without pretending it is already implemented.
