# Requirements: Veyr

**Defined:** 2026-10-03
**Current milestone:** M1 — Compiler Walking Skeleton
**Core Value:** Give programmers system-level control while moving as much error detection as practical into the compiler and integrated analyzer.

## v1 Requirements

Here v1 means the first walking-skeleton milestone, not a stable language release. All requirements are pending hypotheses; no prior implementation/evidence is credited. Every requirement maps to exactly one active phase. P01 forces the compiler path; PLAT_01/02/03/06 force the related platform/build constraints.

### Build and developer foundation

- [ ] **BUILD-01**: Contributor can configure and build the C++23 compiler foundation with target-oriented CMake, Ninja, and the selected Clang toolchain.
- [ ] **BUILD-02**: Contributor can resolve compiler implementation dependencies through reviewed Conan 2 configuration with exact dependency references, rather than Veyr-owned package mechanics.
- [ ] **BUILD-03**: Contributor can consume LLVM 23.1.2 through a suitable Conan recipe, or a narrow project-owned Conan recipe when an exact suitable recipe is unavailable.
- [ ] **BUILD-04**: Contributor can inspect the selected LLVM options and Conan dependency configuration for zlib, zstd, and LibXml2 when required, without enabling unrelated LLVM projects or backends.
- [ ] **BUILD-05**: Contributor can select explicit reviewed Windows x86_64 clang-cl and Linux x86_64 Clang host/build profiles with distinct runtime and ABI settings.
- [ ] **BUILD-06**: Contributor can invoke equivalent setup, doctor, configure, build, test, verify, and all developer operations on Windows and Linux through thin Python orchestration.
- [ ] **BUILD-07**: Contributor receives actionable prerequisite diagnostics for Python, Conan 2, CMake, Ninja, Clang, Git, and genuinely required platform SDK/runtime components.
- [ ] **BUILD-08**: Contributor can discover LLVM through find_package(LLVM CONFIG) using supported Conan CMakeToolchain/CMakeDeps integration, with explicit compatibility-checked managed/system/custom selection and no hidden acquisition in external modes.
- [ ] **BUILD-09**: Contributor can obtain compile_commands.json from each supported development Ninja preset.
- [ ] **BUILD-10**: Contributor can build and run compiler infrastructure tests with GoogleTest/GoogleMock supplied through Conan.
- [ ] **BUILD-11**: Contributor can run an include-style guardrail through a build/check target and CTest entry that rejects quoted or relative Veyr-owned includes.
- [ ] **BUILD-12**: Contributor can reuse compatible Conan packages on repeat workflows and configure/build from prepared dependencies with network acquisition disabled; generated package/build/cache state remains untracked.
- [ ] **BUILD-13**: Contributor can choose documented supported Debug/Release compiler/dependency combinations that preserve platform runtime and ABI compatibility.
- [ ] **BUILD-14**: Contributor can follow a Windows/Linux CI validation path using shared reviewed developer commands and dependency/profile policy, with independently observed developer and CI toolchain identities and platform-specific checks explicitly gated.

### Source management

- [ ] **SRC-01**: Compiler client can register source text under a stable SourceId and retain it for all span and lexeme consumers.
- [ ] **SRC-02**: Compiler client can represent and validate byte-offset SourceSpans, including zero-width end-of-source locations.
- [ ] **SRC-03**: Compiler client can recover original text for a source span through SourceManager without duplicating lexemes into tokens.
- [ ] **SRC-04**: Contributor can display useful source paths and line/column locations under documented Windows/Linux path and newline behavior without freezing final source-encoding policy.

### Diagnostics

- [ ] **DIAG-01**: Compiler client can report structured messages with source identity and relevant primary/related spans independently of LLVM.
- [ ] **DIAG-02**: User can receive useful deterministic plain-text diagnostics on Windows and Linux without requiring color or a specific console.
- [ ] **DIAG-03**: Compiler client can accumulate diagnostics independently of rendering and distinguish errors that prevent successful compilation.

### M1 profile and recovered lexing

