# GSD Project Description — Veyr Starter Pack

Use this as the starting description for a clean GSD new-project workflow.

---

I am building **Veyr**, a compiled, statically typed, strongly typed programming language for native system and application development.

The project is serious and long-term. The first milestone must be intentionally small. Do not attempt to implement the whole language.

## Fixed names

Use these names consistently:

- language: `Veyr`;
- source extension: `.vyr`;
- primary CLI/build/package tool: `veyr`;
- compiler frontend/driver: `veyrc`;
- internal compiler library: `veyrcore`;
- project manifest: `Veyr.toml`;
- lockfile: `Veyr.lock`;
- local cache: `.veyr/`;
- LSP: `veyr-lsp`;
- formatter: `veyrfmt`;
- analyzer: `veyr-analyze`;
- docs: `veyr-doc`;
- binding generator: `veyr-bindgen`;
- future GPU binding generator: `veyr-gpu-bindgen`.

Do not rename these unless explicitly asked.

## Core mission

Veyr should combine low-level control comparable to C/C++ with modern type-driven safety, explicit ownership, deterministic cleanup, predictable performance, and integrated static analysis.

The central question is:

> How can a programmer retain system-level control while moving as much error detection as practical into the compiler and integrated analyzer?

## Constitutional principles

- ahead-of-time native compilation;
- no mandatory VM;
- no mandatory JIT;
- no mandatory garbage collector;
- no hidden global async runtime;
- static and strong typing;
- immutable bindings by default;
- explicit mutability;
- manual memory management is normal and not automatically unsafe;
- unsafe boundaries for unverifiable low-level operations;
- zero-cost or explicit-cost abstractions;
- diagnostics are part of language design.

## Implementation baseline

Initial compiler implementation:

- C++23;
- CMake + Ninja;
- GoogleTest/GoogleMock;
- LLVM backend discovered through `find_package(LLVM CONFIG)`;
- Conan 2 is the third-party dependency/package manager;
- initial LLVM target is 23.1.2;
- use an existing Conan LLVM recipe when suitable, otherwise a narrow project-owned LLVM recipe;
- a clean first source build of LLVM may be long and is acceptable;
- repeated builds should reuse Conan package/cache state;
- use explicit Windows/Linux Conan profiles rather than one inferred universal environment;
- use Conan-native package IDs/revisions/lockfiles where useful instead of a parallel Veyr dependency graph;
- CMake remains the canonical Veyr project build graph;
- a project-owned Python wrapper may orchestrate setup/doctor/configure/build/test/verify but must stay thin;
- platform-sensitive runtime code should prefer narrow capability/platform adapters instead of scattering OS macros through unrelated compiler code.

Do not design a custom source-acquisition system, package solver, binary cache, LLVM SDK
normalizer, auxiliary-provider graph, or third-party lock format when Conan already provides the
capability.

Development presets must export `compile_commands.json`.

Developer environment/bootstrap direction:

- provide equivalent Windows/Linux setup/doctor/configure/build/test/verify entry points;
- keep future macOS feasibility;
- validate Python, Conan, CMake, Ninja, Clang, Git, and genuine platform SDK/runtime prerequisites;
- keep generated build/package/cache state untracked;
- use tracked, reviewed Conan profiles/configuration and exact dependency versions;
- let dedicated package/source-build workflows take the time needed for LLVM instead of
  reconstructing foreign prebuilt SDK runtime dependencies;
- keep `Veyr.lock` conceptually separate from Conan locks: `Veyr.lock` belongs to the future Veyr
  package ecosystem, not the compiler implementation dependency manager.

See `docs/development/DEVELOPER_BOOTSTRAP.md` and
`docs/tooling/CONAN_BUILD_ARCHITECTURE.md`.

## Cross-platform requirement

Windows and Linux are mandatory Tier 1 platforms for the compiler/tooling direction. macOS is a Tier 2 future platform that must not be blocked by early architecture.

Planning or implementation touching paths, filesystem behavior, process execution, exit codes, dynamic libraries, native linking, terminals, threads, networking, windowing, graphics surfaces, package caches, or generated bindings must include a platform-impact note for Windows, Linux, and future macOS.

Do not introduce mandatory Linux-only or Windows-only workflow scripts. Do not hardcode path separators, library suffixes, executable suffixes, POSIX-only assumptions, Win32-only assumptions, or case-sensitive filesystem behavior without explicit platform gating.

Veyr-owned includes must be angle-bracket canonical paths:

```cpp
#include <veyr/...>
```

Do not use quoted or relative includes for Veyr-owned headers.

## GSD workflow hard rule

Discussion and planning must not execute implementation.

During GSD discussion:

