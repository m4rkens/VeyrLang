---
phase: 01-conan-and-compiler-build-foundation
plan: "05"
subsystem: build
tags: [python, cmake, ctest, includes, linux, cross-platform]
requires:
  - phase: "01-04"
    provides: CMake foundation, selected Python and prepared Linux Debug/Release native configurations
provides:
  - Read-only canonical Veyr-owned include checker and temporary-tree positive/negative fixtures
  - Identical direct, check-includes target and include-policy CTest routes
  - Separate passing Linux Debug/Release configure, checker-target and include-policy CTest evidence
affects: ["01-06", "01-07", "01-08", "02"]
tech-stack:
  added: []
  patterns: ["inventory-based owned-header spelling", "literal-directive checks with physical locations", "explicit selected Python and structured root arguments"]
key-files:
  created:
    - scripts/check_includes.py
    - tests/development/test_include_policy.py
    - .planning/phases/01-conan-and-compiler-build-foundation/01-05-EXECUTION.md
    - .planning/phases/01-conan-and-compiler-build-foundation/01-05-SUMMARY.md
  modified:
    - CMakeLists.txt
    - tests/CMakeLists.txt
    - .planning/STATE.md
    - .planning/ROADMAP.md
key-decisions:
  - Follow approved include policy without adding language syntax or a C++ preprocessor framework
  - Temporary fixtures prove rejection; real source files remain compliant
  - Available Linux route acceptance remains separate from native Windows and independent CI evidence
patterns-established:
  - Build target and CTest use the same read-only script with the reviewed absolute Python
requirements-completed: []
requirements-covered: [BUILD-11]
coverage:
  - id: D1
    description: Canonical owned-header policy rejects quoted, relative, absolute, bare and wrong-case forms with locations
    requirement: BUILD-11
    verification:
      - kind: unit
        ref: "tests/development/test_include_policy.py — 20 positive/negative temporary-tree fixtures passed"
        status: pass
    human_judgment: false
  - id: D2
    description: Direct read-only checker accepts compliant repository first-party and recipe test-package sources
    requirement: BUILD-11
    verification:
      - kind: integration
        ref: "/usr/bin/python3 -B scripts/check_includes.py --root . — exit 0"
        status: pass
    human_judgment: false
  - id: D3
    description: Separate Linux Debug/Release build-target and CTest include-policy routes pass using identical selected commands
    requirement: BUILD-11
    verification:
      - kind: integration
        ref: "01-05-EXECUTION.md — both check-includes targets exit 0 and include-policy CTest 1/1 each"
        status: pass
      - kind: manual_procedural
        ref: "Generated build.ninja/CTestTestfile.cmake command inspection matches absolute reviewed Python, script and root"
        status: pass
    human_judgment: false
  - id: D4
    description: Native Windows and Windows/Ubuntu CI execution of include-policy routes
    requirement: BUILD-11
    verification:
      - kind: integration
        ref: "Actual Windows native and independent CI environments remain unavailable/unobserved; fixtures are not native evidence"
        status: unknown
    human_judgment: true
    rationale: "Portable path/case fixtures and Linux results do not establish Windows native or CI acceptance"
duration: 16min
completed: 2026-10-05
status: complete
completion_scope: available-linux-developer-only
---

# Plan 01-05 — Canonical owned-header include guardrail

**Inventory-based Python include checking rejects noncanonical owned paths and passes direct, CMake target and CTest routes in independent Linux Debug/Release configurations.**

## Accomplishments and scope

Task 1 created a standard-library Python checker and 20 temporary-tree fixtures. The checker inventories include/veyr and scans explicit include/src/tests and Conan recipe test_package C/C++ trees. It excludes generated/build/vendor/third-party trees and symlinks; it never edits source/cache/build state. Literal #include directives are distinguished from comments, escaped strings, raw strings and character literals, with line-splice handling and physical file/line diagnostics. Owned includes require exact `<veyr/...>` spelling. Relative/absolute/rooted paths, quoted owned includes, bare/source-directory aliases and wrong-case forms fail. Ordinary external styles pass.

Task 2 adds check-includes and include-policy CTest using identical selected VEYR_PYTHON, script and root arguments. CMake VERBATIM handles build-command quoting. Existing production/runtime/provider guards, native tests, presets and dependency policy remain intact. No external package code or language/compiler pipeline was changed.

