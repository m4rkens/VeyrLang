---
phase: 01-conan-and-compiler-build-foundation
plan: "03"
subsystem: build
tags: [conan, llvm, googletest, python, linux, locking]
requires:
  - phase: "01-01"
    provides: Reviewed native selection, read-only doctor and project-owned Conan CLI bootstrap
  - phase: "01-02"
    provides: Narrow LLVM 23.1.2 recipe and cached native Linux package/probe
provides:
  - Exact consumer recipe and real Linux Debug/Release Conan-native locks
  - Explicit acquiring setup and cache-only prepared generation
  - Shared orchestration, home-conflict guard and mocked Windows/Linux checks
affects: ["01-04", "01-06", "01-07", "01-08"]
tech-stack:
  added: ["gtest/1.18.0", "Conan-owned cmake/4.4.3 build requirement"]
  patterns: ["absolute project-owned Conan invocation", "native superset locks", "explicit setup vs cache-only prepare"]
key-files:
  created:
    - conanfile.py
    - conan/locks/linux-x86_64-clang-Debug.lock
    - conan/locks/linux-x86_64-clang-Release.lock
    - scripts/devtools/conan_ops.py
    - tests/development/test_conan_ops.py
    - .planning/phases/01-conan-and-compiler-build-foundation/01-03-EXECUTION.md
    - .planning/phases/01-conan-and-compiler-build-foundation/01-03-SUMMARY.md
  modified: [scripts/dev.py, tests/development/test_doctor.py, .gitignore, .planning/STATE.md, .planning/ROADMAP.md]
key-decisions:
  - Owner approved Linux Debug/Release execution with Windows locks/native evidence independently deferred
  - Conan-generated managed native locks are accepted for external-provider graph subsets
  - Explicit --init-lock initializes only missing locks; ordinary setup preserves existing locks
patterns-established:
  - One exclusive wrapper-operation marker beside the dedicated home, without modifying Conan storage layout
  - Only explicit setup bootstraps tools/acquires packages; prepare never downloads/builds/retries
requirements-completed: []
requirements-covered: [BUILD-02, BUILD-06, BUILD-12]
coverage:
  - id: D1
    description: Exact Linux consumer references and Conan-native Debug/Release locks with external-provider graph subsets
    requirement: BUILD-02
    verification:
      - kind: integration
        ref: "01-03-EXECUTION.md — two Linux lock creates and four cache-only subset graph checks"
        status: pass
    human_judgment: false
  - id: D2
    description: Explicit Linux dependency setup uses the project-owned CLI and dedicated Conan home
    requirement: BUILD-06
    verification:
      - kind: unit
        ref: "tests/development/test_conan_ops.py — 22 mocked checks; test_doctor.py — 38 regression checks"
        status: pass
      - kind: integration
        ref: ".veyr-dev/logs/01-03-setup-{Debug,Release}.log — exit 0 each"
        status: pass
    human_judgment: false
  - id: D3
    description: Linux prepared generation uses existing packages/locks with no remotes or package builds
    requirement: BUILD-12
    verification:
      - kind: unit
        ref: "test_conan_ops.py — missing-cache no-retry, conflicting-home and setup/preparation boundaries"
        status: pass
      - kind: integration
        ref: ".veyr-dev/logs/01-03-prepare-{Debug,Release}.log — --no-remote --build=never, exit 0 each"
        status: pass
    human_judgment: false
  - id: D4
    description: Windows native locks/developer acceptance and independent Windows/Ubuntu CI identities
    verification:
      - kind: integration
        ref: "Windows host inspection and CI observations not supplied; no Windows locks generated"
        status: unknown
    human_judgment: true
    rationale: "Linux observations and mocked Windows fixtures cannot establish independently pending native/CI evidence"
duration: 33min
completed: 2026-10-05
status: complete
completion_scope: available-linux-developer-only
---

# Plan 01-03 — Linux dependency setup and cache-only preparation

**Exact LLVM/GoogleTest Conan inputs, reviewed native Linux locks and thin setup/preparation orchestration reuse the original LLVM package in Debug and Release.**

