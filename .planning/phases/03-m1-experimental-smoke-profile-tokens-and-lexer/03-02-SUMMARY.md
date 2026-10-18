---
phase: 03-m1-experimental-smoke-profile-tokens-and-lexer
plan: "02"
subsystem: lexing
tags: [m1, experimental-smoke-profile, tokens, checked-lexer, byte-spans, diagnostics]
requires:
  - phase: "02"
    provides: Accepted immutable source snapshots, checked byte spans and owning ordered diagnostics
  - phase: "03-01"
    provides: Owner-accepted experimental M1 profile with dedicated commit and verified remote publication
provides:
  - Thirteen-kind value-semantic tokens containing only kind and SourceSpan
  - Checked synchronous complete M1 byte lexer with recovered Invalid tokens and owning Errors
  - Independent Linux Debug/Release native validation and retained review evidence
affects: ["03-03", "04"]
tech-stack:
  added: []
  patterns: ["one checked immutable source lookup", "unsigned-byte precedence", "recovered tokens are not compilation success"]
key-files:
  created:
    - include/veyr/lexing/token.hpp
    - include/veyr/lexing/lexer.hpp
    - src/lexing/lexer.cpp
    - tests/lexing/token_test.cpp
    - tests/lexing/lexer_test.cpp
    - .planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-02-EXECUTION.md
    - .planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-02-SUMMARY.md
  modified: [CMakeLists.txt, tests/CMakeLists.txt, .planning/STATE.md, .planning/ROADMAP.md, .planning/REQUIREMENTS.md]
key-decisions:
  - Preserve the published experimental profile and all fourteen accepted decisions without semantic conversion or final ABI commitments
  - Recovered tokens require the caller to inspect persistent DiagnosticEngine Error state before any successful compilation handoff
  - Complete this plan for accepted Linux developer evidence while retaining new hosted CI and Wave 3 as separate gates
patterns-established:
  - Every Invalid run has one explicit owning Error with an identical primary span and empty related spans
  - Unknown SourceId propagates its original SourceError without diagnostics or fabricated EOF
requirements-completed: [LEX-02, LEX-03, LEX-04, LEX-05, LEX-06]
requirements-acceptance-scope: owner-accepted-Linux-developer-Debug-Release; new hosted evidence and phase-wide acceptance pending
coverage:
  - id: D1
    description: Thirteen-kind tokens store only kind and original-byte span with value and snapshot-lifetime semantics
    requirement: LEX-02
    verification:
      - kind: unit
        ref: "tests/lexing/token_test.cpp#SmokeProfileToken; Debug/Release task1-token logs, 7/7 each"
        status: pass
    human_judgment: false
  - id: D2
    description: Complete profile recognition and numeric-shape recovery preserve exact original byte boundaries
    requirement: LEX-03
    verification:
      - kind: unit
        ref: "tests/lexing/lexer_test.cpp#FixedSampleHasExactOriginalTokenSpans, DecimalShapeHasNoSemanticRangeOrConversion and 35 normative rows; final focused logs, 55/55 each"
        status: pass
    human_judgment: false
  - id: D3
    description: Exact whitespace, comment rejection and non-ASCII grouping preserve Phase 2 byte contracts
    requirement: LEX-04
    verification:
      - kind: unit
        ref: "tests/lexing/lexer_test.cpp#WhitespacePreservesLfCrlfLoneCrAndEofOffsets, CommentContentsProduceNoNestedDiagnostics and normative rows"
        status: pass
    human_judgment: false
  - id: D4
    description: Known-source scans advance to exactly one source-end EOF; unknown sources propagate checked failure
    requirement: LEX-05
    verification:
      - kind: unit
        ref: "tests/lexing/lexer_test.cpp#expect_scan, IncompleteSingleByteLookaheadTailsMakeProgress and UnknownSourceReturnsExactErrorWithoutDiagnosticOrTokens"
        status: pass
    human_judgment: false
  - id: D5
    description: Recovered Invalid tokens retain exact owned diagnostics, insertion order, duplicates and prior Error state
    requirement: LEX-06
    verification:
      - kind: unit
        ref: "tests/lexing/lexer_test.cpp#RecoveryAppendsInOrderAndRepeatedScansKeepDuplicates, CleanScanCannotErasePriorErrorState and expect_scan"
        status: pass
    human_judgment: false
  - id: D6
    description: Existing CMake integration passes independent Linux configuration, full native tests, identity verification and include policy
    verification:
      - kind: integration
        ref: ".veyr-dev/execution/03-02/commands.jsonl; Debug/Release test 151/151, verify 13 entries and direct includes PASS"
        status: pass
    human_judgment: false
