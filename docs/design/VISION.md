# Veyr Language Vision

**Status:** Foundational design document  
**Version:** 0.1  
**Language name:** Veyr  
**Syntax status:** provisional unless explicitly recorded as accepted

## 1. Mission

Veyr is a compiled, statically typed, strongly typed, general-purpose programming language for native system and application development.

Its goal is to combine:

- low-level control comparable to C and C++;
- explicit and predictable ownership of memory and resources;
- modern type-driven abstractions;
- zero-cost or explicit-cost abstractions;
- built-in static analysis for memory and resource errors;
- native interoperability with operating systems and C ecosystems;
- deterministic execution without a mandatory garbage collector, virtual machine, or global runtime.

The language is not intended to be a safer syntax layer over C++, a smaller Rust clone, or C with cosmetic changes. It should develop its own coherent answer to the following question:

> How can a programmer retain system-level control while moving as much error detection as practical into the compiler and integrated analyzer?

## 2. Constitutional pillars

### 2.1 Control

The programmer should be able to reason about and control:

- allocation and deallocation;
- resource lifetime;
- ownership transfer;
- custom allocators and memory regions;
- data layout;
- native ABI boundaries;
- concurrency and synchronization;
- dynamic dispatch;
- compile-time work;
- interaction with operating-system and native APIs.

The language must not force a garbage collector, virtual machine, JIT, hidden scheduler, mandatory global thread pool, or implicit reference counting.

### 2.2 Safety through analysis

The language should prefer analysis and explicit proof obligations over either extreme:

- unrestricted low-level behavior with delayed runtime failure; or
- banning useful system-level operations simply because they can be misused.

The compiler and analyzer should aim to detect, at minimum:

- use-after-free;
- double free;
- use after move;
- resource leaks;
- invalidated references and views;
- invalid borrowing relationships;
- obvious aliasing violations;
- thread-transfer violations;
- unsynchronized conflicting access in safe code.

More advanced analysis, such as deadlock detection, higher-level race detection, taint tracking, or API misuse analysis, may be added later.

### 2.3 Predictability

A programmer should be able to answer, from the API and code structure, questions such as:

- Does this allocate?
- Does this copy?
- Does this transfer ownership?
- Can this function fail?
- Can this call end the current control flow?
- Does this use dynamic dispatch?
- Does this block?
- Who owns this resource?
- When is cleanup performed?
- Can an existing reference be invalidated?

Important costs and semantic transitions should be visible. Convenience is welcome; hidden behavior is not.

### 2.4 Explicit boundaries

Veyr uses explicit boundaries for operations the compiler cannot fully verify.

Manual memory management itself is not unsafe. Ordinary allocation, deallocation, ownership transfer, custom allocation, deterministic cleanup, and resource handling belong to the analyzable language model.

Operations such as the following belong behind an explicit unsafe boundary:

- dereferencing raw pointers;
- unchecked pointer arithmetic;
- arbitrary memory reinterpretation;
- unchecked casts;
- constructing references from unverifiable raw addresses;
- bypassing ownership or lifetime rules;
- asserting low-level invariants the compiler cannot prove.

The principle is:

> Low-level does not automatically mean unsafe. Unsafe begins where the program deliberately leaves the analyzable model.

## 3. Execution model

Veyr compiles ahead of time to native code.

The long-term compiler pipeline is expected to evolve approximately as follows:

```text
source
  -> lexer
  -> parser
  -> AST
  -> semantic analysis
  -> typed high-level IR
  -> lower-level IR
  -> LLVM IR
  -> object code
  -> executable or library artifact
```

The implementation baseline is:

- C++23 compiler implementation;
- LLVM backend;
- CMake + Ninja;
- GoogleTest / GoogleMock;
- warnings treated seriously, preferably as errors in project code;
- strong module boundaries inside the compiler.

The first milestone must not attempt to build the full language.

## 4. Core language character

The accepted direction is:

- compiled native language;
- static and strong typing;
- immutable bindings by default;
- explicit mutability;
- procedural, object-oriented, functional, generic, data-oriented, and low-level programming styles;
- no requirement that everything be an object;
- no requirement that everything be immutable;
- manual and explicit memory control;
- unique ownership as the core ownership model;
- shared ownership as an optional library-level mechanism;
- deterministic destruction;
- `defer`-style local cleanup;
- tagged unions as the primary enum/variant mechanism;
- exhaustive pattern matching;
- explicit recoverable errors through typed values;
- explicit dynamic dispatch;
- static polymorphism through generics and traits;
- module-first project organization;
- native OS threads and explicit synchronization;
- no mandatory global concurrency runtime.

## 5. Safety model

The accepted safety direction is hybrid:

1. the type system enforces fundamental invariants;
2. a flow-sensitive analyzer tracks control flow, ownership, provenance, lifetime, invalidation, and resource state;
3. operations that cannot be verified remain available through explicit unsafe boundaries.

