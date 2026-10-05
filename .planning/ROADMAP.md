# Roadmap: Veyr

## Overview

**Current milestone:** M1 — Compiler Walking Skeleton.

Deliver the P01 smoke path: one source/root module, fixed provisional entry, one integer-literal return, native executable and observable exit code. The initial roadmap follows the canonical ten-phase breakdown in `docs/design/milestones/MILESTONE_1.md`. Future language, tooling and application probes remain deferred in REQUIREMENTS.md and the canonical milestone strategy.

This roadmap began as fresh initialization, with no previous lifecycle imported. Phase 1 now has eight approved execution plans; Plans 01-01 through 01-04 are complete for the available Linux scope. Phase-wide requirements remain unaccepted. Initialization `--auto` did not authorize later lifecycle transitions, builds or commits.

## Lifecycle and Design Gates

- Use exact owner commands for discussion, planning, execution and verification. Do not auto-advance or use `$gsd-progress --next`.
- Review each stage, then obtain separate explicit approval for its artifact commit when appropriate. Resolve the current commit boundary before moving on; an owner-approved deferred commit must be recorded explicitly.
- Discussion/planning write authorized documents only. No source/tests/CMake/scripts, package operations, compiler/build/test/formatter runs or RED/GREEN loops.
- Phase 3 must author, review and commit the canonical M1 Experimental Smoke Profile before implementation/tests depend on concrete spelling. Execution itself requires `$gsd-execute-phase 3`; profile approval is not execution permission.
- Every sensitive implementation commit requires the owner review packet. Passing tests never grants commit permission.
- During authorized long commands, roughly 60 seconds of silence is a liveness checkpoint. Continue with credible progress; no duplicate retries or destructive cleanup.
- Fresh initialization artifacts were committed separately as `planning: initialize fresh Veyr M1 lifecycle`; later discussion/planning commits remain separate from uncommitted execution work.

## Phases

Integer phases are the active M1 scope. Any inserted decimal phase requires separately authorized roadmap work; future strategy milestone numbers do not create active phases.

- [ ] **Phase 1: Conan and Compiler Build Foundation** — A contributor can build and test the compiler foundation through equivalent Windows/Linux workflows using Conan-owned dependencies.
- [ ] **Phase 2: Source Identity, Spans, and Diagnostics** — Compiler clients can retain source text, recover span text and report useful source-linked diagnostics independently of LLVM.
- [ ] **Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer** — The owner-reviewed milestone-local profile defines the syntax boundary and clients can obtain recovered tokens with precise lexical diagnostics.
- [ ] **Phase 4: Smoke Profile Parser** — Compiler clients can parse the fixed M1 entry/return shape and report useful syntax failures.
- [ ] **Phase 5: Minimal AST and Semantic Smoke** — Compiler clients can obtain a source-linked minimal AST and validate the reviewed constant-return constraints before lowering.
- [ ] **Phase 6: Minimal IR and Lowering** — Backend clients can consume a validated constant-return representation through a small explicit lowering contract.
- [ ] **Phase 7: Single Root-Module Context** — The smoke pipeline carries one stable root-module identity without introducing imports, packages or a module graph.
- [ ] **Phase 8: LLVM Backend Smoke** — Validated lowered smoke input produces verified LLVM IR and a native x86_64 object for either Tier 1 platform.
- [ ] **Phase 9: veyrc Driver Smoke** — A user can compile one reviewed .vyr root into a native executable and understand compilation/link failures.
- [ ] **Phase 10: End-to-End Native Smoke Acceptance** — The owner can review fresh evidence that the complete M1 pipeline builds, rejects invalid input, and runs native smoke programs on both Tier 1 platforms.

## Phase Details

### Phase 1: Conan and Compiler Build Foundation