- [ ] **LEX-01**: Owner can review the completed docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md defining exact provisional spelling, supported forms, semantic-range boundary, valid/invalid examples, expected diagnostics, and explicit non-final status before dependent implementation.
- [ ] **LEX-02**: Compiler client can represent M1 tokens using only TokenKind and SourceSpan, with lexemes retrieved from SourceManager.
- [ ] **LEX-03**: Lexer can recognize the reviewed profile tokens and one-or-more ASCII decimal digits while rejecting signs, prefixes, separators, suffixes, and non-decimal numeric forms as unsupported M1 spellings.
- [ ] **LEX-04**: Lexer can skip profile-defined whitespace without trivia tokens and diagnose comments/non-ASCII input as unsupported in M1 without defining final encoding semantics.
- [ ] **LEX-05**: Lexer can append an explicit EOF token with a zero-width source-end span, including for empty or erroneous input.
- [ ] **LEX-06**: Compiler client can receive recovered tokens plus source-linked diagnostics after the lexer consumes the smallest clearly related offending run and continues.

### Smoke parser

- [ ] **PARSE-01**: Parser can recognize exactly one profile-defined fixed entry containing one integer-literal return followed by EOF.
- [ ] **PARSE-02**: User can receive source-linked parser diagnostics for malformed, incomplete, trailing, or unsupported input without treating recovery as successful compilation.
- [ ] **PARSE-03**: Compiler client can consume a minimal source-linked parse representation without lexer dependencies on parser, AST, or semantic components.

### Minimal AST

- [ ] **AST-01**: Compiler client can represent the fixed smoke entry and integer-literal return in a minimal AST retaining source relationships.
- [ ] **AST-02**: Compiler client can consume the same minimal AST across parser and semantic boundaries without introducing general functions, statements, expressions, or LLVM types.

### Semantic smoke

- [ ] **SEMA-01**: Semantic validation can accept a well-formed smoke program and a decimal return value within the owner-reviewed M1 semantic range.
- [ ] **SEMA-02**: User can receive source-linked semantic diagnostics for return values outside that range or invalid smoke structure, and invalid programs cannot advance as successful lowered input.

### Minimal lowering

- [ ] **IR-01**: Compiler client can lower a validated smoke AST into an explicit minimal constant-return representation preserving the accepted value.
- [ ] **IR-02**: Backend client can consume a lowering contract retaining source/module association without depending on speculative full-language HIR/MIR or ownership semantics.

### Single root-module boundary

- [ ] **ROOT-01**: Compiler client can associate the single root source with one stable milestone-local module identity through validation, lowering, and backend handoff.
- [ ] **ROOT-02**: User can compile within an explicit one-root boundary that excludes imports, package resolution, full module graphs, and compiled semantic module artifacts.

### LLVM backend smoke

- [ ] **BACK-01**: Backend can construct and verify LLVM IR for the validated constant-return smoke representation using LLVM 23.1.2.
- [ ] **BACK-02**: Backend can emit a native x86_64 object for the selected Windows or Linux target with explicit target/layout configuration.
- [ ] **BACK-03**: User can receive useful backend/output diagnostics when target setup, IR verification, or object emission fails, without leaking LLVM dependencies into source/diagnostics layers.

### veyrc driver smoke

- [ ] **DRV-01**: User can invoke veyrc on one .vyr root and obtain a native executable through the integrated source-to-object and native-link path.
- [ ] **DRV-02**: User can supply input/output paths through documented driver behavior that handles spaces, platform path forms, and native artifact conventions.
- [ ] **DRV-03**: User can receive a failing compiler status and useful diagnostics for unreadable input, lexical/parser/semantic errors, backend failures, or linker failures instead of a successful compilation result.
- [ ] **DRV-04**: Contributor can inspect explicit native tool invocation and error handling with structured process arguments, without shell-dependent required workflows or Veyr.toml/package-manager implementation.

### End-to-end milestone acceptance

- [ ] **E2E-01**: Contributor can compile and run reviewed positive M1 fixtures and observe the expected exit status on Windows x86_64.
- [ ] **E2E-02**: Contributor can compile and run the same intended positive M1 smoke cases and observe the expected exit status on Linux x86_64.
- [ ] **E2E-03**: Contributor can run end-to-end negative cases for malformed/unsupported source and reviewed semantic limits that fail compilation and do not report successful executable output.
- [ ] **E2E-04**: Owner can review fresh M1 evidence covering build/test/include checks, diagnostics and native execution on both Tier 1 platforms, with profile-local test naming and an explicit statement that final language grammar and macOS support are not accepted.

## v2 Requirements

These are deferred probe-driven directions, not the next release contract or authorized implementation. Resolve prerequisites, alternatives, invalid examples/diagnostics and owner design decisions before promoting any item. Paths below are relative to `docs/design/tracks/projects/` unless a topic document is named.

