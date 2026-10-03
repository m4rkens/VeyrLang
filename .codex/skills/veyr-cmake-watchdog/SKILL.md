---
name: veyr-cmake-watchdog
description: >
  Use before running CMake, Ninja, CTest, Conan, compiler, linker, dependency, or build commands.
  Enforces lifecycle permission, single-command execution, liveness supervision, and safe failure
  handling without killing healthy quiet builds.
---

# Skill: Veyr CMake/Ninja/Conan Build Supervisor

## Purpose

Prevent stuck or duplicated build/dependency commands while allowing legitimate long-running,
quiet work such as LLVM source builds.

## Trigger

Use before:

- `conan` package/install/build operations;
- `cmake`;
- `ninja`;
- `ctest`;
- compiler/linker commands;
- dependency source builds;
- scripts invoking any of the above.

## Permission gate

These commands may run only during owner-approved execution or verification. They are forbidden
during discussion and planning.

## Soft quiet threshold

After approximately 60 seconds without new stdout/stderr:

1. do **not** assume failure;
2. record a supervisor heartbeat;
3. inspect available liveness signals:
   - process still alive;
   - child processes;
   - CPU-time movement;
   - I/O movement;
   - build/package artifact changes;
4. continue waiting while credible progress exists.

A heartbeat is supervisor output, not tool output.

## Hard-stop conditions

Stop on:

- non-zero process exit;
- owner interruption;
- confirmed process conflict/duplicate build;
- violated precondition/invariant;
- explicit hard timeout established before the command;
- credible evidence the process is deadlocked or no longer progressing.

Do not automatically retry, delete caches/build trees, or launch duplicate work.

## Stall report

When stopping, report:

```text
Build supervisor stop

Command:
Working directory:
Elapsed:
Output-idle time:
Last visible output:
Process/liveness evidence:
Build/package directory touched:
Reason for stop:
Suggested owner action:
```

## GSD boundary

This skill does not grant permission to execute, verify, commit, or advance a phase.
