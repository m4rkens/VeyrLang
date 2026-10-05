---
phase: 01-conan-and-compiler-build-foundation
plan: "02"
subsystem: dependency-packaging
tags: [conan, llvm, cmake, ninja, linux, native-exports, cache]
requires:
  - phase: "01-01"
    provides: Reviewed Linux developer baseline, native profiles and project-owned Conan CLI
provides:
  - Narrow Conan-owned LLVM 23.1.2 source/build/package recipe with upstream native CMake exports
  - Linux Debug and Release C++23 static Support configure/compile-link/native-run evidence
  - Same-package cache reuse evidence for both Linux consumer configurations
affects: [01-03, 01-04, 01-07, 01-08]
tech-stack:
  added: [LLVM 23.1.2 packaged through Conan 2.33.0]
  patterns: [upstream native CMake exports, explicit test_package, optimized runtime-matching package reuse]
key-files:
  created:
    - conan/recipes/llvm/conanfile.py
    - conan/recipes/llvm/conandata.yml
    - conan/recipes/llvm/test_package/conanfile.py
    - conan/recipes/llvm/test_package/CMakeLists.txt
    - conan/recipes/llvm/test_package/src/llvm_probe.cpp
  modified: []
key-decisions:
  - Conan owns source acquisition/checksum verification, build/install, package identity and cache.
  - Native LLVM config/exports remain authoritative; no custom SDK or generated replacement config.
  - LLVM_ABI_BREAKING_CHECKS exports the FORCE_OFF policy string; the probe compares it explicitly and retains the header check for zero.
  - Acceptance is limited to available Omarchy/Arch Linux developer evidence; Windows and CI remain independent and pending.
patterns-established:
  - Absolute project-owned Conan invocation with the dedicated external project home and reviewed host/build profiles
  - Separate Debug/Release native consumers reuse one ABI-matching optimized LLVM package
  - Non-zero command stops once; correction/resumption requires owner instruction
requirements-completed: []
requirements-covered: [BUILD-03, BUILD-04, BUILD-13]
coverage:
  - id: llvm-linux-package
    description: Exact LLVM 23.1.2 source/checksum/build/install and native config/static exports on the Linux developer host
    requirement: BUILD-03
    verification:
      - kind: integration
        ref: "01-02-EXECUTION.md#linux-debug-invocation--started-2026-10-05t084538z; source/build/package passed before the historical consumer-guard failure"
        status: pass
    human_judgment: false
  - id: llvm-linux-debug-consumer
    description: Linux Debug C++23 consumer configures, compiles/links and runs static LLVM Support using the original cached package
    requirement: BUILD-13
    verification:
      - kind: integration
        ref: "01-02-EXECUTION.md#separate-linux-debug-validation; supervised Conan create with Debug host overlay, exit 0"
        status: pass
    human_judgment: false
  - id: llvm-linux-release-consumer
    description: Linux Release C++23 consumer configures, compiles/links and runs static LLVM Support using the same cached package
    requirement: BUILD-13
    verification:
      - kind: integration
        ref: "01-02-EXECUTION.md#separate-linux-release-validation; supervised Conan create with Release host overlay, exit 0"
        status: pass
    human_judgment: false
  - id: llvm-windows-native-acceptance
    description: Intended Windows Debug/Release runtime combinations remain unaccepted pending independent native evidence
    requirement: BUILD-13
    verification: []
    human_judgment: true
    rationale: Actual Windows host identities and native CRT/config/link/run evidence are independently pending; Linux results do not validate Windows.
duration: "Owner-gated sessions; initial create approximately 11m11s, final cached validations approximately 2s each"
completed: 2026-10-05
status: complete
acceptance-scope: Available Omarchy/Arch Linux x86_64 developer recipe and Debug/Release static Support consumers
pending-environments: [windows-developer, ubuntu-ci, windows-ci]
native-compatibility: Validated only for the tested Linux static Support Debug/Release consumers
package-readiness: Exact LLVM package accepted on the available Linux developer host; other environments and broader ABI surfaces remain pending
---

# Phase 1 Plan 02: LLVM Conan package and native consumers

**LLVM 23.1.2 builds through a narrow Conan recipe, preserves native exports, and passes separate Linux Debug/Release Support consumers using one cached package.**

## Accomplishments

- Task 1 built and packaged LLVM 23.1.2 successfully through Conan source/get and CMake helpers. Static X86 target selection, disabled optional dependency facilities/projects/runtimes/tests, assertions OFF and ABI policy FORCE_OFF remain inspectable. Full upstream native config/exports, headers, required static libraries and license are retained; no custom acquisition graph, package IDs, SDK normalization or cache subsystem.
- Task 2's Debug consumer passed exact CONFIG discovery, configuration/generation, C++23 compilation/linking and native Support allocation/destruction run, exit 0.
- A separate Release consumer passed the same checks and native run, exit 0. Both selected the exact original cached optimized LLVM package; neither rebuilt LLVM or dependencies during final validation.
- Owner explicitly authorized this available-Linux closeout. Windows developer/CI and Ubuntu CI evidence remain independently pending; macOS remains deferred. Phase 1 remains executing, now 2/8 plans complete within available-host scope.

