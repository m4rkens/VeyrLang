# Phase 1: Conan and Compiler Build Foundation - Discussion Log

> Audit trail only. Downstream research/planning/execution agents consume `01-CONTEXT.md`; this log preserves the questions and alternatives for owner review.

**Date:** 2026-10-03
**Phase:** 01-conan-and-compiler-build-foundation
**Areas discussed:** Developer commands; Debug/Release support; network/cache behavior; Windows/Linux evidence and CI.
**Mode:** Interactive text questions. The owner's `all` selected all areas, not automatic answers or lifecycle transitions.

## Developer commands

| Question | Alternatives presented | Owner selection |
|----------|------------------------|-----------------|
| How should contributors normally invoke setup/build/test? | 1. One Python entry point; 2. Python plus convenience launchers; 3. Another preference. | 1. One Python entry point. |
| What should normal command output show? | 1. Step headers plus live output; 2. Concise summaries with verbose/log details; 3. Another preference. | 1. Step headers, selected profile, invoked command, live tool output. |
| Should build/test run prerequisite steps automatically? | 1. Explicit steps; 2. Limited chaining with dependency setup explicit; 3. Another preference. | 2. Build may configure; test may build; setup stays explicit. |
| How should routine commands choose their profile/configuration? | 1. Reviewed native defaults with visible selections/overrides; 2. Always require explicit arguments; 3. Another preference. | 1. Reviewed native defaults; no canonical policy from profile detection. |

**Notes:** `python scripts/dev.py setup` was illustrative; no exact script location was separately selected. The owner chose to move to Debug/Release support after these four questions.

## Debug/Release support

| Question | Alternatives presented | Owner selection |
|----------|------------------------|-----------------|
| Which debugging workflow must Phase 1 support? | 1. Debug/Release Veyr with optimized LLVM where compatible; 2. Also a Debug LLVM debugging configuration; 3. Another preference. | 1. Prioritize Veyr debugging and dependency reuse, subject to runtime/ABI proof. |
| Which Veyr configuration should everyday commands default to? | 1. Debug; 2. Release; 3. Another preference. | 1. Debug. |
| How broad should initial toolchain support be? | 1. One reviewed baseline per platform; 2. Multiple versions/a supported range; 3. Another preference. | 1. Exact documented tool/runtime baseline per Tier 1 platform. |
| What should normal commands do outside the baseline? | 1. Stop with actionable instructions; 2. Explicit experimental opt-in, unvalidated results, incompatible ABI still fatal; 3. Another preference. | 1. Stop and report mismatch/expected configuration. |

**Notes:** Exact versions were not chosen. Optimized LLVM is a compatibility-constrained preference, not acceptance of every Debug/Release runtime pairing. The owner chose to move to network/cache behavior.

## Network and cache behavior

| Question | Alternatives presented | Owner selection |
|----------|------------------------|-----------------|
| What may setup do when Conan packages are missing? | 1. Download/source-build with visible progress and no extra per-package confirmation; 2. Separate source-build option; 3. Another preference. | 1. Explicit setup permits downloads and missing-package source builds. |
| Should all include dependency setup? | 1. Setup/configure/build/test/verify; 2. Prepared dependencies only with separate setup; 3. Another preference. | 1. Full workflow, advertised as potentially acquiring/building dependencies. |
| Where should Conan's cache live by default? | 1. Standard user Conan home; 2. Dedicated Conan home per project; 3. Another preference. | 2. Dedicated, untracked, Conan-owned home per project. |
| What should ordinary clean remove? | 1. Selected profile/configuration build output; 2. All project build output; 3. Another preference. | 1. Selected output only; preserve Conan packages and other configurations. |

**Notes:** Cache location/project identity remain research details. No custom package store or implicit package purge is authorized. Build commands use prepared dependencies without hidden acquisition. The owner chose to move to Windows/Linux evidence and CI.

## Windows/Linux evidence and CI

| Question | Alternatives presented | Owner selection |
|----------|------------------------|-----------------|
| What establishes Tier 1 acceptance? | 1. Passing CI on both platforms with local checks supplementary; 2. Owner-reviewed local evidence primary, CI path still provided; 3. Another preference. | 1. Passing Windows and Linux CI. |
| Which CI provider should Phase 1 use? | 1. GitHub Actions workflows in this repository; 2. Another provider/existing setup. | 1. GitHub Actions. |
| When should CI run? | 1. Pull requests/main pushes/manual runs; 2. Manual initially; 3. Different triggers. | 1. Pull requests, pushes to main, and manual runs. |
| Which configurations should routine CI cover? | 1. Debug and Release on both platforms; 2. Debug routine/Release manual with acceptance requiring Release evidence; 3. Another matrix. | 1. Four Windows/Linux x86_64 and Debug/Release combinations. |

**Notes:** No CI execution or platform evidence occurred. Runner capacity and hard limits for uncached LLVM builds remain research gates; the discussion does not authorize silent acceptance changes or custom prebuilt SDK provisioning.

## Completion choice

After all four areas, the assistant summarized the selected preferences and offered: 1. Write Phase 1 context/discussion log for review; 2. Explore more gray areas. The owner selected 1. This authorizes discussion artifacts only; no stage commit, phase planning, implementation, builds/tests, or phase advancement.

## Agent discretion

No "you decide" answers. Unselected implementation details remain for separately authorized research/planning within the captured constraints.

## Deferred ideas

No new capabilities requested. Alternatives not selected are not automatically backlog tasks. Existing M1 and platform exclusions remain in force.
