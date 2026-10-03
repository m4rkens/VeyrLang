# Starter Pack Migration — Managed SDK + GSD Stage Commits

**Status:** owner-approved starter reset direction

This starter revision intentionally changes two foundational assumptions from the previous pack.

## 1. LLVM is no longer "manual external only"

Previous assumption:

```text
LLVM must already exist on the machine and normal Veyr policy never provisions it.
```

New assumption:

```text
LLVM is consumed through Conan-backed CMake package integration.
If exact LLVM 23.1.2 needs a project-owned recipe, keep that recipe narrow.
Do not recreate the earlier custom prebuilt-SDK provisioning layer.
```

Supported provider modes are:

```text
managed
system
custom
```

Managed mode is the normal convenience path. Custom mode remains the escape hatch for a future matching Debug LLVM build or LLVM development.

The motivation is reproducible Windows/Linux development without asking every developer to hand-assemble LLVM, runtime/ABI choices, zlib/zstd/libxml-style native dependencies, paths, and compatibility evidence.

## 2. Developer setup becomes project-owned workflow

The build-foundation phase should plan and implement equivalent Windows/Linux entry points for:

```text
setup
doctor
configure
build
test
verify
all
clean
```

Generated wrapper/build state stays untracked. Conan-owned package/cache state remains Conan-owned; `.veyr-dev/` may hold only Veyr wrapper state where useful.

The scripts are workflow wrappers, not a replacement for CMake/Ninja. Their job is to make the correct CMake/dependency/toolchain workflow repeatable.

## 3. GSD history becomes lifecycle-stage history

Previous default:

```text
one reviewed/verified phase bundle -> one commit
```

New default when tracked artifacts exist:

```text
discussion -> review -> discussion commit
planning   -> review -> planning commit
execution  -> review -> execution commit(s)
verification -> review -> verification commit
```

No empty commits are required. No stage auto-commits. Every commit still needs explicit owner approval. Sensitive execution/gap-fix commits still require the owner review gate.

## 4. What should be regenerated

Because these assumptions affect build-foundation scope and workflow history, a clean GSD restart should regenerate `.planning/` from the updated `gsd/GSD_PROJECT_DESCRIPTION.md` instead of reusing old Phase 1 planning built around the external-only LLVM rule.

Do not import old build directories or machine-local evidence into the clean starter repository.


## 5. v13 multi-platform developer-environment hardening

The next starter revision adds policy that should be planned from the beginning rather than retrofitted after platform-specific build caches exist:

- canonical host/target triplets;
- reviewed Conan profiles/configuration shared appropriately by developer workflows and CI;
- clear separation between Veyr wrapper state and Conan-owned package/cache state;
- no custom SDK archive normalization layer;
- idempotent setup plus explicit repair/force modes;
- `doctor --json` as a machine-readable environment contract;
- machine-local `environment.lock.json` with drift detection;
- strict `clean` vs destructive `purge` separation;
- CI split between bootstrap/provenance validation and build/test validation;
- capability-oriented platform adapters to reduce scattered OS-specific conditionals.

These additions do not make cross-compilation or macOS Tier 1 requirements. They make those future directions possible without redesigning dependency/cache/build identity.
