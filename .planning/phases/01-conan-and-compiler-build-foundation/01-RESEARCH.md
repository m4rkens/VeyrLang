# Phase 1: Conan and Compiler Build Foundation — Research

**Research consulted:** 2026-10-03, during the interrupted planning invocation
**Findings persisted:** 2026-10-04
**Domain:** Conan 2, LLVM 23.1.2, native C++23 build foundation
**Status:** Documentary findings captured; proposed breakdown awaits owner confirmation
**Method:** Inline research role; no sub-agents, package operations, builds or tests

## User Constraints

The following decision/discretion and deferred sections are copied verbatim from the committed discussion context. Recommendations below do not revise them.

## Implementation Decisions

### Developer commands
- **D-01:** Provide one Python entry point with the same command interface on Windows/Linux. Separate shell/PowerShell convenience launchers are not required. `python scripts/dev.py setup` was an illustrative command; the exact script path was not separately selected.
- **D-02:** Normal output includes step headers, the selected profile/configuration, the invoked command, and live Conan/CMake/Ninja/CTest output. Long quiet operations remain supervised; silence alone is not failure.
- **D-03:** Allow limited preparation chaining: `build` may configure and `test` may build. These commands must not implicitly perform dependency setup, acquisition, or missing-package source builds. Missing prepared dependencies produce actionable setup instructions.
- **D-04:** Routine commands select a reviewed native Windows/Linux profile and the development configuration, print their selections, and allow explicit overrides to supported profiles/configurations. Do not adopt `conan profile detect` output as canonical policy.

### Debug/Release support
- **D-05:** Support Debug/Release Veyr builds while using optimized LLVM packages where runtime/ABI compatibility is verified. This is a preference, not evidence that every Debug/Release dependency combination is valid. Research must establish each supported combination; never silently substitute an incompatible runtime or fall back to another provider. A dedicated Debug LLVM debugging configuration is not required in Phase 1.
- **D-06:** Debug is the default Veyr development configuration; Release remains explicitly selectable.
- **D-07:** Start with one reviewed tool/runtime baseline per Tier 1 platform. Document exact versions and validate Debug/Release against each baseline. Exact Python/Conan/CMake/Ninja/Clang/Git and platform SDK/runtime choices remain research/planning work.
- **D-08:** Normal commands stop when the environment falls outside the reviewed baseline. Report the mismatch and expected configuration with actionable instructions; do not automatically accept an unsupported environment or introduce experimental opt-in as the normal workflow.

### Network and cache behavior
- **D-09:** Explicit `setup` permits Conan downloads and missing-package source builds. Show what Conan will do. A long first LLVM build requires no extra per-package confirmation once the setup operation is authorized. This does not authorize running setup during GSD discussion/planning or bypass tool-level execution permissions.
- **D-10:** `all` explicitly includes setup -> configure -> build -> test -> verify, and documents possible dependency downloads/source builds. Dependency acquisition stays in the visible setup step. This is a developer build operation, not permission to advance GSD lifecycle stages. Avoid duplicate work when implementing the composed operations.
- **D-11:** Use a dedicated Conan home per project by default, outside tracked source and fully Conan-owned. Do not default to the standard shared user Conan home, mirror packages into a Veyr store, or define a Veyr package identity/solver/cache format. Exact location and project/worktree identity rules remain research/planning details. Veyr wrapper receipts and Conan package state retain separate ownership.
- **D-12:** Ordinary `clean` removes only the selected profile/configuration's build output. Preserve other configurations and Conan's cache. Package/cache deletion must be a separate explicit destructive action; no automatic cache cleanup, duplicate retries, or repair loops.

