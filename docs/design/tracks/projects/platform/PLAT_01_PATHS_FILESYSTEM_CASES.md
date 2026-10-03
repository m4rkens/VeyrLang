# Platform Path and Filesystem Cases

**Track:** Cross-platform systems  
**Status:** validation project card  
**Primary milestone pressure:** M0-M12, M18-M20

## Minimal project

A tiny tool/probe that normalizes, displays, and compares source paths, output paths, cache paths, and module-root paths on Windows and Linux.

## Forces in Veyr

- path-as-data APIs instead of raw string concatenation;
- source identity with stable spans independent of path separator;
- cache/output directory rules;
- case-sensitivity awareness;
- diagnostics that print useful platform paths.

## Must not accidentally force

- final package manager;
- full virtual filesystem;
- network paths or symlink semantics beyond explicit tests.

## Platform requirement

This project must consider Windows and Linux from the start. macOS behavior should be documented as future/deferred when it cannot yet be implemented.

## Exit criteria

- relative and absolute paths are handled without hardcoded separators;
- diagnostics preserve source file identity;
- build/cache paths remain outside source;
- Windows/Linux behavior is documented.