**Goal:** A contributor can build and test the compiler foundation through equivalent Windows/Linux workflows using Conan-owned dependencies.
**Depends on:** Nothing (first phase)
**Requirements:** BUILD-01, BUILD-02, BUILD-03, BUILD-04, BUILD-05, BUILD-06, BUILD-07, BUILD-08, BUILD-09, BUILD-10, BUILD-11, BUILD-12, BUILD-13, BUILD-14
**Probe pressure:** P01 infrastructure and PLAT_06 CI/platform matrix.
**Scope:** Thin Python operations; prerequisite checks; explicit Conan host/build profiles; exact dependency references/options; LLVM 23.1.2 recipe selection or narrow recipe; CMakeToolchain/CMakeDeps integration and explicit LLVM discovery; Ninja/C++23 targets, GoogleTest/GoogleMock, compile database, include guardrail, supported Debug/Release combinations, cache/offline behavior and Tier 1 CI path.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. Windows and Linux contributors have documented, equivalent setup/doctor/configure/build/test/verify/all entry points; doctor reports real missing host tools/SDK prerequisites. CI shares commands and dependency/profile policy while validating independently observed CI identities, not assumed developer toolchains.
2. Reviewed Conan profiles/configuration supply LLVM 23.1.2 and GoogleTest/GoogleMock with explicit ABI/runtime and supported Debug/Release combinations; optional zlib/zstd/LibXml2 are included only when selected LLVM options require them.
3. The target-oriented C++23/Ninja foundation configures with supported Conan generators and compatibility-checked find_package(LLVM CONFIG) discovery; development presets export compile_commands.json.
4. Compiler infrastructure tests and the owned-header include guardrail run through the intended test/build interfaces on both Tier 1 platforms.
5. A deliberate first dependency source build is supported; repeat workflows reuse compatible Conan packages and prepared dependency state supports acquisition-disabled configure/build without custom package storage or hidden downloads.

**Plans:** 8 plans — reviewed and committed planning; Plans 01-01 through 01-04 complete for available Linux developer scope. Plans 01-01 through 01-03 are committed, most recently `9531def3e372d0bbb27aaf2d96e88fdca26b3f3c`. Plan 01-04 C++23/Ninja core/version-only compiler, exact isolated LLVM Support integration and Conan GoogleTest/GoogleMock passed independent Debug/Release configuration/build/CTest, 11/11 tests per configuration. Owner-authorized available-Linux SUMMARY/closeout recorded at 4/8 using existing evidence; no new commands or implementation changes for closeout. Final validation reused cached LLVM/GoogleTest without dependency downloads/rebuilds; only the reviewed WrongTriple test expectation changed, production guards/manifests/locks preserved. The owner reviewed execution/closeout and explicitly requested the dedicated checkpoint `build: establish CMake foundation and validate Linux tests`; stop for owner review afterward. Resolve its hash from Git history. Windows native locks/developer/runtime and independent Windows/Ubuntu CI evidence remain pending; macOS deferred. Plans 01-05–01-08 unstarted; no phase-wide requirement or phase completion claimed.

Plans:
- [x] 01-01-PLAN.md — Exact native profiles and strict prerequisite doctor (available Linux scope; Windows/CI evidence deferred)
- [x] 01-02-PLAN.md — Narrow LLVM 23.1.2 Conan recipe and native exports (available Linux developer scope; Windows/CI evidence deferred)
- [x] 01-03-PLAN.md — Conan consumer, native locks, explicit setup and cache-only generation (available Linux scope; two Windows locks/native evidence deferred)
- [x] 01-04-PLAN.md — Target-oriented C++23/Ninja, LLVM providers and infrastructure tests (available Linux scope; Windows/CI evidence deferred)
- [ ] 01-05-PLAN.md — Portable owned-header include guardrail
- [ ] 01-06-PLAN.md — Complete developer commands, scoped clean and liveness supervision
- [ ] 01-07-PLAN.md — Native provider/runtime/offline/cache acceptance
- [ ] 01-08-PLAN.md — Four-combination CI, documentation and evidence/capacity review