- do not modify code;
- do not create tests;
- do not create CMake targets;
- do not run CMake/Ninja/CTest/builds.

During GSD planning:

- create planning artifacts only;
- do not edit implementation files;
- do not run builds/tests;
- do not create RED tests.

Execution begins only after the project owner explicitly runs:

```text
$gsd-execute-phase N
```

Do not automatically progress between lifecycle stages and do not automatically create lifecycle-stage commits. When tracked artifacts exist, discussion, planning, execution, and verification should each be reviewed and committed separately before progression.

Every lifecycle transition requires explicit project-owner permission. Prefer exact concrete commands from the project owner, such as:

```text
$gsd-plan-phase 3
$gsd-execute-phase 3
$gsd-verify-phase 3
```

Do not use `$gsd-progress --next` or any equivalent automatic phase selection command unless explicitly re-enabled by the project owner.

If approval is ambiguous, ask for the exact command instead of guessing.

CMake/Ninja/CTest/Conan/build/dependency commands are forbidden during discussion and planning. During execution/verification, approximately 60 seconds without output is a soft liveness checkpoint, not an automatic failure: inspect available process/progress signals and continue while the command is credibly active. Stop only on a real error, owner interruption, invariant violation, process conflict, or an explicit hard timeout.

Do not mark cross-platform-sensitive work complete without a Windows/Linux platform-impact note.

## Session handoff hard rule

Codex chat history is not canonical project state.

The repository is the source of truth. Separate Linux and Windows Codex chats are allowed, but every session must reconstruct context from repository files, `.planning/` artifacts when present, and Git history/status.

When starting a new session, switching machines, reinstalling tooling, opening a new worktree, or returning after a long pause, the agent must:

1. read `AGENTS.md` and required development policies, including `docs/development/SESSION_HANDOFF_POLICY.md`;
2. inspect relevant design docs;
3. inspect `.planning/STATE.md` and current phase files if they exist;
4. inspect `git status --short` and recent commits;
5. produce a session rehydration summary;
6. stop and wait for a concrete project-owner command.

The agent must not execute, verify, commit, run CMake/Ninja/CTest, or advance phases after a handoff until explicitly instructed.

If a decision exists only in chat and not in repository files, it is not authoritative. Ask for confirmation or record it in the appropriate doc before relying on it.

## Milestone 1

M1 is a compiler walking skeleton:

```text
single root module
-> fixed smoke entry
-> single integer-literal return
-> native executable
-> observable exit code
```

M1 must include:

- build foundation;
- source identity/spans;
- diagnostics;
- token model and lexer;
- parser smoke;
- AST smoke;
- semantic smoke;
- minimal lowering;
- LLVM backend smoke;
- `veyrc` driver smoke;
- end-to-end test.

M1 must not implement:

- final grammar;
- imports;
- full module graph;
- general functions;
- variables;
- full expressions;
- ownership system;
- standard library;
- package manager;
- graphics, FFI, concurrency, or project-system features.

## Phase 3 special requirement

Before lexer implementation, create:

```text
docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md
```

This document is the canonical M1 profile. It must define the exact provisional spelling before tests/code depend on it. It must state that implementation support does not accept final Veyr syntax.

Captured lexer/token decisions for M1:

- tiny return-code program;
- fixed smoke entry only;
- single integer-literal return only;
- ASCII decimal digit literal token shape;
- no signs/prefixes/separators/suffixes/non-decimal bases;
- whitespace skipped, no trivia tokens;
- no comments in M1;
- token stores only `TokenKind` and `SourceSpan`;
- lexeme recovered from SourceManager;
- explicit EOF token with zero-width end span;
- diagnostics for invalid token-start characters and unsupported numeric spellings;
- report and consume smallest offending run;
- non-ASCII unsupported in M1 without defining final encoding policy;
- lexing returns recovered tokens plus diagnostics.

## Long-term roadmap

Roadmap must be driven by real probes, including:

- CLI calculator;
- grep/wc text tools;
- config parser;
- todo CLI;
- package manifest parser;
- arena allocator;
- resource wrapper suite;
- static/shared library and plugin host;
- C/native API wrapper;
- multithreaded file hasher;
- job system;
- minimal HTTP server;
- native window;
- software framebuffer;
- OpenGL loader;
- Vulkan loader/device/swapchain/triangle;
- future OpenGL/Vulkan/DX12/Metal abstraction layer;
- Sysmon/Splunk/MITRE/Atomic Red Team security tooling;
- AST/CFG/DDG code-analysis graph exporter;
- vulnerability-pattern scanner;
- simracing telemetry/calibration/FFB tools;
- provider-agnostic agent IDE/tooling platform;
- audio/plugin/tooling probes;
- mini language written in Veyr;
- self-hosting subset;
- WASM/embedded/no-std research;
- OS/research kernel probe.

