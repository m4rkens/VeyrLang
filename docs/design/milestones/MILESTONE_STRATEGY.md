# Veyr Milestone Strategy

**Status:** roadmap strategy, not rigid schedule  
**Principle:** features are pulled by validation projects, not pushed by wishlists.

Veyr should grow through small concrete programs that force language, compiler, tooling, standard-library, FFI, and build-system capabilities. A milestone exists because a project makes the missing capability unavoidable.

## 1. Roadmap law

Do not add a major feature unless at least one real probe or milestone project forces it.

Bad milestone:

```text
Implement advanced generics because advanced generics are cool.
```

Good milestone:

```text
Enable config parser and package manifest parser probes; this requires structs, tagged unions, Option/Result, pattern matching, recursive data ownership, and source spans.
```

## 2. Cross-platform law

Windows and Linux are mandatory Tier 1 platforms throughout the roadmap. macOS is a Tier 2 future target.

Every milestone that touches platform-sensitive behavior must include Windows/Linux consideration and a macOS-future note. Platform-sensitive behavior includes paths, filesystem discovery, process execution, exit codes, native linking, dynamic libraries, terminal output, threads, networking, window handles, GPU surfaces, generated bindings, package caches, and CI.

A milestone is not complete if it introduces an avoidable single-platform assumption.

## 3. Milestone status categories

- **Core path:** required to make the compiler useful.
- **Pressure track:** validates a family of features through projects.
- **Research track:** intentionally delayed until enough lower-level capability exists.
- **Non-goal for now:** documented to prevent premature implementation.

## 4. Expanded milestone overview

| Milestone | Name | Track | Primary pressure |
|---|---|---|---|
| M0 | Clean repository foundation | Core | docs, GSD, workflow, commit rules |
| M1 | Compiler walking skeleton | Core | native executable smoke path |
| M2 | Stable source/diagnostic substrate | Core | source spans, diagnostics, renderer quality |
| M3 | M1 token model and lexer | Core | smoke-profile lexing, recovery diagnostics |
| M4 | M1 parser and AST path | Core | fixed entry + integer return AST |
| M5 | M1 semantic and IR path | Core | exit-code semantic check and lowering |
| M6 | `veyrc` driver smoke | Core | compile `.vyr` to executable |
| M7 | Project CLI shell | Tooling | `veyr check/build/run/test` command skeleton |
| M8 | Minimal useful CLI language | CLI | args echo, calculator, return-code tools |
| M9 | Expressions and control flow | CLI | calculator, if/else, boolean checks |
| M10 | Functions and call model | CLI/Core | helper functions, call-site ownership markers later |
| M11 | Text and file processing | CLI/Text | `wc`, `grep`, file reader |
| M12 | Strings, slices, and buffers | Text/Core | text tools, binary protocol codecs |
| M13 | Structured data | Parser/Data | config parser, todo CLI, manifests |
| M14 | Tagged unions and pattern matching | Parser/Data | Option/Result, parser ASTs |
| M15 | Containers and persistence | Parser/Data | todo store, maps/sets, serialized state |
| M16 | Memory/resource discipline | Systems | arena allocator, wrappers, Drop/defer |
| M17 | Unsafe boundary and raw pointers | Systems | allocators, C wrappers, Vulkan raw handles |
| M18 | Module graph and visibility | Core/Modules | multi-file libs/exes, package/public/private |
| M19 | Multi-target build system | Project | executable/staticlib/sharedlib/plugin targets |
| M20 | `Veyr.toml` project system v0 | Project | target graph, profiles, module roots |
| M21 | Native C interop | FFI | `extern C`, `repr(C)`, callbacks, handles |
| M22 | Dynamic library loading | FFI/Plugins | dlopen/LoadLibrary, symbol lookup |
| M23 | Plugin ABI probe | FFI/Modules | versioned plugin host, stable C ABI boundary |
| M24 | Native windowing | Graphics | window, event loop, native handles |
| M25 | Software framebuffer | Graphics | byte buffers, pixel formats, present loop |
| M26 | OpenGL loader probe | Graphics/FFI | context creation, function pointers |
| M27 | Vulkan loader probe | Graphics/FFI | vkGetInstanceProcAddr, VkInstance |
| M28 | Vulkan surface/device probe | Graphics/FFI | native window interop, queues, extensions |
| M29 | Vulkan swapchain clear | Graphics/Systems | command buffers, sync, handle ownership graph |
| M30 | Vulkan triangle | Graphics | buffers, shaders, pipeline structs |
| M31 | Cross-API GPU core v0 | Graphics | shared concepts over OpenGL/Vulkan first |
| M32 | Concurrency foundations | Concurrency | threads, JoinHandle, Mutex, atomics |
| M33 | Job system and parallel IO | Concurrency | file hasher, file indexer, work queues |
| M34 | Network/server probe | Network | blocking HTTP server, sockets, cleanup |
| M35 | Cybersecurity log tooling | Security | Sysmon/Splunk/Atomic logs and rule matching |
| M36 | Rule engine and reporting | Security | Sigma subset, MITRE mapping, diagnostics UX |
| M37 | Source-code analysis tools | Code analysis | tokenizer, AST graph export |
| M38 | CFG/DDG/vulnerability probes | Code analysis | graph data, pattern scanning, export formats |
| M39 | Developer tooling platform | Tooling | formatter, test runner, LSP diagnostics |
| M40 | Binding generators | Tooling/FFI | C bindgen and GPU bindgen pressure |
| M41 | Simracing hardware tools | Hardware | telemetry, calibration, binary protocols |
| M42 | Audio/app tooling probes | Audio/UI | buffers, metadata, plugin preset management |
| M43 | Mini language written in Veyr | Self-host pressure | lexer/parser/AST/interpreter in Veyr |
| M44 | Veyr compiler subset in Veyr | Self-host | source manager, diagnostics, lexer subset |
| M45 | Package manager and lockfile | Project | dependencies, `Veyr.lock`, cache layout |
| M46 | WASM/no-std research | Research | constrained runtime, alternate targets |
| M47 | Embedded/HID research | Research/Hardware | hardware IO and real-time constraints |
| M48 | Research kernel/freestanding probe | Research | no std assumptions, linker/runtime boundaries |
| M49 | Windows/Linux parity hardening | Cross-platform | CI gates, paths, process, dynamic library, terminal parity |
| M50 | macOS feasibility gate | Cross-platform | `.dylib`, frameworks, toolchain, terminal, paths, Metal questions |
| M51 | Platform module abstraction | Cross-platform/Std | `std.os`, `std.fs`, `std.process`, `std.dylib`, `std.net` |
| M52 | Cross-platform project profiles | Project/Cross-platform | `Veyr.toml` platform conditionals, native links, output suffixes |
| M53 | Cross-platform graphics surface strategy | Graphics/Cross-platform | Win32/X11/Wayland/macOS future window/surface split |

