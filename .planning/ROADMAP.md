# Roadmap: Veyr

## Overview

**Current milestone:** M1 — Compiler Walking Skeleton.

Deliver the P01 smoke path: one source/root module, fixed provisional entry, one integer-literal return, native executable and observable exit code. The initial roadmap follows the canonical ten-phase breakdown in `docs/design/milestones/MILESTONE_1.md`. Future language, tooling and application probes remain deferred in REQUIREMENTS.md and the canonical milestone strategy.

This roadmap began as fresh initialization, with no previous lifecycle imported. Phase 1 remains closed at 8/8 from final four-job run 37826767824; Phase 2 remains owner-accepted at 4/4 from run 38722486719, with published closeout fc208b4b200076fc210c75535cbe9e30f64d3c26. Historical closeout-gate wording in their detailed records describes those earlier checkpoints. Phase 3 is owner-accepted at 3/3 after final four-job run 39107885454 at be84bb9702afa75ad07803a678e7fd5a4e6358d7 and actual canonical goal verification. Its accepted profile was separately committed/published in 52f598aaf031e549ed3bbe5c598bc43f5f7be6bb before lexer implementation; Plan 03-01 closeout was published in 030fd0b5c17d8228f5c2dc86324c325dba5103f3, and Plan 03-02 in da92389b5eb6b6b2bbe949072015dd950d8273b3. Final Phase 3 closeout was accepted and published in 15e00afec923ffd1d38ee9c12832fa7cb2e2f040. All six LEX requirements are accepted; global counts remain 27 accepted/22 pending. Phase 4 discussion was published in 9a3c655a35b7b586cfd871514a40133141b17653 and planning in 5222c585686eecb0eeabca049b6f01d610716a5c. Plan 04-01 Tasks 1–2 were implemented, Linux-owner-accepted and published in 425e5ff456a59cafc04a2fa46ddbaa0f2318e792; run 39222510470 passed all four hosted configurations for that exact commit. Formal Plan 04-01 closeout is owner-accepted locally; Phase 4 is 1/3 formally complete and Plans 04-02/03 remain unstarted. PARSE-01–03 stay pending under the explicit owner exception; closeout publication and Wave 2 execution require separate approval. Windows developer-host inspection remains independently pending/non-blocking; macOS deferred. Initialization `--auto` did not authorize later lifecycle transitions, builds or commits.

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

- [x] **Phase 1: Conan and Compiler Build Foundation** — A contributor can build and test the compiler foundation through equivalent Windows/Linux workflows using Conan-owned dependencies.
- [x] **Phase 2: Source Identity, Spans, and Diagnostics** — Compiler clients can retain source text, recover span text and report useful source-linked diagnostics independently of LLVM.
- [x] **Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer** — The owner-reviewed milestone-local profile defines the syntax boundary and clients can obtain recovered tokens with precise lexical diagnostics. (completed 2026-10-18)
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

**Plans:** 8/8 complete; all eight approved sequential waves preserved. Plans 01-01 through 01-07 retain their historical available-Linux developer closeouts. Plan 01-08 now completes the independent Ubuntu/Windows hosted CI gate: [run 37826767824](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824), commit `2741d6514f5e695d36c20cb792bff4985ac5d754`, passes all four Debug/Release jobs. Each passes policy/bootstrap/doctor/lock checks, archive restore/import, dependency setup, integrity/export/early save, configure/build, CTest 35/35, verify, prepared native 35/35 and controlled empty-cache rejection. LLVM/GoogleTest are reused; same-run Debug snapshots feed Release without collapsing Conan package IDs. Windows Release normal/prepared ABI probes use -MD with Release flags and pass; Windows Debug compiler-alias/C++23 verification and CRLF rejection matching pass. All reviewed recipe revisions and lock bytes are preserved. Source-build capacity was observed in earlier runs 37670754053 (Linux) and 37786566284 (Windows); peak resource measurements and a manual cold_build=true dispatch remain unclaimed. Final fixes, exact job URLs, cache keys and evidence limits are recorded in [01-08-EXECUTION.md](phases/01-conan-and-compiler-build-foundation/01-08-EXECUTION.md) and [01-08-SUMMARY.md](phases/01-conan-and-compiler-build-foundation/01-08-SUMMARY.md). Windows developer-host evidence remains independently pending; macOS deferred. The Phase 1 closeout was committed in `1f7e67d11ea6a68790d29b27bd3e1750a1c5d29f`; it remains closed at 8/8. Phase 2 planning does not reopen it.

