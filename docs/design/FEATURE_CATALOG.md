# Feature Catalog

**Purpose:** track the design status of language capabilities without confusing accepted semantics with future ideas.

## Status vocabulary

- **CORE** — accepted foundational direction of the language.
- **PLANNED** — intended feature, but exact semantics or implementation phase may still be incomplete.
- **CANDIDATE** — promising idea that must be validated by design probes before acceptance.
- **DEFERRED** — intentionally postponed because prerequisites or real use cases are missing.
- **NON-GOAL** — deliberately outside the current language direction.
- **REJECTED** — considered and rejected; preserve the rationale in the decision log when relevant.
- **OPEN** — unresolved design question.

Exact syntax remains provisional unless separately accepted.

# 1. Execution and compiler architecture

| ID | Feature | Status | Notes |
|---|---|---:|---|
| EXEC-001 | Ahead-of-time native compilation | CORE | No mandatory VM or JIT. |
| EXEC-002 | LLVM backend | CORE direction | Initial backend direction. |
| EXEC-003 | Typed high-level IR | PLANNED | Needed for semantic analysis, diagnostics, and later lowering. |
| EXEC-004 | Lower-level IR before LLVM IR | PLANNED | Exact number of IR layers remains open. |
| EXEC-005 | C++23 compiler implementation | CORE implementation choice | Initial implementation. |
| EXEC-006 | CMake + Ninja | CORE implementation choice | Initial build baseline. |
| EXEC-007 | GoogleTest / GoogleMock | CORE implementation choice | Initial testing baseline. |
| EXEC-008 | Incremental compilation | CORE architectural goal | Module artifacts should enable it. |

# 2. Safety and unsafe boundaries

| ID | Feature | Status | Notes |
|---|---|---:|---|
| SAFE-001 | Hybrid safety model | CORE | Type system + flow-sensitive analysis + explicit unsafe escape hatch. |
| SAFE-002 | Manual memory management in safe code | CORE | Manual allocation is not automatically unsafe. |
| SAFE-003 | Explicit unsafe blocks | CORE | Exact syntax provisional. |
| SAFE-004 | Unsafe functions with caller proof obligations | CORE | Useful for raw-memory constructors and FFI. |
| SAFE-005 | Raw pointer dereference requires unsafe | CORE | Raw pointer storage itself need not be unsafe. |
| SAFE-006 | Unchecked pointer arithmetic requires unsafe | CORE | Checked higher-level abstractions remain safe. |
| SAFE-007 | Arbitrary memory reinterpretation restricted | CORE | Defined bit casts may be safe; arbitrary object reinterpretation is unsafe. |
| SAFE-008 | Safe code must not produce language UB | CORE | Checked operations have defined behavior. |
| SAFE-009 | Integrated analyzer | CORE | Not an optional linter. |
| SAFE-010 | Advanced taint/API misuse analysis | DEFERRED | Long-term analyzer direction. |

# 3. Ownership and memory

| ID | Feature | Status | Notes |
|---|---|---:|---|
| MEM-001 | Value semantics | CORE | `T` is an inline value by default. |
| MEM-002 | Unique ownership | CORE | Conceptually `Owner<T>`; exact spelling open. |
| MEM-003 | Explicit move between live named values | CORE | Ownership transfer must be visible. |
| MEM-004 | Inferred move for temporaries and dying values | CORE | Avoids useless ceremony. |
| MEM-005 | Use-after-move rejection | CORE | Tracked statically. |
| MEM-006 | Copy capability | CORE | Only for semantically simple, predictably cheap copies. |
| MEM-007 | Explicit clone for potentially expensive duplication | CORE direction | Exact trait/API open. |
| MEM-008 | Deterministic destruction | CORE | Resource cleanup at lifetime end. |
| MEM-009 | Drop capability/protocol | CORE | Exact syntax provisional. |
| MEM-010 | Custom allocators | CORE direction | Essential system-language capability. |
| MEM-011 | Arena/pool/linear allocation | CORE capability goal | Primarily library abstractions over core primitives. |
| MEM-012 | Shared ownership | PLANNED | Library-level; should not define the core memory model. |
| MEM-013 | Pinning / immovable storage | CANDIDATE | Validate through async, intrusive-container, and FFI probes. |
| MEM-014 | Must-consume resources | CANDIDATE | Useful for join handles and transactions. |