### Windows/Linux evidence and CI
- **D-13:** Phase 1 Tier 1 acceptance requires passing CI on both Windows and Linux. Local checks supplement CI; Linux-only execution cannot establish Windows support. All acceptance evidence remains pending until authorized execution/verification.
- **D-14:** Use GitHub Actions workflows in this repository. CI uses the same reviewed profiles/configuration and Python developer commands as local development.
- **D-15:** Trigger CI on pull requests, pushes to `main`, and manual runs.
- **D-16:** Routine CI covers Windows x86_64 and Linux x86_64, each with Debug and Release Veyr configurations: four combinations. Keep bootstrap validation and configure/build/test validation logically distinct as required by CI policy. Restore Conan-owned caches only as an optimization; a deliberate clean source-build path must remain possible.

### Research and planning discretion

The owner did not delegate any listed preference via a "you decide" answer. Research/planning may select implementation details within these boundaries: exact script location, flags/preset names, version pins, Conan recipe/options/lock integration, cache location, and portable orchestration structure. Those selections belong in the separately authorized research/plan, not executable files produced during discussion.

Required research gates:
- Recheck exact LLVM 23.1.2 recipe suitability and native CMake exports/targets; prefer the existing recipe only if suitable, otherwise the narrow Conan recipe path. Do not substitute an LLVM version silently.
- Select exact dependency versions/revisions and only necessary LLVM options/components/backends. Include zlib/zstd/LibXml2 only when selected LLVM options require them; GoogleTest/GoogleMock remains Conan-owned.
- Establish runtime/ABI-compatible Debug/Release combinations for Windows clang-cl and Linux Clang. If optimized LLVM cannot support a requested combination, record the conflict for owner review rather than treating this discussion as permission to bypass compatibility checks.
- Define strict baseline diagnostics and reproducible tool/SDK selection for local hosts and CI, without creating host-tool/package provisioning infrastructure.
- Resolve dedicated Conan-home location and cache identity using Conan-native mechanisms. Keep package state untracked and separate from optional wrapper receipts and future language-level `Veyr.lock`.
- Define prepared-dependency, acquisition-disabled configure/build behavior. Preserve the explicit managed/system/custom LLVM discovery boundary; managed means Conan, and external modes never acquire dependencies secretly.
- Check GitHub Actions runner resources/hard limits and cache support against expensive LLVM source builds. A quiet-output threshold is not a hard timeout. If CI capacity cannot satisfy the selected matrix/source-build path, report the concrete constraint for owner review; do not invent custom SDK acquisition or narrow Tier 1 acceptance silently.
- Define meaningful compiler-foundation tests, compile database export, and the owned-header include guardrail/build target/CTest entry. Do not create lexer or smoke-syntax tests in Phase 1.


## Deferred Ideas

None introduced as new capabilities. A dedicated Debug LLVM configuration, broader toolchain-version support, convenience launchers, and experimental unsupported-environment opt-in were alternatives not selected for Phase 1; they are not authorized future tasks.

The canonical exclusions remain: macOS implementation, cross-compilation bring-up, full language/project/package tooling, general provisioning/provenance systems, and later M1 compiler stages.


## Project Constraints (from AGENTS.md)

[VERIFIED: repository] Read canonical design/development/project-system docs; explicit owner decisions and DECISION_LOG win. Preserve Veyr/veyrc/veyrcore names and component separation. Planning writes only .planning artifacts; execution requires the exact owner command. No automatic stage/phase movement or commits. Implementation-sensitive commits require their separate review gate.

[VERIFIED: repository] C++23, CMake/Ninja, Clang, Conan 2, GoogleTest/GoogleMock, LLVM CONFIG discovery, compile database and owned-header angle includes are mandatory. Tier 1 is Windows/Linux x86_64; future macOS must remain feasible. Long quiet commands are supervised; 60 seconds is a liveness checkpoint. No prior-run implementation, evidence, lifecycle artifacts, custom package management or SDK normalization.

[VERIFIED: repository] Phase 1 delivers infrastructure only. No language syntax, lexer, parser, AST, diagnostics implementation, semantic analysis, smoke compilation or future Veyr.toml/Veyr.lock behavior. Phase 3's reviewed provisional profile precedes syntax-dependent work.

## Summary