Plans:

- [x] 01-01-PLAN.md — Exact native profiles and strict prerequisite doctor (historical Linux developer scope; CI acceptance supplied by Plan 01-08)
- [x] 01-02-PLAN.md — Narrow LLVM 23.1.2 Conan recipe and native exports (historical Linux developer scope; CI acceptance supplied by Plan 01-08)
- [x] 01-03-PLAN.md — Conan consumer, native locks, explicit setup and cache-only generation (historical Linux scope; reviewed Windows locks and CI evidence subsequently supplied)
- [x] 01-04-PLAN.md — Target-oriented C++23/Ninja, LLVM providers and infrastructure tests (historical Linux developer scope; CI acceptance supplied by Plan 01-08)
- [x] 01-05-PLAN.md — Portable owned-header include guardrail (historical Linux developer scope; CI acceptance supplied by Plan 01-08)
- [x] 01-06-PLAN.md — Complete developer commands, scoped clean and liveness supervision (historical Linux scope; CI acceptance supplied by Plan 01-08)
- [x] 01-07-PLAN.md — Native provider/runtime/offline/cache acceptance (historical Linux provider/cache/clean/offline acceptance; hosted CI completed in Plan 01-08)
- [x] 01-08-PLAN.md — Independent Ubuntu/Windows Debug/Release CI acceptance complete; run 37826767824 passes all four jobs, snapshot reuse and native/offline gates

**Owner-approved baseline clarification (2026-10-04; acceptance updated 2026-10-08):** Omarchy/Arch developer evidence remains separate from Ubuntu CI Clang 18/GCC-13 and Windows CI standalone Clang 20.1.8/DevShell identities. Final native CI acceptance covers those reviewed hosted configurations only. Windows developer-host inspection remains independently pending; no CI result substitutes for it. Conan 2.33.0, LLVM 23.1.2, strict read-only doctor, eight-wave ordering and committed discussion are preserved. General ABI guarantees outside the exercised foundation remain unclaimed; macOS deferred.

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

**Plans:** 4/4 complete; all four approved sequential waves preserved. The owner accepted Phase 2 and all seven requirements after final [Build foundation run 38722486719](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719) completed successfully at `ab596c7c721fd673b1716db6a1f6f5681a7ff126`. Each Ubuntu/Windows Debug/Release job passed SourceManager 12/12, SourceLocation 8/8, Diagnostic 14/14 and PlainTextDiagnostic 27/27, full CTest 96/96, ten selected compiler/directory/C++23 entries, include-policy, prepared/native acceptance and controlled empty-cache rejection. Ubuntu doctor was 22/22 and Windows doctor 20/20. No success is inferred from skipped steps or cache hits.

Earlier checkpoints `4c6be3a2e7d4694782c98ac879380458691edd67` / run 37908907406 (source/location) and `a9d356efaf651eae1dd6c8fb97942f274b632f6f` / run 38705163273 (structured accumulation) remain historical evidence. Omarchy Debug/Release renderer evidence independently passed 27/27 renderer tests, 34/34 prior component tests, full CTest 96/96, ten C++23 entries and direct include checks. Final hosted evidence covers the entire slice, including rendering, while Windows developer-host inspection remains independently pending/non-blocking and macOS deferred.

