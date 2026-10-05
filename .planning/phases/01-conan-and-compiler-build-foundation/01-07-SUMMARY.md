---
phase: 01-conan-and-compiler-build-foundation
plan: "07"
subsystem: build
tags: [conan, llvm, cmake, ctest, acceptance, offline, cache, runner, linux]
requires:
  - phase: "01-02"
    provides: Exact LLVM 23.1.2 Conan package and native Linux consumer observations
  - phase: "01-04"
    provides: C++23 native foundation and strict LLVM provider/runtime rejection policy
  - phase: "01-06"
    provides: Thin developer commands, dedicated cache, scoped clean and supervised runner
provides:
  - Portable prepared/provider/runtime/cache/clean/offline acceptance harness with no hidden setup
  - Real Linux Debug/Release provider and Support ownership/native evidence
  - Separately accepted empty-cache/no-remote rejections preserving earlier successful gates
  - Deterministic runner final-exit propagation at EOF with retained streaming/liveness behavior
  - Four-row native acceptance ledger with explicit independent Windows/CI gaps
affects: ["01-08", "02"]
tech-stack:
  added: []
  patterns: ["preserved per-gate evidence", "strict single-attempt expected rejection", "final child status check after EOF"]
key-files:
  created:
    - scripts/check_foundation.py
    - tests/development/test_foundation_contract.py
    - docs/development/FOUNDATION_ACCEPTANCE.md
    - .planning/phases/01-conan-and-compiler-build-foundation/01-07-EXECUTION.md
    - .planning/phases/01-conan-and-compiler-build-foundation/01-07-SUMMARY.md
  modified:
    - tests/infrastructure/llvm_link_test.cpp
    - scripts/devtools/runner.py
    - tests/development/test_runner.py
    - .planning/STATE.md
    - .planning/ROADMAP.md
key-decisions:
  - Conan remains sole package authority; acceptance never implicitly acquires or repairs
  - Real provider positives use installed native exports/libraries and actual link/run
  - Recognize only the owner-approved exact missing-gtest/no-remote diagnostic pair with exit 1
  - Recover only the failed gate after explicit correction approval, retaining successful earlier evidence
  - Available Linux developer completion does not establish Windows/CI or a general ABI guarantee
requirements-completed: []
requirements-covered: [BUILD-08, BUILD-10, BUILD-12, BUILD-13]
coverage:
  - id: P1
    description: Managed and explicit system/custom provider configure/link/native acceptance
    requirement: BUILD-08
    verification:
      - kind: integration
        ref: "01-07-EXECUTION.md and FOUNDATION_ACCEPTANCE.md — Linux Debug/Release, all three providers CTest 13/13 and four C++23 entries each"
        status: pass
    human_judgment: false
  - id: P2
    description: Nontrivial LLVM Support ownership, native GoogleTest and strict exported-policy rejection
    requirement: BUILD-10
    verification:
      - kind: integration
        ref: "Actual LLVM SmallVector string growth/move/clear/reuse/destruction and infrastructure suites passed in both configurations"
        status: pass
    human_judgment: false
  - id: P3
    description: Warm cached dependencies, acquisition-disabled generation and disposable scoped clean
    requirement: BUILD-12
    verification:
      - kind: integration
        ref: "Separate warm setups reused LLVM/GoogleTest; seven Conan identities unchanged, clean preservation passed Debug/Release before historical offline stops"
        status: pass
    human_judgment: false
  - id: P4
    description: Empty-cache no-remote rejection is narrow, single-attempt and independently recovered
    requirement: BUILD-12
    verification:
      - kind: unit
        ref: "Foundation contract fixtures 16/16; exact diagnostic pair, unexpected exits/success and acquisition reject"
        status: pass
      - kind: integration
        ref: "Debug offline recovery exit 0 (0.589s), Release exit 0 (0.455s); expected Conan exit 1, no download/build/retry"
        status: pass
    human_judgment: false
  - id: P5
    description: Final nonzero child status after EOF propagates without changing success/streaming/liveness
    requirement: BUILD-12
    verification:
      - kind: unit
        ref: "Five deterministic mocked timing tests plus existing Windows mock, 6/6 passed; output preserved and no duplicate launch"
        status: pass
    human_judgment: false
  - id: P6
    description: Reviewed Linux optimized LLVM combinations under actual native libstdc++/header policy
    requirement: BUILD-13
    verification:
      - kind: integration
        ref: "Both consumer configurations passed native Support runtime and version/configuration/triple/runtime/header ABI rejection assertions"
        status: pass
    human_judgment: false
  - id: P7
    description: Independent Windows developer and Windows/Ubuntu CI acceptance, broader ABI and cold-CI capacity
    requirement: BUILD-13
    verification:
      - kind: integration
        ref: "FOUNDATION_ACCEPTANCE.md — Windows Debug/Release and CI explicitly pending; no Linux evidence credited"
        status: unknown
    human_judgment: true
    rationale: "Actual host/runtime/runner observations and broader surfaces unavailable; macOS deferred"
duration: "Owner-gated checkpoints on 2026-10-05; per-command elapsed times in execution ledger"
completed: 2026-10-05
status: complete
completion_scope: available-linux-developer-only
---

# Plan 01-07 — Native foundation acceptance

**Available Linux Debug/Release provider, runtime, warm-cache, clean and offline gates passed; both expected empty-cache/no-remote Conan rejections were correctly recognized after approved bounded matcher and final-exit corrections.**

## Implementation and accepted evidence

Task 1 introduced the portable harness and mocked contract fixtures, extending the LLVM Support test with heap-backed string growth/move/clear/reuse/destruction. It uses existing BuildSelection/Operations/read-only doctor/runner contracts, actual installed native LLVM exports, authoritative profiles/locks, Conan-owned raw cache observations and disposable clean output. No new dependency/provisioning framework, solver, SDK normalization or language compiler semantics.