This is available Omarchy/Arch Linux developer closeout only. Windows developer/locks/runtime and Windows/Ubuntu CI observations remain independent and pending; macOS deferred. BUILD-11 and all phase-wide requirements remain pending at the complete Tier 1 acceptance level. All eight waves remain unchanged; Plan 01-06 is unstarted.

## Checks and existing package reuse

- `/usr/bin/python3 -B -m unittest discover -s tests/development -p test_include_policy.py`: exit 0; 20 tests in 0.048s. Includes both path conventions, exact case, spaced root, optional trees, recipe tests, exclusions, comments/literals, physical lines, all violation locations, CLI outcomes and unchanged contents/mtime checks.
- `/usr/bin/python3 -B scripts/check_includes.py --root .`: exit 0.
- Linux Debug: cache-only configure exit 0 (1.119s), check-includes target exit 0 (0.065s), include-policy CTest exit 0 and 1/1 passed (0.090s).
- Linux Release: independent cache-only configure exit 0 (1.278s), check-includes target exit 0 (0.068s), include-policy CTest exit 0 and 1/1 passed (0.092s).
- Generated target and CTest commands inspected read-only: identical absolute `/usr/bin/python3`, `-B`, checker and repository root in both configurations.
- git diff --check passed; protected inventory confirms 194 other tracked files unchanged, including discussion, all eight plans, presets, native sources, profiles, manifests/locks and previous summaries.

Exact native commands, timestamps and ignored streaming logs are recorded in 01-05-EXECUTION.md. Logs live under .veyr-dev/logs; temporary logging driver lives under /tmp. Both configure operations used project-owned Conan 2.33.0, dedicated project home, existing locks, --no-remote and --build=never. Original LLVM 23.1.2 and matching GoogleTest 1.18.0 packages were Cache/Already installed. No dependency downloads or rebuilds, CLI installation, recipe export, lock updates, purge or forced rebuild. Checker targets do not rebuild native targets. Configure runs retained existing runtime compatibility probes. Existing Plan 01-04 full native 11/11 results are historical evidence; this plan ran only the bounded include-policy CTest route.

## Issues and limits

Initial Debug wrapper exited 2 before launching Conan/CMake because the sandbox denied the external-cache guard with `[Errno 30] Read-only file system`. Runtime-required, tool-approved escalation allowed that exact cache-only operation. Release used approved cache access directly. No native/package command failed; no automatic repair, native/dependency retry or duplicate command. No quiet threshold was reached. Existing Conan profile quotation warnings remain visible; profiles were not changed.

The checker enforces literal include operands; it does not expand C++ macros or evaluate conditional compilation. All literal branches are inspected. Symlink/generated/external trees are excluded deliberately. Spaced roots and Windows path/case forms are fixture-covered, without claiming native Windows or native paths-with-spaces configure acceptance. Broader ABI/provider/path and cold CI capacity gaps remain pending.

## Files and owner boundary

Created scripts/check_includes.py, tests/development/test_include_policy.py, 01-05-EXECUTION.md and this summary. Modified CMakeLists.txt, tests/CMakeLists.txt, .planning/STATE.md and .planning/ROADMAP.md. Exact packet: eight files.

The owner reviewed the completed execution/closeout and explicitly requested one dedicated eight-file checkpoint `build: enforce canonical Veyr-owned includes`. This summary is included in that checkpoint; resolve its hash from Git history. No separate task or automatic metadata commit. STATE/ROADMAP retain 5/8 plans complete within available Linux scope, zero completed phases. Stop for owner review afterward; no Plan 01-06, separate phase verification, push or phase advancement.

Platform impact:
- Windows: Both separator/absolute-path conventions and exact canonical casing tested independently of host lookup; structured paths, selected interpreter and CMake VERBATIM preserve Windows viability. Native developer/CI evidence pending.
- Linux: Direct checker and independent Debug/Release configure/target/CTest routes passed on observed Omarchy host; Ubuntu CI remains independent and pending.
- macOS future: Deferred; standard Python/CMake interfaces preserve feasibility without Darwin implementation or CI claims.

## Self-check

Scoped files and actual exit-0 evidence recorded. Protected artifacts, eight waves, existing dependency identities and missing platform evidence preserved. Summary coverage distinguishes passed Linux/fixture checks from unobserved Windows/CI routes; no phase-wide requirement or phase completion inferred.
