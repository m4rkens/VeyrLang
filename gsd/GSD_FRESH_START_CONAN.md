
# Fresh GSD start — Conan architecture note

This starter is intended to begin a new `.planning/` lifecycle.

Do not import generated `.planning/` artifacts, retained SDK transactions, custom auxiliary
provider graphs, or custom provisioning implementation from an earlier Veyr run.

Carry forward the canonical language/design documents in this starter and the build architecture:

```text
thin Python wrapper -> Conan 2 -> CMake -> Ninja -> Clang
```

Conan owns third-party package mechanics.

The first build-foundation phase should be sufficient to reach real compiler development, not a
project to implement another package manager.

A long first LLVM build is acceptable.
