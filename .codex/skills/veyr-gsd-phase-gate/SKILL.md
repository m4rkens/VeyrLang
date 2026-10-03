---
name: veyr-gsd-phase-gate
description: >
  Use before or after GSD commands and near lifecycle boundaries to prevent automatic transitions between discussion, planning, execution, verification, commit, and the next phase. Requires explicit owner approval for phase movement.
---

# Skill: Veyr GSD Phase Gate

## Purpose

Prevent accidental GSD lifecycle hopping and keep discussion, planning, execution, verification, commit, and phase advancement separate.

## Trigger

Use this skill before or after every GSD command, and whenever the owner says `next`, `continue`, `looks good`, `go on`, `progress`, or anything ambiguous near a phase boundary.

## Lifecycle boundaries

These transitions require explicit owner permission:

```text
discussion -> discussion commit (if tracked) -> planning
planning -> planning commit -> execution
execution -> execution commit -> verification
verification -> verification commit -> next phase
any step -> $gsd-progress --next
```

## Forbidden automatic behavior

Do not:

- run `$gsd-progress --next` by default;
- infer permission to commit or progress from a completed command;
- interpret `ok`, `continue`, `looks good`, `nice`, `ship it`, or `next` as permission to execute, verify, commit, or advance;
- create tests or implementation files during discussion/planning;
- run CMake/Ninja/CTest during discussion/planning.

## Required response at a boundary

When at a boundary, say:

```text
Current lifecycle boundary: <from> -> <to>
I will not proceed automatically.
To proceed, give a concrete command, for example:
  $gsd-execute-phase 3
```

## Safe proposals

The agent may propose the next command, but it must not run it.
