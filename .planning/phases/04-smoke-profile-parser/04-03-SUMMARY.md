---
phase: 04-smoke-profile-parser
plan: "03"
implementation-commit: 08e50520afdab9e46da39182a8dcc4b56be66801
subsystem: parsing
tags: [m1, parser-integration, source-lifetime, diagnostics, renderer, native-evidence]
requires:
  - phase: "04-01"
    provides: Published checked M1 parser and independent three-span value
  - phase: "04-02"
    provides: Published independent conformance/precedence matrices and formal prerequisite closeout
provides:
  - Thirty-two actual lexer/parser/source/diagnostic/renderer integration cases
  - Token/caller-storage destruction, copy/move and source-registration lifetime evidence
  - Exact original-byte newline/EOF/opener fields and independently owning escaped rendering
  - Accepted Linux Debug/Release evidence and reconciled four-configuration hosted validation
  - Archive-aware original manifest and executed-identity retention
affects: ["05"]
tech-stack:
  added: []
  patterns: [real-component integration, literal expected spans/messages, owning diagnostic snapshots, identity-based evidence reconciliation]
key-files:
  created:
    - tests/parsing/parser_integration_test.cpp
    - .planning/phases/04-smoke-profile-parser/04-03-EXECUTION.md
  modified:
    - tests/CMakeLists.txt
key-decisions:
  - Preserve all sixteen accepted Phase 4 decisions and experimental M1 boundaries
  - No production/parser/API or semantic conversion changes were required
  - Formal plan closure and phase-wide requirement acceptance remain distinct owner gates
patterns-established:
  - Destroy temporary storage before resolving saved spans through the live originating manager
  - Recovered/parsed values never imply successful compilation with accumulated Errors
  - Compare actual hosted Start/Passed and GoogleTest RUN/OK identities with retained acceptance sets
requirements-completed: [PARSE-01, PARSE-02, PARSE-03]
requirements-delivered: [PARSE-01, PARSE-02, PARSE-03]
requirements-completion-exception: Cross-plan deferral preserved historically; only accepted after actual passed phase verification and explicit final owner acceptance
coverage:
  - id: D1
    description: Independent three-span values survive token/caller storage destruction and preserve literal shape without numeric semantics
    requirement: PARSE-03
    verification:
      - kind: integration
        ref: "tests/parsing/parser_integration_test.cpp#OwnedThreeSpanResultOutlivesTokens; DecimalShapeDoesNotPerformSemantics; WhitespaceAndPunctuationKeepTightSyntaxBounds; accepted local and hosted identities"
        status: pass
    human_judgment: false
  - id: D2
    description: Real lexical rejection, checked failures and accumulated diagnostics preserve ordering, exact alternatives and Error state without partial success
    requirement: PARSE-02
    verification:
      - kind: integration
        ref: "tests/parsing/parser_integration_test.cpp#InvalidStreamPreservesEveryLexicalDiagnostic; EarlierErrorsDoNotGateValidSyntax; RepeatedSyntaxErrorsPreserveOrderedDuplicates; checked source/stream failures"
        status: pass
    human_judgment: false
  - id: D3
    description: Fixed-profile syntax integrates with original newline/EOF locations and independent owning safe rendering
    requirement: PARSE-01
    verification:
      - kind: integration
        ref: "tests/parsing/parser_integration_test.cpp#OriginalNewlineEofLocations; ParserErrorsRenderWithEscapedOwningMetadata; hosted run 39580960005"
        status: pass
    human_judgment: false
duration: "Active implementation time unmeasured; 51.186335 seconds summed across 19 retained command receipts, including one sandbox-blocked configure"
implementation-completed: 2026-10-21
completed: 2026-10-21
formal-acceptance-recorded-at: "2026-10-21T15:20:21.322895+00:00"
status: complete
---

# Phase 4 Plan 03: Real M1 parser integration Summary

**Thirty-two real-component cases establish original-byte span ownership, lexical-error handoff, ordered diagnostics, newline/EOF locations and independent escaped rendering.**

## Acceptance boundary

