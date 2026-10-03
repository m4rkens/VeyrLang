# Veyr

## What This Is

Veyr is a compiled, statically typed, strongly typed language for native systems and application development. It aims to combine low-level control, explicit ownership, deterministic cleanup, predictable costs, native interoperability, and compiler-integrated analysis. This fresh GSD lifecycle covers only Milestone 1: a compiler walking skeleton that turns a single-root, fixed-entry, integer-return smoke program into a native executable with an observable exit code.

## Core Value

Give programmers system-level control while moving as much error detection as practical into the compiler and integrated analyzer.

## Requirements

### Validated

None. Canonical design decisions constrain the project but are not implemented or verified capabilities. No previous run's planning, implementation, test results, or evidence is imported.

### Active

- [ ] Establish the compiler build with C++23, Conan 2, CMake, Ninja, Clang, GoogleTest/GoogleMock, and LLVM 23.1.2.
- [ ] Provide equivalent Windows/Linux developer operations through thin Python orchestration and explicit reviewed profiles.
- [ ] Establish source identity, byte-offset spans, and portable plain-text diagnostics independent of LLVM.
- [ ] Define and review the M1 Experimental Smoke Profile in Phase 3 before syntax-dependent implementation.
- [ ] Implement token recovery, parsing, a minimal AST, semantic validation, and lowering for that profile only.
- [ ] Carry one root-module identity through the pipeline without imports or package resolution.
- [ ] Emit native code through LLVM and expose the smoke path through `veyrc`.
- [ ] Demonstrate positive and negative end-to-end behavior on Windows x86_64 and Linux x86_64 under separately authorized verification.

Detailed, phase-mapped requirements are in [REQUIREMENTS.md](REQUIREMENTS.md).

### Out of Scope

- Final grammar, general functions, variables, arithmetic, imports, and a module graph: M1 validates infrastructure rather than language completeness.
- Ownership/borrowing implementation, standard library, generics, FFI, concurrency, graphics, security tooling, and self-hosting: future probes must force these capabilities.
- Implementing `veyr`, parsing `Veyr.toml`, resolving Veyr packages, or generating `Veyr.lock`: future product work, separate from compiler implementation dependencies.
- A custom third-party package solver, source-acquisition framework, binary cache, LLVM SDK normalizer, auxiliary-provider graph, or third-party lock format: Conan owns those mechanics.
- macOS implementation and CI: deferred Tier 2; preserve feasibility without claiming support.
- WASM, embedded, no-std, and freestanding implementation: future research tracks.
- Automatic lifecycle transitions and commits: repository policy requires explicit owner commands and review.

## Context

### Canonical inputs and precedence

Initialization was requested with `$gsd-new-project --auto @gsd/GSD_PROJECT_DESCRIPTION.md`. Explicit owner instructions and `docs/design/DECISION_LOG.md` take precedence over dedicated topic documents, vision/catalog/open questions, generated planning artifacts, and implementation details.

Primary inputs:

- `AGENTS.md`, `README.md`, `gsd/GSD_PROJECT_DESCRIPTION.md`, `gsd/GSD_FRESH_START_CONAN.md`.
- `docs/design/**`, including milestone strategy, M1 scope, smoke-profile template, and project cards.
- Required development policies, `docs/tooling/TOOLCHAIN.md`, and `docs/tooling/CONAN_BUILD_ARCHITECTURE.md`.
- `docs/project-system/**` and root `Veyr.toml` as future product design inputs, not a compiler build manifest.

The starting branch is `main`, with commit `8292237` (`Initial commit`). The working tree contains owner-provided starter material: modified `.gitignore` and `README.md`, and untracked `.codex/`, `AGENTS.md`, `Veyr.toml`, `docs/`, `examples/`, `gsd/`, and `templates/`. No `.planning/`, compiler implementation, build directories, or generated evidence existed at initialization. Existing starter files remain owner work.

### Fixed names

Language `Veyr`; primary project CLI `veyr`; compiler `veyrc`; compiler library `veyrcore`; source `.vyr`; manifest `Veyr.toml`; language package lock `Veyr.lock`; product local state `.veyr/`. Supporting names are `veyr-lsp`, `veyrfmt`, `veyr-analyze`, `veyr-doc`, `veyr-bindgen`, and future `veyr-gpu-bindgen`.

### Constitutional direction

Native AOT compilation; static and strong typing; immutable bindings by default; explicit mutability; no mandatory VM, JIT, garbage collector, or hidden async runtime. Manual memory management is normal and analyzable; unsafe marks unverifiable operations. Unique ownership, explicit moves between live named values, deterministic Drop, flow-sensitive inferred borrowing, typed recoverable errors, and diagnostics are accepted long-term directions, not M1 implementation scope.

### Current milestone and future pressure

