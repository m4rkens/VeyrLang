---
name: veyr-developer-bootstrap
description: >
  Use when planning or reviewing Veyr developer setup, Conan profiles/recipes/cache integration,
  LLVM dependency configuration, setup/doctor/build wrappers, or Windows/Linux/macOS bootstrap
  parity.
---

# Skill: Veyr Developer Bootstrap

## Purpose

Keep developer setup reproducible and cross-platform while delegating third-party package
mechanics to Conan 2.

## Trigger

Use when work touches:

- Conan installation/configuration;
- Conan profiles, lockfiles, recipes or package options;
- LLVM 23.1.2 packaging/integration;
- CMakeToolchain/CMakeDeps integration;
- Python setup/doctor/configure/build/test wrappers;
- local cache/home selection;
- Windows/Linux developer parity;
- CI bootstrap/reuse.

## Required checks

Confirm that proposed work:

- keeps Conan as the third-party package/dependency authority;
- does not recreate dependency solving/source acquisition/package storage in Veyr scripts;
- uses explicit reviewed platform profiles;
- keeps Windows and Linux toolchain assumptions separate;
- keeps generated package/build state untracked;
- uses exact dependency versions/recipe choices appropriate to the phase;
- keeps the Python wrapper thin;
- allows long source builds when needed and relies on cache/reuse afterward;
- keeps future macOS structurally feasible;
- distinguishes compiler-implementation Conan state from future `Veyr.lock`.

## LLVM

Prefer a suitable existing Conan recipe. If LLVM 23.1.2 requires a project-owned recipe, keep it
narrow and upstream-oriented rather than creating a general provisioning framework.

## Long-running commands

Apply `veyr-cmake-watchdog`. The ~60-second quiet threshold is a liveness checkpoint, not an
automatic stop condition.

## GSD boundary

This skill does not grant permission to plan, execute, verify, commit, install tools, mutate
caches, or advance a phase.
