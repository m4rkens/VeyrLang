# Veyr

## What This Is

Veyr is a compiled, statically typed, strongly typed language for native systems and application development. It aims to combine low-level control, explicit ownership, deterministic cleanup, predictable costs, native interoperability, and compiler-integrated analysis. This fresh GSD lifecycle covers only Milestone 1: a compiler walking skeleton that turns a single-root, fixed-entry, integer-return smoke program into a native executable with an observable exit code.

## Core Value

Give programmers system-level control while moving as much error detection as practical into the compiler and integrated analyzer.

## Requirements

### Validated

- [x] Establish the compiler build with C++23, Conan 2, CMake, Ninja, Clang, GoogleTest/GoogleMock, and LLVM 23.1.2. Validated in Phase 1: Conan and Compiler Build Foundation; BUILD-01 through BUILD-14 accepted.
- [x] Provide equivalent Windows/Linux developer operations through thin Python orchestration and explicit reviewed profiles. Validated in Phase 1 through preserved Omarchy developer evidence and independent Ubuntu/Windows hosted Debug/Release execution. Windows developer-host inspection remains independently pending/non-blocking; hosted CI does not establish local-host acceptance.
- [x] Establish source identity, byte-offset spans, and portable plain-text diagnostics independent of LLVM. Validated in Phase 2: Source Identity, Spans, and Diagnostics; SRC-01 through SRC-04 and DIAG-01 through DIAG-03 accepted.
- [x] Define, review and separately publish the experimental M1 smoke profile before syntax-dependent implementation. Validated in Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer; LEX-01 accepted. The profile is milestone-local and does not freeze final syntax, semantics, encoding or ABI.
- [x] Provide checked kind/span tokens and deterministic original-byte lexical recovery with explicit Invalid tokens, owning ordered Error diagnostics and final EOF. Validated in Phase 3; LEX-02–LEX-06 accepted, including source-snapshot lifetime and separate renderer integration, adversarial coverage and Ubuntu/Windows hosted Debug/Release evidence.

These capabilities were implemented and verified in this fresh lifecycle; no previous run's artifacts or evidence are credited. Long-term language-design decisions remain constraints rather than implemented language capabilities.

### Active

- [ ] Implement parsing, a minimal AST, semantic validation and lowering for the accepted experimental profile only, under separately authorized later phases.
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

## Current State

Phase 1 is closed at 8/8, Phase 2 at 4/4 and Phase 3 at 3/3, all owner-accepted. Actual canonical Phase 2 and Phase 3 goal verification is recorded in their verification artifacts. There are 27 accepted requirements and 22 pending later requirements across the ten-phase M1 roadmap. Phase 4 remains unstarted and unauthorized; M1 is not complete.

Phase 1 acceptance is recorded in [01-08-SUMMARY.md](phases/01-conan-and-compiler-build-foundation/01-08-SUMMARY.md), final four-job [run 37826767824](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824). Phase 2 acceptance is recorded in [02-04-SUMMARY.md](phases/02-source-identity-spans-and-diagnostics/02-04-SUMMARY.md), final four-job [run 38722486719](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719) at ab596c7c721fd673b1716db6a1f6f5681a7ff126. Ubuntu and Windows hosted Debug/Release jobs validated the complete Phase 2 slice: source 12, location 8, diagnostic 14 and renderer 27 tests, full CTest 96/96, ten selected C++23 compilation entries, include-policy, prepared/native and controlled acquisition-disabled rejection.

