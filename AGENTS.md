# AGENTS.md — Veyr Agent Rules

This repository contains the design and implementation of **Veyr**.

The language name is **Veyr**. The compiler is `veyrc`. The primary project CLI is `veyr`. Source files use `.vyr`. Project manifests are named `Veyr.toml`.

Do not rename these without an explicit project-owner decision.

# 1. Required reading before work

Before planning or implementation, read the relevant files:

1. `docs/design/DECISION_LOG.md`
2. `docs/design/VISION.md`
3. `docs/design/FEATURE_CATALOG.md`
4. `docs/design/MEMORY_MODEL.md`
5. `docs/design/TYPE_SYSTEM.md`
6. `docs/design/MODULE_SYSTEM.md`
7. `docs/design/DESIGN_PROBES.md`
8. `docs/design/milestones/MILESTONE_STRATEGY.md`
9. `docs/development/GSD_WORKFLOW.md`
10. `docs/development/BUILD_POLICY.md`
11. `docs/development/DEVELOPER_BOOTSTRAP.md`
12. `docs/development/INCLUDE_POLICY.md`
13. `docs/development/CROSS_PLATFORM_POLICY.md`
14. `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md`
15. `docs/development/SESSION_HANDOFF_POLICY.md`
16. `docs/development/COMMIT_POLICY.md`
17. `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`
18. `docs/development/GIT_WORKFLOW.md`
19. `docs/project-system/VEYR_TOML.md`
20. `docs/tooling/TOOLCHAIN.md`
21. `docs/development/LOCAL_CODEX_SKILLS.md`
22. `docs/development/SKILL_USAGE_GUIDE.md`
23. `docs/development/AGENT_ROLE_PROTOCOL.md`
24. `docs/agents/AGENT_ROLES.md`
25. `docs/design/OPEN_QUESTIONS.md`

When documents conflict, explicit project-owner decisions and `DECISION_LOG.md` win.

# 2. Non-negotiable GSD workflow rules

## 2.1 Discussion means discussion

During GSD discussion:

- do not edit implementation files;
- do not create tests;
- do not create CMake targets;
- do not run CMake, Ninja, tests, formatters, or verification;
- only capture requirements, constraints, alternatives, risks, and decisions.

## 2.2 Planning means planning

During GSD planning:

- do not create or modify `include/`, `src/`, `tests/`, `cmake/`, `CMakeLists.txt`, or generated implementation files;
- do not run CMake/Ninja/CTest/build verification;
- planning artifacts may be created only in `.planning/` or explicitly requested planning documents;
- implementation tasks may be described, but not executed.

Execution begins only when the project owner explicitly starts `$gsd-execute-phase N`.

## 2.3 No automatic phase hopping

Do not automatically proceed between GSD lifecycle stages and do not automatically create lifecycle-stage commits. Reviewed discussion, planning, execution, and verification artifacts should be committed separately when tracked changes exist.

Every lifecycle transition requires explicit project-owner permission. Finishing one command never grants permission to run the next command.

Avoid `$gsd-progress --next` and any equivalent automatic progress command until the project owner explicitly re-enables it.

## 2.4 Exact command gate

For GSD phase transitions, ask for or wait for the concrete command. The agent may propose the next command, but must not run it unless the project owner explicitly asks for that command or that exact step.

Ambiguous approval such as `continue`, `next`, `go on`, or `looks good` is not enough when the action would create a lifecycle-stage commit or cross into the next GSD stage/phase. Ask for a concrete command.

## 2.5 Long-running command supervision

CMake, Ninja, CTest, compiler invocations, Conan operations, dependency builds, and other
long-running tool commands may run only during permitted execution/verification steps.

Silence alone is not failure. After roughly 60 seconds without new output, treat the threshold as
a **soft liveness checkpoint**: inspect process state, child processes, CPU/I/O movement, and
relevant build-artifact changes where available. Continue waiting while there is credible
progress.

Stop only on a real error, owner interruption, invariant violation, process conflict, or an
explicit hard timeout established for that command. Do not launch duplicate retries or destructive
cleanup automatically.

See `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md`.

## 2.6 Session handoff and multi-machine context

The repository, not Codex chat history, is the canonical project memory.

Separate Linux, Windows, future macOS, IDE, or web Codex chats are allowed, but they must all reconstruct context from the repository. A new session must not assume hidden state from another chat or another machine.

Before continuing work after switching machines, reinstalling tools, opening a new worktree, or starting a fresh Codex session, the agent must:

- read `docs/development/SESSION_HANDOFF_POLICY.md`;
- inspect the required canonical docs and `.planning/` artifacts if present;
- inspect `git status --short` and recent commits;
- produce a session rehydration summary;
- stop and wait for an explicit project-owner command.