Do not implement these in M1. Use them to shape future milestone planning.

## Expected initial GSD output

Generate a project plan that starts small, respects the phase-boundary rules, and treats design documents as source of truth.

The initial build-foundation planning must explicitly cover:

- Conan 2 as the implementation dependency/package manager;
- Windows and Linux developer entry points for `setup`, `doctor`, `configure`, `build`, `test`, `verify`, and `all`;
- explicit Windows/Linux Conan profiles;
- LLVM 23.1.2 through Conan, using a narrow project-owned recipe only if an existing suitable recipe is unavailable;
- CMake integration through supported Conan 2 generators/toolchain integration;
- cache/reuse strategy for expensive dependency builds;
- a thin Python orchestration wrapper that does not reimplement package-management mechanics;
- GoogleTest/GoogleMock through Conan;
- Windows/Linux Tier 1 verification and future macOS feasibility.

The plan should explicitly include guardrails against:

- accidental final syntax acceptance;
- planning-time code generation;
- build/test execution during planning;
- accidental reimplementation of Conan/package-manager responsibilities in Veyr scripts;
- machine-specific manual dependency setup as the only supported developer path;
- broad feature creep;
- implementing future probes too early.

## Commit policy hard rule

Agents must not create Git commits unless the project owner explicitly approves that specific lifecycle-stage commit.

Do not auto-commit after discussion, planning, execution, verification, or successful tests.

Preferred GSD history is separate reviewed commits for discussion (when it writes tracked artifacts), planning, execution, and verification. Empty stages do not require empty commits. Execution may use multiple coherent task-bundle commits when justified by the approved plan. Avoid microscopic implementation commits and never mix planning/verification artifacts into an implementation commit.

Commit messages must follow:

```text
<area>: <imperative summary>
```

Examples:

```text
docs: define M1 experimental smoke profile
build: establish compiler CMake foundation
lexer: add M1 token model and recovered diagnostics
```

Forbidden examples:

```text
fix
wip
update files
misc
codex changes
phase 3
```

Before committing, inspect `git status --short`, `git diff --stat`, and relevant verification results. Do not stage generated build/caches or accidental implementation artifacts created during planning.

## Sensitive-file pre-commit review hard rule

Before any commit that includes CMake/build-system files, C/C++ source/header files, tests, scripts, automation, future `.vyr` source files, `Veyr.toml`, or `Veyr.lock`, stop and request explicit project-owner review.

This includes at minimum:

- `CMakeLists.txt`, `cmake/**`, `**/*.cmake`, `CMakePresets.json`;
- `include/**`, `src/**`, `tests/**`, `lib/**`, `tools/**`;
- `**/*.cpp`, `**/*.hpp`, `**/*.c`, `**/*.h`, `**/*.cc`, `**/*.hh`, `**/*.cxx`, `**/*.hxx`;
- `scripts/**`, `.github/workflows/**`, `**/*.sh`, `**/*.ps1`, `**/*.py`, `**/*.bat`, `**/*.cmd`;
- `**/*.vyr`, `Veyr.toml`, and `Veyr.lock` when they affect project behavior.

Passing tests or verification does not bypass this. The agent must present a sensitive-file review packet listing changed sensitive files, commands run, verification status, platform impact, and risks, then wait for explicit approval such as:

```text
approve this sensitive-file commit
commit these reviewed implementation changes
```

Ambiguous approval such as `ok`, `continue`, `next`, or `looks good` is not enough.

# Local skills and helper roles

The project includes repository-local Codex skills under `.codex/skills/` and helper role docs under `docs/agents/`.

GSD agents must treat these as mandatory workflow support instructions:

- use `veyr-session-rehydrate` after environment/session/worktree/tooling changes;
- use `veyr-gsd-phase-gate` at phase boundaries;
- use `veyr-cmake-watchdog` before CMake/Ninja/CTest/build/dependency commands;
- use `veyr-sensitive-commit-review` before sensitive-file commits;
- use `veyr-design-probe-planner` when deriving language requirements from projects;
- use `veyr-cross-platform-impact` for platform-sensitive work;
- use `veyr-project-system-steward` for `Veyr.toml`, `Veyr.lock`, target/profile/dependency semantics.

These skills do not grant permission to execute, verify, commit, retry stalled commands, or advance phases. They only structure safe review and handoff.


## Local skill usage guide

Agents must consult `docs/development/SKILL_USAGE_GUIDE.md` when deciding which local skill applies. The guide contains required scenario examples for session rehydration, GSD phase gates, CMake watchdog behavior, sensitive commits, design-probe planning, project-system stewardship, and cross-platform impact review.
