# Local Codex Skills for Veyr

This directory contains local, repository-owned Codex skill instructions for Veyr work.

They are not a replacement for `AGENTS.md`. They are small operational protocols that should be invoked when a task matches their trigger.

Core rule:

```text
The repository is canonical.
Chat history is not canonical.
Skills may inspect, summarize, and propose.
Skills must not silently execute phase transitions, verification, commits, or destructive actions.
```

## Available skills

- `veyr-session-rehydrate` — reconstruct project context after switching machines, sessions, shells, worktrees, or after reinstalling tools.
- `veyr-gsd-phase-gate` — enforce GSD lifecycle boundaries and stop automatic phase hopping.
- `veyr-cmake-watchdog` — supervise CMake/Ninja/CTest/build commands and stop on no-output stalls.
- `veyr-sensitive-commit-review` — prepare owner review packets before commits touching sensitive files.
- `veyr-design-probe-planner` — turn project pressure into language-design tasks without jumping to syntax.
- `veyr-cross-platform-impact` — review Windows/Linux mandatory parity and future macOS impact.
- `veyr-project-system-steward` — guard `Veyr.toml`, `Veyr.lock`, project profiles, and build-system semantics.

## Usage examples

Scenario-based examples for when to use each skill are documented in:

```text
docs/development/SKILL_USAGE_GUIDE.md
```

That guide is the practical trigger matrix for new sessions, environment switches, GSD phase boundaries, CMake stalls, sensitive commits, feature planning, project-system changes, and cross-platform work.

## Invocation policy

A skill may be invoked by the project owner explicitly, or by an agent when the trigger clearly applies. When invoked automatically, the agent must state which skill is being used and why.

A skill must stop and ask for a concrete owner command if the next action would:

- start a new GSD lifecycle step;
- run CMake/Ninja/CTest/build commands;
- modify implementation/build/source files during planning;
- commit;
- advance to another phase;
- rely on chat-only decisions.

- `veyr-developer-bootstrap` — Conan/toolchain/LLVM setup, profiles/cache, and cross-platform developer workflow guardrails.
