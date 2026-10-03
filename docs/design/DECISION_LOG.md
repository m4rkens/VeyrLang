# Decision Log

This file records accepted design directions in compact form.

**Date baseline:** 2026-07-09  
**Note:** exact syntax remains provisional unless explicitly stated.

## D-001 — Native compiled language

**Status:** Accepted  
**Decision:** ahead-of-time native compilation; no mandatory VM or JIT.

## D-002 — Implementation baseline

**Status:** Accepted  
**Decision:** initial compiler implementation in C++23 using LLVM, CMake, Ninja, GoogleTest/GoogleMock.

## D-003 — Static and strong typing

**Status:** Accepted  
**Decision:** static, strong typing with local inference and explicit public/API boundaries.

## D-004 — Immutable by default

**Status:** Accepted  
**Decision:** bindings are immutable by default; mutability is explicit.

## D-005 — Hybrid safety model

**Status:** Accepted  
**Decision:** type-system invariants + flow-sensitive analyzer + explicit unsafe escape hatch.

## D-006 — Manual memory management is not automatically unsafe

**Status:** Accepted  
**Decision:** manual allocation, deallocation, ownership, custom allocators, and resource management belong to the normal analyzable model.

## D-007 — Unsafe boundary

**Status:** Accepted  
**Decision:** raw dereference, unchecked pointer arithmetic, arbitrary reinterpretation, unchecked casts, and manual bypass of ownership/lifetime rules require explicit unsafe context.

## D-008 — Zero-cost or explicit-cost abstractions

**Status:** Accepted  
**Decision:** hidden avoidable cost is contrary to language philosophy.

## D-009 — No language UB in checked code

**Status:** Accepted direction  
**Decision:** ordinary checked operations have defined behavior; UB is restricted to unsafe/foreign contract violations.

## D-010 — Diagnostics are part of design

**Status:** Accepted  
**Decision:** features require valid examples, invalid examples, expected diagnostics, and remediation paths.

## D-011 — Memory categories

**Status:** Accepted direction  
**Decision:** ordinary values, unique owners, immutable borrows, mutable borrows, slices, planned shared ownership, and raw pointers are distinct concepts.

## D-012 — Unique ownership core

**Status:** Accepted  
**Decision:** unique ownership is core; shared ownership is optional library-level functionality.

## D-013 — Explicit move

**Status:** Accepted  
**Decision:** move between live named values is explicit; temporaries and dying values may move implicitly.

## D-014 — Copy versus clone

**Status:** Accepted direction  
**Decision:** implicit Copy is only for semantically simple, predictably cheap types; expensive duplication is explicit.

## D-015 — Deterministic Drop

**Status:** Accepted  
**Decision:** resource cleanup is deterministic and tied to lifetime end.

## D-016 — `defer`

**Status:** Accepted  
**Decision:** local scope cleanup is core, LIFO, registered at runtime control reach, and analyzed for lifetime effects.

## D-017 — `errdefer`

**Status:** Candidate / strong planned direction  
**Decision:** preserve as a future rollback mechanism tied to typed error exits.

## D-018 — Lifetime inference first

**Status:** Accepted  
**Decision:** ordinary code should not require explicit lifetime annotations.

## D-019 — Non-lexical, flow-sensitive borrowing

**Status:** Accepted  
**Decision:** borrows end at last possible use; analysis tracks control flow, provenance, fields, and invalidation.

## D-020 — Safe references are non-null

**Status:** Accepted  
**Decision:** Ref/MutRef-like concepts are non-null; absence uses Option; raw pointers may be null.

## D-021 — Result/Option error model

**Status:** Accepted  
**Decision:** recoverable failures are typed values; absence is a tagged union.

## D-022 — Exceptions are not core

**Status:** Accepted  
**Decision:** exceptions are not the primary recoverable-error mechanism.

## D-023 — Panic for broken invariants

**Status:** Accepted  
**Decision:** panic is defined behavior for broken invariants; initial strategy is abort.

## D-024 — `?`-style explicit propagation

**Status:** Accepted direction  
**Decision:** error propagation is visible at call sites.

## D-025 — Result must-use

**Status:** Accepted  
**Decision:** ignoring a recoverable failure requires explicit action.

## D-026 — Tagged unions

**Status:** Accepted  
**Decision:** tagged unions are the primary enum/variant mechanism.

## D-027 — Exhaustive match

**Status:** Accepted  
**Decision:** pattern matching is core and exhaustive by default; match is an expression.

## D-028 — Strong nominal newtypes

**Status:** Accepted  
**Decision:** newtypes provide distinct semantic types with zero-cost representation.

## D-029 — Limited implicit conversions

**Status:** Accepted  
**Decision:** only narrow classes of provably safe widening conversions are implicit; lossy and semantic conversions are explicit.

## D-030 — Defined arithmetic failure

**Status:** Accepted direction  
**Decision:** ordinary integer overflow and division by zero panic rather than causing UB; wrapping/saturating/checked modes are explicit.