**Owner-approved baseline clarification (2026-10-04; acceptance updated 2026-10-05):** Linux developer baseline is the observed Omarchy/Arch x86_64 host. Ubuntu 24.04 Linux CI, Windows developer and Windows CI identities remain independently pending. Authorized CLI-only setup installed project-owned Conan 2.33.0. The owner-approved read-only doctor correction is implemented/reviewed; corrected native doctor passes 22/22 without changing Conan home or tool environment. Plan 01-01 prerequisite and Plan 01-02 exact LLVM package/static Support Debug/Release acceptance are complete for available Linux developer scope. Broader ABI surfaces and independent Windows/CI package readiness remain unverified. Backend LLVM remains 23.1.2. Committed discussion and all eight waves are preserved; macOS deferred.

**Research/design gate:** Recheck recipe/version availability, actual exported LLVM config/targets, exact host tools and runtime combinations during discussion/planning. Record Windows evidence as pending until actually supplied; do not expand this phase into a package/provisioning platform.

Platform impact:
- Windows: clang-cl with reviewed SDK/CRT/linker/profile settings and equivalent Python operations.
- Linux: clang/clang++ with reviewed standard-library ABI/profile and equivalent Python operations.
- macOS future: Defer Apple profiles/CI; retain host/target distinctions and standard package/build boundaries.

### Phase 2: Source Identity, Spans, and Diagnostics

**Goal:** Compiler clients can retain source text, recover span text and report useful source-linked diagnostics independently of LLVM.
**Depends on:** Phase 1
**Requirements:** SRC-01, SRC-02, SRC-03, SRC-04, DIAG-01, DIAG-02, DIAG-03
**Probe pressure:** P01; PLAT_01 paths/filesystem and PLAT_03 terminal diagnostics.
**Scope:** Stable SourceId and source storage; byte spans and bounds/end locations; span text retrieval; path/line/column behavior; structured accumulated diagnostics and deterministic plain-text rendering.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. Compiler clients can register sources with stable identity and retrieve original text through validated byte spans, including a zero-width end-of-source location.
2. Source text remains available for token/AST/diagnostic consumers under the reviewed lifetime contract.
3. Users receive source-linked messages and relevant related locations in deterministic plain text on Windows/Linux without mandatory terminal color or LLVM dependencies.
4. Path, newline and location cases follow documented platform-aware behavior while final source-encoding semantics remain open.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 2` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Review source identity, text lifetime, newline/location presentation and path edge cases before implementation; avoid a general virtual filesystem or final encoding decision.

Platform impact:
- Windows: Preserve useful Windows path spelling and handle CRLF/location display without console encoding assumptions.
- Linux: Preserve useful Linux paths and line boundaries without assuming all filesystems are case-sensitive.
- macOS future: Keep path/location APIs extensible; Darwin-specific filesystem/terminal validation is deferred.

### Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer

**Goal:** The owner-reviewed milestone-local profile defines the syntax boundary and clients can obtain recovered tokens with precise lexical diagnostics.
**Depends on:** Phase 2
**Requirements:** LEX-01, LEX-02, LEX-03, LEX-04, LEX-05, LEX-06
**Probe pressure:** P01 fixed-entry integer-return smoke; diagnostic recovery supports later tooling pressure without implementing it.
**Scope:** Define the canonical M1 profile; minimal kind/span tokens; SourceManager lexemes; unsigned ASCII decimal shape; whitespace skip/no trivia; no comments; non-ASCII/unsupported numeric diagnostics; zero-width EOF; smallest offending-run recovery.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md explicitly defines provisional spelling, valid/invalid examples, supported/unsupported forms, diagnostic expectations and the M1 semantic-range boundary, with non-final warnings.
2. The profile is owner-reviewed and its dedicated artifact commit is explicitly approved and completed before syntax-dependent implementation/tests begin.
3. Tokens contain only TokenKind and SourceSpan; lexemes come from SourceManager and every recovered stream ends in explicit zero-width EOF.
4. Lexer behavior recognizes profile-defined tokens/ASCII decimal sequences, skips defined whitespace, and diagnoses comments, unsupported numeric forms and non-ASCII input as M1 limitations.
5. Malformed input consumes the smallest clearly related offending run, reports its source span, and continues with recovered tokens plus diagnostics.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 3` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Hard gate: do not choose exact spelling or range during initialization. Discussion/planning may author this explicitly required design document; no lexer source/tests until profile review/commit and an exact execution command. Profile support never accepts final grammar.

