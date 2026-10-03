# CI Matrix Policy

**Status:** future implementation contract

CI is not required to exist in the starter pack, but build/bootstrap design must support this matrix without restructuring the repository.

## Required Tier 1 direction

```text
windows-x86_64-msvc
linux-x86_64-gnu
```

Each Tier 1 environment should eventually run two logically distinct job classes:

```text
1. managed-bootstrap validation
2. configure/build/test/verification
```

Separating them prevents SDK/provenance/cache problems from being reported as compiler implementation failures.

## Optional validation

```text
linux-x86_64-libcxx
```

may be added to catch accidental libstdc++ assumptions.

## Tier 2 direction

```text
macos-aarch64
macos-x86_64
```

macOS jobs may begin as experimental/allowed-failure while the platform is Tier 2. Do not label macOS supported until its required build/test contract actually passes.

## CI rules

- use the same reviewed Conan profiles/lock/configuration as local setup;
- prefer the same `doctor --json` readiness contract used locally when implemented;
- do not duplicate dependency versions/options in CI YAML;
- preserve offline/no-network build checks once managed cache population is complete;
- keep host/target triplet visible in artifact names and logs;
- cache only verified managed artifacts and invalidate cache on manifest/schema changes.
