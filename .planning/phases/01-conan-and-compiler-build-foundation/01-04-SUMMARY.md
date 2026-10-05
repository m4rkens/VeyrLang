---
phase: 01-conan-and-compiler-build-foundation
plan: "04"
subsystem: build
tags: [cmake, ninja, cpp23, llvm, googletest, googlemock, linux]
requires:
  - phase: "01-03"
    provides: Prepared LLVM and GoogleTest packages, native Linux locks and cache-only configure orchestration
provides:
  - Four native Windows/Linux Debug/Release configure/build/test preset definitions
  - Target-oriented C++23 veyrcore and version-only veyrc with isolated LLVM Support integration
  - Independent Linux Debug/Release compile databases and 11 passing CTest cases per configuration
affects: ["01-05", "01-06", "01-07", "01-08"]
tech-stack:
  added: []
  patterns: ["target-scoped C++23 and runtime policy", "LLVM-only imported configuration mapping", "specific-diagnostic policy negatives"]
key-files:
  created:
    - CMakeLists.txt
    - CMakePresets.json
    - cmake/VeyrLLVM.cmake
    - cmake/VeyrTargets.cmake
    - include/veyr/build_info.hpp
    - src/core/build_info.cpp
    - src/driver/main.cpp
    - tests/CMakeLists.txt
    - tests/infrastructure/build_info_test.cpp
    - tests/infrastructure/llvm_link_test.cpp
    - .planning/phases/01-conan-and-compiler-build-foundation/01-04-EXECUTION.md
    - .planning/phases/01-conan-and-compiler-build-foundation/01-04-SUMMARY.md
  modified: [.planning/STATE.md, .planning/ROADMAP.md]
key-decisions:
  - Available Linux developer acceptance closes this plan without claiming Windows or CI completion
  - Owner-approved correction changes only the Linux WrongTriple test expectation, preserving production guards
  - Existing LLVM and matching GoogleTest configurations are reused through cache-only preparation
patterns-established:
  - Core and version-only driver remain independent of LLVM and language syntax
  - Imported LLVM configuration mapping never changes GoogleTest configuration selection
requirements-completed: []
requirements-covered: [BUILD-01, BUILD-08, BUILD-09, BUILD-10]
coverage:
  - id: D1
    description: Native Linux Debug/Release C++23 Ninja core and version-only compiler foundation
    requirement: BUILD-01
    verification:
      - kind: integration
        ref: "01-04-EXECUTION.md — Debug and Release configure/build exit 0; Driver.Version and Driver.UnsupportedArguments pass"
        status: pass
    human_judgment: false
  - id: D2
    description: Exact managed LLVM native exports and Linux runtime/header ABI policy with isolated Support link/run
    requirement: BUILD-08
    verification:
      - kind: integration
        ref: "Debug/Release LLVMIntegration.SupportAllocationUseAndDestruction and five LLVMPolicy negatives pass"
        status: pass
    human_judgment: false
  - id: D3
    description: Separate Linux Debug/Release compile databases contain the selected Clang and C++23 flags
    requirement: BUILD-09
    verification:
      - kind: manual_procedural
        ref: "01-04-EXECUTION.md — read-only inspection of four compile_commands.json entries per configuration"
        status: pass
    human_judgment: false
  - id: D4
    description: Conan GoogleTest/GoogleMock infrastructure cases execute in both Linux configurations
    requirement: BUILD-10
    verification:
      - kind: integration
        ref: "01-04-ctest-Debug-approved.log and 01-04-ctest-Release.log — 11/11 passed each"
        status: pass
    human_judgment: false
  - id: D5
    description: Independent Windows native Debug/Release and Windows/Ubuntu CI acceptance
    verification:
      - kind: integration
        ref: "Native Windows toolchain/locks/runtime and independent CI observations remain pending"
        status: unknown
    human_judgment: true
    rationale: "Windows preset definitions and Linux results do not prove actual Windows or CI readiness"
  - id: D6
    description: Positive external-provider compatibility and native paths containing spaces
    requirement: BUILD-08
    verification:
      - kind: integration
        ref: "No positive external native-provider or paths-with-spaces acceptance run in this plan; later bounded acceptance remains pending"
        status: unknown
    human_judgment: true
    rationale: "Written provider boundaries and synthetic export negatives do not prove all external installations or path layouts"