# 4. Borrowing and lifetime analysis

| ID | Feature | Status | Notes |
|---|---|---:|---|
| BOR-001 | Immutable borrowed reference | CORE | Conceptually `Ref<T>`. Non-null. |
| BOR-002 | Mutable borrowed reference | CORE | Conceptually `MutRef<T>`. Exclusive for overlapping mutable access. |
| BOR-003 | Immutable slice | CORE | Borrowed contiguous range with length. |
| BOR-004 | Mutable slice | CORE | Mutable borrowed range with overlap restrictions. |
| BOR-005 | Non-lexical lifetimes | CORE | Borrow ends at last possible use. |
| BOR-006 | Flow-sensitive lifetime analysis | CORE | Control-flow and data-flow aware. |
| BOR-007 | Provenance tracking | CORE | References and views remember their origin. |
| BOR-008 | Resource identity tracking | CORE direction | Ownership may move while the underlying resource remains stable. |
| BOR-009 | Invalidation effects | CORE | APIs can declare that operations invalidate borrows, slices, or iterators. |
| BOR-010 | Field-sensitive borrowing | CORE direction | Distinct fields may be borrowed independently. |
| BOR-011 | Disjoint mutable region analysis | CORE direction | Allow provably non-overlapping mutable slices. |
| BOR-012 | Inferred borrow contracts across modules | CORE | Stored in compiled module metadata. |
| BOR-013 | Explicit lifetime annotations | CANDIDATE | Escape hatch for cases inference cannot express. |

# 5. Cleanup

| ID | Feature | Status | Notes |
|---|---|---:|---|
| CLN-001 | Drop-based type cleanup | CORE | Deterministic. |
| CLN-002 | `defer`-style scope cleanup | CORE | Runs on controlled scope exit. |
| CLN-003 | LIFO deferred cleanup | CORE | Reverse registration order. |
| CLN-004 | Deferred-use lifetime analysis | CORE | Values used by defer remain live as required. |
| CLN-005 | `errdefer` | CANDIDATE / strong planned direction | Depends on final error semantics; runs on Err-based exits. |
| CLN-006 | Unified cleanup stack | CORE design direction | Drop/defer/errdefer ordering should be coherent. |
| CLN-007 | Cleanup does not propagate a second error | CORE | Explicit close/flush may return errors before scope exit. |

# 6. Errors and panic

| ID | Feature | Status | Notes |
|---|---|---:|---|
| ERR-001 | Typed recoverable errors as values | CORE | Result-like model. |
| ERR-002 | Option-like absence type | CORE | No null in ordinary values or references. |
| ERR-003 | `?`-style explicit propagation | CORE direction | Exact syntax accepted conceptually. |
| ERR-004 | Result is must-use by default | CORE | Ignoring a failure requires an explicit action. |
| ERR-005 | Panic for broken invariants | CORE | Not for ordinary recoverable failures. |
| ERR-006 | Panic is defined behavior, not UB | CORE | Initial strategy: abort. |
| ERR-007 | Panic unwind | CANDIDATE | Not needed for initial runtime. |
| ERR-008 | Exceptions as core error model | NON-GOAL | May still require FFI translation for foreign exceptions. |
| ERR-009 | Explicit error conversion by default | CORE direction | Automatic conversion requires an explicit contract if added later. |
| ERR-010 | Error-union syntax | CANDIDATE | Tagged unions are sufficient initially. |
| ERR-011 | Type-erased dynamic error container | PLANNED | Higher-level library feature. |
| ERR-012 | OOM policy | OPEN | Core allocators likely fallible; high-level policy unresolved. |

# 7. Primitive and composite types

