---
phase: 01-conan-and-compiler-build-foundation
plan: "06"
subsystem: build
tags: [python, conan, cmake, ctest, supervision, clean, linux, cross-platform]
requires:
  - phase: "01-03"
    provides: Explicit Conan setup, dedicated cache, existing locks and cache-only preparation
  - phase: "01-04"
    provides: C++23 CMake/Ninja foundation, native tests and compilation databases
  - phase: "01-05"
    provides: Canonical owned-header checker and include-policy CTest route
provides:
  - Complete thin developer command composition with explicit acquisition boundary and invocation-local deduplication
  - Selected-output clean preserving other configuration, Conan CLI tool environment and dedicated package cache
  - Single-child structured execution, streamed output and soft quiet liveness checkpoints
  - Independent Linux Debug/Release entry-point and composed all acceptance plus native Debug clean preservation evidence
affects: ["01-07", "01-08", "02"]
tech-stack:
  added: []
  patterns: ["invocation-local completed steps", "cache-only stale configure regeneration", "quiet liveness observations without implicit timeouts", "selected-output clean"]
key-files:
  created:
    - scripts/devtools/runner.py
    - tests/development/test_dev_commands.py
    - tests/development/test_runner.py
    - .planning/phases/01-conan-and-compiler-build-foundation/01-06-EXECUTION.md
    - .planning/phases/01-conan-and-compiler-build-foundation/01-06-SUMMARY.md
  modified:
    - scripts/dev.py
    - scripts/devtools/conan_ops.py
    - .planning/STATE.md
    - .planning/ROADMAP.md
key-decisions:
  - Preserve approved chaining: ordinary setup explicit, build may configure, test may build, all includes setup
  - Conan remains sole owner of third-party dependency/package mechanics and existing locks/cache
  - Verify reuses equivalent CTest completed within the same invocation, including include-policy
  - Unknown or quiet liveness never becomes an implicit hard timeout
  - Native clean removes only explicitly selected output; preserve cached packages, CLI tools and other configuration
patterns-established:
  - Existing Conan orchestration accepts optional runner handoff without changing package policy
  - Ignored scalar configure-input fingerprint detects stale generated configuration without storing a package graph
requirements-completed: []
requirements-covered: [BUILD-06, BUILD-12]
coverage:
  - id: D1
    description: Developer defaults/overrides, chaining/deduplication, cache-only boundary and stale/missing-state behavior
    requirement: BUILD-06
    verification:
      - kind: unit
        ref: "tests/development/test_dev_commands.py — 16 temporary-tree/mocked tests passed"
        status: pass
      - kind: integration
        ref: "01-06-EXECUTION.md — independent Linux Debug/Release build/test/verify and all exit 0; CTest 12/12 each"
        status: pass
    human_judgment: false
  - id: D2
    description: Soft quiet checkpoints continue progress/unknown telemetry; confirmed stops propagate without retry
    requirement: BUILD-12
    verification:
      - kind: unit
        ref: "tests/development/test_runner.py — 14 short-child/injected-clock/platform fixtures passed"
        status: pass
    human_judgment: false
  - id: D3
    description: Scoped clean refuses unsafe paths and preserves other configuration/tools/cache
    requirement: BUILD-06
    verification:
      - kind: unit
        ref: "test_dev_commands.py — symlink/junction/root/cache refusals and temporary-tree preservation"
        status: pass
      - kind: integration
        ref: "Native Debug clean exit 0; Release 184, Conan tools 2382, package cache 405455 entries unchanged by read-only metadata comparison"
        status: pass
    human_judgment: false
  - id: D4
    description: Existing doctor/bootstrap and Conan policy contracts remain intact
    requirement: BUILD-06
    verification:
      - kind: unit
        ref: "Existing test_doctor.py 38/38 and test_conan_ops.py 22/22 passed; no package/native build work in fixtures"
        status: pass
      - kind: integration
        ref: "Native all reused exact project-owned Conan 2.33.0 with pip check; original LLVM/GoogleTest packages and locks preserved"
        status: pass
    human_judgment: false
  - id: D5
    description: Native Windows developer and independent Windows/Ubuntu CI command/clean/process acceptance
    requirement: BUILD-06
    verification:
      - kind: integration
        ref: "Windows path/suffix/tree fixtures and Linux results do not establish native Windows/CI acceptance; pending"
        status: unknown
    human_judgment: true
    rationale: "Independent native hosts/runners and cold-capacity observations unavailable; no fabricated evidence"
  - id: D6
    description: Native Windows child-tree termination and long expensive-command quiet telemetry
    requirement: BUILD-12
    verification:
      - kind: integration
        ref: "Platform mocks/short Python children passed; native acceptance commands finished below quiet threshold"
        status: unknown
    human_judgment: true
    rationale: "Fast fixtures prove policy behavior but do not establish Windows native termination or expensive-build telemetry"
duration: 30min
completed: 2026-10-05
status: complete
completion_scope: available-linux-developer-only
---

# Plan 01-06 — Developer commands, scoped clean and liveness supervision

**Thin command composition passed independent Linux Debug/Release entry points and composed all; explicitly authorized native Debug clean removed only its selected output while preserving Release, project Conan tools and the dedicated package cache.**

## Implementation and bounded integration

Task 1 extends scripts/dev.py with build/test/verify/all/clean while preserving doctor/setup/configure. One Operations context records completed steps: build may configure from existing lock/cache, test may build, all visibly performs explicit setup, and verify reuses equivalent CTest already completed during that invocation. Routine operations never acquire dependencies or call --build=missing. Stale selected profile/tool policy/lock/provider/generated identity triggers cache-only regeneration. Ignored selected-output .veyr-config-inputs.json stores one configure-input SHA256, not a dependency/package graph, SDK normalization layer, acquisition/provisioning system or replacement for Conan locks. BuildSelection remains unchanged.