Formal local 04-03 plan closeout was explicitly authorized and recorded at
**2026-10-21T15:20:21.322895+00:00**. Tasks 1–2, independent owner review, Linux validation,
publication and fresh hosted validation passed. Actual canonical Phase 4 technical
verification is recorded in [04-VERIFICATION.md](04-VERIFICATION.md).

All three plans are complete (18/18 written plans). At the original technical
verification checkpoint the implementation baseline recorded 2/3 Phase 4 plans
and 17/18 overall; this accepted closeout supersedes that lifecycle state. Final whole-phase owner acceptance is recorded at 2026-10-21T15:37:51.333576+00:00.
PARSE-01/02/03 are accepted only after the actual passed canonical phase verification
and explicit owner decision. Earlier plan-closeout deferral remains historical.
Summary existence is discovery, never execution or Phase 5 authorization.

## Performance

- Tasks: 2/2 locally implemented, independently reviewed and owner-accepted.
- Published three-file implementation: 08e50520afdab9e46da39182a8dcc4b56be66801,
  `test(parser): add M1 parser integration coverage`; parent 02e01063f4b5c4a70a9cfbc68b1749a8b87a2437.
- Local receipts: 19, of which 18 successful and one sandbox-blocked prerequisite
  configure. Summed command time is not active implementation or hosted duration.
- Recorded command interval: 2026-10-21T09:20:05.847682+00:00 through 2026-10-21T09:30:42.545516+00:00; includes pauses and the sandbox-blocked attempt, not active work duration.
- Formal local plan completion: 2026-10-21T15:20:21.322895+00:00; actual event, not backdated.

## Accomplishments

- Saved entry [1,39), return [23,36), integer [30,35) survive caller strings,
  lexical/result wrappers, token storage and engine destruction, copy/move and
  3,000 later registrations while the originating SourceManager remains alive.
- Six decimal shapes include 256, leading zeros and 16,384 digits, without value
  conversion/range validation. Whitespace/punctuation retain tight original bounds.