Phase 3 acceptance is recorded in [03-03-SUMMARY.md](phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-03-SUMMARY.md) and [03-VERIFICATION.md](phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-VERIFICATION.md). Final [run 39107885454](https://github.com/m4rkens/VeyrLang/actions/runs/39107885454) at be84bb9702afa75ad07803a678e7fd5a4e6358d7 passed Ubuntu/Windows Debug/Release: 419/419 native test, verify and prepared acceptance, 323 focused cases including all 256 byte values and 35 normative examples, thirteen selected compiler/directory/C++23 entries, include-policy and controlled empty-cache rejection. Canonical verification passed five roadmap criteria, 21 original plan truths, 22 deduplicated scored truths, 12 artifact entries, 11 directly inspected links, six requirements and all fourteen discussion decisions. This validates the token/lexer foundation, not a parser, final Veyr grammar, general type system or native smoke compiler pipeline.

Linux remains the primary development host; Omarchy developer and Ubuntu CI baselines remain separate. Windows hosted CI is validated; Windows developer-host inspection is independently pending/non-blocking. macOS remains deferred. Canonical lifecycle reconciliation changes planning documentation only; the final Phase 3 closeout packet needs separate commit/publication authorization. No later phase is started by this acceptance.

## Context

### Canonical inputs and precedence

Initialization was requested with `$gsd-new-project --auto @gsd/GSD_PROJECT_DESCRIPTION.md`. Explicit owner instructions and `docs/design/DECISION_LOG.md` take precedence over dedicated topic documents, vision/catalog/open questions, generated planning artifacts, and implementation details.

Primary inputs:

- `AGENTS.md`, `README.md`, `gsd/GSD_PROJECT_DESCRIPTION.md`, `gsd/GSD_FRESH_START_CONAN.md`.
- `docs/design/**`, including milestone strategy, M1 scope, smoke-profile template, and project cards.
- Required development policies, `docs/tooling/TOOLCHAIN.md`, and `docs/tooling/CONAN_BUILD_ARCHITECTURE.md`.
- `docs/project-system/**` and root `Veyr.toml` as future product design inputs, not a compiler build manifest.

At initialization the branch was `main`, with commit `8292237` (`Initial commit`). The working tree contained owner-provided starter material: modified `.gitignore` and `README.md`, and untracked `.codex/`, `AGENTS.md`, `Veyr.toml`, `docs/`, `examples/`, `gsd/`, and `templates/`. No `.planning/`, compiler implementation, build directories, or generated evidence existed then. This historical starting point is not the current worktree state; subsequent reviewed commits and phase records establish the validated capabilities above.

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
- **Workflow:** Initialization created `.planning/` documents only. Discussion/planning cannot create implementation or run CMake/Ninja/CTest/Conan/compiler/dependency commands. Execution requires `$gsd-execute-phase N`; verification and every stage commit require separate explicit authorization.
- **Supervision:** Approximately 60 seconds of quiet output triggers a liveness check, not automatic termination. No duplicate retries or destructive cleanup.
- **Git:** Planning documents remain eligible for reviewed tracking; `commit_docs: true` is not commit permission. Current lifecycle reconciliation authorizes no staging, commit or push.

## Key Decisions

These summarize canonical inputs and accepted phase results. Phase completion does not accept new final language semantics.

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Fresh M1 lifecycle with no imported evidence | Owner request and fresh-start note | Preserved; Phase 1 and Phase 2 use this lifecycle's evidence only |
| Conan 2 owns third-party mechanics | D-064 supersedes custom SDK provisioning | Phase 1 validated with project-owned Conan 2.33.0 CLI and Conan-owned package state |
| LLVM 23.1.2 through Conan | D-065; exact target confirmed by official release | Narrow recipe/package integration validated in Phase 1; unchanged in Phase 2 |
| Thin Python wrapper → Conan → CMake → Ninja → Clang | Build architecture; avoid a second package manager | Shared operations validated in Phase 1; used unchanged for Phase 2 |
| Windows/Linux x86_64 Tier 1, macOS deferred | Owner request and platform policy | Hosted Debug/Release validated in both phases; Windows developer host independently pending; macOS deferred |
| Phase 3 profile precedes concrete syntax work | M1 syntax must stay milestone-local | Accepted profile published in 52f598a before lexer implementation; actual remote/ancestry/content evidence preserved |
| Ten M1 phases with explicit stage gates | Canonical M1 decomposition and workflow | Phase 1 8/8, Phase 2 4/4 and Phase 3 3/3 accepted; Phase 4 unstarted; 22 future requirements pending |
| `--auto` applies within initialization only | Exact-command and commit policy override workflow auto-chain defaults | No automatic advancement or commits; each publication needs explicit approval |

## Non-blocking Follow-up Candidates

- Enforce explicit initialization of `Diagnostic::severity` before diagnostic construction becomes widespread. Current callers must initialize before reading; no arbitrary default or constructor change is accepted here.
- Add a combined left-clipping, mixed-escape and empty-span renderer regression test.
- Consider documenting/testing unsupported initialized severity values.
- Optional per-case traces for looped long-run/EOF lexer regression fixtures.

These are recorded opportunities, not implemented changes or new phase requirements. They do not reopen accepted Phase 2 scope.

## Platform impact

- Windows: Reviewed standalone clang-cl with DevShell SDK/CRT/linker environment; hosted Debug/Release foundation, source/diagnostic and token/lexer/adversarial integration validated. Developer-host inspection remains independently pending/non-blocking.
- Linux: Primary Omarchy developer Debug/Release and separate Ubuntu CI Debug/Release evidence validated; reviewed toolchains remain independent. Source/location/diagnostic/lexer contracts preserve original bytes and stored display spelling.
- macOS future: Keep target/host distinctions and platform boundaries; defer Darwin profiles, Mach-O/framework/linking validation and CI.

## Evolution

Update this document only at owner-authorized lifecycle or milestone boundaries. Reassess active requirements, move actually verified capabilities to Validated, record invalidated scope, and keep the current description accurate. Accepted language semantics belong in the canonical decision log; milestone-local choices belong in the smoke profile and reviewed phase artifacts. Stage completion never authorizes a transition or commit.

---
*Last updated: 2026-10-18 after owner acceptance of Phase 3 and bounded GSD closeout preparation; Phases 1–3 validated, Phase 4 unstarted and unauthorized, final closeout publication pending separate owner approval.*