[CITED: official sources consulted before interruption] ConanCenter's llvm-core version index stops at 19.1.7, while the selected backend is 23.1.2. Use the allowed narrow project-owned Conan recipe for that exact version. The older recipe also removes native LLVM CMake config/exports during packaging; a version bump alone would not preserve the required native discovery boundary. [LLVM recipe index](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/llvm-core/config.yml), [existing recipe](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/llvm-core/all/conanfile.py).

[PROPOSED] Build only X86 and required static LLVM components. Keep LLVM optimized for Veyr Debug/Release only where runtime and exported ABI settings match. Windows needs two runtime-specific package identities. This is a validation candidate, not proven platform support. Source-build cost and CI capacity remain measured execution gates.

## Phase Requirements

[VERIFIED: repository] Exact requirement text follows; support is allocated in the proposed breakdown and validation contract.

| ID | Description |
|---|---|
| BUILD-01 | Contributor can configure and build the C++23 compiler foundation with target-oriented CMake, Ninja, and the selected Clang toolchain. |
| BUILD-02 | Contributor can resolve compiler implementation dependencies through reviewed Conan 2 configuration with exact dependency references, rather than Veyr-owned package mechanics. |
| BUILD-03 | Contributor can consume LLVM 23.1.2 through a suitable Conan recipe, or a narrow project-owned Conan recipe when an exact suitable recipe is unavailable. |
| BUILD-04 | Contributor can inspect the selected LLVM options and Conan dependency configuration for zlib, zstd, and LibXml2 when required, without enabling unrelated LLVM projects or backends. |
| BUILD-05 | Contributor can select explicit reviewed Windows x86_64 clang-cl and Linux x86_64 Clang host/build profiles with distinct runtime and ABI settings. |
| BUILD-06 | Contributor can invoke equivalent setup, doctor, configure, build, test, verify, and all developer operations on Windows and Linux through thin Python orchestration. |
| BUILD-07 | Contributor receives actionable prerequisite diagnostics for Python, Conan 2, CMake, Ninja, Clang, Git, and genuinely required platform SDK/runtime components. |
| BUILD-08 | Contributor can discover LLVM through find_package(LLVM CONFIG) using supported Conan CMakeToolchain/CMakeDeps integration, with explicit compatibility-checked managed/system/custom selection and no hidden acquisition in external modes. |
| BUILD-09 | Contributor can obtain compile_commands.json from each supported development Ninja preset. |
| BUILD-10 | Contributor can build and run compiler infrastructure tests with GoogleTest/GoogleMock supplied through Conan. |
| BUILD-11 | Contributor can run an include-style guardrail through a build/check target and CTest entry that rejects quoted or relative Veyr-owned includes. |
| BUILD-12 | Contributor can reuse compatible Conan packages on repeat workflows and configure/build from prepared dependencies with network acquisition disabled; generated package/build/cache state remains untracked. |
| BUILD-13 | Contributor can choose documented supported Debug/Release compiler/dependency combinations that preserve platform runtime and ABI compatibility. |
| BUILD-14 | Contributor can follow Windows/Linux CI using shared reviewed command/dependency/profile policy with independently observed developer and CI toolchain identities. |

Research support: BUILD-01/08/09 use target-oriented CMake with native LLVM exports; BUILD-02/03/04/12 use Conan recipes, options, profiles, locks and cache; BUILD-05/07/13 use exact host baselines plus runtime checks; BUILD-06 uses thin operation chaining; BUILD-10/11 use infrastructure tests/include enforcement; BUILD-14 uses four native CI combinations.

## Architectural Responsibility Map

[PROPOSED within canonical ownership]

| Layer | Responsibility |
|---|---|
| Python developer entry | Select reviewed inputs, validate tools, call tools with argument arrays, report status; no acquisition graph |
| Conan | Recipes, dependency closure, source download/build, package identities/revisions/cache and locks |
| LLVM Conan recipe | Exact upstream source/options, standard Conan CMake helpers, install native exports, test_package |
| CMake | Veyr targets, LLVM CONFIG discovery/compatibility checks, Ninja presets, CTest/include target |
| GitHub Actions | Shared commands/dependency policy, independently reviewed CI identities, separate bootstrap/native stages, cache optimization and four configuration results |
| Future Veyr project system | Deferred; Veyr.toml and Veyr.lock are not compiler dependency inputs |

