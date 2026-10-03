# Open Design Questions

This document contains questions that are intentionally unresolved.

An unresolved question is not permission for an implementation agent to choose silently. Candidate decisions must be discussed, tested through design probes, and recorded.

# Priority A — must be resolved before the relevant subsystem is implemented

## OQ-001 — Tool names and file extensions

**Status:** open  
**Accepted language name:** Veyr

The language name is final. The remaining naming questions are:

- compiler executable name;
- package/build tool name;
- source extension;
- compiled module artifact extension;
- package manifest filename;
- repository and organization naming conventions.

**Current candidates:** `veyr`, `veyrc`, and `.vyr` may be used in examples, but are not final tooling decisions until separately recorded.

## OQ-002 — Exact ownership/reference syntax

Accepted semantics:

- value;
- immutable borrow;
- mutable borrow;
- explicit move;
- raw pointer;
- unique owner.

Open syntax questions:

- `Ref<T>` versus another spelling;
- `MutRef<T>` versus another spelling;
- `ref value` versus `&value` or another call-site marker;
- `mut ref value` spelling;
- `move value` grammar;
- raw pointer syntax;
- owner type spelling.

**Decision trigger:** syntax sketches from CLI, parser, arena, and job-system probes.

## OQ-003 — OOM policy

Current direction:

- low-level allocator APIs should not pretend allocation cannot fail;
- high-level APIs should not necessarily force `?` after every container growth.

Options to investigate:

1. all allocation is fallible;
2. ordinary allocation panics/aborts on OOM, with `try_` alternatives;
3. allocator-defined policy;
4. a hybrid where low-level allocators are fallible and higher layers choose explicit policies.

**Decision probes:** arena allocator, parser, file indexer, HTTP server, Vulkan renderer.

## OQ-004 — Exact panic runtime strategy

Accepted initial direction: abort.

Open future questions:

- whether unwind should ever be supported;
- whether unwind is target/profile selectable;
- whether defer/Drop run during unwind;
- thread panic transport through join handles;
- FFI boundaries.

**Decision trigger:** only after real need appears.

## OQ-005 — Exact error-conversion model

Accepted direction:

- error conversion is explicit by default;
- `?` must not perform arbitrary hidden conversion.

Open questions:

- `FromError<E>`-like trait;
- automatic conversion only with unique explicit implementation;
- dedicated mapping syntax;
- error-union syntax.

**Decision probes:** config parser, HTTP server, FFI wrappers, compiler diagnostics.

## OQ-006 — String model

Open questions:

- UTF-8 ownership representation;
- string literal type;
- code-point versus byte indexing;
- grapheme handling;
- substring/view representation;
- mutation semantics;
- null termination for FFI;
- small-string optimization policy;
- invalidation rules.

**Decision probes:** grep-like tool, config parser, compiler frontend, HTTP server, C FFI.

## OQ-007 — Standard library boundary

Need to define what belongs in:

- compiler core language;
- minimal core/prelude;
- standard library;
- optional packages.

Open candidates:

- Option and Result;
- allocators;
- String/StringView;
- Vector;
- File/Socket;
- Thread/Mutex/Atomic;
- job system;
- formatting;
- reflection helpers.

**Principle:** the prelude should remain small.

## OQ-008 — Stable representation syntax

Need exact forms for:

- C-compatible struct/enum representation;
- packed layout;
- transparent newtype layout;
- explicit integer representation of fieldless enums;
- alignment controls.

**Decision probes:** C API wrapper, Vulkan, binary file format, plugin ABI.

# Priority B — resolve when the first real design probe demands it

## OQ-009 — Shared<T> semantics

Questions:

- atomic versus non-atomic reference counting;
- weak references;
- cycle handling policy;
- allocator integration;
- thread-safety constraints;
- whether multiple shared ownership types are better than one universal type.

**Principle:** shared ownership must not become the default memory model.

## OQ-010 — Explicit lifetime annotations

Default is inference.

Open questions:

- whether annotations are required at all;
- how to express returned-borrow relationships that inference cannot export;
- how borrowed fields in long-lived structs work;
- how to express self-referential or lending abstractions.

**Decision probes:** parser views, iterators, FFI callbacks, intrusive structures.

## OQ-011 — Pinning / immovable values

Need to determine whether a `Pin<T>`-like mechanism is required.

**Decision probes:** async state machine, intrusive container, self-referential parser buffer, foreign APIs requiring stable addresses.

## OQ-012 — Named and default arguments

Questions:

- are public parameter names part of source API?;
- overload interaction;
- ABI interaction;
- whether defaults are copied into caller metadata or resolved in callee interface;
- source compatibility when defaults change.

**Decision probes:** networking and graphics configuration APIs.

## OQ-013 — Exact method-consumption syntax

Accepted semantic rule: consuming a live named receiver should require visible ownership transfer.

Open syntax:

- `(move value).into_bytes()`;
- `move value.into_bytes()`;
- another form.

## OQ-014 — Dynamic-trait compatibility rules

Need precise rules for when a trait may be used dynamically.

Questions:

- generic methods;
- methods returning Self;
- associated types;
- associated constants;
- receiver forms;
- lifetime/borrow metadata in dispatch tables.

**Decision probes:** renderer, plugin system, heterogeneous collections.

## OQ-015 — Delegation

Candidate: explicit method or trait delegation.

