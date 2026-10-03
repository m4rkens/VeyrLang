
# Veyr Developer Bootstrap

**Status:** Required starter direction

## 1. Goal

Developer bootstrap should make the normal Veyr workflow predictable on Windows and Linux without
turning the repository into a package manager.

The desired stack is:

```text
thin Python wrapper
    -> Conan 2
    -> CMake
    -> Ninja
    -> Clang
```

## 2. Host prerequisites

The initial developer contract should stay small:

- Python;
- Conan 2;
- CMake;
- Ninja;
- Clang;
- Git;
- platform SDK/runtime components genuinely required by the selected toolchain.

Exact supported versions belong to fresh GSD planning and CI policy.

## 3. Wrapper responsibilities

Repository-owned Python tooling may expose equivalent developer operations such as:

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

The wrapper may:

- validate host tools;
- select tracked Conan/CMake profiles;
- set the intended Conan home/configuration;
- invoke Conan;
- invoke CMake/Ninja/CTest;
- provide concise human-readable diagnostics;
- optionally provide stable machine-readable `doctor --json` output when useful.

The wrapper must not:

- solve third-party dependency graphs itself;
- maintain its own third-party source registry;
- implement a binary package cache;
- normalize arbitrary LLVM SDK archives;
- duplicate Conan lock/package identity semantics.

## 4. Conan configuration

Use explicit project-reviewed Windows/Linux profiles.

Do not accept `conan profile detect` output as permanent canonical configuration without review.

Use Conan-native package IDs/revisions and lockfiles where useful. Keep compiler implementation
dependency locking separate from the future language-level `Veyr.lock`.

The normal workflow should make network/build behavior visible to the developer. It is acceptable
for a missing binary to trigger a deliberate source build when the selected workflow permits it.

## 5. LLVM

Initial LLVM target: 23.1.2.

Consume LLVM through Conan.

If a suitable exact recipe does not exist in the selected recipe source, use a narrow
project-owned LLVM recipe.

A fresh LLVM build may be expensive and quiet for long periods. This is expected. Do not work
around that cost by growing a bespoke SDK acquisition/provisioning subsystem.

Reuse compatible Conan packages/cache in subsequent local and CI runs.

## 6. Local state

Generated developer state remains untracked.

The project may use `.veyr-dev/` for Veyr-owned wrapper state/receipts if useful, but Conan's
package cache/home remains Conan-owned and must not be mirrored into a second Veyr dependency
store.

`clean` should remove ordinary build output. Destructive package/cache purging must be explicit.

## 7. Cross-platform behavior

Windows and Linux expose equivalent high-level operations while using independent profiles and
platform-specific toolchain details.

Do not hardcode POSIX paths into Windows logic or Windows SDK assumptions into Linux logic.

macOS remains future Tier 2 and should not be structurally blocked.

## 8. CI

CI should reuse the same tracked Conan profiles/configuration used by developers.

Typical flow:

```text
validate tools
-> conan install / package resolution
-> cmake configure
-> build with Ninja
-> ctest
```

Cache restoration is an optimization. A clean environment must remain buildable.

## 9. Long-running operations

LLVM/package builds can legitimately run for a long time with little output.

The ~60-second threshold is a liveness-inspection point only. Do not automatically terminate a
healthy build merely because it is quiet.

Stop on real failure, owner interruption, invariant violation, process conflict, or a deliberate
hard timeout.

## 10. Relationship to project build/package tooling

Conan is only for building the Veyr implementation and its native third-party dependencies.

The future user-facing `veyr` package/build system and `Veyr.toml`/`Veyr.lock` ecosystem remain
separate product features.