Platform impact:
- Windows: Byte spans and whitespace handling must account for the reviewed Windows newline cases.
- Linux: Use the same profile/recovery rules on Linux; no host locale or signed-char assumptions.
- macOS future: ASCII smoke rules are profile-local; final encoding and Darwin support stay deferred.

### Phase 4: Smoke Profile Parser

**Goal:** Compiler clients can parse the fixed M1 entry/return shape and report useful syntax failures.
**Depends on:** Phase 3
**Requirements:** PARSE-01, PARSE-02, PARSE-03
**Probe pressure:** P01 parser portion only.
**Scope:** Consume tokens through EOF, enforce the fixed entry and single integer-literal return, return a minimal source-linked parse representation and syntax diagnostics; define recovery/failure contracts.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. Parser accepts exactly the reviewed fixed-entry/single-integer-return profile and requires EOF after it.
2. Incomplete, malformed, trailing and unsupported forms produce precise parser diagnostics under a documented recovery policy.
3. Parser output retains source relationships and error state; the lexer remains independent of parser/AST/semantics.
4. Tests and diagnostics label the M1 profile explicitly rather than declaring general functions or final Veyr grammar valid.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 4` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Use only the committed profile. Phase 4 may introduce the minimal representation needed to return parsing results; Phase 5 consolidates the AST boundary without a second parser or speculative general grammar.

Platform impact:
- Windows: Identical profile behavior and source spans under reviewed Windows fixtures.
- Linux: Identical profile behavior and source spans under reviewed Linux fixtures.
- macOS future: Parser contracts remain host-independent; macOS builds deferred.

### Phase 5: Minimal AST and Semantic Smoke

**Goal:** Compiler clients can obtain a source-linked minimal AST and validate the reviewed constant-return constraints before lowering.
**Depends on:** Phase 4
**Requirements:** AST-01, AST-02, SEMA-01, SEMA-02
**Probe pressure:** P01 requires a validated return constant, not a whole type system.
**Scope:** Durable fixed-entry/return AST boundary; accepted decimal value conversion; profile-defined semantic range; source-linked semantic failures and explicit validity handoff.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. Minimal AST represents the fixed entry and single return while retaining the relevant source spans.
2. Parser and semantic clients share one minimal AST boundary without LLVM types or general function/expression facilities.
3. Semantic smoke accepts reviewed valid values and rejects out-of-range constants/invalid shape with useful source-linked diagnostics.
4. Invalid programs cannot be consumed as successfully validated lowering input.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 5` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Apply the reviewed Phase 3 range; do not invent final integer types, overflow, entry ABI or ownership semantics. Any newly required unresolved behavior returns to owner design review.

Platform impact:
- Windows: Semantic range and agreed smoke fixtures must support predictable Windows process observation.
- Linux: The same reviewed smoke cases must avoid hidden POSIX exit-status truncation assumptions.
- macOS future: Keep semantic validation host-independent; future process/ABI details stay deferred.

### Phase 6: Minimal IR and Lowering