| ID | Feature | Status | Notes |
|---|---|---:|---|
| TYPE-001 | Fixed-width signed integers | CORE | int8..int128 direction. |
| TYPE-002 | Fixed-width unsigned integers | CORE | uint8..uint128 direction. |
| TYPE-003 | isize / usize | CORE | Pointer-width integers. |
| TYPE-004 | float32 / float64 | CORE | Additional widths later if justified. |
| TYPE-005 | bool as a real logical type | CORE | No implicit integer truthiness. |
| TYPE-006 | byte distinct from uint8 | CORE direction | Same possible layout, different semantics. |
| TYPE-007 | Unicode scalar `char` | CORE direction | Not an 8-bit integer. |
| TYPE-008 | void as a real zero-size type | CORE direction | Useful in generic Result-like types. |
| TYPE-009 | never type | CORE | For panic, abort, and non-returning paths. |
| TYPE-010 | Structs | CORE | Product types. |
| TYPE-011 | Tuples | CORE | Small local grouping and destructuring. |
| TYPE-012 | Fixed-size arrays | CORE | Length known at compile time. |
| TYPE-013 | Dynamic vector | PLANNED stdlib | Not a primitive language type. |
| TYPE-014 | Type aliases | CORE | Same semantic type, new name. |
| TYPE-015 | Zero-cost nominal newtypes | CORE | Same representation, distinct semantic type. |
| TYPE-016 | Nullable ordinary values/references | NON-GOAL | Use Option-like type. |
| TYPE-017 | Raw null pointer | CORE low-level | Restricted to raw pointer world. |

# 8. Tagged unions and matching

| ID | Feature | Status | Notes |
|---|---|---:|---|
| SUM-001 | Tagged unions as primary enum mechanism | CORE | Variants may carry payloads. |
| SUM-002 | Payload variants | CORE | Positional and named-field forms. |
| SUM-003 | Exhaustive pattern matching | CORE | Missing cases are compile errors unless explicit fallback. |
| SUM-004 | Match expressions | CORE | Branch types must be compatible. |
| SUM-005 | Nested patterns | CORE | Important for AST and Result/Option composition. |
| SUM-006 | Pattern guards | PLANNED | High-value feature, later parser phase. |
| SUM-007 | Ownership-aware matching | CORE | Match by value, borrow, or mutable borrow. |
| SUM-008 | Partial-move tracking | CORE direction | Field-sensitive moved state. |
| SUM-009 | Closed variants by default | CORE | Supports exhaustiveness. |
| SUM-010 | Non-exhaustive public enums | PLANNED | Needed for evolvable APIs. |
| SUM-011 | Niche layout optimization | CORE optimization principle | No semantic change. |
| SUM-012 | Untagged unions | LOW-LEVEL / PLANNED | FFI and layout tricks; unsafe reads when activity is not proven. |

# 9. Conversions and arithmetic

| ID | Feature | Status | Notes |
|---|---|---:|---|
| CONV-001 | Limited implicit widening | CORE | Same-family, non-lossy conversions only. |
| CONV-002 | Compile-time literal fitting | CORE | Literals may fit contextual types when provable. |
| CONV-003 | Explicit checked conversion | CORE direction | Failure is represented explicitly. |
| CONV-004 | Explicit lossy conversion modes | CORE direction | Truncate, round, saturate, wrap, etc. |
| CONV-005 | Defined bit cast | PLANNED | Separate from arbitrary reinterpretation. |
| ARITH-001 | Ordinary integer overflow panics | CORE direction | Same semantics across build modes. |
| ARITH-002 | Wrapping arithmetic | CORE | Explicit operation. |
| ARITH-003 | Saturating arithmetic | CORE | Explicit operation. |
| ARITH-004 | Checked arithmetic | CORE | Explicit operation. |
| ARITH-005 | Division by zero panics | CORE direction | Checked alternative available. |

# 10. Functions and closures

| ID | Feature | Status | Notes |
|---|---|---:|---|
| CALL-001 | Explicit parameter and return types at API boundaries | CORE | Local inference remains allowed. |
| CALL-002 | Value parameters | CORE | Copy or ownership transfer according to type. |
| CALL-003 | Borrow parameters | CORE | Immutable and mutable forms. |
| CALL-004 | Call-site ownership markers | CORE direction | Conceptually `ref`, `mut ref`, `move`. Exact grammar open. |
| CALL-005 | Left-to-right argument evaluation | CORE | Defined ordering. |
| CALL-006 | Function overloading | CORE | Deterministic rules; ambiguity is an error. |
| CALL-007 | Return-type-only overloading | NON-GOAL | Too ambiguous. |
| CALL-008 | Default arguments | CANDIDATE / PLANNED | Validate API evolution implications. |
| CALL-009 | Named arguments | PLANNED | Public parameter names may become API. |
| CALL-010 | Methods with explicit receiver semantics | CORE | Observe, mutate, consume. |
| CALL-011 | First-class function values | CORE | Function pointers distinct from closures. |
| CALL-012 | Closures | CORE design | Explicit capture modes. |
| CALL-013 | No hidden closure heap allocation | CORE | Type-erased heap callables are explicit library types. |
| CALL-014 | One-shot callable capability | CORE direction | Important for jobs and ownership capture. |
| CALL-015 | Typed variadics | PLANNED | Needed for formatting and tuple utilities. |
| CALL-016 | C-style varargs | FFI ONLY | Not a safe core mechanism. |