Verify requires prepared native executables, include-policy CTest registration and selected compiler/C++23 compilation entries. Clean canonicalizes selected output under the project build root, refuses roots/escapes/symlink/junction ancestors/destinations and preserves the tool environment, dedicated cache and other configuration. No automatic cleanup, retry, repair or purge.

Task 2 adds a standard-library single-child runner using structured argv/cwd/env, immediate merged output and one active-command guard. Approximately 60 quiet seconds prompts inspection. Linux process-group child/CPU/I/O changes supply liveness observations; uncertain/unavailable telemetry prints inspection guidance and continues. Silence, PID existence and unchanged counters never prove a stall. Actual failure/interruption/confirmed stop or explicit hard timeout reports context and stops once. Windows process-group/tree handling is fixture-covered, not native-proven. No generic process framework or new dependency added.

Rule 2 bounded missing-integration deviation adds an optional runner handoff to scripts/devtools/conan_ops.py so actual setup/prepare/configure children are supervised. Existing default APIs, cache guard, locks/profiles, absolute project Conan executable and package policy remain unchanged. Full setup/all pass the runner to existing CLI validation; reviewed tools-only behavior and read-only doctor remain intact. Five implementation/test paths comprise the change; no plan/discussion or implementation outside this bounded integration was modified.

## Actual acceptance

- New command fixtures: 16/16; runner fixtures: 14/14. Existing doctor 38/38 and Conan operations 22/22 regression checks passed. Fast injected clocks and short isolated Python children cover quiet progress/unknown telemetry, stall, timeout, failure, conflict and interruption without real 60-second waits. These suites were not repeated after acceptance.
- Separate prepared Linux Debug/Release build/test/verify commands all exited 0; CTest 12/12 and four selected C++23 compile-database entries each. Initial configure was cache-only (--no-remote --build=never), reusing original dependencies.
- Separately owner-authorized all Debug exited 0 in 5.031s; Release exited 0 in 5.026s. One setup/configure/build/test/verify chain and one CTest suite per invocation; verify reused that suite. CLI environment reused Conan 2.33.0 and pip check passed without reinstall/upgrade. Explicit setup retained --build=missing with existing locks; cache-only configure retained --no-remote --build=never. No package downloads, LLVM/GoogleTest source rebuild, forced rebuild or lock change.
- Standard Conan test_package behavior rebuilt its small consumer probe, configured/compiled/linked/ran successfully in each configuration. These probe rebuilds are distinct from cached dependency package builds. Native Veyr build needed only C++ dyndep generation, no target compile/link work.
- Separately owner-authorized Debug clean exited 0 in 0.083s. Selected Debug output is now absent. Read-only before/after inventories of Release (184 entries), Conan tools (2382 entries) and dedicated cache (405455 entries) match exactly in entry/type/mode/size/mtime/ctime/device/inode/link/ownership/symlink-target identity. Access times deliberately excluded because reads may affect them; this is metadata preservation evidence, not rehashing all cached file bytes. No symlink traversal, Conan command or cache-state write in the comparison.
- Original LLVM package/revisions and manifest SHA256 2eba146976c0f4460a0fb60fdf7363eb9c91a00b9a9926b47b893f0dd6cb297e remain unchanged. All 198 protected tracked files and all five implementation/test files unchanged during native all/clean continuations. Syntax and diff-whitespace checks passed; no checks rerun without a change requiring them.

Exact commands, timestamps, ignored streaming logs, preservation counts/digests and boundaries are in 01-06-EXECUTION.md. Snapshot/comparison helpers and full metadata inventories live under /tmp only. No builds/tests/setup/package operations ran after clean; its successful Debug evidence remains in logs although generated Debug output was deliberately removed. Release output remains prepared. Do not silently rebuild Debug on closeout.

## Limits and platform impact

Available Omarchy/Arch Linux developer scope only. Native Windows developer/locks/runtime, Windows CI and Ubuntu CI remain independently pending; macOS deferred. No phase-wide requirement/phase completion inferred. Existing profile quotation warnings remain visible; native spaced-path/provider/broader ABI and cold-CI capacity gaps remain. Quiet native commands finished below 60 seconds; fast fixtures provide supervision evidence. Windows native termination is unobserved.

Platform impact:
- Windows: Structured argv/paths, native executable suffix, junction refusal and process-group/taskkill tree behavior fixture-covered. Actual Windows developer and Windows CI baseline/commands/clean/runtime acceptance pending.
- Linux: Independent Debug/Release entry points/all and explicit Debug-only clean passed on Omarchy. Full preserved-tree metadata comparison passed; Ubuntu CI remains separate/pending.
- macOS future: Deferred; pathlib, standard-library subprocess seams and explicit platform gates preserve feasibility without Darwin support/CI claims.

## Normal closeout and owner boundary

Normal Plan 01-06 SUMMARY/EXECUTION/STATE/ROADMAP closeout records available-Linux completion, 6/8 plans and zero completed phases. Packet: five implementation/test files and four normal planning/execution records (nine files total). All eight waves, approved decisions and pending environment evidence preserved. No implementation edits during clean or closeout; no commit/staging/push, dedicated phase verification, Plan 01-07 start or phase advancement.

The owner reviewed completed execution/normal closeout and explicitly requested one dedicated nine-file checkpoint: `build: complete developer commands and liveness supervision`. This summary is included; resolve the resulting hash from Git history. All five reviewed implementation/test files remain unchanged. Stop for owner review after the checkpoint; no Plan 01-07 start, new builds/tests/package operations or phase advancement.