P01 in `docs/design/DESIGN_PROBES.md` forces the M1 pipeline. Platform cards PLAT_01, PLAT_02, PLAT_03, and PLAT_06 force the path, process, diagnostics, and build parity needed by that pipeline. Later CLI, text, structured-data, memory/resource, module/plugin, C interop, graphics, concurrency/network, security/code-analysis, simracing, audio, agent IDE, and self-hosting probes remain visible in REQUIREMENTS.md and the canonical milestone strategy, without becoming current phases.

The initial roadmap follows the ten-phase M1 breakdown in `docs/design/milestones/MILESTONE_1.md`. The wider milestone strategy uses M2-M6 labels for later infrastructure consolidation; those strategy labels are not additional active GSD milestones or evidence of completed work.

## Constraints

- **Scope:** One root source/module, one fixed smoke entry, one integer-literal return, native output, observable exit code. No exact spelling or numeric semantic range is chosen during initialization.
- **Design:** Phase 3 must establish `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md`, review it, and obtain its dedicated approved artifact commit before dependent implementation. Illustrative syntax in other docs is not authority.
- **Build:** C++23; CMake owns Veyr targets; Ninja executes builds; Clang is the primary compiler family; Conan 2 owns third-party packages. LLVM 23.1.2 is fixed unless the owner explicitly changes it.
- **Dependencies:** Prefer a suitable exact Conan LLVM recipe; otherwise a narrow project-owned recipe. Long initial source builds are acceptable; compatible Conan package/cache reuse amortizes them. No custom provisioning subsystem.
- **Discovery:** Keep `find_package(LLVM CONFIG)` and explicit managed/system/custom selection at the CMake boundary. Managed means Conan. External modes use already supplied compatible packages; they do not add acquisition infrastructure.
- **Platforms:** Windows x86_64 and Linux x86_64 are mandatory Tier 1. Separate runtime/SDK/linker/profile details; native development initially. macOS is deferred Tier 2.
- **Architecture:** Source and diagnostics do not depend on LLVM or driver state; lexer does not depend on parser/AST/semantics. LLVM remains a backend boundary.
- **Includes/editor:** Veyr-owned headers use `<veyr/...>`; development presets export `compile_commands.json`; include guardrail has a build/check target and CTest entry.
- **Workflow:** Initialization creates `.planning/` documents only. Discussion/planning cannot create implementation or run CMake/Ninja/CTest/Conan/compiler/dependency commands. Execution requires `$gsd-execute-phase N`; verification and every stage commit require separate explicit authorization.
- **Supervision:** Approximately 60 seconds of quiet output triggers a liveness check, not automatic termination. No duplicate retries or destructive cleanup.
- **Git:** Planning documents remain eligible for reviewed tracking; `commit_docs: true` is not commit permission. No commit, staging, push, or modification of existing starter inputs occurs in this command.

## Key Decisions

These summarize canonical inputs. Initialization does not accept new final language semantics.

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Fresh M1 lifecycle with no imported evidence | Owner request and fresh-start note | Constraint; implementation pending |
| Conan 2 owns third-party mechanics | D-064 supersedes custom SDK provisioning | Accepted direction; implementation pending |
| LLVM 23.1.2 through Conan | D-065; exact target confirmed by official release | Accepted direction; recipe integration pending |
| Thin Python wrapper → Conan → CMake → Ninja → Clang | Build architecture; avoid a second package manager | Accepted direction; implementation pending |
| Windows/Linux x86_64 Tier 1, macOS deferred | Owner request and platform policy | Accepted direction; verification pending |
| Phase 3 profile precedes concrete syntax work | M1 syntax must stay milestone-local | Profile not yet authored |
| Ten M1 phases with explicit stage gates | Canonical M1 decomposition and workflow | Initial roadmap; owner review pending |
| `--auto` applies within initialization only | Exact-command and commit policy override workflow auto-chain defaults | No auto advance; no commits |

## Platform impact

- Windows: Plan clang-cl with reviewed SDK/CRT/linker configuration, structured paths, portable plain diagnostics, and explicit process invocation. No Windows build evidence exists yet.
- Linux: Plan clang/clang++ with a reviewed ABI/standard-library profile, platform-native linking, and explicit process handling. No Linux build evidence exists yet.
- macOS future: Keep target/host distinctions and platform boundaries; defer Darwin profiles, Mach-O/framework/linking validation and CI.

## Evolution

Update this document only at owner-authorized lifecycle or milestone boundaries. Reassess active requirements, move actually verified capabilities to Validated, record invalidated scope, and keep the current description accurate. Accepted language semantics belong in the canonical decision log; milestone-local choices belong in the smoke profile and reviewed phase artifacts. Stage completion never authorizes a transition or commit.

---
*Last updated: 2026-10-03 after fresh project initialization.*