# 11. Object model and traits

| ID | Feature | Status | Notes |
|---|---|---:|---|
| OBJ-001 | `struct` for data | CORE | Exact keyword likely accepted conceptually. |
| OBJ-002 | `impl` for behavior | CORE direction | Separates data from behavior. |
| OBJ-003 | Named creation functions | CORE direction | No magic constructor hierarchy. |
| OBJ-004 | Traits as contracts/capabilities | CORE | Explicit nominal implementation. |
| OBJ-005 | Trait default methods | CORE direction | Behavior reuse without state inheritance. |
| OBJ-006 | Trait refinement/composition | CORE direction | Compose contracts. |
| OBJ-007 | Associated types | CORE design | Important for iterators and abstractions. |
| OBJ-008 | Associated constants | PLANNED | Compile-time protocol data. |
| OBJ-009 | Static dispatch by default | CORE | Generics + traits. |
| OBJ-010 | Explicit dynamic dispatch | CORE | Conceptually `dyn Trait`. |
| OBJ-011 | Dynamic dispatch without hidden allocation | CORE | Borrowed and owned forms. |
| OBJ-012 | Class/state inheritance | NON-GOAL for core | Revisit only if probes reveal an unsolved problem. |
| OBJ-013 | Composition for state reuse | CORE direction | Preferred model. |
| OBJ-014 | Explicit delegation | CANDIDATE | Avoid implicit method promotion. |
| OBJ-015 | External arbitrary inherent methods | NON-GOAL direction | Extend foreign types through traits. |
| OBJ-016 | Runtime Any/type erasure | CANDIDATE / PLANNED | Not fundamental. |
| OBJ-017 | Mandatory RTTI for all types | NON-GOAL | Runtime metadata must be opt-in or demand-driven. |

# 12. Generics and compile-time programming

| ID | Feature | Status | Notes |
|---|---|---:|---|
| GEN-001 | Generic functions | CORE | Checked against declared constraints. |
| GEN-002 | Generic types | CORE | Containers and abstractions. |
| GEN-003 | Trait constraints | CORE | No accidental template duck typing. |
| GEN-004 | Monomorphization | CORE implementation model | Compiler may merge equivalent instances internally. |
| GEN-005 | Conditional methods/impls | CORE | Availability based on constraints. |
| GEN-006 | Const generics | CORE design | Start with conservative compile-time value kinds. |
| GEN-007 | Generic associated types | CANDIDATE | Later, driven by lending-iterator probes. |
| GEN-008 | Variadic generics | PLANNED | Formatting, tuples, FFI helpers. |
| GEN-009 | Higher-kinded types | DEFERRED | No current design probe requires them. |
| GEN-010 | User-visible specialization | DEFERRED / CANDIDATE | Prefer traits and overloads first. |
| CT-001 | Same-language compile-time evaluation | CORE | No separate template language. |
| CT-002 | Const functions | CORE direction | Deterministic evaluation. |
| CT-003 | Typed compile-time introspection | PLANNED | Fields, variants, capabilities, metadata. |
| CT-004 | Runtime reflection | CANDIDATE | Separate from compile-time introspection. |
| CT-005 | Derive-style generation | PLANNED | Built on introspection. |
| CT-006 | Textual preprocessor | NON-GOAL | No C-style token substitution. |
| CT-007 | Hygienic structured macros | CANDIDATE | Only after const eval and introspection. |

# 13. Modules, packages, and artifacts