| ID | Deferred capability | Canonical probe pressure | Promotion boundary |
|----|---------------------|--------------------------|--------------------|
| FUT-CLI | CLI argument echo and calculator | CLI_01_ARG_ECHO.md; CLI_02_CALCULATOR.md | Entry arguments, basic output, expressions, control flow, calls; after M1 |
| FUT-TEXT | wc/grep text tools and strings/slices | CLI_03_WC_TOOL.md; CLI_04_GREP_TOOL.md | Files, loops, bytes, Result I/O, reviewed string/encoding semantics |
| FUT-DATA | Config parser, todo persistence, manifest parser | CLI_05_TODO_CLI.md; CLI_06_CONFIG_PARSER.md; CLI_07_PACKAGE_MANIFEST_PARSER.md | Structs, tagged unions, Option/Result, matching, containers |
| FUT-MEM | Arena/linear allocator, resource wrappers, ownership stress | memory/MEM_01–04 cards | Ownership, moves, Drop/defer, borrows, invalidation, unsafe; unresolved semantics need owner review |
| FUT-MOD | Staticlib+exe, platform modules, plugin host | modules/MOD_01–03 cards | Module graph, visibility, targets, exports and a reviewed C ABI boundary |
| FUT-FFI | C wrapper and dynamic library loader | ffi/FFI_01–02 cards | Representation, raw pointers, callbacks, function pointers and symbol lifetimes |
| FUT-GFX | Native window, framebuffer, GL clear, Vulkan loader/device/swapchain/triangle | graphics/GFX_01–08 cards | Prove backend-specific FFI/resources before cross-API abstraction; DX12/Metal later |
| FUT-CONC | File hasher, job system, blocking HTTP | concurrency/CONC_01–02; network/NET_01 | Ownership transfer, threads, synchronization, networking; async remains probe-gated |
| FUT-SEC | Sysmon/Splunk, MITRE, Atomic results, Sigma subset | security/SEC_01–04; CYBERSECURITY_CODE_ANALYSIS.md | Structured logs, rules, reports; no production SIEM promise |
| FUT-GRAPH | Tokenizer, AST exporter, CFG/DDG explorer, vulnerability scanner | code-analysis/CODE_01–04 cards | Graphs, serialization, flow analysis and explainable diagnostics |
| FUT-HW | Telemetry, pedal calibration, FFB profiles, binary codec, HID | simracing/SIM_01–04; SIMRACING_HARDWARE.md | Binary layout/I/O, units, checksums, native handles; firmware deferred |
| FUT-AUDIO | Metadata/presets, buffer processing, plugin host | audio/AUDIO_01–03; AUDIO_APP_TOOLING.md | Application tools, callbacks, real-time allocation/effect pressure |
| FUT-TOOLS | Formatter, LSP, Veyr test runner, C/GPU bindgen | tooling/TOOL_01–03, TOOL_05–06 | Stable compiler API, reviewed grammar and FFI prerequisites |
| FUT-IDE | Provider-agnostic agent IDE/tooling platform | AI_IDE_AGENT_PLATFORM.md | Diagnostics, indexing, LSP, process/workspace/plugin pressure |
| FUT-SELF | Mini-language and compiler subset written in Veyr | self-host/LANG_01–03 cards | Sufficient language subset before self-hosting implementation |
| FUT-PKG | veyr project CLI, Veyr.toml, Veyr.lock and Veyr packages | TOOL_04_PACKAGE_MANAGER.md; VEYR_TOML.md | Language product system only; separate from implementation Conan locks |
| FUT-RESEARCH | WASM, embedded/no-std, research kernel and game/engine slice | DESIGN_PROBES.md P49–P52 | Target/runtime research only after useful native compiler |
| FUT-MAC | macOS feasibility and future bring-up | PLAT_07_MACOS_FEASIBILITY_NOTES.md | Tier 2 profiles, Mach-O/frameworks/terminal/windowing; implementation deferred |

## Out of Scope

