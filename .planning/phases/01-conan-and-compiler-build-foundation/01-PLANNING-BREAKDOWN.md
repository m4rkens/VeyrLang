# Phase 1 — Approved Planning Breakdown

**Checkpoint:** confirm_breakdown resolved; completed-planning owner review next
**Status:** Owner selected option 1 on 2026-10-04; eight PLAN.md files written and checked
**Captured:** 2026-10-04
**Inputs:** Committed 01-CONTEXT.md and 01-DISCUSSION-LOG.md, canonical requirements/roadmap, resumed 01-RESEARCH.md
**Execution policy:** No implementation, tools/packages/builds/tests, commits or lifecycle advancement performed

## Reachable Outcome

A contributor selects a reviewed native profile and Debug/Release configuration, validates prerequisites with doctor, explicitly prepares Conan packages with setup, then configures/builds/tests using prepared dependencies. Managed LLVM comes from Conan; explicit system/custom providers pass the same compatibility contract. Include checking, a compile database and infrastructure tests are reachable through the developer interface. Four GitHub Actions combinations establish Tier 1 acceptance during later authorized work.

This is infrastructure for P01 and PLAT_06. No .vyr syntax/compiler pipeline or future language project/package system is introduced.

## Owner-approved baseline correction — 2026-10-04

The eight-wave/task organization remains approved and unchanged. Linux developer candidate is the observed Omarchy/Arch x86_64 host; Ubuntu 24.04 is a distinct later CI environment with independently pending toolchain identity. Windows developer evidence waits for its actual host; Windows CI evidence is separately pending. Conan 2.33.0 remains required and missing, LLVM backend remains 23.1.2, and no package/ABI readiness is inferred. Scope D-07/D-14 through this dated owner clarification while preserving the committed discussion snapshot and shared commands/dependency policy. These planning corrections do not resume execution.

## Plan and Wave Breakdown

Eight approved plans, eighteen tasks, sequential waves. Parallelization remains disabled. Each plan owns a coherent concern; shared script/CMake files appear only in later dependent waves.

| Plan | Wave | Depends on | Concern / proposed tasks | Requirements |
|---|---:|---|---|---|
| 01-01 | 1 | — | Inspect/select exact environment-specific developer candidates; owner checkpoint separates selection from missing prerequisite readiness; implement strict doctor and project-local Conan CLI-only bootstrap with mocked isolation/reuse/failure tests | BUILD-05, BUILD-06, BUILD-07, BUILD-13 |
| 01-02 | 2 | 01-01 | Implement narrow exact LLVM recipe/options/source integrity/native exports; implement Conan test_package proving exported headers/targets and matching runtime link/run | BUILD-03, BUILD-04, BUILD-13 |
| 01-03 | 3 | 01-01, 01-02 | Add exact consumer refs/generators/profiles and Conan-native lock generation/review; extend setup with Conan-owned dependency/package work after Plan 01-01 CLI bootstrap, dedicated Conan home and cache-only prepared generation with boundary tests | BUILD-02, BUILD-06, BUILD-12 |
| 01-04 | 4 | 01-03 | Add target-oriented C++23/Ninja/presets/LLVM provider integration and compile database; wire independent veyrcore/version stub plus Conan GoogleTest/GoogleMock infrastructure harness | BUILD-01, BUILD-08, BUILD-09, BUILD-10 |
| 01-05 | 5 | 01-04 | Implement owned-header include checker with positive/negative fixtures; integrate check-includes target and CTest entry | BUILD-11 |
| 01-06 | 6 | 01-03, 01-04, 01-05 | Complete configure/build/test/verify/all/clean orchestration without duplicate acquisition/work; supervise child commands and test quiet progress, confirmed stall and clean/chaining boundaries | BUILD-06, BUILD-12 |
| 01-07 | 7 | 01-06 | Add provider/runtime/offline/cache integration acceptance checks; run authorized native Debug/Release foundation acceptance and document actual results/gaps | BUILD-08, BUILD-10, BUILD-12, BUILD-13 |
| 01-08 | 8 | 01-07 | Add four-combination GitHub Actions with independently observed CI identities, distinct bootstrap/native steps and cache-disabled path; document shared commands/dependency policy; owner checkpoint for remote/CI evidence or capacity | BUILD-14, BUILD-06, BUILD-07, BUILD-12, BUILD-13 |

This is the approved organization for the written PLAN.md files, not execution authorization. Plans 01-01 and 01-08 have autonomous: false because of their checkpoints. Other plans remain bounded by the project's execution/review/commit gates.

## Intended File Ownership

Paths below are future implementation locations, not files created during planning.