duration: 22min
completed: 2026-10-05
status: complete
completion_scope: available-linux-developer-only
---

# Plan 01-04 — CMake foundation and Linux infrastructure acceptance

**C++23/Ninja core and version-only compiler targets, exact isolated LLVM Support integration, and Conan GoogleTest/GoogleMock pass independent Linux Debug/Release validation.**

## Accomplishments and acceptance limits

- Created the ten planned implementation/configuration files. Four concrete Windows/Linux Debug/Release configure/build/test preset definitions consume the selected build directory, generated Conan toolchain and reviewed Python through child environment inputs. Development configurations export compile_commands.json.
- CMake remains the target graph; Conan owns dependency/package mechanics. Exact managed/system/custom discovery has no acquisition fallback. Static optimized X86 LLVM, generated header ABI, exported platform/runtime identities and the selected consumer runtime are checked; imported configuration mapping is scoped to LLVM.
- veyrcore implements only veyr::build_version(); veyrc supports --version and rejects unsupported arguments. Core/driver stay independent of LLVM. No source manager, diagnostics, tokens, parser, semantic pipeline, IR or .vyr syntax was added. Build identity 0.1.0 is compiler infrastructure and does not consume the future-product Veyr.toml sketch.
- Separate Linux Debug and Release native configuration/build/CTest evidence passed. Both CTest suites passed all 11 cases: three build-identity/GoogleMock output cases, LLVM Support allocation/use/destruction, driver version and unsupported arguments, and five version/configuration/triple/runtime/header-policy negatives. Specific-diagnostic synthetic fixtures are test assertions, not Windows evidence.
- Four compile database entries per configuration were inspected: selected Clang, -std=c++23, Debug -g or Release -O3/-DNDEBUG; no LLVM includes on core/driver, exact LLVM header definitions on its isolated test.

This is available Omarchy/Arch Linux developer closeout only. Phase-wide BUILD requirements remain pending; Windows developer/locks/runtime and Windows/Ubuntu CI evidence are independent and pending. macOS remains deferred. Positive external-provider and native paths-with-spaces checks are not claimed complete.

## Existing execution evidence

All command cwd values were /home/m4rkens/Projects/VeyrLang. Exact commands, timestamps, historical failure and owner-approved resumption are preserved in 01-04-EXECUTION.md. No command was repeated for this closeout.

| Check | Existing result | Ignored log |
|---|---|---|
| Original Debug configure/build | exit 0; build 16 Ninja steps, then four steps after the bounded GoogleMock callable warning correction | 01-04-configure-Debug.log, 01-04-build-Debug.log, 01-04-build-Debug-callable.log |
| Final Debug configure | exit 0, 1.253s | 01-04-configure-Debug-approved.log |
| Final Debug CTest | exit 0, 11/11, 0.226s | 01-04-ctest-Debug-approved.log |
| Independent Release configure | exit 0, 1.702s | 01-04-configure-Release.log |
| Independent Release build | exit 0, 16 Ninja steps, 3.989s | 01-04-build-Release.log |
| Independent Release CTest | exit 0, 11/11, 0.244s | 01-04-ctest-Release.log |

Logs live under ignored .veyr-dev/logs. Prior 22 mocked orchestration checks and presets JSON/diff checks passed during implementation; completed tests were not repeated during the bounded resumption or closeout.