Normal final records: [02-04-EXECUTION.md](phases/02-source-identity-spans-and-diagnostics/02-04-EXECUTION.md), [02-04-SUMMARY.md](phases/02-source-identity-spans-and-diagnostics/02-04-SUMMARY.md) and the installed workflow's required [02-VERIFICATION.md](phases/02-source-identity-spans-and-diagnostics/02-VERIFICATION.md). Under separate owner authorization, canonical GSD 1.6.1 verify_phase_goal was performed inline against actual artifacts, wiring, requirements and retained native evidence: all four roadmap criteria and twenty plan truths map to 20/20 deduplicated verified truths, with no blocker or outstanding human item; the canonical verification.status query returned passed. PROJECT.md was reconciled with completed capabilities and remaining active work. The seven-file closeout changes planning documentation only and awaits owner publication approval. Accepted implementation/evidence, four locks and toolchain/dependency policy remain unchanged; no builds/tests or CI executions were rerun. Phase 1 stays closed at 8/8; Phase 3 is unstarted.

**Wave 1 — owned snapshot/span substrate**

- [x] 02-01-PLAN.md — Owned immutable sources, fresh manager-local IDs and checked original-byte span retrieval (SRC-01/SRC-02/SRC-03); owner-closed for Linux scope, with subsequent Ubuntu/Windows hosted source evidence from 37908907406 and 38705163273; Windows developer-host inspection remains separate.

**Wave 2 — byte locations (complete for validated Linux developer scope)**

- [x] 02-02-PLAN.md — Indexed LF/CRLF/EOF byte locations and exact display metadata (SRC-04); owner-closed for Linux scope, with subsequent Ubuntu/Windows hosted source evidence from 37908907406 and 38705163273; Windows developer-host inspection remains independently pending/non-blocking.

**Wave 3 — structured diagnostics (complete for validated Linux developer scope)**

- [x] 02-03-PLAN.md — Owned optional-primary/ordered-related diagnostics, emission order and severity/error accumulation (DIAG-01/DIAG-03); owner-accepted for validated Linux developer scope, published with Ubuntu/Windows Debug/Release CI success in 38705163273; Windows developer-host inspection remains independent.

**Wave 4 — plain output and integration (complete; owner-accepted four-job evidence)**

- [x] 02-04-PLAN.md — Checked bounded renderer, byte-safe presentation and complete source-to-diagnostic integration (DIAG-02); Tasks 1–3 complete, final Ubuntu/Windows Debug/Release evidence accepted in 38722486719. Documentation-only closeout publication remains separately gated.

**Cross-cutting constraints:** Preserve D-01–D-16, exact immutable bytes, checked infrastructure errors and manager-local identities. Source → diagnostics → renderer dependencies only; no LLVM/driver coupling. Reuse the accepted C++23/GoogleTest foundation and unchanged Conan/package/toolchain/CI policy. No new final syntax/encoding, filesystem/VFS, generic result/reporting framework or package subsystem. Plans 02-01/02-02 implement source/span/location APIs; 02-03 adds owner-accepted owned diagnostics with completed four-job hosted evidence. Plan 02-04 now has accepted Linux developer and four-job hosted renderer/integration evidence; all seven Phase 2 requirements are accepted. Windows developer-host inspection stays independently pending/non-blocking, and macOS remains deferred. Execution, publication, commits and phase movement require separate explicit owner commands.

**Research/design gate:** All sixteen discussion contracts and committed research are authoritative; no settled decision was reopened. The planning packet was reviewed/committed before the owner's explicit execution command. Owner approved Plan 02-01 Linux-scope closeout/publication and explicitly started 02-02 with a renewed execution command. Owner separately approved 02-02 Linux-scope closeout and one dedicated checkpoint commit/push. Owner accepted 02-02 following four-job evidence and authorized Wave 3/02-03 only. Owner subsequently accepted/closed/published 02-03; its four-job CI prerequisite passed. Wave 4 Tasks 1–2 were explicitly authorized and locally validated/reviewed. Owner subsequently authorized the exact eight-file 02-04 implementation/checkpoint publication, accepted final four-job evidence and explicitly accepted Phase 2. Current authorization permits final closeout documentation only. Stop before its commit/publication or any Phase 3 work pending separate approval; no manual CI action or next-phase movement is authorized. No general virtual filesystem, final encoding or Phase 3 syntax work.

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