## Accomplishments and acceptance limits

- Task 1 is complete for available Linux scope: exact `gtest/1.18.0`/managed `llvm/23.1.2@veyr/m1`, static GoogleTest/GoogleMock options, standard Ninja consumer generators, two actual Conan-generated locks, and four system/custom × Debug/Release graph subset checks.
- Task 2 implementation and available Linux acceptance passed: explicit setup reused project-owned Conan 2.33.0, built missing native GoogleTest packages, reused LLVM, ran the managed recipe probe, and generated separate consumer toolchain/dependency output. Prepared generation then passed with `--no-remote --build=never` for both configurations.
- Progress is 3/8 plans complete **within available Linux developer scope**. Neither four-lock all-platform acceptance nor Phase 1 Tier 1 acceptance is complete. Windows lock files remain absent pending native inspection; Windows developer/CI and Ubuntu CI evidence remain independently pending; macOS deferred.
- No compiler project CMake configure/build, GTest consumer link/native run, Plan 01-04 execution or later compiler feature work occurred. Shared configure dispatch is fixture-tested and gated until Plan 01-04 files exist. BUILD-02/06/12 receive partial coverage, not phase-wide requirement completion.

## Recorded checks

| Check | Result | Evidence |
|---|---|---|
| Consumer and changed Python syntax | Pass | Compile-only checks, no Conan import/CLI |
| Native locks | Debug/Release exit 0 | Real recipe revisions and native profiles in EXECUTION |
| External-provider subsets | Four exit-0 graph checks | LLVM absent; unused managed lock entry accepted |
| New orchestration fixtures | 22/22 | Temporary paths and fake subprocesses only |
| CLI/bootstrap/doctor regression | 38/38 | Rerun justified by changed CLI dispatch/guard and stale setup assertion |
| Linux Debug explicit setup | Exit 0, 19.569s | `.veyr-dev/logs/01-03-setup-Debug.log` |
| Linux Release explicit setup | Exit 0, 20.924s | `.veyr-dev/logs/01-03-setup-Release.log` |
| Debug prepared generation | Exit 0, 0.474s | `.veyr-dev/logs/01-03-prepare-Debug.log` |
| Release prepared generation | Exit 0, 0.461s | `.veyr-dev/logs/01-03-prepare-Release.log` |

Both setup commands explicitly passed `--profile linux-x86_64-clang`, their configuration, `--environment linux-developer` and `--conan-home /home/m4rkens/.cache/veyr/conan/22e2b2472775`. The existing CLI environment was reused; only launch validation and isolated `pip check` ran, with no CLI installation/upgrade. Read-only doctor production code was unchanged.

Both setups ran the cached LLVM test_package with matching native profiles, `--no-remote --build=never`: configure 0.5s each, four Ninja compile/link steps each, native `LLVM 23.1.2: Support allocation/destruction passed`, exit 0. This validates the new setup orchestration seam; the completed Plan 01-02 implementation was not changed or rebuilt.

## Conan identities and cache reuse

- LLVM unchanged: `llvm/23.1.2@veyr/m1#aa4cad25b3148c3bdd22ed96fba04654:a214d896e569f9b075f68e0b581ce595d09d80df#cd9a88c23979fe65c10c97b796943a64`. Every install/probe/preparation used Cache/Already installed. No LLVM source build, package rebuild, cache purge or forced build.
- Original LLVM manifest SHA256 remains `2eba146976c0f4460a0fb60fdf7363eb9c91a00b9a9926b47b893f0dd6cb297e`.
- GoogleTest Debug: `gtest/1.18.0#e2507fc4efe2ec9e2db9e5c91eefab9f:77ee4a6c9da2ff6b1df8eac330ceb33aad0d1d69#3604fb7abde2c4493bc7897a104b3995`.
- GoogleTest Release: `gtest/1.18.0#e2507fc4efe2ec9e2db9e5c91eefab9f:4c47d11dc631ceaef479f42595e1124ab519c480#f244293f6c361ee557e76d8fed0392f4`.
- Conan-owned CMake build tool: `cmake/4.4.3#8b46ce9dbf1937af1c05f19579c1f105:63fead0844576fc02943e16909f08fcdddd6f44b#e75518647d197cbde7df069c10071164`; binary downloaded during Debug setup and reused for Release. Conan resolved/pinned this upstream GoogleTest build requirement; no second installer or tool registry.
- Both Linux recipe-only lock files have SHA256 `b76a5f857ba40da4a59d2bdcd8690fba6d12d15d53bd3b0dc6aceb252585f30b`, unchanged after setup/preparation. Identical recipe locks do not equate Debug and Release package IDs; the native profile/configuration selections remain separate.