Each final configure used the project-owned Conan 2.33.0 absolute executable, the selected dedicated home, native profiles/lock and --no-remote --build=never. LLVM and matching Debug/Release GoogleTest packages were Cache/Already installed. No dependency download, source/package rebuild, CLI reinstall, recipe export, new lock generation, purge or force-build occurred. Release built only Veyr-owned foundation/test targets; final Debug reused existing executables.

Original LLVM remained llvm/23.1.2@veyr/m1#aa4cad25b3148c3bdd22ed96fba04654:a214d896e569f9b075f68e0b581ce595d09d80df#cd9a88c23979fe65c10c97b796943a64. Manifest SHA256 remained 2eba146976c0f4460a0fb60fdf7363eb9c91a00b9a9926b47b893f0dd6cb297e. Both native locks retained SHA256 b76a5f857ba40da4a59d2bdcd8690fba6d12d15d53bd3b0dc6aceb252585f30b. GoogleTest Debug/Release package IDs/revisions from Plan 01-03 were reused. Production guard/configuration and Conan manifests/locks were hash-checked unchanged throughout the final correction/validation; only tests/CMakeLists.txt WrongTriple expectation was corrected.

## Issues and bounded corrections

The first Debug CTest stopped at exit 8: production WrongTriple guard rejected correctly, but its test pattern omitted GNU. Execution stopped without repair/retry. The owner explicitly approved tests/CMakeLists.txt only: Linux now requires the full intended Linux/x86_64/GNU policy diagnostic; the existing Windows expectation remains unchanged. Approved Debug fixture regeneration/CTest and independent Release validation then all exited 0. No guard weakening or new scope.

Earlier successful Debug build reported deprecated GoogleMock Invoke(callable). Directly passing the callable removed that warning within the planned test file before the first CTest; the affected test target was rebuilt after that source edit. This is separate from the final expectation-only resumption. Existing profile quotation warnings remain visible; no profile edits or diagnostic suppression. Broader native ABI/provider/path and independent CI capacity acceptance remain pending.

## Files created or modified

Implementation/configuration: CMakeLists.txt, CMakePresets.json, cmake/VeyrLLVM.cmake, cmake/VeyrTargets.cmake, include/veyr/build_info.hpp, src/core/build_info.cpp, src/driver/main.cpp, tests/CMakeLists.txt, tests/infrastructure/build_info_test.cpp, tests/infrastructure/llvm_link_test.cpp.

Normal execution/closeout artifacts: 01-04-EXECUTION.md, 01-04-SUMMARY.md, .planning/STATE.md and .planning/ROADMAP.md. Total Plan 01-04 packet: 14 files. This closeout step changes only those four normal artifacts; it preserves the ten reviewed implementation/configuration files, prior plans/summaries, discussion and all eight waves.

## Commits and next boundary

The owner reviewed the completed execution/closeout and explicitly requested one dedicated checkpoint with message `build: establish CMake foundation and validate Linux tests`. This summary and all 14 scoped files are included in that checkpoint; resolve its hash from Git history. No separate task or automatic metadata commit.

Plan 01-04 Tasks 1–2 are complete for available Linux developer scope; STATE/ROADMAP record 4/8, zero completed phases. Plan 01-05 remains unstarted. No new builds/tests/package operations ran for closeout; no separate phase verification, next-plan execution or phase advancement authorized. Stop for owner review.

Platform impact:
- Windows: Concrete clang-cl/v143 runtime/iterator policy and presets exist; actual native locks/CRT/SDK and CI evidence remain pending.
- Linux: Observed Omarchy/Arch Debug/Release configure/build/CTest and compile databases passed independently; Ubuntu CI does not inherit this identity.
- macOS future: Deferred; platform-gated policy and standard CMake/Conan seams preserve bring-up feasibility.

## Self-check

Closeout reuses recorded exit-0 evidence. Exact four artifact scope, 4/8 progress, zero completed phases, pending Windows/CI/ABI/provider gaps and unchanged protected implementation/planning documents checked by read-only inspection. No phase-wide requirement completion or all-platform acceptance claimed.