## D-031 — Ownership-visible calls

**Status:** Accepted direction  
**Decision:** free-function call sites should make borrow, mutable borrow, and move semantically visible.

## D-032 — Left-to-right argument evaluation

**Status:** Accepted  
**Decision:** argument evaluation order is defined.

## D-033 — Function overloading

**Status:** Accepted  
**Decision:** overloads are allowed; return type alone cannot distinguish them; ambiguity is an error.

## D-034 — Closure capture ownership is explicit

**Status:** Accepted  
**Decision:** copy/ref/mut-ref/move capture modes are explicit; closures do not silently heap-allocate.

## D-035 — Struct + impl + trait model

**Status:** Accepted  
**Decision:** data, behavior, and contracts are separate concepts.

## D-036 — No core class/state inheritance

**Status:** Accepted direction  
**Decision:** composition, traits, trait defaults, and possible explicit delegation replace state inheritance in the core model.

## D-037 — Static dispatch by default

**Status:** Accepted  
**Decision:** generics + trait constraints use static dispatch; dynamic dispatch is explicit.

## D-038 — Dynamic dispatch does not imply allocation

**Status:** Accepted  
**Decision:** borrowed or owned trait erasure must not silently heap-allocate.

## D-039 — Trait coherence

**Status:** Accepted direction  
**Decision:** avoid foreign-trait + foreign-type global implementations; use newtypes for adaptation.

## D-040 — Generics checked against declared contracts

**Status:** Accepted  
**Decision:** no accidental C++ template duck typing; generic bodies are type-checked from constraints.

## D-041 — Monomorphization

**Status:** Accepted implementation direction  
**Decision:** static generic instantiation is the default model; internal code merging is an optimization.

## D-042 — Same-language compile-time programming

**Status:** Accepted  
**Decision:** const evaluation uses the same language, not a separate template language.

## D-043 — Typed introspection before macros

**Status:** Accepted direction  
**Decision:** const evaluation and typed introspection precede derive/code generation; textual preprocessor is a non-goal.

## D-044 — Controlled operator customization

**Status:** Accepted direction  
**Decision:** fixed operator set with trait-based customization; no arbitrary operator invention; ownership/control-flow operators are not overloadable.

## D-045 — Universal module model

**Status:** Accepted  
**Decision:** every executable, static library, and shared library is built from modules.

## D-046 — All code belongs to modules

**Status:** Accepted  
**Decision:** no source declarations exist outside the module system.

## D-047 — One file equals one module by default

**Status:** Accepted direction  
**Decision:** hierarchical submodules are preferred; multi-file modules remain candidate-only.

## D-048 — Imports are semantic and side-effect free

**Status:** Accepted  
**Decision:** imports do not include text and do not execute hidden runtime initialization.

## D-049 — Module DAG

**Status:** Accepted direction  
**Decision:** initial module graph is acyclic; type-only cycles are deferred.

## D-050 — Public source API separate from binary export

**Status:** Accepted  
**Decision:** language visibility and exported linker symbols are different concepts.

## D-051 — Compiled semantic module artifacts

**Status:** Accepted architecture  
**Decision:** module artifacts carry public types, generic typed representations, borrow relationships, invalidation effects, and other semantic metadata.

## D-052 — Native threads, no mandatory scheduler

**Status:** Accepted  
**Decision:** native OS threads are core; no required global concurrency runtime.

## D-053 — Safe code prevents memory data races

**Status:** Accepted direction  
**Decision:** conflicting shared access requires synchronization or atomics.

## D-054 — Transfer and sharing are distinct capabilities

**Status:** Accepted  
**Decision:** Send-like and Sync-like concepts are distinct and mostly inferred.

## D-055 — Shared state synchronization is explicit

**Status:** Accepted  
**Decision:** Mutex<T>-like types own protected data; guards provide scoped access.

## D-056 — Atomics are core

**Status:** Accepted  
**Decision:** safe default memory ordering with explicit weaker ordering.

## D-057 — Job system is a library

**Status:** Accepted  
**Decision:** language primitives should be sufficient to build a high-performance job system without mandatory compiler/runtime magic.

## D-058 — async/await deferred to probes

**Status:** Accepted  
**Decision:** do not add async syntax until real I/O probes justify it.

## D-059 — Apache-2.0 current repository license

**Status:** Accepted for current phase  
**Decision:** repository starts under Apache License 2.0; future relicensing remains possible while ownership permits.

## D-060 — Veyr is the final language name

**Status:** Accepted  
**Decision:** the programming language is named **Veyr**. Use `Veyr` consistently across design documentation, GSD planning, Codex instructions, code examples, repository naming, and future ecosystem discussions. Tool names and file extensions remain separate open questions until explicitly decided.

---

## D-050 — Toolchain names

**Status:** Accepted starter direction  
**Decision:** use `veyr` as the primary project/build/package CLI and `veyrc` as the compiler frontend/driver. Use `veyrcore` for the internal compiler library.