**Goal:** Backend clients can consume a validated constant-return representation through a small explicit lowering contract.
**Depends on:** Phase 5
**Requirements:** IR-01, IR-02
**Probe pressure:** P01 semantic-to-backend handoff; OQ-028 remains future IR design pressure.
**Scope:** Lower the validated smoke AST, preserve the accepted value and source/module association, define handoff invariants without freezing full HIR/MIR architecture.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. Validated smoke AST lowers to a representation carrying the same accepted return value.
2. Lowered input retains source/module association sufficient for diagnostics and Phase 7 identity integration.
3. Backend handoff has explicit validity requirements and excludes LLVM types from foundational compiler components.
4. No general CFG, ownership IR, expressions or speculative multi-layer framework is required for the smoke program.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 6` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Representation/API details belong to phase planning; preserve a migration boundary instead of resolving whole-language OQ-028 by accident.

Platform impact:
- Windows: No Windows-specific target details leak into the minimal lowered form.
- Linux: No ELF or Linux-linker details leak into the minimal lowered form.
- macOS future: Target-independent handoff permits a later Mach-O backend path.

### Phase 7: Single Root-Module Context

**Goal:** The smoke pipeline carries one stable root-module identity without introducing imports, packages or a module graph.
**Depends on:** Phase 6
**Requirements:** ROOT-01, ROOT-02
**Probe pressure:** P01 single-root boundary; full module/plugin cards remain deferred.
**Scope:** Integrate source/root identity through validation/lowering/backend context; explicit one-root limitation; placeholder milestone-local identity where broader module semantics remain unresolved.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. One root source is associated with one stable milestone-local module identity through the integrated compiler handoff.
2. Identity and source relationships remain inspectable when validation/lowering/backend clients communicate.
3. Inputs stay inside an explicit one-root contract; imports, package resolution, module graph traversal and semantic artifact persistence remain absent.
4. Root selection does not consume Veyr.toml or invent final package/module naming semantics.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 7` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Keep this phase small. Reuse source identity from Phase 2; do not add discovery/resolution infrastructure or a final module artifact format.

Platform impact:
- Windows: Review root identity across Windows path spellings without universal drive/case assumptions.
- Linux: Review root identity across Linux path spellings without universal case-sensitivity assumptions.
- macOS future: Identity representation must not rule out future Darwin paths/package identity.

### Phase 8: LLVM Backend Smoke

**Goal:** Validated lowered smoke input produces verified LLVM IR and a native x86_64 object for either Tier 1 platform.
**Depends on:** Phase 7
**Requirements:** BACK-01, BACK-02, BACK-03
**Probe pressure:** P01 requires native output; no language FFI/JIT feature is forced.
**Scope:** Pinned LLVM 23.1.2 API; native target/data layout; constant-return IR; verifier; object emission and explicit output diagnostics.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. Backend constructs and verifies LLVM IR for the reviewed constant-return representation using LLVM 23.1.2.
2. Native x86_64 target setup emits a Windows or Linux object with explicit target/data-layout configuration.
3. Target, verifier and output failures produce useful compilation diagnostics.
4. LLVM dependencies remain behind the backend boundary; no LLDB/JIT/LTO/every-backend expansion enters M1.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 8` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Research the pinned release API and exact package component exports. Object output does not yet prove executable linking or end-to-end success.

Platform impact:
- Windows: Native x86_64 Windows target/COFF and reviewed LLVM/runtime package compatibility.
- Linux: Native x86_64 Linux target/ELF and reviewed LLVM/ABI package compatibility.
- macOS future: Keep target/object-format selection explicit so Darwin/Mach-O can be added later.

### Phase 9: veyrc Driver Smoke

**Goal:** A user can compile one reviewed .vyr root into a native executable and understand compilation/link failures.
**Depends on:** Phase 8
**Requirements:** DRV-01, DRV-02, DRV-03, DRV-04
**Probe pressure:** P01 full compilation path and PLAT_02 process/exit behavior.
**Scope:** Document narrow driver interface; integrate compiler stages/root; input/output path handling; native tool invocation/linking; structured process arguments, failing status and diagnostics.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. veyrc compiles a reviewed one-root .vyr smoke input through source, tokens, parser, AST, validation, lowering and LLVM object generation into a native executable.
2. Documented input/output path behavior handles spaces and platform-native artifact conventions.
3. Unreadable/invalid source and backend/linker failures return a failing compiler status with useful diagnostics rather than successful compilation.
4. Native tool invocation uses explicit executable/argument/error contracts with Windows/Linux parity; no shell-only workflow or manifest/package system is required.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 9` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Driver flags, link command and artifact behavior are phase-local interface choices to review, not automatic adoption of illustrative toolchain sketches. Final entry ABI semantics remain out of scope.

Platform impact:
- Windows: Review argument quoting, linker/CRT selection, output .exe convention and process failure status.
- Linux: Review structured subprocess invocation, native linker/ABI and exit/signal failure distinctions.
- macOS future: Separate process/link policy to permit future Apple linker/SDK behavior.

