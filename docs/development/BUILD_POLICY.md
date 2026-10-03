
# Veyr Build Policy

**Status:** Required implementation policy

## 1. Baseline

The compiler implementation uses:

- C++23;
- CMake;
- Ninja;
- Clang;
- Conan 2 for third-party implementation dependencies;
- GoogleTest/GoogleMock;
- LLVM 23.1.2 as the initial backend target.

Windows and Linux are Tier 1. macOS is future Tier 2.

## 2. Dependency/package boundary

Conan owns third-party package mechanics:

- recipes;
- dependency resolution;
- transitive dependency closure;
- source builds;
- package IDs/revisions;
- package storage/cache;
- options;
- CMake dependency integration.

Veyr scripts must not recreate a package solver, downloader framework, package cache, SDK
normalizer, provider graph, or parallel third-party lock format.

Use exact dependency versions and reviewed profile/recipe settings.

## 3. LLVM

LLVM is an implementation dependency supplied through Conan.

Initial target: LLVM 23.1.2.

Prefer an existing suitable Conan recipe. If unavailable, maintain a narrow project-owned LLVM
recipe.

A clean LLVM source build may be long. That is acceptable. Repeated builds should reuse Conan's
package cache where compatible.

Do not add LLDB, JIT, LTO, every backend, or other optional LLVM projects to M1 unless the active
milestone requires them.

At the Veyr CMake boundary, prefer normal package discovery/imported targets, including
`find_package(LLVM CONFIG REQUIRED)` when exposed by the chosen Conan integration.

## 4. Conan/CMake integration

Fresh planning should use the currently supported Conan 2 CMake integration, expected initially
to be `CMakeToolchain` + `CMakeDeps`.

CMake remains authoritative for Veyr's own targets.

Do not hardcode Conan cache paths into project CMake files.

## 5. Profiles

Use explicit project-reviewed platform profiles.

Expected conceptual profiles:

```text
linux-x86_64-clang
windows-x86_64-clangcl
```

Do not treat auto-detected profiles as canonical project policy.

Windows and Linux may intentionally differ in compiler runtime, linker, standard library and SDK
details.

## 6. Build types

Do not force artificial symmetry between Veyr build type and dependency build type when Conan
package identity/options make another configuration appropriate.

The initial plan should explicitly define supported Debug/Release developer combinations.

## 7. Warnings and diagnostics

Keep warnings useful and portable. Do not silently weaken diagnostics merely to accept a broken
dependency configuration.

Veyr-owned includes use canonical angle-bracket form:

```cpp
#include <veyr/...>
```

Development configuration exports `compile_commands.json`.

## 8. Generated files and caches

Generated build/package artifacts stay out of tracked source.

Typical local locations may include:

```text
build/
.veyr/
.veyr-dev/
```

Conan's configured cache/home is also untracked.

Do not commit machine-local package binaries, build trees, compiler caches, or generated tool
configuration.

## 9. Long-running commands

CMake, Ninja, CTest, compiler, Conan, and dependency-build commands are execution/verification
actions, not discussion/planning actions.

Approximately 60 seconds without output is a soft liveness checkpoint. Inspect process/liveness
signals where possible and continue if the command is credibly progressing.

Do not launch duplicate retries. Stop on actual failure, owner interruption, invariant violation,
process conflict, or an explicit hard timeout.

## 10. Cross-platform requirement

Any build-system change must include a Windows/Linux impact note and must not structurally block
future macOS support.

See:

- `docs/tooling/CONAN_BUILD_ARCHITECTURE.md`
- `docs/development/DEVELOPER_BOOTSTRAP.md`
- `docs/development/CROSS_PLATFORM_POLICY.md`