## Standard Stack

[OWNER-APPROVED CANDIDATE CORRECTION: 2026-10-04] The observed Omarchy/Arch x86_64 host is the Linux developer baseline candidate. This supersedes the earlier proposed Ubuntu developer assumption and shared host-tool pins; it does not approve installed Conan, package readiness or ABI compatibility. Source evidence is the unchanged 01-01-HOST-INVENTORY.json. Original D-07/D-14 discussion text above remains historical: shared commands and dependency/profile policy do not imply identical developer/CI toolchain identities.

| Tool/dependency | Linux developer candidate / observation | Windows developer candidate (uninspected) | Qualification |
|---|---|---|---|
| Python | 3.14.7 | 3.13.15 | Exact interpreter per environment |
| Conan | 2.33.0 required; missing | 2.33.0 | Conan 2 package authority; no installed readiness claim |
| CMake | 4.4.3 | 3.31.8 | Ninja generator; keep shared build semantics |
| Ninja | 1.13.2 | 1.13.2 | Native executor |
| Clang/clang-cl host compiler | 22.1.8 | 20.1.8 | Separate from LLVM backend 23.1.2 |
| GCC/libstdc++ toolchain | 16.2.1 | Not applicable | Linux compiler.libcxx=libstdc++11 ABI mode |
| libc / linker | glibc 2.44 / GNU Binutils 2.47 | Actual CRT/linker pending | Native ABI/link/run remains unproven |
| Git | 2.55.0 | 2.55.0.windows.5 | Exact environment identity |
| Backend library | LLVM 23.1.2 | LLVM 23.1.2 | Proposed local reference llvm/23.1.2@veyr/m1 |
| Tests | gtest/1.18.0 | gtest/1.18.0 | GoogleMock enabled; static package and matching runtime |

| Environment | Identity evidence / gate |
|---|---|
| Linux developer | Omarchy 4.0.4 / Arch x86_64; observed tool identities above; Conan missing |
| Windows developer | Independently pending until actual Windows x86_64 host inspection; retain candidate identities |
| Linux GitHub Actions | Ubuntu 24.04; exact CI toolchain independently pending until workflow observation |
| Windows GitHub Actions | Windows-2022 runner family; exact CI toolchain independently pending until workflow observation |

No Ubuntu developer host is required. Selecting Omarchy candidate policy does not mean packages are ready; missing Conan still blocks package operations. CI shares commands, dependency references/options and reviewed profile policy, not assumed installed tool or native ABI identity. macOS remains deferred.

