# Design Probes

## 1. Purpose

Design probes are realistic projects used to discover and validate Veyr semantics.

They are not just tutorials and not automatically implementation milestones. A probe exists to pressure the language into answering real questions about:

- syntax;
- types;
- ownership;
- borrowing;
- lifetime analysis;
- error handling;
- FFI;
- modules;
- packages;
- concurrency;
- compile-time behavior;
- diagnostics;
- project/build configuration.

Design order:

```text
real project need
  -> semantic problem
  -> alternatives
  -> chosen semantics
  -> syntax
  -> compiler implementation
```

## 2. Probe discipline

For every probe:

1. write a small target program in pseudo-syntax;
2. list required capabilities;
3. identify unresolved semantics;
4. compare design options when non-obvious;
5. record invalid examples;
6. define expected diagnostics;
7. classify features as core/planned/candidate/deferred/non-goal.

## 3. Core probe set

### P01 — M1 smoke executable

Single root module, fixed entry, integer-literal return, native executable exit code.

Forces compiler walking skeleton only.

### P02 — CLI calculator

Forces expressions, integer operations, basic control flow, function calls, errors.

### P03 — Argument echo / CLI args

Forces entry contract, strings/views, argument representation, exit codes.

### P04 — `wc`-like text tool

Forces files, loops, bytes, strings/views, Result I/O.

### P05 — `grep`-like text tool

Forces string search, slices, large input, error reporting.

### P06 — Config parser

Forces structs, tagged unions, Option/Result, spans, diagnostics.

### P07 — Todo CLI with persistence

Forces containers, serialization, file updates, domain errors.

### P08 — Package manifest parser

Forces `Veyr.toml` design, versioning, target graph, diagnostics.

### P09 — Arena allocator

Forces allocation APIs, raw pointers, ownership/lifetime constraints, unsafe boundary.

### P10 — Resource wrapper suite

Forces Drop, defer, move-only values, file/socket/mutex cleanup.

### P11 — Static library + executable

Forces modules, targets, visibility, reusable artifacts.

### P12 — Shared library/plugin host

Forces ABI boundaries, dynamic loading, handles, symbol lifetimes.

### P13 — C API wrapper

Forces extern C, repr(C), raw pointers, nullable pointers, callbacks.

### P14 — System info tool

Forces platform modules, OS APIs, conditional compilation/module selection.

### P15 — Multithreaded file hasher

Forces threads, ownership transfer, JoinHandle, mutex/atomic decisions.

### P16 — Job system prototype

Forces Send/Sync-like capabilities, move-only closures, scoped jobs.

### P17 — Minimal HTTP server

Forces sockets, resource cleanup, buffers, handlers, concurrency pressure.

## 4. Graphics/GPU probes

### P18 — Native window

Create window, event loop, close.

### P19 — Software framebuffer

Window + CPU-side pixel buffer + present.

### P20 — Dynamic library loading

Load native library, resolve symbol, call function.

### P21 — OpenGL loader clear

Create context, load OpenGL symbols, clear screen.

### P22 — Vulkan loader instance

Load Vulkan, create instance, enumerate devices.

### P23 — Vulkan surface/device

Window surface, physical device selection, logical device.

### P24 — Vulkan swapchain clear

Acquire image, command buffer, clear color, present.

### P25 — Vulkan triangle

Shaders, buffers, pipeline, draw.

### P26 — Cross-API graphics abstraction v0

Common layer over OpenGL/Vulkan first; DX12/Metal future probes.

See `docs/design/tracks/GFX_WINDOWING_GPU.md`.

## 5. Cybersecurity/code-analysis probes

### P27 — Sysmon/Splunk log parser

Large structured log processing, filtering, reporting.

### P28 — MITRE technique rule matcher

Rule DSL/config, matching, explainable diagnostics.

### P29 — Sigma-like subset

Structured rule language and validation.

### P30 — Atomic Red Team result parser

Command output parsing, report aggregation, technique mapping.

### P31 — Source tokenizer in Veyr

Compiler-like text processing with spans and diagnostics.

### P32 — AST extractor / graph exporter

AST/CFG/DDG-like graph export for vulnerability research.

### P33 — Vulnerability-pattern scanner

Security diagnostics, analyzer-like flow, false-positive handling.

See `docs/design/tracks/CYBERSECURITY_CODE_ANALYSIS.md`.

## 6. Simracing/hardware probes

### P34 — Telemetry binary parser

Bytes, endianness, fixed arrays, binary layouts.

### P35 — Pedal calibration CLI

Units, numeric ranges, config validation.

### P36 — Force-feedback profile tool

Curves, structured config, binary export.

### P37 — HID/serial device probe

Native handles, OS APIs, blocking/async pressure.

See `docs/design/tracks/SIMRACING_HARDWARE.md`.

## 7. Developer tooling / IDE probes

### P38 — Diagnostic navigator

Structured diagnostics, JSON/SARIF, source spans.

### P39 — Project indexer

Filesystem, cache, module graph, concurrency.

### P40 — LSP quality probe

Recovered syntax trees, semantic queries, editor diagnostics.

### P41 — Agent workflow runner

Process spawning, project sandbox, provider-agnostic tool adapters.

See `docs/design/tracks/AI_IDE_AGENT_PLATFORM.md`.

## 8. Audio/application probes

### P42 — Sample metadata indexer

Filesystem + binary/text metadata.

### P43 — Audio buffer processor

Slices, tight loops, numeric performance.

### P44 — Plugin host sketch

Dynamic loading, callbacks, ABI, thread-affine resources.

See `docs/design/tracks/AUDIO_APP_TOOLING.md`.

## 9. Language/self-hosting probes

### P45 — Mini expression language

Lexer, parser, AST, interpreter, diagnostics written in Veyr.

### P46 — Veyr lexer in Veyr

Self-hosting subset starts with source/token/diagnostic infrastructure.

### P47 — Veyr source manager in Veyr

Spans, line/column lookup, source storage.

### P48 — Veyr diagnostic renderer in Veyr

Human-readable compiler diagnostics generated by Veyr code.

## 10. Future research probes

### P49 — WASM target smoke

Alternate backend/target constraints.

### P50 — Microcontroller/no-std probe

No heap, explicit runtime, embedded constraints.

### P51 — OS/research kernel probe

Freestanding compilation, no OS assumptions, unsafe boundary pressure.

### P52 — Game/engine slice

Window, input, renderer, asset loading, ECS/data-oriented pressure.

## 11. Probe acceptance rule

A probe may justify a feature only when it identifies:

- the minimal feature needed;
- at least one invalid example;
- expected diagnostics;
- interaction with ownership/errors/modules;
- why a simpler alternative is insufficient.

# Project-card expansion

Detailed validation project cards now live in:

```text
docs/design/tracks/projects/
```

These cards are the preferred way to justify new features. A feature should be pulled by a concrete card, not added because it appears in another language.

Track index:

```text
docs/design/tracks/TRACK_INDEX.md
```

Milestone matrix:

```text
docs/design/milestones/MILESTONE_MATRIX.md
```


## Cross-platform systems track

Mandatory pressure track for Windows/Linux parity and future macOS readiness.

Representative probes:

- platform path and filesystem cases;
- process exit code / args / environment;
- terminal diagnostics portability;
- dynamic library suffix and loader behavior;
- socket platform shim;
- CI platform matrix;
- macOS feasibility notes.

See:

```text
docs/development/CROSS_PLATFORM_POLICY.md
docs/design/tracks/CROSS_PLATFORM_SYSTEMS.md
docs/design/tracks/projects/platform/
```
