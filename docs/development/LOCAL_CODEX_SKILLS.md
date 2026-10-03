# Local Codex Skills Policy

Veyr uses repository-owned local Codex skill instructions under:

```text
.codex/skills/
```

These skills are designed to reduce workflow accidents when switching environments, running GSD, supervising CMake/Ninja, planning language features, preparing commits, and reviewing cross-platform impact.

## Canonical rule

```text
Repository files are canonical.
Codex chat history is not canonical.
```

A skill may summarize, inspect, and propose. It must not silently execute lifecycle transitions, verification, commits, cleanup, retries, or destructive actions.

## Required skills

The following local skills are part of the Veyr workflow:

| Skill | Purpose |
|---|---|
| `veyr-session-rehydrate` | Rebuild context after switching Linux/Windows/macOS, new sessions, new clone, tool reinstall. |
| `veyr-gsd-phase-gate` | Stop automatic GSD phase hopping. |
| `veyr-cmake-watchdog` | Supervise long CMake/Ninja/CTest/Conan/build commands; ~60s silence triggers liveness inspection, not automatic termination. |
| `veyr-developer-bootstrap` | Guard Conan/toolchain/LLVM setup, profiles, cache-aware workflows, and cross-platform developer wrappers. |
| `veyr-sensitive-commit-review` | Require owner review before sensitive commits. |
| `veyr-design-probe-planner` | Tie language features to real project/probe pressure. |
| `veyr-cross-platform-impact` | Enforce Windows/Linux Tier 1 and future macOS analysis. |
| `veyr-project-system-steward` | Guard `Veyr.toml`, `Veyr.lock`, `.veyr/`, targets, profiles, dependency semantics. |

## Usage guide

Practical examples and scenario-based triggers are documented in:

```text
docs/development/SKILL_USAGE_GUIDE.md
```

Use that guide when deciding which skill applies, how to announce it, and where the agent must stop for owner review.

## Automatic invocation

The agent may invoke a skill automatically only when the trigger clearly applies. When doing so, it must say which skill it is applying and why.

Example:

```text
Applying local skill: veyr-session-rehydrate
Reason: this is a fresh Windows session after Linux work.
```

## Non-negotiable stops

A skill must stop and ask for a concrete owner command before:

- crossing any GSD lifecycle boundary or creating its stage commit;
- running CMake/Ninja/CTest/build commands;
- retrying a stalled command;
- deleting/recreating build directories;
- modifying sensitive files during planning;
- committing without explicit approval for the current lifecycle-stage commit;
- advancing to another phase;
- converting chat-only decisions into implementation.

## Installation note

These are repository-local instructions. If a Codex environment requires copying or enabling local skills explicitly, do that as a separate owner-reviewed setup step. Do not silently assume global skills are installed.

## Required SKILL.md metadata

Every local skill file must begin with YAML frontmatter containing at least `name` and `description`. See `docs/development/SKILL_FILE_FORMAT.md`. Codex may skip a skill as invalid if this metadata is missing.

```yaml
---
name: veyr-session-rehydrate
description: >
  Reconstruct Veyr project state after switching machines or starting a new Codex session.
---
```