**Plans:** 3/3 complete and owner-accepted. Final [run 39107885454](https://github.com/m4rkens/VeyrLang/actions/runs/39107885454) completed/success at be84bb9702afa75ad07803a678e7fd5a4e6358d7 across Ubuntu/Windows Debug/Release. Each job passed 419/419 native test, verify and prepared acceptance, all 323 focused names including every byte index 0–255 and all 35 normative examples, thirteen selected C++23 compilation entries, include-policy and expected isolated empty-cache rejection without acquisition/retry. Independent Linux developer evidence remains separate. Canonical verification passed all five roadmap criteria, 21 plan truths, 22/22 deduplicated scored truths, 12/12 artifact entries, 11/11 directly inspected links, six requirements and fourteen decisions; verification.status returned passed. See [03-03-SUMMARY.md](phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-03-SUMMARY.md) and [03-VERIFICATION.md](phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-VERIFICATION.md). Final closeout was separately owner-approved and published in 15e00afec923ffd1d38ee9c12832fa7cb2e2f040; its former publication gate is satisfied.

**Wave 1 — complete; owner-accepted documentation-only scope**

- [x] 03-01-PLAN.md — Fill the experimental M1 profile from its template, preserving D-01–D-14 and the exact proposed mixed-input recovery table (LEX-01). Blocking owner review, explicit dedicated commit approval/verification, separate publication approval/push, actual authorized-branch containment/content evidence and ancestry if advanced, then stop for fresh Wave 2 authorization. The corrected profile is owner-accepted and published in 52f598aaf031e549ed3bbe5c598bc43f5f7be6bb; actual origin refs/heads/main matched that SHA. See 03-01-EXECUTION.md and completed 03-01-SUMMARY.md. Formal plan acceptance and canonical LEX-01 bookkeeping are complete; closeout was separately published in 030fd0b5c17d8228f5c2dc86324c325dba5103f3. No native lexer acceptance or automatic next action is implied.

**Wave 2 — complete; accepted Linux scope supplemented by final hosted/phase acceptance**

- [x] 03-02-PLAN.md — Exact kind/span tokens, checked synchronous complete byte lexer, Invalid/Error recovery and source-end EOF; existing SourceManager/DiagnosticEngine, CMake and focused native tests (LEX-02–06). The exact accepted profile/dedicated SHA, approved publication, fresh actual remote observation, ancestry/content and fresh Wave 2 authorization were verified before edits; local commit/stale ref alone was insufficient. Omarchy Debug/Release passed doctor 22/22, focused 55/55, existing component 61/61, full CTest 151/151, thirteen selected compiler/directory/C++23 entries and direct includes. See 03-02-EXECUTION.md and 03-02-SUMMARY.md for that accepted historical scope. Publication da92389b5eb6b6b2bbe949072015dd950d8273b3 passed all four jobs in run 39105535767. Final Wave 3 and hosted/phase acceptance below now cover the complete lexer slice.

**Wave 3 — complete; adversarial integration and final hosted/canonical evidence accepted**

- [x] 03-03-PLAN.md — All-byte, long-run, exact recovery, lifetime, newline/EOF, diagnostic accumulation and renderer integration tests completed without production or CMake changes. Independent local Debug/Release evidence and owner review passed, execution was separately published in be84bb9, and run 39107885454 validated all four hosted configurations. Canonical phase-goal verification and final owner acceptance are complete. Bounded supported GSD closeout operations completed Phase 3 bookkeeping, and the separately approved final documentation was published in 15e00afec923ffd1d38ee9c12832fa7cb2e2f040. Phase 4 execution remains separately gated.

**Cross-cutting constraints:** Preserve all fourteen discussion decisions and the Phase 2 immutable original-byte/checked-error/location contracts. Tokens contain only kind/span; SourceManager owns lexemes. Exact lowercase whole words, four whitespace bytes and contiguous arrow are profile-local. Each rejected run retains Invalid plus owned Error, with monotonic consumption and exactly one zero-width source-end EOF. Error state survives recovered output; later driver success must consult it. Signed32/0–255 is an experimental later semantic boundary, not a lexical range check or final ABI. Reuse existing diagnostics and prepared dependencies; no new framework, parser/AST/semantic/backend/driver implementation, encoding policy or toolchain change. Windows developer-host inspection remains independent/non-blocking; macOS deferred.

**Planning verification:** Research/pattern mapping and all three checker cycles remain historical planning evidence; cycle 3 passed with zero blockers/warnings after the remote-publication gate correction. Decision/gap checks passed 14/14 and 20/20. See 03-PLAN-REVIEW.md and 03-PLANNING-BREAKDOWN.md. Actual later native/hosted evidence and canonical goal verification above, rather than planning checks or cache hits, support final owner acceptance.

**Research/design gate:** Hard gate: do not choose exact spelling or range during initialization. Discussion/planning may author this explicitly required design document; no lexer source/tests until profile review/commit and an exact execution command. Profile support never accepts final grammar.

Platform impact:

- Windows: Hosted Debug/Release validates original-byte/newline, token/recovery and diagnostic/renderer integration. Developer-host inspection remains independently pending/non-blocking.
- Linux: Independent Omarchy developer and Ubuntu hosted Debug/Release validate the same profile/recovery rules without locale or signed-char assumptions.
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

**Plans:** 1/3 formally complete; three sequential plans.
Planning is published. Plan 04-01 implementation and Linux task acceptance are
published in 425e5ff456a59cafc04a2fa46ddbaa0f2318e792; hosted run 39222510470
completed/success across Ubuntu/Windows Debug/Release. Its logs contain all
thirteen parser tests, 432/432 full tests and fifteen selected C++23 entries per
configuration, with prepared and controlled empty-cache acceptance. The owner
authorized formal local 04-01 closeout recorded at 2026-10-20T15:50:37.879175+00:00.
The finalized summary/execution record and STATE now agree with discovery.
Under an explicit owner exception, PARSE-01–03 remain pending across later plans;
global requirements stay 27 accepted/22 pending. Waves 2/3 remain unstarted and
unauthorized; final lifecycle review and closeout publication are separate gates.
Deferred Stage 2 performance remediation is not a compiler-work blocker.

Plans:
**Wave 1**

- [x] 04-01-PLAN.md — Complete checked parser, public value/outcomes and baseline tests implemented, locally accepted and published; four-job hosted evidence passed. Formal local plan closeout is owner-accepted; see 04-01-EXECUTION.md and 04-01-SUMMARY.md. PARSE acceptance is explicitly deferred; closeout publication remains gated.

**Wave 2** *(04-01 prerequisite complete; separate publication/owner execution gates remain)*

- [ ] 04-02-PLAN.md — Independent structural/priority and syntax/EOF conformance matrices.

**Wave 3** *(blocked on Wave 2 completion)*

- [ ] 04-03-PLAN.md — Lexer/renderer/lifetime integration and independent final Debug/Release evidence.

**Planning verification:** Checker cycle 2 passed with zero blockers/warnings
after one bounded research-resolution annotation. All three requirements, four
roadmap success criteria and sixteen accepted decisions are covered. The
canonical decision gate passed 16/16 and post-planning gap analysis passed 19/19.
See 04-PLAN-REVIEW.md, 04-PLANNING-BREAKDOWN.md and 04-VALIDATION.md. These are
planned coverage checks, not native parser or hosted acceptance evidence.

**Cross-cutting execution constraints:** Preserve the published experimental M1
profile and all sixteen decisions; use existing source/token/diagnostic/renderer
APIs, checked original-byte spans, full structural validation before Invalid and
first-error syntax rejection, and an independent three-span result. Keep numeric
conversion/semantic range checks, general AST/grammar and final ABI out of scope.
Require separate owner commands for each wave and publication, then fresh
four-job hosted acceptance and canonical phase verification. No automatic
commit, dependency acquisition or lifecycle advance is authorized.

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
| 1. Conan and Compiler Build Foundation | 8/8 | Complete; final four-job native/offline CI acceptance run 37826767824; closeout committed; Windows developer host independently pending; macOS deferred | 2026-10-08 |
| 2. Source Identity, Spans, and Diagnostics | 4/4 | Complete; owner-accepted final four-job native/renderer CI run 38722486719; seven requirements accepted; closeout published in fc208b4b200076fc210c75535cbe9e30f64d3c26 | 2026-10-15 |
| 3. M1 Experimental Smoke Profile, Tokens, and Lexer | 3/3 | Complete; owner-accepted; closeout published in 15e00afec923ffd1d38ee9c12832fa7cb2e2f040 | 2026-10-18 |
| 4. Smoke Profile Parser | 1/3 | In progress — 04-01 complete; Waves 2/3 unstarted and unauthorized |  |
| 5. Minimal AST and Semantic Smoke | 0/TBD | Not started | — |
| 6. Minimal IR and Lowering | 0/TBD | Not started | — |
| 7. Single Root-Module Context | 0/TBD | Not started | — |
| 8. LLVM Backend Smoke | 0/TBD | Not started | — |
| 9. veyrc Driver Smoke | 0/TBD | Not started | — |
| 10. End-to-End Native Smoke Acceptance | 0/TBD | Not started | — |
**Coverage:** 49 active requirements mapped to exactly one phase; 0 unmapped; 27 accepted requirements: BUILD-01 through BUILD-14 from Phase 1, SRC-01 through SRC-04 and DIAG-01 through DIAG-03 from Phase 2, and LEX-01 through LEX-06 from completed Phase 3. Documentation-only LEX-01 and the earlier Linux developer-scope LEX-02–06 checkpoints are preserved as history; final Phase 3 acceptance includes successful four-job hosted run 39107885454 and canonical phase-goal verification. All 22 future requirements remain pending, including PARSE-01 through PARSE-03. Phase 4 is 1/3 formally complete after owner-authorized local 04-01 closeout; its published implementation and hosted evidence do not complete PARSE requirements under the explicit cross-plan exception. Windows developer-host evidence remains an independent deferred inspection, not credited from CI; macOS remains deferred.

---
**Non-blocking follow-up candidates:** Enforce explicit Diagnostic::severity initialization before construction becomes widespread; add combined left-clipping/mixed-escape/empty-span coverage; consider documenting/testing unsupported initialized severity values. No improvement is implemented during closeout, and no arbitrary default severity is introduced.

**Historical checkpoint — Plan 03-02 local closeout, before final Phase 3 hosted acceptance:**

> *Last updated: 2026-10-18; owner accepted 03-02 Linux Tasks 1–2 and authorized canonical closeout preparation. Phase 1 8/8, Phase 2 4/4, Phase 3 2/3; global ledger 27 accepted/22 pending. New hosted lexer CI, Wave 3 and phase-wide acceptance remain pending. Nothing staged, committed or published; no new native/dependency operation or CI mutation.*

*Current checkpoint: 2026-10-20; Phase 1 8/8, Phase 2 4/4 and Phase 3 3/3 complete and owner-accepted. Phase 3 closeout remains published in 15e00afec923ffd1d38ee9c12832fa7cb2e2f040. Phase 4 planning and 04-01 implementation are published; run 39222510470 passed all four native configurations. Formal local 04-01 closeout is owner-accepted; Phase 4 is 1/3 formally complete, PARSE-01–03 pending by explicit exception, and Waves 2/3 unstarted and unauthorized. Final lifecycle review and separate closeout publication approval remain pending. Global requirements remain 27 accepted/22 pending. Only bounded local 04-01 lifecycle bookkeeping accompanies this reconciliation; no native build/test, dependency operation, commit/publication, CI mutation or next-wave execution occurred.*