duration: "Active elapsed work not measured; recorded native/static command time 38.856 seconds"
completed: 2026-10-18
status: complete
---

# Phase 3 Plan 02: Kind/span tokens and checked recovered M1 lexing Summary

**Owned token vectors and exact source-linked lexical Errors implement the published experimental M1 profile without parsing or semantic conversion.**

## Performance

- **Implementation validation:** 2026-10-17, first recorded command 16:18:49 UTC; final recorded command 18:03:42 UTC.
- **Owner acceptance and closeout preparation:** 2026-10-18.
- **Duration:** 38.856 seconds summed across 28 retained command receipts; this is command time, not active development time or total wall-clock duration.
- **Tasks:** 2/2 accepted for Linux developer scope.
- **Files:** Seven implementation/test/integration files; five planning/checkpoint files in the prepared closeout packet, twelve tracked/new candidates overall.

## Accomplishments

- Value-semantic Token objects contain exactly TokenKind and SourceSpan. The thirteen kinds are Fn, Main, Int32, Return, LeftParen, RightParen, Arrow, LeftBrace, RightBrace, Semicolon, IntegerLiteral, Invalid and EndOfFile. Tokens own neither lexemes nor numeric payloads.
- Synchronous `lex(const SourceManager&, SourceId, DiagnosticEngine&)` returns `std::variant<std::vector<Token>, SourceError>`. One checked lookup precedes scanning; an unknown ID propagates the exact original infrastructure error, without lexical diagnostics or fabricated EOF.
- The unsigned-byte scanner follows the published precedence, preserves exact half-open source spans, advances on every non-EOF branch and appends exactly one source-end EOF for every completed known-source scan. Numeric range conversion remains later semantic work.
- Every rejected run retains an Invalid token and one owning explicitly initialized Error with the matching primary span and empty related spans. Existing order, duplicates and Error state survive. A recovered token vector alone never establishes compilation success.
- Independent Debug/Release validation and the owner architecture/security/evidence review support this local scope. All 35 normative cases actually executed in each final configuration; no new hosted-CI success is claimed.

## Ownership and lifetime contracts

SourceManager owns immutable snapshots. The lexer borrows their text only during the call; returned vectors own token values. Later lexeme resolution requires the originating manager to remain alive. New registrations do not invalidate old source views; numeric IDs from unrelated managers are not automatically origin-checked.

Diagnostics own their strings and spans remain manager-local. No borrowed DiagnosticEngine view is retained across emission. Callers initialize Token fields and diagnostic severity explicitly. Allocation exceptions retain ordinary C++ semantics, including possible earlier diagnostic accumulation; no atomic batch rollback or new OOM policy is promised.

## Validation and evidence

| Gate | Debug | Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Token / lexer focused | 55/55 = 7 token + 13 lexer + 35 normative cases | 55/55, independently |
| Existing components | 61/61 | 61/61 |
| Full CTest / dev.py test | 151/151 | 151/151 |
| dev.py verify | 13 selected compiler/directory/C++23 entries; CTest 151/151 | 13 entries; CTest 151/151 |
| Direct include policy | PASS | PASS |

The retained packet is under ignored .veyr-dev/execution/03-02/. It includes REVIEW.md, evidence.json, commands.jsonl, actual raw logs and independent prepared metadata copies. Final focused logs are Debug-accepted-focused.log and Release-focused.log; the Debug filename labels selected evidence, not automatic owner acceptance.

