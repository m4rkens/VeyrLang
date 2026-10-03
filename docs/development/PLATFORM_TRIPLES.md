# Platform Triples and Build Identity

**Status:** required naming and cache-identity policy

Veyr distinguishes host identity from target identity. A single string such as `Windows` or `x64` is not sufficient to identify an ABI-compatible developer environment.

## Canonical initial triplets

```text
windows-x86_64-msvc
linux-x86_64-gnu
linux-x86_64-libcxx
macos-aarch64
macos-x86_64
```

These are repository-level identities, not a claim that every combination is already implemented.

## Required dimensions

A canonical identity should encode, directly or through an associated profile, the dimensions that make build/cache artifacts incompatible:

- operating system;
- CPU architecture;
- compiler/ABI family where it matters;
- standard-library family where it matters.

Do not encode transient patch versions in the triplet. Versions belong in manifest/lock/evidence data.

## Host and target

Use separate concepts:

```text
VEYR_HOST_TRIPLE
VEYR_TARGET_TRIPLE
```

Native development normally uses the same value for both. Future cross-compilation must not require redesigning cache paths, manifests, or diagnostics.

## Build/cache identity

At minimum, build and machine-local dependency state must be isolated by:

```text
host triplet + target triplet + profile/provider + toolchain identity
```

Never reuse a CMake build directory after changing one of those compatibility dimensions.

## Diagnostics

`doctor`, verification output, CI logs, and environment lockfiles should print canonical triplets so platform issues can be reproduced without inferring architecture/ABI from unrelated paths.