[CITED: original candidate research] Release/version identities were checked against official sources during the interrupted research: [Python](https://www.python.org/downloads/release/python-31315/), [Conan PyPI metadata](https://pypi.org/pypi/conan/2.33.0/json), [CMake](https://github.com/Kitware/CMake/releases/tag/v3.31.8), [Ninja](https://github.com/ninja-build/ninja/releases/tag/v1.13.2), [host Clang](https://github.com/llvm/llvm-project/releases/tag/llvmorg-20.1.8), [LLVM releases](https://github.com/llvm/llvm-project/releases), [GoogleTest index](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/gtest/config.yml). These preserve the original documentary findings, not evidence for the new Omarchy observations or successful integration.

[OWNER-SELECTED CANDIDATE] Linux developer baseline: observed Omarchy/Arch x86_64, Clang 22.1.8, explicitly selected GCC/libstdc++ 16.2.1 toolchain, Conan libcxx=libstdc++11, glibc 2.44 and Binutils 2.47. Exact paths are observed prerequisites, not a normalized SDK. Conan 2.33.0 is required and missing. [PROPOSED; INDEPENDENTLY PENDING] Windows baseline: Windows x86_64, clang-cl 20.1.8, VS 2022 17.14.37710.0, SDK 10.0.26100.0 and v143 dynamic runtime. Candidate Debug runtime identity is 14.44.35211; the exact installed VCToolsVersion must be read and reviewed before enforcing a complete runtime/toolset pin. Do not treat the redistributable version as the toolset/header version.

[CITED: original documentary research only] Public runner inventories informed earlier candidates: [Windows 2022 inventory](https://raw.githubusercontent.com/actions/runner-images/main/images/windows/Windows2022-Readme.md), [Ubuntu 24.04 inventory](https://raw.githubusercontent.com/actions/runner-images/main/images/ubuntu/Ubuntu2404-Readme.md). These are not observed Veyr workflow execution evidence or proof of developer identity. Both CI toolchain identities remain independently pending until actual workflow observations. Use named runner families, not a moving latest alias; any authorized prerequisite installation stays conventional and outside a Veyr provisioning framework.

## Package Legitimacy Audit

[CITED] Package provenance was checked through official ConanCenter recipe sources, LLVM release/source pages and Conan's PyPI metadata. The generic GSD legitimacy seam supports npm/PyPI/crates, not Conan; no seam OK/SUS/SLOP verdict is claimed. No package-manager command or installation ran during planning. Do not mistake this documentary review for recipe execution.

| Package | Authoritative identity | Disposition |
|---|---|---|
| llvm/23.1.2@veyr/m1 | Proposed project-owned recipe from official llvm-project source | New narrow recipe; owner review and source integrity/test_package required during execution |
| gtest/1.18.0 | ConanCenter gtest index and recipe | Exact recipe selected; lock actual revisions through Conan |
| Conan 2.33.0 | JFrog/conan-io provenance in PyPI metadata | Host-tool candidate; validate installed identity in doctor |
| zlib/zstd/LibXml2 | Not required by proposed disabled LLVM options | Omitted; any later enabling must supply exact reviewed Conan refs |

[CITED] GoogleTest's recipe exposes build_gmock and static/shared options. Select static GoogleTest with GoogleMock enabled and the Veyr configuration/runtime. [GoogleTest recipe](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/gtest/all/conanfile.py).

## Architecture Patterns

### Narrow LLVM recipe and native exports

[PROPOSED] Introduce conan/recipes/llvm/{conanfile.py,conandata.yml,test_package/}. Source exact upstream 23.1.2 through Conan's normal get/source helpers with reviewed SHA-256; retain upstream install layout, LLVMConfig.cmake and LLVMExports. Package source preparation remains inside this one Conan recipe. No SDK root normalizer or wrapper archive downloader.

[CITED] LLVM 23's CMake tree refers to shared monorepo CMake/libc utilities. Source packaging must retain those siblings. Configure X86 only, no clang/lld/LLDB/MLIR projects or runtimes, no JIT/LTO feature work. Explicitly disable unused optional compression/XML/FFI and other discovered facilities to prevent host-dependent closure. [LLVM 23.1.2 CMake inputs](https://raw.githubusercontent.com/llvm/llvm-project/llvmorg-23.1.2/llvm/CMakeLists.txt).

[PROPOSED] Preserve recipe builddirs for lib/cmake/llvm and use cmake_find_mode=none for LLVM. Use CMakeDeps for GoogleTest; use Conan's CMakeToolchain for the consumer. Native LLVM targets are validated by test_package and the foundation's link/run probe.

[CITED] Conan documents opting out of generated find modules for packages shipping native config files, and generated toolchains provide package search paths. Keep generated presets/toolchains beneath ignored build output; disable unwanted source-root user preset output if necessary. [CMakeDeps](https://docs.conan.io/2/reference/tools/cmake/cmakedeps.html), [CMakeToolchain](https://docs.conan.io/2/reference/tools/cmake/cmaketoolchain.html).

### Runtime-aware optimized LLVM

[PROPOSED; unproven until native execution]

| Native combination | Veyr / GoogleTest | LLVM candidate | Required proof |
|---|---|---|---|
| Linux developer Debug | Debug, Clang 22, GCC 16.2.1/libstdc++11 ABI | Release, same selected environment compiler/libcxx | Headers, symbols, allocation/destruction and executable link/run; unproven |
| Linux developer Release | Release, same selected developer ABI | Same compatible Release LLVM | Native link/run and package reuse; unproven |
| Windows Debug | Debug, v143 dynamic Debug CRT (/MDd) | Release optimization, dynamic Debug CRT | Matching CRT, iterator level and exported LLVM ABI settings |
| Windows Release | Release, v143 dynamic Release CRT (/MD) | Release optimization, dynamic Release CRT | Matching CRT/iterator level and native link/run |

[CITED] Conan distinguishes compiler/runtime/runtime_type settings; profile package-qualified settings allow LLVM build_type=Release while preserving the selected runtime. Windows clang-cl profiles must set compiler executables and runtime explicitly. [Conan settings](https://docs.conan.io/2/reference/config_files/settings.html), [profiles](https://docs.conan.io/2/reference/config_files/profiles.html), [clang-cl toolchain example](https://docs.conan.io/2/examples/tools/cmake/cmake_toolchain/llvm_clang.html).

[CITED] MSVC's default iterator debug level differs between Debug and Release. LLVM exports its own build type, runtime library setting and ABI-breaking-check configuration. Check those values and imported target mappings; do not allow an LLVM config import to silently rewrite the consumer runtime. [MSVC iterator settings](https://learn.microsoft.com/en-gb/cpp/standard-library/iterator-debug-level), [LLVMConfig template](https://raw.githubusercontent.com/llvm/llvm-project/llvmorg-23.1.2/llvm/cmake/modules/LLVMConfig.cmake.in).

[PROPOSED] Limit any Debug-to-Release imported configuration mapping to LLVM targets, never GoogleTest globally. Keep Debug STL checks; do not force iterator level zero or erase runtime from Conan package identity to force reuse. If optimized LLVM plus the requested CRT does not work, stop for owner review; do not claim compatibility or silently substitute another provider/version.

### Prepared dependency and provider boundary

[PROPOSED] Explicit setup exports/creates the local recipe, obtains required packages with --build=missing, resolves exact recipe revisions and creates reviewed native Conan locks. Configure uses cache-only Conan generation with --no-remote --build=never and explicit host/build profiles/lock inputs. Build may configure; test may build. Neither may run setup.

[CITED] Conan install supports cache-only resolution and explicit missing-build behavior. Conan lockfiles capture recipe version/revision resolution; they do not replace compiler/runtime profiles or prove binary ABI compatibility. [Conan install](https://docs.conan.io/2/reference/commands/install.html), [lockfiles](https://docs.conan.io/2/tutorial/versioning/lockfiles.html).

[PROPOSED] CMake exposes managed/system/custom LLVM provider modes and exact 23.1.2 CONFIG discovery. Managed selects the Conan-produced native config. System/custom require explicit prepared external configuration and the same version/runtime/target compatibility checks; no fallback or acquisition. Consumer options omit the managed LLVM requirement for external modes, while GoogleTest remains Conan-owned. Positive external-mode coverage may use an explicitly selected installed Conan-produced config; foreign installations remain subject to compatibility checks, not an assumed support claim.

### Cache identity and clean

[PROPOSED] Dedicated Conan home outside checkout: Linux under XDG_CACHE_HOME (or ~/.cache)/veyr/conan/<project-key>; Windows under LOCALAPPDATA/Veyr/conan/<project-key>. Derive project-key from resolved Git common-directory path so worktrees share a project cache; this is home selection only, not a package ID/cache format. Print resolved home and allow explicit external-home override. Isolate Veyr build output by native profile, provider, tool baseline and configuration.

[CITED] CONAN_HOME selects Conan-owned storage; the Conan cache is not concurrent. Use separate homes per CI job and serialize conflicting local wrapper operations. No package mirroring or parallel mutation of one cache. [Conan environment](https://docs.conan.io/2/reference/environment.html).

[PROPOSED] Clean removes only the selected build directory after path-boundary checks. Preserve Conan cache and other configurations. No cache purge command is required by this phase. Receipts, if needed, identify wrapper tools/profile/configuration only; they never duplicate the Conan graph or future Veyr.lock.

### Thin operations and meaningful tests

[PROPOSED] One scripts/dev.py interface: doctor/setup/configure/build/test/verify/all/clean; Debug default, explicit supported overrides. Use structured subprocess arguments and streamed output. All composes operations without repeated configure/build/test work. After roughly 60 seconds quiet, inspect credible process/child CPU/I/O/artifact progress; continue healthy work. A confirmed stall stops once and reports. No duplicate retry or automatic cleanup.

[PROPOSED] Foundation targets retain veyrcore independent of LLVM. A minimal veyrc build/version stub and an isolated LLVM link/run probe provide infrastructure coverage; neither parses .vyr nor implements Phase 8 backend lowering. Add GoogleTest/GoogleMock harness and negative include fixtures. Include checker examines Veyr-owned sources/headers, rejects quoted/relative owned includes and integrates as check-includes plus CTest entry. Python unittest suffices for wrapper/process/include checks.

### CI capacity and evidence

[PROPOSED] GitHub Actions: pull_request, push main, workflow_dispatch; Windows-2022 and Ubuntu-24.04 × Debug/Release. Ubuntu CI identity is separately observed when its workflow exists; Omarchy developer evidence does not satisfy it. Windows CI evidence does not replace native Windows developer inspection. Share commands/dependency policy, but select each reviewed environment's exact Conan/tool inputs. Separate bootstrap validation and native configure/build/test stages logically. Restore Conan homes only as optimization; manual cache-disabled cold setup remains available. Cache keys include actual environment identities, profiles, recipes/locks, configuration and tool/runtime baseline; avoid concurrent cache writes.

[CITED] GitHub-hosted jobs have a six-hour hard job limit. Standard public runners provide finite CPU, memory and disk; private runner allocation differs. Long LLVM builds are allowed by policy, but this external hard limit still applies. Cold-build wall time, peak disk/memory and compiler/link parallelism must be measured rather than promised. [Actions limits](https://docs.github.com/en/actions/reference/limits), [hosted runners](https://docs.github.com/en/actions/reference/runners/github-hosted-runners).

[PROPOSED] If four native combinations or a cold build cannot fit the selected runner, report the measured constraint for owner choice of capacity. Do not reduce Tier 1 acceptance, substitute a foreign SDK, or create a binary-cache service. Publishing workflows/triggering remote CI requires its own authorized action; creating YAML locally is not passing CI evidence.

## Don't Hand-Roll

[VERIFIED: canonical architecture] Delegate dependency solving, transitive closure, downloads/source extraction, builds/package storage, revisions and locking to Conan. Delegate target/build graph and imported targets to CMake. Do not promote documentation templates into executable state schemas. Do not add a distro abstraction, environment installer, SDK normalizer, Veyr package cache or artifact provenance platform.

## Common Pitfalls

[PROPOSED review checks]
- Declaring 23.1.2 available from ConanCenter because a newer upstream release exists.
- Shipping CMakeDeps-generated LLVM targets while silently discarding native exports.
- Mixing Windows /MD and /MDd or STL iterator ABIs; global imported-config remapping.
- Hardcoding cached package paths or treating locks as runtime/package-identity locks.
- Optional LLVM autodetection introducing unreviewed dependencies.
- Hidden setup in configure/build/test or source builds in --build=never paths.
- Shared Conan-home concurrent CI jobs, broad clean deletion or automatic cache repair.
- Treating alive-but-idle process existence alone as credible progress.
- Reporting Linux-only local checks as passing Windows CI or cold-build capacity.

## Code Examples

Planning examples only; none were run:

```text
python scripts/dev.py doctor --profile linux-x86_64-clang --config Debug
python scripts/dev.py setup --profile windows-x86_64-clangcl --config Debug
python scripts/dev.py configure --profile windows-x86_64-clangcl --config Debug
python scripts/dev.py all --profile linux-x86_64-clang --config Release
```

[PROPOSED] Entry point resolves complete reviewed inputs and logs argument arrays. Underlying configure uses conan install with explicit -pr:h/-pr:b, --lockfile, --no-remote and --build=never. Native CMake uses find_package(LLVM 23.1.2 EXACT CONFIG REQUIRED), selected provider config, C++23 target features and Ninja compile database export. Commands become executable only in approved execution.

## Assumptions and Open Questions

1. Optimized LLVM with Windows Debug CRT is a candidate, not an established support claim. Recipe test_package and Veyr/native tests must prove it.
2. Windows exact VCToolsVersion/CRT/SDK/linker remains pending native inspection. Omarchy Linux developer identities are recorded, but Conan is missing and ABI remains unproven. Ubuntu and Windows CI identities remain independently pending; candidate selection does not establish readiness.
3. Record official 23.1.2 source digest and Conan recipe revisions during authorized source/lock work; do not invent hashes/revisions during planning.
4. Cold build capacity and useful parallelism require measurement on the selected CI account/runners. Capacity failure requires owner review.
5. External LLVM providers must prove the same compatibility contract; provider selection is not permission for fallback.
6. Package-legitimacy seam coverage for Conan is unavailable; official provenance review is documented, not mislabeled as a seam verdict.
7. No language design question is resolved by this foundation.

## Environment Availability

[VERIFIED: read-only checks before interruption] include/, src/, tests/, cmake/, conan/, scripts/, workflow and codebase-map directories were absent. Conan was not on PATH; CMake, Ninja, Clang and Git paths existed. No build-tool version command, package command or build/test ran. Interpreter used for document inspection was Python 3.14.7; it is not evidence of the proposed supported host baseline. Native Windows availability remains uninspected.

## Validation Architecture

[PROPOSED] Two feedback lanes: quick stdlib Python prerequisite/operation/include negative checks after relevant task changes, and full native Conan recipe/CMake/GoogleTest/provider/offline/runtime checks per native combination. Aim for quick checks under 30 seconds; source builds and native full checks have no invented short limit. Missing infrastructure is created by execution tasks, never by planning.

Required cases: tool mismatch/missing SDK; argument/path spaces; managed and explicit external provider positives; wrong LLVM version/runtime/triple negatives; cache-only missing-package failure without download/build; repeat compatible package reuse; clean preserves other configurations/cache; all avoids duplicate work; healthy quiet child continues and confirmed stall stops once; include quoted/relative negatives; compile database for four combinations; LLVM/GTest link/run under each runtime.

01-VALIDATION.md records the requirement map and pending infrastructure. No green evidence, Nyquist sign-off or phase completion is claimed.

## Security Domain

[VERIFIED: scope] Native developer tooling and compiler dependency integration, not a web application. No authentication/session/ASVS web feature is being added. Integrity work stays with normal Conan recipe digests/revisions, explicit profiles, HTTPS sources, structured subprocess calls and clean path-boundary checks; no general security platform.

## Sources and Metadata

Primary source links above preserve the documentary findings gathered before interruption. No completed research was rerun on resume. Current-source URLs may change; execution revalidation is explicitly required where installed host/runner identity matters.

- Confidence: documented interface/provenance findings are source-cited; proposed integration and performance remain unproven.
- Scope: fresh Phase 1 only; no imported prior-run logs, code, plans or outcomes.
- Research gate: documentary capture complete; native compatibility, exact host observations and CI capacity remain required execution checkpoints.
- Next incomplete step: interactive confirmation of the proposed plan/wave breakdown, then executable PLAN.md writing and inline plan review.
