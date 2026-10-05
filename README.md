# Veyr Project Starter Pack

This repository contains the design and initial compiler build foundation for
**Veyr**, a native compiled systems/application programming language.

It began from a clean cross-platform starter. The fresh M1 lifecycle now tracks
reviewed `.planning/` artifacts and authorized infrastructure implementation;
no prior-run implementation or evidence was imported. Windows and Linux remain
Tier 1; available native evidence currently covers the Omarchy/Arch Linux host.
Windows developer and independent Windows/Ubuntu CI evidence remain pending.

## Developer quick start

Use the reviewed base Python from the repository root. On the current Omarchy
host this is `/usr/bin/python3`; exact tool/runtime policy is in
`scripts/devtools/baseline.json`.

```text
python scripts/dev.py setup --tools-only
python scripts/dev.py doctor --json
python scripts/dev.py setup --config Debug
python scripts/dev.py build --config Debug
python scripts/dev.py test --config Debug
python scripts/dev.py verify --config Debug
```

Setup is explicit and Conan may download/source-build missing dependencies.
Build may configure and test may build from existing cache only. For the composed
Release workflow use `python scripts/dev.py all --config Release` once; it includes
setup/configure/build/test/verify. `python scripts/dev.py clean --config Debug`
removes only that selected output, preserving Release output, project-owned
Conan 2.33.0 under `.veyr-dev/tools/conan-2.33.0` and the dedicated package cache.

Linux selects `linux-x86_64-clang`; Windows selects `windows-x86_64-clangcl`.
Explicit `--profile`, `--environment`, `--config`, `--conan-home`,
`--llvm-provider managed|system|custom` and external `--llvm-dir` inputs are
documented in [DEVELOPER_BOOTSTRAP.md](docs/development/DEVELOPER_BOOTSTRAP.md).
Missing/unreviewed Windows or CI prerequisites fail; Linux checks do not certify
those environments. LLVM 23.1.2 and GoogleTest/GoogleMock remain Conan-owned;
future language packages and `Veyr.lock` are separate product features.

The local [CI workflow draft](.github/workflows/build-foundation.yml) covers
Ubuntu 24.04 and Windows 2022 × Debug/Release. It is unpublished/unrun in this
execution and blocks at independent CI identity review before package setup.
See [CI policy](docs/development/CI_MATRIX_POLICY.md) and the
[acceptance ledger](docs/development/FOUNDATION_ACCEPTANCE.md) for pending
runner/runtime/lock/cold-capacity evidence. No Phase 1 completion is claimed.

## What Veyr is

Veyr is a compiled, statically typed, strongly typed, native programming language for system-level and application development.

The long-term goal is to combine:

- low-level control comparable to C/C++;
- explicit ownership of memory and resources;
- deterministic cleanup;
- zero-cost or explicit-cost abstractions;
- native OS/C ecosystem interoperability;
- strong diagnostics;
- integrated static analysis for memory, lifetime, resource, and API misuse problems;
- a coherent toolchain and project/build system.

Veyr is not a cosmetic C++ layer, a smaller Rust clone, or a syntax-first experiment. Features must be justified by real programs and design probes.

## Key policies included

This pack folds in the lessons from the first GSD/Codex run:

- hard rules preventing discussion/planning phases from creating code or running builds;
- Conan 2 dependency policy: third-party C++ dependencies and LLVM are package-managed through Conan, with a thin Python orchestration layer;
- include-style policy for Veyr-owned C++ code;
- compile database and developer workflow requirements;
- Milestone 1 smoke-profile discipline before lexer work;
- token/lexer decisions from the Phase 3 discussion;
- tool names and project-manifest direction;
- a much larger milestone strategy driven by real projects;
- dedicated graphics/windowing/GPU, cybersecurity/code-analysis, simracing/hardware, agent-IDE, and application tracks;
- mandatory cross-platform policy: Windows/Linux Tier 1 from the start, macOS Tier 2 future;
- mandatory pre-commit owner review gate for CMake/build files, C/C++ source, tests, scripts, `.vyr` files, and project manifest/build behavior changes.

## Source-of-truth order

When documents conflict, use this order:

1. explicit project-owner decisions made after this pack;
2. `docs/design/DECISION_LOG.md`;
3. dedicated topic documents in `docs/design/`, `docs/tooling/`, `docs/project-system/`, and `docs/development/`;
4. `docs/design/VISION.md`;
5. `docs/design/FEATURE_CATALOG.md`;
6. `docs/design/OPEN_QUESTIONS.md`;
7. GSD `.planning/` artifacts after they are generated and reviewed;
8. implementation details and comments.

Candidate syntax examples are illustrative unless a decision explicitly says otherwise.

## Tooling names established in this pack

Initial Veyr toolchain names:

- `veyr` — primary project/build/package CLI;
- `veyrc` — compiler frontend/driver;
- `veyrcore` — internal compiler library name;
- `veyr-lsp` — language server;
- `veyrfmt` — formatter;
- `veyr-analyze` — static analyzer frontend/standalone analyzer mode;
- `veyr-doc` — documentation generator;
- `veyr-bindgen` — C/native binding generator;
- `veyr-gpu-bindgen` — future GPU API binding generator/probe tool;
- `veyr-test` — test runner interface, likely invoked through `veyr test`;
- `veyr-pkg` — package registry/subcommand interface, likely integrated into `veyr`.

Initial source/config names:

- source extension: `.vyr`;
- package/project manifest: `Veyr.toml`;
- lockfile: `Veyr.lock`;
- local project cache: `.veyr/`.

These are accepted as the starter toolchain direction and should be used consistently unless the project owner changes them explicitly.

## Important files

- `Veyr.toml` — starter manifest sketch for the future Veyr project/build system.
- `AGENTS.md` — non-negotiable rules for Codex/GSD/coding agents.
- `docs/development/GSD_WORKFLOW.md` — exact GSD workflow guardrails.
- `docs/development/BUILD_POLICY.md` — CMake/Conan/LLVM/build rules.
- `docs/development/DEVELOPER_BOOTSTRAP.md` — Conan/toolchain setup, explicit platform profiles, thin Python orchestration, cache-aware dependency builds, and cross-platform developer scripts.
- `docs/development/PLATFORM_TRIPLES.md` — canonical host/target/cache identities.
- `docs/development/CI_MATRIX_POLICY.md` — Tier 1 bootstrap/build CI matrix contract.
- `templates/development/` — starter manifest and machine-local environment-lock shapes.
- `docs/development/STARTER_PACK_MIGRATION.md` — summary of the managed-SDK and stage-commit policy reset from the previous starter.
- `docs/development/CROSS_PLATFORM_POLICY.md` — mandatory Windows/Linux portability and future macOS policy.
- `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md` — mandatory review before committing CMake/source/script-sensitive changes.
- `docs/development/INCLUDE_POLICY.md` — include and header discipline.
- `docs/design/milestones/MILESTONE_STRATEGY.md` — long-term roadmap.
- `docs/design/milestones/MILESTONE_1.md` — walking-skeleton milestone.
- `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE_TEMPLATE.md` — required Phase 3 smoke-profile document template.
- `docs/tooling/TOOLCHAIN.md` — Veyr toolchain names and responsibilities.
- `docs/tooling/CONAN_BUILD_ARCHITECTURE.md` — compiler implementation dependency/build ownership: Python wrapper → Conan 2 → CMake → Ninja → Clang.
- `docs/project-system/VEYR_TOML.md` — project/build manifest design.
- `docs/project-system/Veyr.full.example.toml` — maximal commented manifest reference example; documentation only, not an implementation checklist.
- `docs/design/DESIGN_PROBES.md` — validation projects that force features.
- `gsd/GSD_PROJECT_DESCRIPTION.md` — ready-to-feed GSD project description.

## Clean-start workflow

Recommended clean restart:

```text
1. Unpack this starter pack into a fresh repository.
2. Do not copy old `.planning/`, `build/`, `include/`, `src/`, `tests/`, or Windows-generated CMake state.
3. Review the docs and make any final owner edits.
4. Commit the design foundation.
5. Initialize GSD from `gsd/GSD_PROJECT_DESCRIPTION.md`.
6. Review generated planning before executing anything.
7. Run each phase manually and preserve reviewed lifecycle boundaries as separate commits: discussion -> planning -> execution -> verification.
```

Avoid automatic progress commands until the workflow is trusted.

## Initial implementation baseline

- Compiler implementation language: C++23.
- Backend direction: LLVM.
- Build system for the compiler implementation: CMake + Ninja.
- Mandatory platform posture: Windows + Linux Tier 1; macOS future Tier 2.
- Test framework: GoogleTest / GoogleMock.
- Dependencies: Conan 2 with explicit reviewed Windows/Linux profiles and exact version/recipe choices.
- LLVM: initial target 23.1.2 through Conan; use a narrow project-owned recipe if a suitable exact recipe is unavailable.
- Developer setup: thin Python wrapper around Conan/CMake/Ninja/CTest; long first dependency builds are acceptable and subsequent runs should reuse compatible cache state.
- License for the current phase: Apache-2.0 direction.

## Workflow and roadmap hardening

This starter pack includes workflow and roadmap hardening:

- `docs/development/COMMIT_POLICY.md` — explicit restrictions on when agents may commit and what commit messages may look like.
- `docs/development/GIT_WORKFLOW.md` — phase-aware Git workflow.
- `docs/design/milestones/MILESTONE_MATRIX.md` — feature pressure matrix.
- `docs/design/tracks/TRACK_INDEX.md` — track navigation.
- `docs/design/tracks/projects/` — many small validation project cards across CLI, memory, modules, FFI, graphics, security, code analysis, simracing, tooling, audio, and self-hosting.

