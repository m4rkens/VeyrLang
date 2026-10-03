
# Conan Build and Dependency Architecture

**Status:** Accepted starter direction for fresh GSD planning

## 1. Fundamental split

The implementation build stack is:

```text
thin Veyr Python developer wrapper
        -> Conan 2
        -> CMake
        -> Ninja
        -> Clang
```

Each layer has a deliberately narrow responsibility.

### Veyr Python wrapper

The repository may provide a cross-platform Python wrapper for:

- validating required host tools;
- choosing tracked Conan/CMake profiles;
- invoking Conan;
- invoking CMake, Ninja, and CTest;
- presenting developer-friendly `setup`, `doctor`, `configure`, `build`, `test`, `verify`, and `all` workflows;
- reporting concise diagnostics.

The wrapper must remain orchestration. It must not become a second package manager.

### Conan 2

Conan owns third-party dependency/package mechanics:

- package recipes;
- dependency resolution;
- transitive dependency closure;
- source builds;
- package IDs/revisions;
- package storage/cache;
- dependency options;
- supported CMake integration.

Do not create a Veyr-specific package solver, source-acquisition framework, package cache,
SDK normalizer, auxiliary-provider graph, or parallel lockfile format for third-party C/C++
dependencies when Conan already provides the capability.

### CMake

CMake is the canonical build description for Veyr itself.

Prefer target-oriented CMake and imported package targets. The initial integration direction is
Conan 2 `CMakeToolchain` + `CMakeDeps`, subject to fresh GSD research against the currently
supported Conan interface.

### Ninja and Clang

Ninja is the initial build executor/generator target.

Clang is the primary compiler family:

- Linux: `clang` / `clang++`;
- Windows: `clang-cl` where appropriate for the selected Windows toolchain.

## 2. LLVM direction

The initial backend target is LLVM **23.1.2**, unless a fresh GSD research/owner decision
explicitly revises it.

LLVM should be consumed as a Conan package.

Prefer an existing suitable Conan recipe when it provides the exact required version and
configuration. If exact LLVM 23.1.2 is unavailable, maintain one narrow project-owned Conan
recipe rather than introducing a custom SDK provisioning subsystem.

A first clean build of LLVM may be slow. That is acceptable.

Prefer Conan package/cache reuse and CI caching over adopting foreign prebuilt LLVM archives
that introduce host-specific runtime dependency closure problems.

Only enable/link LLVM components required by the active milestone. Initial M1 does not require
LLDB, a JIT, LTO, or every backend.

## 3. Initial dependency family

Fresh GSD planning should research and configure at least:

- LLVM 23.1.2;
- zlib when required;
- zstd when required;
- LibXml2 when required by the selected LLVM build;
- GoogleTest/GoogleMock or the accepted project test package.

Exact versions/options/recipe revisions belong to Conan-native configuration and fresh GSD
planning.

## 4. Profiles and locking

Maintain explicit project-reviewed profiles for Tier-1 platforms. Expected shape:

```text
conan/profiles/linux-x86_64-clang
conan/profiles/windows-x86_64-clangcl
```

Do not treat auto-detected profiles as canonical project policy.

Use Conan-native lockfiles/package revisions where they materially improve repeatability.
Do not invent another third-party dependency lock format beside Conan.

The language's future `Veyr.lock` is a separate product concept for Veyr packages and must not be
confused with the compiler implementation's Conan lock state.

## 5. Platform posture

Tier 1:

- Windows x86_64;
- Linux x86_64.

macOS remains future Tier 2.

Windows and Linux may have different compiler runtime, linker, SDK, and profile details. Do not
force Linux assumptions onto Windows or vice versa.

## 6. Build-cost and cache philosophy

A long first source build is preferable to permanent custom provisioning complexity.

Cache is an optimization, not project truth.

Fresh or CI environments may rebuild third-party packages when required by the selected workflow.
Repeated developer builds should reuse compatible Conan packages where possible.

## 7. Security and reproducibility

Use normal established package-manager/build mechanisms first:

- exact dependency versions;
- reviewed recipes/options;
- explicit compiler/toolchain selection;
- Conan profiles and lockfiles where useful;
- HTTPS and package/source integrity facilities provided by the selected recipe/source workflow;
- CI verification.

Do not expand M1 into a general supply-chain security or provenance platform unless a concrete
project threat model later requires it.

## 8. Developer prerequisites

Keep host prerequisites small and conventional:

- Python;
- Conan 2;
- CMake;
- Ninja;
- Clang;
- Git;
- platform SDK/runtime components genuinely required by the selected Windows/Linux toolchain.

The project may automate validation and setup ergonomics, but should not reimplement these tools.

## 9. Non-goals

This architecture explicitly does not aim to create:

- a Veyr package manager for C++ implementation dependencies;
- a custom third-party registry;
- a custom package solver;
- a custom binary-cache protocol;
- a custom LLVM SDK acquisition/extraction/normalization framework;
- a generic artifact provenance framework;
- a distro abstraction layer.