| Feature/action | Reason |
|----------------|--------|
| Final grammar acceptance | Exact M1 spellings are provisional; final language decisions require owner review and decision-log record |
| General functions, variables, arithmetic, comments, imports, full module graph | Beyond the one-root fixed-entry integer-return smoke profile |
| Standard library, ownership/borrow analyzer, FFI, concurrency, graphics and future probes in M1 | Prerequisite language/design work is deferred |
| Custom third-party provisioning, solver, source registry, binary cache, SDK normalizer, provider graph or lock format | Conan 2 owns implementation package mechanics |
| Mandatory VM, JIT, GC, hidden async runtime, textual preprocessor | Contrary to accepted language direction |
| Stable native Veyr ABI | Deferred; early stable external boundaries should use reviewed C ABI |
| macOS implementation/CI in M1 | Owner explicitly deferred it; keep future architecture viable |
| Prior-run planning, retained SDK transactions, copied builds, tests or verification credit | Fresh lifecycle requirement |
| Auto-execution, auto-verification, auto-commits and auto-phase progression | Explicit lifecycle/commit authorization remains mandatory |

## Milestone Gates and Definition of Done

- Phase 3 must define, review and obtain an explicitly approved dedicated commit for the M1 Experimental Smoke Profile before dependent lexer/parser/AST/semantic/backend source or tests. Initialization does not create that profile.
- Builds, package commands, compiler invocations, tests and formatters run only during separately authorized execution/verification.
- Stage review and commits remain separate; sensitive implementation commits require the owner review packet.
- M1 acceptance requires new Windows and Linux evidence. Pending remote/manual platform checks remain gaps; design notes alone do not establish parity.
- `Veyr.toml` stays a future product artifact; Conan-native implementation locking never becomes `Veyr.lock`.

## Platform impact

- Windows: explicit clang-cl/SDK/runtime/native-link/profile, paths, diagnostics and process contracts are required.
- Linux: explicit Clang/ABI/native-link/profile, paths, diagnostics and process contracts are required.
- macOS future: no implementation required; preserve platform/target boundaries and record deferred work.

## Traceability

| Requirement | Phase | Status |
|-------------|-------|--------|
| BUILD-01 | Phase 1 | Pending |
| BUILD-02 | Phase 1 | Pending |
| BUILD-03 | Phase 1 | Pending |
| BUILD-04 | Phase 1 | Pending |
| BUILD-05 | Phase 1 | Pending |
| BUILD-06 | Phase 1 | Pending |
| BUILD-07 | Phase 1 | Pending |
| BUILD-08 | Phase 1 | Pending |
| BUILD-09 | Phase 1 | Pending |
| BUILD-10 | Phase 1 | Pending |
| BUILD-11 | Phase 1 | Pending |
| BUILD-12 | Phase 1 | Pending |
| BUILD-13 | Phase 1 | Pending |
| BUILD-14 | Phase 1 | Pending |
| SRC-01 | Phase 2 | Pending |
| SRC-02 | Phase 2 | Pending |
| SRC-03 | Phase 2 | Pending |
| SRC-04 | Phase 2 | Pending |
| DIAG-01 | Phase 2 | Pending |
| DIAG-02 | Phase 2 | Pending |
| DIAG-03 | Phase 2 | Pending |
| LEX-01 | Phase 3 | Pending |
| LEX-02 | Phase 3 | Pending |
| LEX-03 | Phase 3 | Pending |
| LEX-04 | Phase 3 | Pending |
| LEX-05 | Phase 3 | Pending |
| LEX-06 | Phase 3 | Pending |
| PARSE-01 | Phase 4 | Pending |
| PARSE-02 | Phase 4 | Pending |
| PARSE-03 | Phase 4 | Pending |
| AST-01 | Phase 5 | Pending |
| AST-02 | Phase 5 | Pending |
| SEMA-01 | Phase 5 | Pending |
| SEMA-02 | Phase 5 | Pending |
| IR-01 | Phase 6 | Pending |
| IR-02 | Phase 6 | Pending |
| ROOT-01 | Phase 7 | Pending |
| ROOT-02 | Phase 7 | Pending |
| BACK-01 | Phase 8 | Pending |
| BACK-02 | Phase 8 | Pending |
| BACK-03 | Phase 8 | Pending |
| DRV-01 | Phase 9 | Pending |
| DRV-02 | Phase 9 | Pending |
| DRV-03 | Phase 9 | Pending |
| DRV-04 | Phase 9 | Pending |
| E2E-01 | Phase 10 | Pending |
| E2E-02 | Phase 10 | Pending |
| E2E-03 | Phase 10 | Pending |
| E2E-04 | Phase 10 | Pending |

**Coverage:**
- v1 requirements: 49 total
- Mapped to exactly one phase: 49
- Unmapped: 0
- Implemented/verified: 0

---
*Last updated: 2026-10-03 after fresh requirements and roadmap definition.*