The goal is to make future Veyr features emerge from small projects that prove the need for those features.


## Explicit step and command gates

- Absolute explicit-step gate: the agent must never automatically move between GSD stages or create their stage commits. Reviewed discussion/planning/execution/verification artifacts are committed separately when tracked changes exist.
- Exact command requirement for GSD phase transitions.
- `$gsd-progress --next` and equivalent automatic progress commands forbidden by default.
- ~60-second soft liveness checkpoint for quiet CMake/Ninja/CTest/Conan/build commands.
- Quiet commands continue while process/progress signals remain credible; stop only on a real hard-stop condition.
- New required policy: `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md`.

## Cross-platform discipline

This pack includes cross-platform discipline as a non-negotiable rule:

- Windows and Linux are mandatory Tier 1 platforms from the beginning.
- macOS is a Tier 2 future platform and must not be structurally blocked.
- Platform-impact notes are required for phases touching paths, filesystem behavior, processes, exit codes, terminals, dynamic loading, native linking, threads, networking, windowing, graphics surfaces, package caches, or generated bindings.
- New cross-platform policy document: `docs/development/CROSS_PLATFORM_POLICY.md`.
- New cross-platform track: `docs/design/tracks/CROSS_PLATFORM_SYSTEMS.md`.
- New platform project cards under `docs/design/tracks/projects/platform/`.
- Milestone strategy now includes Windows/Linux parity gates and future macOS bring-up milestones.

## Session handoff and multi-environment context

- New required policy: `docs/development/SESSION_HANDOFF_POLICY.md`.
- Codex chat history is explicitly non-canonical; repository files, `.planning/` artifacts, and Git history/status are the source of truth.
- Separate Linux and Windows Codex histories are allowed, but every new session must rehydrate context from the repository.
- After switching machines, reinstalling tools, opening a new worktree, or starting a fresh session, the agent must produce a session rehydration summary and wait for an explicit next command.
- Decisions that exist only in chat must be confirmed or written into the appropriate project document before the agent relies on them.
- End-of-session handoff packets are required before moving work between Linux, Windows, or future macOS environments.

## Local Codex skills and helper roles

This pack includes repository-local Codex skills and helper agent roles for safer multi-environment work.

New local skills live under:

```text
.codex/skills/
```

Important skills:

- `veyr-session-rehydrate` — read repository state after switching Linux/Windows/macOS/session/worktree/tool install;
- `veyr-gsd-phase-gate` — prevent automatic discussion/planning/execution/verification/commit/next-phase transitions;
- `veyr-cmake-watchdog` — supervise quiet long-running CMake/Ninja/CTest/Conan/build commands without treating silence as failure;
- `veyr-developer-bootstrap` — guard Conan/toolchain/LLVM setup, profiles/cache, and Windows/Linux developer wrappers;
- `veyr-sensitive-commit-review` — require owner review before commits touching CMake/source/scripts/tests/manifest behavior;
- `veyr-design-probe-planner` — keep language changes project-pressure-driven;
- `veyr-cross-platform-impact` — preserve Windows/Linux Tier 1 and future macOS feasibility;
- `veyr-project-system-steward` — guard `Veyr.toml`, `Veyr.lock`, targets, profiles, dependency semantics.

New helper role docs live under:

```text
docs/agents/
```

The key rule remains:

```text
Repository is canonical.
Chat history is not canonical.
Skills and roles may inspect/summarize/propose, but they do not bypass owner review gates.
```


## Skill usage guide

This pack includes a practical skill usage guide:

```text
docs/development/SKILL_USAGE_GUIDE.md
```

This document explains when to use each local Codex skill and includes scenario examples for:

- new Linux/Windows sessions and environment switching;
- GSD discussion/planning/execution/verification/commit boundaries;
- CMake/Ninja/CTest/Conan/build commands and long-running liveness supervision;
- sensitive commits touching CMake/source/tests/scripts/`.vyr`/manifest behavior;
- feature planning driven by project probes;
- `Veyr.toml`/project-system changes;
- Windows/Linux mandatory and future macOS platform-impact work;
- graphics/windowing/GPU work.

When a skill is applied, the agent must state:

```text
Applying local skill: <skill-name>
Reason: <specific trigger>
```


## Local Codex skills

This starter includes local Codex skills under `.codex/skills/`. Each skill has a `SKILL.md` file with required YAML frontmatter (`name`, `description`) so Codex can discover and load it. Skill format details are documented in `docs/development/SKILL_FILE_FORMAT.md`; usage scenarios are documented in `docs/development/SKILL_USAGE_GUIDE.md`.
