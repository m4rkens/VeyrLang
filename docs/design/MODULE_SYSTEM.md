# Module, Package, Target, and Artifact Model

**Status:** accepted architectural direction with some open surface syntax.

## 1. Fundamental rule

Every program is a graph of modules.

This includes:

- executables;
- static libraries;
- shared libraries;
- tests;
- tools.

Artifact kind changes linking and export behavior, not the source-organization model.

## 2. Conceptual hierarchy

```text
workspace
  -> packages
      -> modules
      -> targets
          -> artifacts
```

### 2.1 Workspace

A workspace groups related packages.

It is organizational and build-oriented. It is not itself a library or executable.

Example long-term workspace:

```text
language/
  compiler/
  std/
  runtime/
  package-manager/
  language-server/
  tests/
```

### 2.2 Package

A package is a unit of:

- versioning;
- dependency declaration;
- publishing;
- build configuration.

A package may contain many modules and produce multiple targets.

### 2.3 Target

A target answers:

> What artifact should be built from which root module and configuration?

Initial target kinds:

- executable;
- static library;
- shared library;
- test.

Later target types may include benchmarks, plugins, or object collections.

### 2.4 Artifact

An artifact is the output of a target, such as:

- executable binary;
- static archive;
- shared library;
- test binary;
- compiled module metadata.

## 3. Root modules

Every target has a root module.

### 3.1 Executable

The root module contains or exposes the program entry point.

The entry point is not outside the module system.

### 3.2 Static library

The root module defines the public semantic surface reachable from the library target.

### 3.3 Shared library

The root module defines the source-level API, while binary exports remain explicit.

## 4. All code belongs to modules

There is no global namespace containing unowned source declarations.

A module is expected to provide:

- namespace identity;
- visibility boundary;
- dependency boundary;
- semantic-compilation unit;
- incremental-build unit.

## 5. One file equals one module by default

Initial direction:

```text
one source file = one semantic module
```

Example:

```text
src/
  compiler/
    lexer.lang
    parser.lang
    ast.lang
```

may map conceptually to:

```text
package.compiler.lexer
package.compiler.parser
package.compiler.ast
```

Large systems should prefer hierarchical submodules.

Multi-file modules remain a candidate only if design probes show a compelling need.

## 6. Canonical module identity

Every module has one canonical identity.

The same source must not become multiple semantic modules merely because it was reached through different import paths.

Package identity participates in global module identity.

## 7. Imports

Imports are semantic dependency declarations.

```text
import std.io as io;
```

means:

> Make the public semantic interface of `std.io` available under a local name.

It does not mean:

- copy source text;
- execute module initialization;
- leak macros;
- depend on include order.

### 7.1 No runtime import side effects

Importing a module must not:

- open a database;
- start a thread;
- register global callbacks;
- execute hidden constructors.

Runtime initialization is explicit through ordinary functions or explicit target/runtime hooks if such hooks are later added.

### 7.2 Import forms

Core direction:

- import module under default final-segment name;
- import module with alias.

Planned:

- selective named imports.

Wildcard imports are likely to be rejected or heavily restricted because they reduce source clarity and API stability.

## 8. Dependency graph

The initial module dependency graph is a DAG.

Cycles are rejected.

A cycle should usually be resolved by:

- merging overly coupled modules;
- extracting shared contracts into another module;
- redesigning ownership of responsibilities.

Type-only cycles remain deferred until real design probes demonstrate a need.

## 9. Visibility

Private is the default direction.

At least three semantic visibility levels are expected:

- module-private;
- package-visible;
- public to external packages.

Exact keywords are open.

Public fields are allowed. Encapsulation is a tool, not a religion.

## 10. Source visibility versus binary export

Source-level public visibility and binary symbol export are different concepts.

A public declaration means other language modules/packages may use it.

A binary export means the final shared-library or executable artifact exposes a symbol to external linkers or other languages.

They must not be conflated.

## 11. Native language API versus stable external ABI