Generated toolchains, GTest configs and Ninja presets exist only under `build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-5221f1471fc7/{Debug,Release}/generators`. No root user preset generated. Raw logs/graphs, build output, project-owned CLI, Conan packages and operation marker remain untracked. The exclusive marker was released after commands.

## Files created/modified

- `conanfile.py`; Linux Debug/Release Conan-native locks.
- `scripts/devtools/conan_ops.py`; `scripts/dev.py`.
- `tests/development/test_conan_ops.py`; one stale setup assertion in `test_doctor.py`.
- `.gitignore` — root generated user presets/Python bytecode only; planning and Conan locks stay trackable.
- Normal `01-03-EXECUTION.md`, this SUMMARY, `.planning/STATE.md`, `.planning/ROADMAP.md`.

## Decisions, deviations and issues

- Owner explicitly chose available-Linux continuation instead of fabricating two Windows locks. All eight waves and Tier 1 requirements remain unchanged; Windows candidates were not rewritten.
- Required bounded consistency correction: the old test asserting plain setup must remain deferred contradicted Task 2. Replaced only that assertion with dependency-dispatch coverage; production doctor/baseline/profile files unchanged.
- New suite first had one fixture failure (wrong-lock fixture file absent, so the earlier missing-lock diagnostic was correct); added that fixture file, then 22/22 passed. No native/package command failed or was retried.
- First lock command emitted an unscoped-option warning; subsequent commands use `&:llvm_provider` explicitly. Existing Linux profile quoting warnings persisted during native configure; no warning suppression or profile modification. Paths containing spaces have mocked orchestration coverage, not native toolchain evidence.
- Upstream GoogleTest recipe used Unix Makefiles for its internal Conan-owned source builds; consumer generator and LLVM probe used Ninja. No claim that a future cold host's implicit Make prerequisite has been separately reviewed. Native dependency/runner behavior remains subject to platform/CI review; no custom build/provisioning framework added.
- `DEVELOPER_BOOTSTRAP.md` still contains its earlier conditional description of the then-future Plan 01-03 implementation; full phase documentation refresh belongs to Plan 01-08. This execution/summary records the current behavior.

## Commits and next boundary

The owner explicitly requested one dedicated checkpoint for the reviewed Plan 01-03 implementation/configuration and normal closeout records, with message `build: wire Conan dependency setup and cache-only preparation`. This summary is included in that checkpoint; its hash is available from Git history. No separate task commit or automatic lifecycle commit.

Stop for owner review after the authorized checkpoint commit. Plan 01-04 remains unstarted; starting it, separate phase verification, push and phase advancement require their explicit owner instructions. No new package operations or builds are part of this commit step.

Platform impact:
- Windows: Shared structured argv/executable/profile/home behavior is mocked; native locks/runtime evidence and CI are pending.
- Linux: Observed Omarchy/Arch baseline only; Debug/Release setup/probe/preparation passed independently. Ubuntu CI remains separate and pending.
- macOS future: Deferred; standard Conan/CMake boundaries and platform-gated native profiles remain extensible.

## Self-check

Available-Linux artifacts and exit-0 logs verified; original LLVM manifest and lock contents unchanged, no root user presets or lingering operation marker. Mocked/syntax checks passed. Windows/CI/future-plan gaps remain explicit. No phase-wide requirement or phase completion marked.