Need to decide:

- syntax;
- conflict resolution;
- visibility;
- whether delegation may expose only selected methods;
- whether trait implementation may delegate wholesale.

**Decision probes:** buffered reader, wrapper types, platform abstraction.

## OQ-016 — Any and runtime type erasure

Questions:

- runtime type identity;
- opt-in metadata;
- downcasting;
- ownership;
- dynamic loading boundaries;
- cross-library identity.

**Decision probes:** plugin host, editor tooling, serialization framework.

## OQ-017 — Runtime reflection

Compile-time introspection is planned and separate.

Need to determine whether runtime reflection is needed at all, and if so:

- opt-in mechanism;
- metadata retention;
- stripping;
- ABI/versioning.

## OQ-018 — Hygienic macros

Only consider after:

- const evaluation;
- typed introspection;
- derive-style generation.

Need real unresolved use cases before adding syntax transformation.

## OQ-019 — Specialization

Prefer traits, overloads, and conditional impls.

Possible future need:

- SIMD algorithms;
- small-vector optimization;
- optimized byte containers;
- serialization fast paths.

Need strict coherence and non-overlap rules if added.

## OQ-020 — Variadic generics

Likely needed for:

- formatting;
- tuples;
- event systems;
- FFI helpers.

Exact syntax and constraints remain open.

# Priority C — long-term architecture questions

## OQ-021 — Async/await

Status: candidate.

Must be justified by real probes.

Questions:

- state-machine lowering;
- borrow model across suspension;
- pinning requirements;
- cancellation;
- executor model;
- FFI I/O integration;
- whether async syntax adds enough value over job systems/event loops.

**Decision probes:** high-concurrency HTTP server, GUI/event loop, asynchronous file/network I/O.

## OQ-022 — Effect contracts

Candidate effects:

- no_alloc;
- no_panic;
- blocks;
- may_allocate;
- pure.

Potential use cases:

- audio callbacks;
- real-time systems;
- embedded code;
- kernels;
- hard performance contracts.

Do not build a full effect system without strong evidence.

## OQ-023 — Module cycles

Initial direction: module DAG.

Potential future exception: type-only import cycles.

Need proof that the benefit outweighs complexity in:

- type checking;
- module artifacts;
- incremental builds;
- tooling.

## OQ-024 — Multi-file modules

Initial direction: one file equals one module.

Potential pressure points:

- generated bindings;
- huge platform modules;
- generated Vulkan/OpenGL declarations.

Prefer hierarchical submodules before adding multi-file identity.

## OQ-025 — Native language ABI stability

Early versions should not freeze it.

Long-term questions:

- stable package binary distribution;
- dynamic trait representation;
- generic ABI;
- layout evolution;
- compiler version compatibility.

Use explicit C ABI for early stable boundaries.

## OQ-026 — Plugin model

Questions:

- C ABI plugin interface;
- native-language plugin interface;
- version negotiation;
- ownership across dynamic-library boundaries;
- allocator ownership;
- panic/exception boundaries;
- runtime type identity.

**Design probe:** plugin host + shared library.

## OQ-027 — Package manager and build language

Questions:

- manifest format;
- lockfile;
- native dependency discovery;
- custom build steps;
- explicit build inputs;
- code generation;
- reproducibility;
- sandboxing.

Avoid creating another full hidden programming language unless necessary.

## OQ-028 — Compiler IR layering

Need to determine practical IR architecture:

- AST;
- typed AST or HIR;
- ownership/lifetime representation;
- control-flow graph;
- MIR-like lowered representation;
- LLVM IR boundary.

Decision should follow implementation experience rather than ideology.

---

# Additional Open Questions

## Project system

- Exact `Veyr.toml` schema after M9.
- Dependency version solver model.
- Registry protocol and publishing rules.
- Workspace inheritance rules.
- Build-script policy and sandboxing.
- Generated-source policy.

## Toolchain

- Exact division between `veyr` and `veyrc` flags.
- Whether `veyrfmt`, `veyr-analyze`, and `veyr-doc` are standalone binaries, subcommands, or both.
- Stable JSON/SARIF diagnostic format.

## Graphics/GPU

- Whether windowing should be in stdlib, official package, or external package.
- Raw Vulkan binding generation strategy.
- Safe wrapper ownership model for dependent GPU resources.
- DX12/Metal abstraction boundaries.
- Shader compilation and resource pipeline integration.

## Cybersecurity/code analysis

- Rule DSL versus config-driven matcher.
- SARIF support priority.
- Analyzer API exposed to user code/tools.
- Graph export schema for AST/CFG/DDG.

## Embedded/WASM/OS research

- No-std model.
- Panic/abort configuration.
- Allocator availability.
- Target feature model.
- Freestanding ABI assumptions.


## Cross-platform and macOS future

- What is the first official macOS bring-up milestone after Windows/Linux parity is stable?
- How should `Veyr.toml` represent macOS frameworks and SDK requirements?
- How should Veyr expose `.dll`, `.so`, `.dylib`, and framework loading without pretending they are identical?
- What is the minimal future Metal binding path, and how does it relate to the Vulkan/OpenGL-first GPU probes?
- Which filesystem/path edge cases must be normalized, preserved, or diagnosed?
- What terminal encoding/color behavior should the diagnostics renderer guarantee on Windows, Linux, and future macOS?