A package may expose a rich Veyr API using:

- generics;
- traits;
- borrow metadata;
- tagged unions;
- native module contracts.

The same package may separately expose a stable C ABI.

Early compiler versions should not promise long-term stability for the native language ABI.

Stable external boundaries should use:

- explicit ABI;
- explicit representation;
- explicit exports;
- carefully designed ownership rules.

## 12. No headers

The language does not use textual header/source duplication.

A module source is compiled into a semantic artifact.

Downstream imports consume the semantic interface, not source text.

## 13. Compiled module artifacts

A compiled module artifact should eventually contain enough information for separate compilation and downstream analysis.

Candidate contents:

- public declarations;
- type metadata;
- trait implementations;
- generic typed IR needed for later monomorphization;
- inferred borrow relationships;
- ownership effects;
- invalidation effects;
- compile-time metadata;
- public layout contracts;
- dependency fingerprints/hashes.

The exact file format is an implementation detail and should not be frozen prematurely.

## 14. Generic separate compilation

A module that publishes a generic function must make a typed representation available to downstream compilation without requiring C++-style textual headers.

This is a core architectural requirement.

## 15. Borrow and invalidation metadata

Downstream code must be analyzable without reopening full implementation source.

A public function may export semantic metadata such as:

- returned reference borrows from parameter X;
- parameter Y escapes into returned object;
- method may invalidate element borrows of receiver;
- operation transfers ownership;
- callback may execute on another thread.

The exact annotation language is open; the metadata capability is accepted.

## 16. Incremental compilation

Modules are intended to support incremental compilation.

A private implementation change that does not alter relevant public semantic metadata should avoid unnecessary downstream rebuilds when practical.

Relevant fingerprints may include:

- source hash;
- public interface hash;
- generic IR hash;
- dependency hashes;
- compile configuration;
- target configuration.

## 17. Package dependencies

Dependency declaration and source imports are separate.

The package manifest says which packages and native libraries are available.

Source code says which modules are imported.

Source code does not choose dependency versions.

A lockfile or equivalent build-resolution artifact should eventually preserve resolved versions.

## 18. Native system dependencies

Native dependencies such as:

- Vulkan;
- Windows libraries;
- POSIX/Linux libraries;
- OpenSSL or similar C libraries;

should be declared explicitly in package/target build configuration.

The source language should not hide linker directives inside arbitrary source code.

## 19. Platform-specific modules

Preferred direction:

- separate platform implementations at module level;
- stable higher-level public contract;
- build/target selection chooses implementation.

Avoid scattering textual preprocessor checks throughout ordinary code.

Conditional compilation, if present, should be typed and build-aware.

## 20. Tests are modules

Tests use the same language and module system.

A test target builds a graph containing normal modules and test modules.

No separate test language is needed.

## 21. Example future package

```text
compiler-package/
  manifest
  src/
    main.lang
    compiler/
      api.lang
      source.lang
      diagnostic.lang
      token.lang
      lexer.lang
      ast.lang
      parser.lang
      semantic.lang
      hir.lang
      codegen.lang
      driver.lang
  tests/
    lexer.lang
    parser.lang
    end_to_end.lang
```

Possible targets:

```text
compiler-cli     -> executable, root module: package.main
compiler-core    -> static library, root module: package.compiler.api
compiler-shared  -> shared library, root module: package.compiler.api
compiler-tests   -> test target
```

The same modules may participate in multiple targets.

---

# Milestone 1 Module Boundary Addendum

For M1, the smoke executable is compiled as a single root module with stable module identity.

M1 does not implement:

- imports;
- package resolution;
- multi-file modules;
- full module graph;
- compiled semantic module artifacts;
- versioned dependencies.

This is a milestone boundary, not a rejection of the full module system.

Future `Veyr.toml` targets choose root modules. A target kind such as `executable`, `staticlib`, `sharedlib`, `plugin`, `test`, or `bench` determines the artifact kind built from the module graph.