| Plan | Intended files |
|---|---|
| 01-01 | conan/profiles/linux-x86_64-clang; conan/profiles/windows-x86_64-clangcl; conan/profiles/debug; conan/profiles/release; scripts/dev.py; scripts/devtools/baseline.json; scripts/devtools/doctor.py; tests/development/test_doctor.py; docs/development/DEVELOPER_BOOTSTRAP.md |
| 01-02 | conan/recipes/llvm/conanfile.py; conan/recipes/llvm/conandata.yml; conan/recipes/llvm/test_package/conanfile.py; conan/recipes/llvm/test_package/CMakeLists.txt; conan/recipes/llvm/test_package/src/llvm_probe.cpp |
| 01-03 | conanfile.py; conan/locks/linux-x86_64-clang-Debug.lock; conan/locks/linux-x86_64-clang-Release.lock; conan/locks/windows-x86_64-clangcl-Debug.lock; conan/locks/windows-x86_64-clangcl-Release.lock; scripts/dev.py; scripts/devtools/conan_ops.py; tests/development/test_conan_ops.py; .gitignore |
| 01-04 | CMakeLists.txt; CMakePresets.json; cmake/VeyrLLVM.cmake; cmake/VeyrTargets.cmake; include/veyr/build_info.hpp; src/core/build_info.cpp; src/driver/main.cpp; tests/CMakeLists.txt; tests/infrastructure/build_info_test.cpp; tests/infrastructure/llvm_link_test.cpp |
| 01-05 | scripts/check_includes.py; tests/development/test_include_policy.py; CMakeLists.txt; tests/CMakeLists.txt |
| 01-06 | scripts/dev.py; scripts/devtools/runner.py; tests/development/test_dev_commands.py; tests/development/test_runner.py |
| 01-07 | scripts/check_foundation.py; tests/development/test_foundation_contract.py; tests/infrastructure/llvm_link_test.cpp; docs/development/FOUNDATION_ACCEPTANCE.md |
| 01-08 | .github/workflows/build-foundation.yml; scripts/devtools/baseline.json (observed CI expectations only); docs/development/DEVELOPER_BOOTSTRAP.md; docs/development/CI_MATRIX_POLICY.md; docs/development/FOUNDATION_ACCEPTANCE.md; README.md |

Files shared across plans are intentional sequential updates. The initial CMake plan is limited to ten files and two cohesive tasks. Later plan review must split it if task complexity exceeds the planner's context target. Lockfiles are Conan outputs created and reviewed only during execution; planning does not fabricate revision hashes.

## Required Wiring and Proof

- Baseline/profile selection feeds doctor, Conan host/build profiles, CMake presets and CI.
- Narrow recipe's native LLVM install feeds managed exact CONFIG discovery.
- Plan 01-01 Task 3 owns project-local Conan CLI-only bootstrap before Wave 2; Plan 01-03 extends full setup with Conan-owned dependency/package acquisition/source builds. Cache-only install feeds configure. All Conan children use BuildSelection.conan_executable and the dedicated home. Normal clean preserves both CLI environment and package cache.
- CMake links imported GoogleTest and isolated LLVM infrastructure targets; veyrcore stays independent of LLVM.
- Include checker feeds both check-includes and CTest; verify composes include/native checks once.
- CI invokes the same entry point and dependency/profile policy with independently reviewed environment inputs; four native outcomes and cold-build capacity are observable evidence, never inferred from YAML or developer inventory.

## Unresolved Execution Gates

1. Omarchy developer candidate evidence is recorded; Conan 2.33.0 is missing and readiness remains unproven. Exact Windows developer VCToolsVersion/CRT/SDK/linker requires actual host observations; Ubuntu Linux CI and Windows CI identities are separately pending before their readiness is accepted.
2. Optimized LLVM plus Windows Debug CRT must pass source build, native exports and link/run checks. Failure stops for owner review; no ABI weakening/provider fallback.
3. Official LLVM source integrity and Conan revisions must be recorded through normal recipe/lock mechanics, not invented here.
4. Hosted cold LLVM jobs must fit actual runner resources and the six-hour hard limit. Capacity failure requires an owner decision.
5. GitHub workflow publication/remote runs need explicit authorization. If evidence is unavailable, record pending four-combination CI acceptance rather than claiming Phase 1 complete.

## Platform Impact

Platform impact:
- Windows: clang-cl, exact VS/SDK/CRT identity, runtime-specific optimized LLVM packages; argument/path spaces, process termination and include-case checks.
- Linux: Omarchy/Arch developer Clang 22/GCC 16/libstdc++/glibc/linker candidate and independently pending Ubuntu CI identity; equivalent process/cache/offline/clean behavior and unproven ABI.
- macOS future: No Darwin implementation/runner. Preserve host/target identity, pathlib/argument-array boundaries and standard Conan/CMake integration.

## Project-System Impact Packet

Files affected: Planning artifacts only now; future paths listed above.
Manifest/build behavior changed: None now. Planned compiler implementation dependency/build inputs only.
Targets affected: Future veyrcore/veyrc infrastructure, tests and isolated LLVM probe.
Cross-platform effect: Four Windows/Linux native combinations; macOS deferred.
Compatibility/open questions: Runtime proof, exact host observations, source integrity and CI capacity as above.
Owner review required before commit: Yes for future sensitive implementation files; no commit is authorized now.

## Resume Contract

Completed: Owner confirmed this breakdown; 01-01-PLAN.md through 01-08-PLAN.md are written, task-level validation is mapped, and inline plan-check/source-grounding review is recorded in 01-PLAN-REVIEW.md.

Next action: Owner reviews completed planning and explicitly requests its dedicated commit if accepted. Do not execute or commit automatically.

The interactive gsd-planner contract says: “Present breakdown with wave structure. Wait for confirmation in interactive mode.” This gate was satisfied by owner option 1; config remains interactive and no execution/commit approval is inferred.