## Recorded evidence

Detailed commands, output, package settings, failure/correction history and logs are in [01-02-EXECUTION.md](01-02-EXECUTION.md). This closeout reuses that evidence without running commands again.

Official source asset: `llvm-project-23.1.2.src.tar.xz`; expected SHA-256 `c98bbef08a2b4c2613cd50e9aa9ae7b69b1fe6c16b2c40373bc0ab6116fdf78a`, sourced from [official release metadata](https://api.github.com/repos/llvm/llvm-project/releases/tags/llvmorg-23.1.2). Conan checked the checksum and extracted the intact monorepo with shared cmake/libc siblings before the successful source build.

Full package reference, identical in both final consumer graphs:

```text
llvm/23.1.2@veyr/m1#aa4cad25b3148c3bdd22ed96fba04654:a214d896e569f9b075f68e0b581ce595d09d80df#cd9a88c23979fe65c10c97b796943a64
```

Conan executable: `/home/m4rkens/Projects/VeyrLang/.veyr-dev/tools/conan-2.33.0/bin/conan`.
Dedicated home: `/home/m4rkens/.cache/veyr/conan/22e2b2472775`.
Package directory: that home's `p/b/llvm3c80d92357c17/p`.
Reviewed native profiles select Linux x86_64, Clang 22/libstdc++11, cppstd 23 and Release LLVM; each consumer has its own Debug/Release overlay/build directory.

| Linux developer consumer | Configure | Compile/link | Native run | LLVM package |
|---|---|---|---|---|
| Debug | Passed, 0.4s | Passed | Passed, exit 0 | Original cached Release package |
| Release | Passed, 0.5s | Passed | Passed, exit 0 | Same original cached Release package |

Both report `Already installed!`; both probes print `LLVM 23.1.2: Support allocation/destruction passed`. Four consumer Ninja steps ran for each configuration. Package manifest SHA-256 remained `2eba146976c0f4460a0fb60fdf7363eb9c91a00b9a9926b47b893f0dd6cb297e` before/after final validation. No LLVM/dependency rebuild, download, forced build or package-cache cleanup occurred during those final checks; only consumer probes were compiled/linked.

Recorded native command window: initial create started 2026-10-05T08:45:38Z; final Release output 2026-10-05T09:11:10.719Z, including intervening owner review. Initial source/build/package command took approximately 11m11s; final cached Debug/Release commands approximately 2s each. Two tasks complete for available Linux scope; five implementation files produced, four normal closeout/checkpoint files written or updated. Owner-authorized closeout recorded 2026-10-05T09:20:24Z.

## Issues and owner-approved correction

The first Debug create built/packaged LLVM successfully but exited 1 at the new consumer guard, before compilation/link/run. The guard incorrectly treated LLVM_ABI_BREAKING_CHECKS `FORCE_OFF` as a boolean. Execution stopped once for owner review. Owner then approved the one-file explicit string comparison; the C++ header check for LLVM_ENABLE_ABI_BREAKING_CHECKS zero remained unchanged. Both subsequent separately authorized consumer commands passed. No automatic retry, provider/version/runtime fallback or recipe expansion.

Exact upstream source exposes no LLVM_ENABLE_TERMINFO option, so no unused flag was passed. Pre-existing Linux profile quoting warnings remain recorded (16 per successful consumer configure); profiles were unchanged and paths with spaces are not validated by these results.

## Commits and remaining boundaries

No separate task commits. After reviewing completed execution and closeout, the owner explicitly requested one dedicated checkpoint: `build: package LLVM 23.1.2 and validate Linux consumers`. This summary is included with the five implementation files, execution checkpoint, STATE.md and ROADMAP.md; its commit hash is available from Git history. Earlier discussion, planning and Plan 01-01 commits remain separate. Generated tool/cache/build/log state is excluded.

This closes Plan 01-02 for the owner-approved available Linux scope, not independent Windows/CI acceptance or all LLVM ABI surfaces. Windows optimized Debug-CRT/Release-CRT combinations require real native identities and configure/link/run evidence. Ubuntu CI, Windows CI and cold-build CI capacity remain independent execution checks. Linux developer timing is not CI capacity evidence.

BUILD-03/04/13 receive plan-level contributions; `requirements-completed` stays empty and REQUIREMENTS.md remains unchanged because phase-wide platform/integration acceptance is pending. All eight waves and completed discussion/Plan 01-01 artifacts are preserved. Plan 01-03 remains unstarted and requires a later explicit owner execution instruction after the current review/commit disposition. No automatic commit, dedicated phase verification, phase completion or advancement.

Platform impact:
- Windows: Default Conan runtime identities and native CMake probe contract retained; developer/CI, CRT and native acceptance independently pending.
- Linux: Omarchy/Arch developer recipe/package and both consumer configurations accepted; Ubuntu CI identity/evidence remains separate.
- macOS future: Standard Conan/native CMake seams retained; no Darwin implementation or CI, macOS deferred.