| ID | Feature | Status | Notes |
|---|---|---:|---|
| MOD-001 | All code belongs to modules | CORE | No code outside module system. |
| MOD-002 | Executables are module graphs | CORE | Same model as libraries. |
| MOD-003 | Static libraries are module graphs | CORE | Artifact kind changes linking, not source organization. |
| MOD-004 | Shared libraries are module graphs | CORE | Explicit binary exports. |
| MOD-005 | One file = one module by default | CORE direction | Hierarchical submodules for larger systems. |
| MOD-006 | Multi-file modules | CANDIDATE | Only if probes justify complexity. |
| MOD-007 | Semantic imports | CORE | No textual inclusion. |
| MOD-008 | Imports have no runtime side effects | CORE | No hidden initialization. |
| MOD-009 | Module graph is acyclic | CORE direction | Type-only cycles deferred. |
| MOD-010 | Private by default | CORE direction | Publicness is explicit. |
| MOD-011 | Module/package/public visibility levels | CORE direction | Exact keywords open. |
| MOD-012 | `pub` separate from binary export | CORE | Source API and binary API are different. |
| MOD-013 | Compiled module artifacts | CORE architecture | Semantic metadata, not headers. |
| MOD-014 | Generic typed IR in module artifact | CORE architecture | Enables later instantiation. |
| MOD-015 | Borrow/invalidation metadata in module artifact | CORE architecture | Enables downstream analysis. |
| MOD-016 | Package manifests | PLANNED / required | Dependencies, targets, native libs. |
| MOD-017 | One package may produce multiple targets | CORE direction | exe/static/shared/test. |
| MOD-018 | Workspaces | PLANNED | Multiple packages. |
| MOD-019 | Type-only import cycles | DEFERRED | Revisit if design probes demand them. |

# 14. Concurrency

| ID | Feature | Status | Notes |
|---|---|---:|---|
| CONC-001 | Native OS threads | CORE | No mandatory scheduler. |
| CONC-002 | Fallible thread creation | CORE direction | Returns typed error. |
| CONC-003 | Explicit ownership transfer to threads | CORE | Thread closures obey normal move rules. |
| CONC-004 | Safe code prevents data races | CORE guarantee direction | Memory races, not all logical races. |
| CONC-005 | Send-like transfer capability | CORE design | Exact naming open. |
| CONC-006 | Sync-like shared-reference capability | CORE design | Exact naming open. |
| CONC-007 | Automatic capability inference | CORE direction | Structural where valid. |
| CONC-008 | Unsafe manual thread-safety assertion | CORE low-level | Needed for FFI wrappers. |
| CONC-009 | Thread-affine negative capability | PLANNED | Explicitly non-transferable types. |
| CONC-010 | Scoped threads | PLANNED high priority | Safe borrowed concurrency. |
| CONC-011 | Join handles as owned one-shot resources | CORE direction | Join or explicit detach. |
| CONC-012 | Mutex<T> owning protected state | PLANNED stdlib core | Guard-based access. |
| CONC-013 | Atomic<T> | CORE | Safe default ordering, explicit weaker ordering. |
| CONC-014 | Shared<T> | PLANNED stdlib | Shared ownership, not shared mutability. |
| CONC-015 | Channels | PLANNED stdlib | Ownership transfer. |
| CONC-016 | Job system | PLANNED reference library | Not mandatory runtime. |
| CONC-017 | Scoped jobs | PLANNED | Borrow-safe parallelism. |
| CONC-018 | Parallel algorithms | PLANNED | Library layer. |
| CONC-019 | async/await | CANDIDATE | Must be justified by I/O probes. |
| CONC-020 | Mandatory async runtime | NON-GOAL | Even if async syntax is added. |
| CONC-021 | Structured concurrency | PLANNED strong direction | Exact model open. |
| CONC-022 | Cooperative cancellation | PLANNED | Avoid forced thread termination. |
| CONC-023 | Forced safe thread termination | NON-GOAL | Low-level OS API may remain unsafe. |
| CONC-024 | Thread-local storage | PLANNED | Important but later. |
| CONC-025 | Deadlock analysis | DEFERRED analyzer feature | Long-term. |

# 15. FFI and ABI

| ID | Feature | Status | Notes |
|---|---|---:|---|
| FFI-001 | C ABI interoperability | CORE direction | Essential ecosystem bridge. |
| FFI-002 | Raw extern declarations | CORE direction | Exact syntax open. |
| FFI-003 | Explicit representation contracts | CORE | Structs/enums for ABI-sensitive cases. |
| FFI-004 | Raw FFI layer separate from safe wrapper layer | CORE direction | Preserve native ABI while enabling idiomatic safe APIs. |
| FFI-005 | Stable C ABI boundary | CORE direction | Preferred early stable external boundary. |
| FFI-006 | Stable native language ABI | DEFERRED | Do not freeze during early compiler evolution. |
| FFI-007 | Foreign exception containment | PLANNED | Exceptions must not cross unsupported boundaries. |
| FFI-008 | Platform API wrappers | PLANNED | Windows, POSIX/Linux, Vulkan. |
| FFI-009 | Callback thread-effect metadata | CANDIDATE / PLANNED | Needed for foreign-thread callbacks. |