The numbers are sequencing hints. Tracks may branch and merge. A later track can define pressure earlier, but implementation should not jump ahead without prerequisites.

## 5. Core milestones in detail

### M0 — Clean repository foundation

Forces:

- starter docs reviewed;
- GSD initialized cleanly;
- no old `.planning` copied;
- commit policy installed;
- workflow guardrails installed.

Exit criteria:

- initial docs commit only;
- no implementation files accidentally generated;
- `AGENTS.md` and GSD description agree on workflow rules.

### M1 — Compiler walking skeleton

Validation project:

```text
single root module -> fixed smoke entry -> integer literal return -> native executable exit code
```

Forces:

- CMake foundation;
- source identity and spans;
- diagnostics;
- token model and lexer;
- parser skeleton;
- AST skeleton;
- minimal semantic check;
- minimal IR/lowering;
- LLVM backend path;
- `veyrc` driver smoke;
- end-to-end executable test.

Non-goals:

- final Veyr syntax;
- general functions;
- imports;
- variables;
- expressions beyond the smoke return;
- ownership system;
- standard library;
- project manager.

### M2-M7 — Compiler infrastructure consolidation

These milestones turn the walking skeleton into a controllable compiler project:

- source and diagnostics quality;
- explicit phase boundaries;
- driver behavior;
- test harness;
- CLI shell;
- build-system integration;
- stable failure modes.

The goal is not language richness yet. The goal is that future work has a reliable substrate.

## Cross-platform gates

These gates apply across the whole roadmap, not only to M49-M53.

### Gate P0 — repository portability

Required from M0:

- no generated Windows/Linux state copied into the starter pack;
- no mandatory single-shell workflow;
- path rules documented;
- cross-platform policy read by agents.

### Gate P1 — compiler smoke portability

Required by M1-M7:

- CMake/Ninja build path documented for Windows and Linux;
- source paths and diagnostics do not assume one separator;
- process exit-code tests are represented portably;
- platform-specific tests are labeled.

### Gate P2 — project-system portability

Required by M18-M20 and M45:

- `Veyr.toml` can represent platform-specific native libraries and output conventions;
- cache/output directories are platform-aware;
- workspaces do not assume POSIX path layout.

### Gate P3 — native interop portability

Required by M21-M23 and M26-M31:

- dynamic library suffixes/loaders are abstracted;
- C ABI and symbol lookup are platform-gated;
- plugin and GPU loader probes do not pretend `.dll`, `.so`, and `.dylib` are the same.

### Gate P4 — windowing/graphics portability

Required by M24-M31 and M53:

- native windowing and GPU surfaces are separate layers;
- Windows/Linux surface paths are real; macOS/Metal remains future but unblocked;
- cross-API GPU core is not designed before backend-specific probes.

### Gate P5 — macOS future bring-up

Required before claiming macOS support:

- LLVM/toolchain discovery checked;
- `.dylib` and frameworks handled;
- terminal/path behavior reviewed;
- windowing and Metal interop questions recorded;
- CI feasibility documented.

## 6. Validation project families

Project cards live under:

```text
docs/design/tracks/projects/
```

Each card states:

- minimal scope;
- what it forces in Veyr;
- what it must not accidentally force;
- exit criteria;
- related milestones.

Use those cards when proposing a feature. If no project card forces the feature, the default answer is: defer.

## 7. Feature pressure matrix

See:

```text
docs/design/milestones/MILESTONE_MATRIX.md
```

for a compact mapping from projects to language/compiler features.