Before closeout edits, all 65 manifest entries and all 28 command-log hashes were verified. The manifest and original evidence are not rewritten. Closeout preserves the three superseded planning inputs under closeout/accepted-inputs/ and separately records subsequent checks; the original 65 accepted byte sets remain verifiable. All seven accepted implementation/test/CMake files retain their original accepted hashes. The prior Phase 2 packet also remains intact: 34 references still match live files and its two original CMake inputs remain preserved under baseline/.

Task 1's missing-token-header build and Task 2's missing-lexer-header syntax check were deliberate expected RED results, followed by GREEN validation. They are not unresolved or unexpected verification failures. Existing dependencies were reused using cache-only configure with no remote and no package build. Conan 2.33.0, LLVM 23.1.2 and all four locks remain unchanged.

## Task Commits

None yet. Repository policy and the explicit owner gate defer all staging, commits and publication. The accepted prerequisite profile is dedicated commit 52f598aaf031e549ed3bbe5c598bc43f5f7be6bb; the published 03-01 closeout/base is 030fd0b5c17d8228f5c2dc86324c325dba5103f3. Those hashes identify prerequisites, not a new lexer commit.

## Files Created/Modified

- `include/veyr/lexing/token.hpp` — exact token kinds and kind/span value.
- `include/veyr/lexing/lexer.hpp` — synchronous checked API and ownership/error contracts.
- `src/lexing/lexer.cpp` — complete profile recognition and recovery.
- `tests/lexing/token_test.cpp` — representation, checked lexemes and snapshot lifetime.
- `tests/lexing/lexer_test.cpp` — exact tokens, spans, diagnostics, boundaries, recovery and checked errors.
- `CMakeLists.txt` and `tests/CMakeLists.txt` — narrow existing-target integration.
- STATE.md, ROADMAP.md, REQUIREMENTS.md and this plan's execution/summary — scoped acceptance and canonical bookkeeping.

## Decisions Made

No new language decisions. All fourteen accepted decisions and the published profile remain binding only for experimental M1. Signed-32-bit/0–255 semantics are not lexer range checks or final ABI guarantees. The block terminator search begins after the full opener; `/*/return` retains Invalid/Error [0,9) and EOF [9,9), with no Return token.

## Deviations from Plan

No production scope deviation or dependency addition. Generic GSD task/metadata commits and automatic next-plan execution remain deferred under explicit repository/owner policy. Canonical state, roadmap and requirement operations are recorded in [03-02-EXECUTION.md](03-02-EXECUTION.md); generic metadata inference is reconciled against actual numbered plans and completed summaries.

## Issues Encountered

- A scratch enum counter omitted Int32; the counter was corrected and native/structural checks passed.
- An intermediate parameter printer caused CTest to omit five non-ASCII cases. Deterministic ASCII-hex labels restored all 55 focused tests, including all 35 normative rows, independently in both configurations. Intermediate 50-test logs remain preserved and are not credited as final acceptance.
- Previously recorded generated Conan toolchain quotation warnings remain non-blocking; no new production/test compiler warning was observed.

## User Setup Required

None. Existing prepared dependencies and ignored IDE bindings remain unchanged.

## Next Phase Readiness

03-03 remains unstarted and requires fresh owner authorization. Its planned coverage includes all 256 byte values, long malformed runs, progress/whitespace-gap invariants, returned-token lifetime, renderer integration and adversarial inputs. These are planned verification work, not newly discovered implementation defects.

Publication remains separately gated. Prior four-job CI run 39047427666 passed for the 03-01 closeout, not this uncommitted lexer. New Ubuntu/Windows hosted lexer CI, Wave 3 evidence, canonical phase-goal verification and final Phase 3 owner acceptance remain pending. Windows developer-host inspection remains independently pending/non-blocking; macOS remains deferred.

## Self-Check: PASSED

Local implementation, native evidence, owner acceptance and documentation/bookkeeping checks support this plan's Linux developer scope. Phase 1 remains 8/8, Phase 2 remains 4/4 and Phase 3 remains in progress at 2/3. No staging, commit, publication or subsequent-wave execution is implied.