If a decision exists only in chat, do not rely on it silently. Confirm it or write it into the appropriate repository document before using it.

See `docs/development/SESSION_HANDOFF_POLICY.md`.

## 2.7 Sensitive-file pre-commit review gate

Before any commit that includes CMake files, build-system files, C/C++ source/header files, tests, scripts, automation, future `.vyr` source files, or project manifest/build behavior changes, stop and request explicit project-owner review.

Sensitive files include at minimum:

- `CMakeLists.txt`, `cmake/**`, `**/*.cmake`, `CMakePresets.json`;
- `include/**`, `src/**`, `tests/**`, `lib/**`, `tools/**`;
- `**/*.c`, `**/*.cc`, `**/*.cpp`, `**/*.cxx`;
- `**/*.h`, `**/*.hh`, `**/*.hpp`, `**/*.hxx`, `**/*.ipp`, `**/*.inc`;
- C++ module candidates such as `**/*.ixx`, `**/*.cppm`, `**/*.mpp`;
- `scripts/**`, `.github/workflows/**`, `**/*.sh`, `**/*.ps1`, `**/*.py`, `**/*.bat`, `**/*.cmd`;
- future Veyr source/config artifacts such as `**/*.vyr`, `Veyr.toml`, and `Veyr.lock` when they affect build/project behavior.

Passing tests or verification does not bypass this gate. The agent must present a sensitive-file review packet and wait for explicit approval such as:

```text
approve this sensitive-file commit
```

Ambiguous approval such as `ok`, `continue`, `next`, or `looks good` is not enough.

See `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`.

# 2.8 Commit discipline

Do not create Git commits unless the project owner explicitly asks for a commit.

Do not commit *inside* an active discussion/planning/execution/verification command. After each stage stops and is reviewed, propose its dedicated stage commit when tracked artifacts exist. Never auto-commit; every stage commit needs explicit owner approval.

Commit rules are defined in:

```text
docs/development/COMMIT_POLICY.md
docs/development/GIT_WORKFLOW.md
```

Default expectation:

```text
reviewed discussion/plan/execute/verify artifacts -> separate explicit stage commits
```

Avoid microscopic commits and vague commit messages. Never use messages such as `fix`, `wip`, `update files`, `codex changes`, `phase 3`, or `misc`.

Before committing, inspect status/diff and summarize staged changes.


## 2.9 Local skills and helper agent roles

Repository-local Codex skills live under:

```text
.codex/skills/
```

The required policy is documented in:

```text
docs/development/LOCAL_CODEX_SKILLS.md
docs/development/SKILL_USAGE_GUIDE.md
docs/development/AGENT_ROLE_PROTOCOL.md
docs/agents/AGENT_ROLES.md
```

Use `docs/development/SKILL_USAGE_GUIDE.md` for concrete examples of when each skill applies. Use the local skills when their trigger applies:

- `veyr-session-rehydrate` after environment/session/worktree/tooling changes;
- `veyr-gsd-phase-gate` at every lifecycle boundary;
- `veyr-cmake-watchdog` before CMake/Ninja/CTest/build commands;
- `veyr-developer-bootstrap` for Conan/toolchain setup, profiles, cache-aware dependency workflows, and developer wrappers;
- `veyr-sensitive-commit-review` before sensitive-file commits;
- `veyr-design-probe-planner` when mapping projects to language features;
- `veyr-cross-platform-impact` for platform-sensitive work;
- `veyr-project-system-steward` for `Veyr.toml`, `Veyr.lock`, targets, profiles, and dependency semantics.

A local skill may inspect, summarize, and propose. It must not silently execute phase transitions, run verification, retry stalled commands, commit, or advance phases.

When a skill is used automatically, state it explicitly:

```text
Applying local skill: veyr-session-rehydrate
Reason: fresh Windows session after Linux work.
```

# 3. Language-design discipline

## 3.1 Do not invent core semantics silently

A coding agent must not silently decide final:

- syntax;
- ownership semantics;
- borrow/lifetime behavior;
- ABI guarantees;
- async model;
- OOM policy;
- runtime reflection model;
- macro system;
- lifetime-annotation syntax;
- package semantics;
- module graph semantics;
- project-manifest semantics beyond the documented starter direction.

If a task reaches an unresolved question, stop, record the uncertainty, and ask for review.

## 3.2 Semantics before syntax

For every new language feature, use this order:

```text
real project pressure
-> design probe
-> semantic problem
-> alternatives
-> selected semantics
-> interactions
-> syntax proposal
-> valid examples
-> invalid examples
-> expected diagnostics
-> lowering/IR implications
-> tests
-> decision record
```

Do not add syntax just because another language has it.

## 3.3 Candidate examples are not final grammar

Illustrative examples in docs are not final syntax unless the decision log explicitly accepts them. Tests and implementation must not convert provisional examples into final language design by accident.

# 4. Milestone 1 hard guardrails

Milestone 1 is a compiler walking skeleton, not the language.

Before any lexer, parser, AST, semantic, or backend work that depends on concrete syntax, Phase 3 must create:

```text
docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md
```

This document must define the temporary Milestone 1 smoke subset and state that it is milestone-local and non-final.

The first executable is a single-root-module, fixed-entry, integer-return smoke program. It is not evidence of final function syntax, module syntax, integer literal syntax, or return-statement syntax.

# 5. Cross-platform mandate

Veyr must keep **Windows and Linux** as mandatory Tier 1 platforms from the beginning. **macOS** is a Tier 2 future platform that must not be structurally blocked by early decisions.

Any work touching paths, filesystem behavior, process execution, exit codes, dynamic libraries, native linking, terminals, threads, networking, windowing, graphics surfaces, package caches, or generated bindings must include a platform-impact note:

```text
Platform impact:
- Windows: ...
- Linux: ...
- macOS future: ...
```

Do not introduce mandatory Linux-only or Windows-only scripts. Do not hardcode `/`, `.so`, `.dll`, executable suffixes, drive-less paths, POSIX-only behavior, Win32-only behavior, or case-sensitive filesystem assumptions without explicit platform gating.

See `docs/development/CROSS_PLATFORM_POLICY.md`.

# 6. Implementation baseline rules

- Compiler implementation: C++23.
- Backend: LLVM discovered through `find_package(LLVM CONFIG)` using managed/system/custom provider modes.
- Build: CMake + Ninja.
- Tests: GoogleTest / GoogleMock.
- Conan 2 is the implementation dependency/package manager.
- LLVM 23.1.2 is initially consumed through Conan; a narrow project-owned recipe is allowed when needed.
- A long first dependency/LLVM source build is acceptable; repeated work should reuse Conan's cache.
- Do not reimplement dependency acquisition, solving, package storage, or SDK normalization in Veyr scripts.
- Managed developer setup follows `docs/development/DEVELOPER_BOOTSTRAP.md` and `docs/tooling/CONAN_BUILD_ARCHITECTURE.md`.
- Development presets must export `compile_commands.json`.
- Veyr-owned C++ includes must use angle brackets: `#include <veyr/...>`.
- Do not use quoted includes for Veyr-owned headers.
- Do not use relative include paths such as `../`.

# 7. Architectural boundaries

Early compiler components should remain separated:

```text
source management
-> diagnostics
-> token model / lexer
-> parser
-> AST
-> semantic analysis
-> IR/lowering
-> LLVM backend
-> driver/tooling
```

Do not make lexer depend on parser/AST/semantics. Do not make diagnostics depend on LLVM. Do not make core source/span logic depend on the build driver.

# 8. Tooling names

Use these names consistently:

- `veyr`: primary CLI for build/run/test/fmt/doc/package/check/init/clean.
- `veyrc`: compiler frontend/driver.
- `veyrcore`: internal compiler library.
- `veyr-lsp`: language server.
- `veyrfmt`: formatter.
- `veyr-analyze`: analyzer frontend/standalone analyzer.
- `veyr-doc`: documentation generator.
- `veyr-bindgen`: C/native binding generator.
- `veyr-gpu-bindgen`: future GPU/Vulkan/OpenGL/DX/Metal binding tool.
- `.vyr`: source extension.
- `Veyr.toml`: project manifest.
- `Veyr.lock`: dependency lockfile.

# 9. Design-probe discipline

Features should be pulled by real projects, not pushed by wishlists.

Important probe tracks include:

- CLI and text processing;
- config/parser/todo tools;
- memory/resource allocators;
- modules/libraries/plugins;
- C/native interop;
- cross-platform Windows/Linux systems behavior, with future macOS awareness;
- windowing and GPU APIs;
- cybersecurity/log/SAST tooling;
- AST/CFG/DDG/code-analysis tooling;
- simracing/hardware/binary-protocol tooling;
- agent IDE/tooling platform;
- mini-language and self-hosting subset.

When a feature is requested, identify which probe forces it.

# 10. Do not carry over broken state

This starter pack must not import old `.planning/`, Windows-generated build directories, accidental Phase 3 lexer tests, or implementation files created during a planning phase. Start clean.
