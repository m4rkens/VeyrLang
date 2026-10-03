# Milestone 1 — Compiler Walking Skeleton

**Status:** First implementation milestone direction

## 1. Goal

Milestone 1 proves that the Veyr compiler pipeline can turn a tiny source file into a native executable returning a known exit code.

It is not a language completeness milestone.

## 2. Scope

M1 includes:

- repository/build foundation;
- source identity and source manager;
- byte-offset source spans;
- diagnostics and plain renderer;
- token model;
- lexer for the M1 Experimental Smoke Profile;
- parser for the smoke profile only;
- minimal AST for the smoke program;
- minimal semantic validation;
- minimal lowering path;
- LLVM backend smoke;
- compiler driver smoke;
- end-to-end executable exit-code test.

## 3. Smoke profile rule

Before lexer/parser work, create:

```text
docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md
```

This file is the canonical M1 syntax authority. It must state that all concrete spellings are milestone-local and non-final.

## 4. Smoke program shape

The smoke program is conceptually:

```text
single root module
fixed entry point
single integer-literal return
```

Exact spelling is chosen in the smoke-profile document, not in code first.

## 5. Non-goals

M1 must not implement:

- final grammar;
- general functions;
- imports;
- package system;
- module graph;
- comments;
- full expressions;
- arithmetic;
- variables;
- user-defined types;
- generics;
- ownership semantics;
- standard library;
- general CLI project system;
- GPU/FFI/concurrency features.

## 6. Phase breakdown suggestion

| Phase | Scope |
|---|---|
| 1 | CMake/Ninja/C++23/GTest/LLVM discovery foundation |
| 2 | Source identity, source manager, source spans, diagnostics |
| 3 | M1 smoke profile document, token model, lexer |
| 4 | Parser for smoke profile |
| 5 | Minimal AST and semantic validation |
| 6 | Minimal IR/lowering model |
| 7 | Single root module path / module identity placeholder |
| 8 | LLVM backend smoke |
| 9 | `veyrc` driver smoke |
| 10 | End-to-end executable exit-code verification |

## 7. Success criteria

- clean configure/build/test;
- no hidden dependency downloads when disabled;
- LLVM provider/bootstrap policy honored; managed/system/custom paths remain explicit and compatibility-checked;
- include-style check passes;
- diagnostics have source spans;
- invalid M1 input produces lexical/parser/semantic diagnostics as appropriate;
- executable returns expected code;
- docs distinguish M1 profile from final language design.