# 16. Standard library direction

| ID | Feature | Status | Notes |
|---|---|---:|---|
| STD-001 | Small core/prelude | CORE direction | Avoid importing half the standard library implicitly. |
| STD-002 | String / StringView | PLANNED | Exact UTF-8 and indexing semantics open. |
| STD-003 | Vector<T> | PLANNED | Major ownership/invalidation probe. |
| STD-004 | Result / Option | CORE semantic types | Normal tagged unions with compiler conveniences. |
| STD-005 | Allocator interfaces | CORE direction | Needed for system-level control. |
| STD-006 | File / socket RAII wrappers | PLANNED | Validate Drop and Result. |
| STD-007 | Synchronization primitives | PLANNED | Mutex, atomics, guards, etc. |
| STD-008 | Thread and job abstractions | PLANNED | Library-built, no mandatory runtime. |
| STD-009 | Dynamic/type-erased utilities | CANDIDATE | Any, dynamic errors, dynamic callables. |

# 17. Tooling and diagnostics

| ID | Feature | Status | Notes |
|---|---|---:|---|
| TOOL-001 | Diagnostics as design requirement | CORE | Valid/invalid examples and remediation for major features. |
| TOOL-002 | Source spans | CORE compiler foundation | Required early. |
| TOOL-003 | Provenance-aware diagnostics | CORE goal | Show origin, invalidation, and use. |
| TOOL-004 | Generic constraint diagnostics | CORE goal | Explain missing capability directly. |
| TOOL-005 | Module metadata introspection | PLANNED | Useful for IDE/LSP and analysis. |
| TOOL-006 | Language server | PLANNED | Later milestone. |
| TOOL-007 | Formatter | PLANNED | After grammar stabilizes sufficiently. |
| TOOL-008 | Package manager | PLANNED long-term | Not first milestone. |
| TOOL-009 | Playground | PLANNED long-term | Not first milestone. |

---

# Tooling, Project System, and Probe Tracks

## Toolchain naming

**Status:** Accepted starter direction

- primary CLI: `veyr`;
- compiler driver: `veyrc`;
- compiler library: `veyrcore`;
- language server: `veyr-lsp`;
- formatter: `veyrfmt`;
- analyzer frontend: `veyr-analyze`;
- docs: `veyr-doc`;
- native binding generator: `veyr-bindgen`;
- GPU binding generator/probe: `veyr-gpu-bindgen`.

## Source/project files

**Status:** Accepted starter direction

- source extension: `.vyr`;
- project manifest: `Veyr.toml`;
- lockfile: `Veyr.lock`;
- local cache: `.veyr/`.

## Project build system

**Status:** Planned

Veyr projects should be described by `Veyr.toml` and built through `veyr`.

Early implementation of the compiler itself remains CMake/Ninja/C++23.

## Graphics/GPU track

**Status:** Planned validation track

A sequence of probes should validate windowing, software framebuffer, dynamic library loading, OpenGL loader, Vulkan loader, Vulkan device/swapchain/triangle, and eventually a cross-API layer over OpenGL/Vulkan/DX12/Metal.

## Cybersecurity/code-analysis track

**Status:** Planned validation track

Security tooling, Sysmon/Splunk/MITRE/Atomic Red Team parsing, source-code analysis, AST/CFG/DDG graph export, and vulnerability-pattern scanning should guide analyzer and data-structure decisions.

## Simracing/hardware track

**Status:** Planned validation track

Telemetry parsing, pedal calibration, force-feedback profile tooling, binary protocol work, and HID/serial probes should pressure binary I/O, units, native APIs, and no-std/embedded research.

## Agent IDE/tooling track

**Status:** Candidate/planned validation track

A future provider-agnostic IDE/agent wrapper can pressure diagnostics, LSP, project graph, process orchestration, and plugin design.