Independent Linux Debug/Release managed/system/custom configure/build/link/native suites passed 13/13 each; four selected Clang/C++23 compile entries each. All three modes used the same real compatible LLVM 23.1.2 installation, explicitly selected for external modes. Runtime subsets passed 5/5, including both Support tests and strict wrong-version/triple/runtime rejection; full suites also checked wrong configuration/header ABI and include policy. General ABI compatibility and arbitrary external installation support are not inferred.

Separate explicit warm setups passed (Debug 3.555s, Release 3.912s), reusing Conan CLI 2.33.0 and the original LLVM/GoogleTest packages. All seven raw Conan recipe/package revision identities matched before/after. Disposable selected clean preserved sibling configuration, normal outputs, tool environment and actual dedicated cache under read-only metadata comparison. No dependency download, remote enablement, dependency rebuild, forced rebuild, purge or automatic retry. Small consumer probes and acceptance targets did compile/link; they are distinct from cached dependency builds.

Historical Debug all-checks exited 1 at a diagnostic matcher mismatch; Release all-checks later exited 1 at a runner EOF/exit race. Execution stopped each time. Owner approved each bounded correction separately: require the exact missing-gtest/no-remote pair, then check final child returncode before successful runner return. Neither package/runtime policy nor expected-failure matching was weakened. Streaming/liveness and successful-command behavior remain unchanged. No duplicate active invocation or automatic retry.

After correction, Debug offline-only recovery passed in 0.589s and Release recovery in 0.455s. Each used a separate empty home, tracked profiles/lock and --no-remote --build=never; expected child exit 1 was correctly reported and narrowly recognized, harness exit 0. Debug was not rerun during Release recovery. Successful earlier warm/provider/runtime/cache/clean gates remain accepted. The two historical combined runs keep their failed exits and absent final results.json; separate successful recovery results do not fabricate combined success or persisted clean digests.

Latest relevant mocked checks: foundation 16/16 (0.063s), selected mocked runner 6/6 (0.107s), with AST and diff-whitespace checks passed. Existing successful liveness/command/include suites remain current-lifecycle historical evidence and were not unnecessarily rerun. Task 2 created FOUNDATION_ACCEPTANCE.md with four native rows, exact observed Linux tools/runtime/profile/lock/package identities, actual command/result/database references and independently pending environments. It also records earlier Plan 01-02 cold LLVM/package evidence without repeating the build or claiming CI capacity.

## Limits and platform impact

Windows developer SDK/toolset/CRT/iterator/runtime identities, locks and native Debug/Release execution remain pending. Windows CI and Ubuntu Linux CI toolchain identities/four-combination results remain independent/pending. Broader native ABI surfaces, cold-CI capacity, arbitrary external SDKs and native spaced-path behavior remain gaps; existing Linux profile quote warnings preserved. macOS deferred. No phase-wide requirement or dedicated phase-verification acceptance inferred from local task checks.

Platform impact:
- Windows: Portable selection/process fixtures and shared final-exit correction retained; actual native developer/CI evidence pending.
- Linux: Omarchy/Arch Debug/Release execution-task scope complete, including separate accepted offline recoveries; Ubuntu CI independent/pending.
- macOS future: Deferred; standard pathlib/subprocess/Conan/CMake seams preserved without Darwin implementation or acceptance.

## Owner-authorized normal closeout

Owner explicitly instructed available-Linux Plan 01-07 closeout. This step creates only the acceptance ledger and normal SUMMARY, updating EXECUTION/STATE/ROADMAP from preserved evidence. Five closeout files changed; the five pre-existing implementation/test files remain unchanged. Execution packet totals ten files, including the two owner-approved runner/test corrections. Progress is 7/8 plans, zero phases complete; Phase 1 remains executing and Plan 01-08 unstarted. All eight plans/waves, discussion, requirements, profiles, locks and recipes preserved.

No new builds/tests/setup/package operations, implementation edits, staging/commit/push, dedicated phase verification or phase advancement during closeout. Stop for owner review. Recommended separately authorized dedicated checkpoint message: `build: validate native foundation acceptance`.


## Owner-requested nine-file checkpoint — 2026-10-05T17:31:04Z

After reviewed available-Linux execution/closeout, the owner explicitly requested one dedicated checkpoint with message `build: validate native foundation acceptance`, containing exactly:

- scripts/devtools/runner.py
- tests/development/test_runner.py
- scripts/check_foundation.py
- tests/development/test_foundation_contract.py
- docs/development/FOUNDATION_ACCEPTANCE.md
- .planning/phases/01-conan-and-compiler-build-foundation/01-07-EXECUTION.md
- .planning/phases/01-conan-and-compiler-build-foundation/01-07-SUMMARY.md
- .planning/STATE.md
- .planning/ROADMAP.md

The reviewed tests/infrastructure/llvm_link_test.cpp extension was absent from the requested list and remains modified, uncommitted and unstaged. Native 13/13 and nontrivial Support observations record the actual executed working tree including that extension; the nine-file checkpoint does not include it. Do not discard it or silently add it to this commit. All nine selected implementation/docs files retain reviewed behavior/evidence; only normal commit-status wording changed. Resolve resulting commit hash from Git history.

Progress remains 7/8 plans complete for available Linux developer scope, zero completed phases. Both offline gates, deterministic expected-failure/final-exit handling and prior warm/runtime/cache/clean acceptance retained. Windows/CI independently pending; macOS deferred. No new builds/tests/package operations, dependency downloads/remotes/rebuilds, Plan 01-08, dedicated phase verification or phase advancement. Stop for owner review after the requested checkpoint, including the remaining C++ worktree change.
