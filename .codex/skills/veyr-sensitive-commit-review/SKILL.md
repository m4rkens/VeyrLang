---
name: veyr-sensitive-commit-review
description: >
  Use before any commit that includes CMake, source, tests, scripts, future .vyr files, or project/build manifest changes. Requires a sensitive-file review packet and explicit owner approval before committing.
---

# Skill: Veyr Sensitive Commit Review

## Purpose

Stop before sensitive execution/gap-fix commits that contain build-system, implementation, source, script, test, automation, future `.vyr`, or project-manifest behavior changes.

## Trigger

Use this skill before any commit if the diff includes:

- `CMakeLists.txt`, `cmake/**`, `**/*.cmake`, `CMakePresets.json`
- `include/**`, `src/**`, `tests/**`, `tools/**`, `lib/**`
- `**/*.c`, `**/*.cc`, `**/*.cpp`, `**/*.cxx`
- `**/*.h`, `**/*.hh`, `**/*.hpp`, `**/*.hxx`, `**/*.ipp`, `**/*.inc`
- `scripts/**`, `.github/workflows/**`, `**/*.sh`, `**/*.ps1`, `**/*.py`, `**/*.bat`, `**/*.cmd`
- `**/*.vyr`
- `Veyr.toml`, `Veyr.lock` when they affect project/build behavior

## Forbidden behavior

- Do not commit automatically after successful tests.
- Do not commit merely because a stage finished; stage commits still require owner approval.
- Do not commit on ambiguous approval.
- Do not create many microscopic implementation commits unless the approved plan justifies a coherent split. Keep discussion/planning/execution/verification artifacts in their own stage commits.

## Required review packet

Before committing, present:

```text
Sensitive-file commit review packet

Proposed commit intent:
Proposed commit message:
Verification performed:
Sensitive files changed:
Non-sensitive files changed:
Risk areas:
Platform impact:
Files I will stage:
Files I will not stage:
Owner approval required:
  approve this sensitive-file commit
```

Then stop.

## Valid approval

Valid examples:

```text
approve this sensitive-file commit
commit these reviewed implementation changes
```

Invalid/insufficient examples:

```text
ok
looks good
continue
next
ship it
```