- Eleven lexical rows include signs, nondecimal input, comments, high bytes, NUL,
  duplicates and /*/return. Invalid suppresses parser diagnostics, preserves owning
  fields/order/Error state and never yields a successful partial representation.
- Independent valid syntax may yield a value with prior Errors; a test-local
  consumer predicate rejects a compilation-success handoff. Duplicate syntax
  failures remain ordered. Checked source/stream failures remain distinct.
- Eight EOF/location rows cover LF/CRLF/lone CR, empty/whitespace input and matching
  openers; three traced rendering rows check complete escaped owning output.

## Validation and evidence

| Gate | Local Debug | Local Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Parser discovered/executed | 142/142 | 142/142 |
| New / retained parser cases | 32/32 + 110/110 | 32/32 + 110/110 |
| Selected regressions | 93/93 | 93/93 |
| Full test / strict verify CTest | 561/561 each | 561/561 each |
| Selected C++23 entries | 16/16 | 16/16 |
| Direct include policy | PASS | PASS |

Accepted local evidence.json remains 83 entries; commands.jsonl retains 19 hashes.
Historical/04-01/04-02 mappings are preserved, never rewritten. This verification executed only the four owner-approved existing behavioral spot-checks; no full native suite or build was repeated.

[Hosted run 39580960005](https://github.com/m4rkens/VeyrLang/actions/runs/39580960005)
completed/success at the exact published implementation. Each job executed 561/561
in native test, strict verify and prepared acceptance, with exactly 142 parser
identities (32 integration + 110 retained = 13 baseline + 97 conformance).
All 529 previous full-suite identities were retained. Sixteen C++23 compile
entries and all three parser translation units are established by logs and two
actual compile databases per job. Include policy, Conan integrity/setup, prepared
acceptance and exactly one controlled isolated empty-cache rejection passed.

| Job | Conclusion | Actual completedAt (UTC) |
|---|---|---|
| windows-ci-Debug | success | 2026-10-21T10:14:27Z |
| ubuntu-ci-Release | success | 2026-10-21T10:05:11Z |
| ubuntu-ci-Debug | success | 2026-10-21T10:01:31Z |
| windows-ci-Release | success | 2026-10-21T10:23:15Z |

Raw hosted logs, all four downloaded evidence artifacts, full identity sets,
actual native/prepared GoogleTest RUN/OK records, compiler databases and per-step
conclusions are retained in ignored closeout-preparation/. hosted-acceptance.json
records reconciliation, not canonical phase verification. Expected offline errors
are negative acceptance evidence, not regressions. Existing Conan generated
quotation warnings and upstream Node/action deprecations are non-blocking.
Windows baseline JSON hashes match exact CRLF checkout bytes; Conan recipes,
profiles and locks retain mandated LF. No recorded hash was rewritten.

## Requirement traceability

| Requirement | Delivered phase evidence | Remaining acceptance gate |
|---|---|---|
| PARSE-01 | Complete fixed shape/EOF in 04-01; 04-02 syntax/prefix/unsupported forms; 04-03 real lexical pipeline and decimal/whitespace success | Technical phase verification passed; owner acceptance recorded at final boundary |
| PARSE-02 | Checked structure/precedence and exact messages in 04-01/02; lexical suppression, persistent state, ordered owning diagnostics, original EOF/opener/rendering in 04-03 | Technical verification passed; owner acceptance recorded; recovered values do not establish compilation success |
| PARSE-03 | Exactly three copyable/movable spans and distinct ParseResult alternatives; destroyed token storage, live manager and separate lexer/source/renderer boundaries | Technical verification passed; owner acceptance recorded; no general AST or cross-manager provenance promise |

## Task Commits

Tasks 1–2 share the separately approved three-file implementation commit
08e50520afdab9e46da39182a8dcc4b56be66801. No automatic per-task commits were made.
The owner authorized formal metadata/verification publication; no future
commit identity is invented here.

## Files Created/Modified

- `tests/parsing/parser_integration_test.cpp` — 32 integration cases; file-local helpers.
- `tests/CMakeLists.txt` — only add the new parser_tests source; PRE_TEST unchanged.
- 04-03-EXECUTION.md — original factual local checkpoint, publication/hosted
  reconciliation appended without deleting checkpoint history.
- STATE/ROADMAP count this formally closed plan locally; final whole-phase owner acceptance/publication remain pending.

## Decisions Made

None beyond accepted D-01–D-16 and owner gates. The experimental M1 profile remains
byte-identical to 52f598aaf031e549ed3bbe5c598bc43f5f7be6bb. No final grammar,
integer semantics, ABI, AST, lowering/backend or driver commitment is introduced.

## Deviations from Plan

No implementation or production correction. Owner-required summary discovery,
requirement deferral, explicit commit/publication and phase acceptance gates override
generic automatic GSD task commits, summary promotion and phase transitions.

## Issues Encountered

The first local configure could not acquire a Conan operation lock inside the
read-only sandbox. Its permitted offline rerun succeeded; no native test failed.
Prepared pinned dependencies were reused with --no-remote --build=never. Hosted
platform spelling/GetParam descriptions are normalized only for stable identity
comparison; all original logs and observations remain retained.

## User Setup Required

None for this preparation. No tools/dependencies installed, no CI mutation.

## Next Phase Readiness

Actual canonical goal verification has been performed; its factual report and
status routing accompany this local closeout. The owner accepted the verified Phase 4 scope and PARSE requirements;
the owner has explicitly authorized this controlled closeout publication. Phase 5 remains unstarted/unauthorized;
AST consolidation and semantic decimal/range work are not credited here.

Platform impact:
- Windows: Actual hosted Debug/Release passed; developer-host inspection remains independently pending/non-blocking.
- Linux: Accepted Omarchy local Debug/Release and independent Ubuntu hosted evidence passed.
- macOS future: Deferred; no new platform dependency.

## Self-Check: PASSED

Canonical summary schema and all three automated coverage rows passed. All
referenced implementation files and published commits exist; two source-list
paths are explicitly checked by the installed verify-summary query. All four
approved fresh behavioral spot-checks passed. Local plan completion itself did not accept phase requirements; the later explicit
owner phase acceptance does. Phase 5 work remains unauthorized.