## D-051 — Source and manifest file names

**Status:** Accepted starter direction  
**Decision:** Veyr source files use `.vyr`; project manifests are `Veyr.toml`; lockfiles are `Veyr.lock`; local project metadata/cache lives under `.veyr/`.

## D-052 — Supporting tool names

**Status:** Accepted starter direction  
**Decision:** use `veyr-lsp`, `veyrfmt`, `veyr-analyze`, `veyr-doc`, `veyr-bindgen`, and future `veyr-gpu-bindgen` for the toolchain roadmap.

## D-053 — GSD phase boundary rule

**Status:** Accepted process decision  
**Decision:** GSD discussion and planning phases must not create implementation files or run CMake/Ninja/tests. Execution starts only after an explicit `$gsd-execute-phase N` from the project owner.

## D-054 — Dependency and LLVM provider policy

**Status:** Superseded by D-061  
**Decision:** The original external-only LLVM rule is retained as historical context but is no longer the active policy.

## D-055 — Include style policy

**Status:** Accepted implementation policy  
**Decision:** Veyr-owned C++ headers are included with `<veyr/...>` angle-bracket paths only. Quoted includes and relative include paths are forbidden for Veyr-owned headers.

## D-056 — Compile database policy

**Status:** Accepted implementation policy  
**Decision:** development CMake presets must enable `CMAKE_EXPORT_COMPILE_COMMANDS=ON` for clangd/editor tooling.

## D-057 — M1 Experimental Smoke Profile

**Status:** Accepted process/design boundary  
**Decision:** before lexer implementation, create a dedicated `M1_EXPERIMENTAL_SMOKE_PROFILE.md` that defines the milestone-local smoke subset. Implementation support for that profile is not final Veyr grammar acceptance.

## D-058 — M1 single root module boundary

**Status:** Accepted milestone boundary  
**Decision:** the first executable is compiled as one single root module with stable module identity. M1 does not implement imports, package resolution, full module graph, or compiled semantic module artifacts.

## D-059 — Project manifest direction

**Status:** Accepted starter direction  
**Decision:** Veyr projects use `Veyr.toml` as the package/target/profile/dependency manifest, with `veyr` responsible for project orchestration and `veyrc` responsible for compilation.

## D-060 — Roadmap by validation projects

**Status:** Accepted roadmap principle  
**Decision:** major language/toolchain features should be pulled by design probes and milestone projects, not implemented because they are fashionable or present in other languages.


---

## D-061 — Managed LLVM SDK provisioning

**Status:** Superseded by D-064  
**Decision:** Historical implementation policy from the earlier starter. The custom managed
prebuilt-SDK provisioning direction is no longer active for the fresh GSD lifecycle.

## D-062 — Cross-platform developer bootstrap

**Status:** Accepted implementation policy  
**Decision:** Veyr will provide equivalent developer bootstrap/build entry points for Windows and Linux, with future macOS feasibility. The workflow should expose setup, doctor, configure, build, test, verify, and all-style operations while keeping platform-specific implementation behind equivalent wrappers. Managed SDK/dependency state and generated compatibility evidence are machine-local and must not be committed.

## D-063 — GSD lifecycle stage commits

**Status:** Accepted process decision  
**Decision:** Reviewed GSD lifecycle artifacts should be committed at their own boundaries instead of being folded into one end-of-phase commit. When tracked changes exist, discussion, planning, execution, and verification each receive separate commits after owner review/approval. Empty stages do not require empty commits. Execution/gap-fix commits remain subject to the sensitive-file owner review gate. No stage completion grants automatic commit permission.


## D-064 — Conan 2 for compiler implementation dependencies

**Status:** Accepted starter architecture direction  
**Decision:** Conan 2 owns third-party C/C++ dependency/package mechanics for the Veyr compiler
implementation. CMake remains the Veyr build graph, Ninja the initial build executor, Clang the
primary compiler family, and a thin Python wrapper may orchestrate the workflow. The project must
not recreate Conan's dependency solving, source acquisition, package storage/cache, or transitive
package graph in bespoke Veyr infrastructure.

## D-065 — LLVM through Conan

**Status:** Accepted starter architecture direction  
**Decision:** The initial LLVM target is 23.1.2 and should be consumed through Conan. Prefer an
existing suitable recipe; if exact LLVM 23.1.2 is unavailable, maintain a narrow project-owned
recipe. A long first source build is acceptable and should be amortized through Conan cache/CI
reuse instead of custom prebuilt-SDK provisioning.

## D-066 — Quiet build supervision

**Status:** Accepted workflow correction  
**Decision:** Approximately 60 seconds without stdout/stderr is a soft liveness checkpoint, not an
automatic failure. Long CMake/Ninja/Conan/compiler/dependency builds continue while credible
process/progress signals exist. Automatic duplicate retries and destructive cleanup remain
forbidden.