Safe or checked language operations must not produce language-level undefined behavior when used according to their contracts.

The current direction is:

- ordinary out-of-bounds access: defined panic, not UB;
- ordinary integer overflow: defined panic, not UB;
- checked, wrapping, saturating, and other arithmetic modes: explicit operations;
- unchecked memory access: unsafe;
- FFI: a low-level boundary whose safety depends on explicit contracts and wrappers.

## 6. Error philosophy

The language distinguishes:

```text
recoverable failure          -> typed Result-like value
missing optional value       -> Option-like value
broken invariant             -> panic
immediate process termination -> abort
unverifiable low-level action -> unsafe
```

Recoverable failures are values and part of function signatures.

Exceptions are not part of the core error model.

Error propagation should be explicit at the call site, with `?`-style syntax or an equivalent mechanism.

Panic is reserved for broken invariants and programmer errors. The initial panic strategy is expected to be process abort, with unwind support only as a future candidate.

## 7. Zero-cost and explicit-cost abstractions

The project adopts the following design rule:

> An abstraction should not impose a hidden cost that an equivalent manual implementation could avoid. When an abstraction has a cost, that cost should be predictable and part of its contract.

This applies especially to:

- allocations;
- copies;
- reference counting;
- synchronization;
- dynamic dispatch;
- runtime metadata;
- bounds checks;
- stack traces;
- compile-time generated runtime support.

This does not mean every abstraction is free. It means costs should not be accidental or invisible.

## 8. Data and object model

The accepted direction separates several concerns:

```text
struct       -> data
impl         -> behavior
trait        -> contract or capability
composition  -> state reuse
trait defaults / delegation -> behavior reuse
static generics -> static polymorphism
dyn Trait-like form -> explicit dynamic polymorphism
```

Class/state inheritance is not part of the core direction.

OOP remains fully possible through state, methods, encapsulation, ownership, traits, and explicit dynamic dispatch.

## 9. Modules are universal

Every project is a graph of modules.

This is true for:

- executables;
- static libraries;
- shared libraries;
- test targets;
- tools.

Artifact kind changes how a module graph is linked and exported; it does not create a different code-organization model.

All code belongs to a module. Imports reference semantic module interfaces, not source text. Imports do not have runtime side effects.

## 10. Concurrency model

The accepted direction is:

- native OS threads as a foundation;
- explicit ownership transfer across threads;
- safe code prevents memory data races;
- explicit synchronization for shared mutable state;
- atomic types as core low-level synchronization primitives;
- job systems as libraries built on language primitives;
- channels as ownership-transfer abstractions;
- async/await deferred until real I/O design probes demonstrate the need.

No mandatory scheduler or global runtime is assumed.

## 11. Tooling and diagnostics

Diagnostics are a language-design requirement, not a late implementation detail.

A feature is not fully designed until the project can describe:

- a valid use;
- an invalid use;
- why the invalid use is unsafe or incorrect;
- the source locations involved;
- a useful remediation path.

For example, a use-after-free diagnostic should identify:

- where a reference or view originated;
- where the owner or backing storage was released or invalidated;
- where the invalid use occurs;
- how the code can be restructured.

The compiler should avoid diagnostic styles that expose only internal implementation details.

## 12. Non-goals

The current non-goals are:

- mandatory garbage collection;
- mandatory virtual machine;
- dynamic typing as the primary language model;
- mandatory JIT;
- everything-is-an-object semantics;
- hidden heap allocation as a normal language mechanism;
- arbitrary operator invention;
- textual C-style preprocessor;
- implicit module initialization through imports;
- unrestricted class/state inheritance in the core model;
- exceptions as the primary recoverable-error mechanism;
- forced thread termination in the safe standard API;
- requiring explicit lifetime annotations for ordinary programming.

## 13. Design process

New language features must follow this order:

1. identify a real use case;
2. reference at least one design probe;
3. describe the semantic problem;
4. compare alternatives;
5. define semantics before syntax;
6. specify interactions with ownership, borrowing, errors, generics, modules, FFI, and concurrency;
7. provide valid examples;
8. provide invalid examples;
9. design diagnostics;
10. define compiler lowering and metadata requirements;
11. add tests;
12. record the decision.

Syntax-first feature design is explicitly discouraged.

## 14. Provisional syntax sample

The following is a sketch, not final grammar:

```text
import std.io as io;

func main() -> int32 {
    let a: uint32 = 12;
    let mut b := 3;

    io.println("Hello, world!");

    return 0;
}
```

Conceptual ownership-aware calls may look like:

```text
validate(ref config);
normalize(mut ref config);
start_server(move config);
```

These examples preserve accepted semantics but do not freeze exact token spelling.