### Phase 10: End-to-End Native Smoke Acceptance

**Goal:** The owner can review fresh evidence that the complete M1 pipeline builds, rejects invalid input, and runs native smoke programs on both Tier 1 platforms.
**Depends on:** Phase 9
**Requirements:** E2E-01, E2E-02, E2E-03, E2E-04
**Probe pressure:** P01 acceptance; PLAT_02 process and PLAT_06 parity gate.
**Scope:** Portable fixture/process harness; agreed positive exit-code cases; malformed/unsupported/out-of-range negative cases; fresh Windows/Linux evidence; include/build/test checks; provisional-profile and scope review.
**Success Criteria** (observable outcomes after authorized execution/verification):

1. Reviewed positive smoke fixtures compile/link/run on Windows x86_64 and return the expected observable status.
2. The same intended smoke cases compile/link/run on Linux x86_64 and return the expected observable status.
3. End-to-end malformed/unsupported/range-limit cases fail compilation and do not report successful executable output.
4. Fresh evidence records required build/test/include/diagnostic/native-run checks separately for Windows and Linux; missing platform runs remain explicit gaps.
5. Owner review confirms profile-local test/diagnostic naming and M1 non-goals; final grammar and macOS support are not claimed.

**Plans:** TBD — created only by an explicit `$gsd-plan-phase 10` command after the preceding review/commit boundary is resolved.

**Research/design gate:** Execution creates the harness only when explicitly commanded. Dedicated verification remains a separate command and stage. No old logs/builds/test outcomes satisfy these criteria.

Platform impact:
- Windows: Record actual Windows native build/link/run evidence with argument/path and process-status cases.
- Linux: Record actual Linux native build/link/run evidence with argument/path and process-status cases.
- macOS future: Feasibility note only; no Darwin build/CI evidence required or support claimed.

## Future Milestone Direction

After M1 acceptance and its reviewed verification commit, the owner may start a separate milestone. Argument echo/calculator and text/config/todo tools provide the next concrete language pressure. Memory/resources, modules/targets, C interop/plugins, graphics, concurrency/networking, security/graph tools, hardware/audio, developer tooling/agent IDE, self-hosting and constrained targets remain deferred. The language package ecosystem and Veyr.toml/Veyr.lock stay separate from implementation Conan mechanics. macOS implementation remains deferred Tier 2.

## Progress

**Execution order:** Phases 1–10 in numeric order, with explicit stage/commit gates between them. Plan counts stay TBD until authorized planning; no implementation-task identifiers or plans are fabricated during new-project initialization.

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 1. Conan and Compiler Build Foundation | 4/8 | Executing; reviewed 01-04 available-Linux execution/closeout checkpoint explicitly requested, then owner review; 01-05 unstarted; Windows locks/CI pending | — |
| 2. Source Identity, Spans, and Diagnostics | 0/TBD | Not started | — |
| 3. M1 Experimental Smoke Profile, Tokens, and Lexer | 0/TBD | Not started | — |
| 4. Smoke Profile Parser | 0/TBD | Not started | — |
| 5. Minimal AST and Semantic Smoke | 0/TBD | Not started | — |
| 6. Minimal IR and Lowering | 0/TBD | Not started | — |
| 7. Single Root-Module Context | 0/TBD | Not started | — |
| 8. LLVM Backend Smoke | 0/TBD | Not started | — |
| 9. veyrc Driver Smoke | 0/TBD | Not started | — |
| 10. End-to-End Native Smoke Acceptance | 0/TBD | Not started | — |
**Coverage:** 49 active requirements mapped to exactly one phase; 0 unmapped; 0 accepted complete at phase-wide requirement level. Plan 01-01 supplies partial BUILD-05/06/07/13 coverage; Plan 01-02 supplies available-Linux BUILD-03/04/13 contributions. Independent environment and later-plan evidence remain pending.

---
*Last updated: 2026-10-05 for the owner-requested reviewed Plan 01-04 available-Linux checkpoint; Phase 1 remains in progress, with no phase completed or advanced.*
