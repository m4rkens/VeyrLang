---
phase: 03-m1-experimental-smoke-profile-tokens-and-lexer
plan: "03"
subsystem: lexing
tags: [m1, experimental-smoke-profile, adversarial-tests, integration, verification]
requires:
  - phase: "03-01"
    provides: Accepted experimental profile with dedicated commit and verified remote publication
  - phase: "03-02"
    provides: Checked complete M1 lexer, kind/span tokens and accepted Phase 2 integration
provides:
  - All-byte, long-run and mixed-boundary lexer regression coverage
  - Source snapshot lifetime, diagnostic accumulation and renderer integration coverage
  - Independent Linux developer and Ubuntu/Windows hosted Debug/Release evidence
affects: ["04"]
tech-stack:
  added: []
  patterns: [independent exact byte oracles, retained historical evidence, explicit owner acceptance gates]
key-files:
  created:
    - .planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-03-EXECUTION.md
    - .planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-03-SUMMARY.md
  modified: [tests/lexing/token_test.cpp, tests/lexing/lexer_test.cpp]
key-decisions:
  - Preserve the accepted lexer and profile; adversarial coverage required no production or CMake correction
  - Recovered token vectors do not imply compilation success; persistent Error state remains authoritative
  - Successful hosted evidence permits canonical verification, not automatic phase completion or publication
patterns-established:
  - Exact independent token/span/Error oracles complement progress and whitespace-gap invariants
  - Superseded checkpoint inputs retain their original bytes without rewriting accepted manifests
requirements-completed: [LEX-02, LEX-03, LEX-04, LEX-05, LEX-06]
requirements-acceptance-scope: owner-accepted final Phase 3; Linux developer and Ubuntu/Windows hosted Debug/Release evidence
coverage:
  - id: D1
    description: All 256 byte values and mixed malformed inputs retain exact token, Error and EOF outcomes
    requirement: LEX-03
    verification:
      - kind: unit
        ref: "tests/lexing/lexer_test.cpp#SmokeProfileLexerAllBytes, AdversarialMixedPrecedenceHasExactIndependentOracles; Debug/Release focused logs and hosted run 39107885454"
        status: pass
    human_judgment: false
  - id: D2
    description: Large runs, newline locations, whitespace gaps and source-end EOF preserve bounded forward progress
    requirement: LEX-05
    verification:
      - kind: unit
        ref: "tests/lexing/lexer_test.cpp#LongRunsKeepLexicalShapeAndBoundedProgress, ManyIndependentErrorsPreserveEveryDelimiterAndWhitespaceGap, TokenAndDiagnosticLocationsPreserveOriginalNewlineBytes, EmptyAndTrailingBreakEofLocationsRemainByteBased"
        status: pass
    human_judgment: false
  - id: D3
    description: Returned tokens and original source views survive caller destruction and additional registrations
    requirement: LEX-02
    verification:
      - kind: unit
        ref: "tests/lexing/token_test.cpp#ReturnedTokensOutliveCallerStorageAndFurtherRegistrations; Linux Debug/Release and four hosted jobs"
        status: pass
    human_judgment: false
  - id: D4
    description: Original whitespace and comment/high-byte boundaries remain compatible with safe rendering
    requirement: LEX-04
    verification:
      - kind: integration
        ref: "tests/lexing/lexer_test.cpp#AdversarialMixedPrecedenceHasExactIndependentOracles, RealLexicalErrorsRenderHostileBytesIntoStableOwnedAscii"
        status: pass
    human_judgment: false
  - id: D5
    description: Diagnostic order, duplicates, checked failures and persistent Error state survive scanning and rendering
    requirement: LEX-06
    verification:
      - kind: integration
        ref: "tests/lexing/lexer_test.cpp#SharedEnginePreservesEveryPriorSeverityAndRejectsSuccessHandoff, UnknownSourcePreservesMixedDiagnosticsOnNonemptyManager, RenderingCheckedFailureCannotClearAccumulatedLexicalError"
        status: pass
    human_judgment: false
  - id: D6
    description: Final test packet passes independent developer and four-job hosted native, verify and prepared acceptance gates
    verification:
      - kind: integration
        ref: ".veyr-dev/execution/03-03/commands.jsonl, evidence.json and verification/hosted.json; run 39107885454 at be84bb9702afa75ad07803a678e7fd5a4e6358d7"
        status: pass
    human_judgment: false
duration: "Active elapsed work not measured; 52.230 seconds recorded local validation command time"
completed: 2026-10-18
status: complete
closeout-status: owner-accepted; closeout commit and publication pending separate approval
---

# Phase 3 Plan 03: Adversarial lexer and integrated-contract evidence Summary

**All-byte, hostile-run, snapshot-lifetime and diagnostic-renderer regression tests validate the unchanged experimental M1 lexer on Linux and Windows hosted CI.**

## Performance

- Local execution began: 2026-10-18T06:56:36.695078+00:00, from the retained entry receipt.
- Local validation: 16 successful commands; summed command time 52.230 seconds.
- Hosted evidence reviewed: 2026-10-18, run 39107885454.
- Tasks: 2 locally implemented and owner-accepted; hosted validation now confirmed.
- Execution files: 2 test files and 1 execution checkpoint. This summary is subsequent verification documentation.
- Active development, owner-wait and total wall-clock durations were not measured.

## Accomplishments

- D1: Added 256 independently discovered byte cases and exact mixed recovery oracles; preserved all 35 normative examples.
- D2: Covered 65,536-byte runs, 4,096 independent Errors, positive scanner progress, whitespace-only gaps and original newline/EOF locations.
- D3: Proved returned token/source-view lifetime across caller destruction and 3,000 further source registrations.
- D4: Exercised hostile original bytes and display names through separate owning plain-text rendering.
- D5: Proved ordered accumulation, repeated diagnostics, checked SourceError propagation and Error preservation through successful and failed rendering.
- D6: Retained independent developer Debug/Release and completed four-job hosted native/verification/prepared/offline evidence.

## Task Commits

Tasks 1–2 were reviewed and published together in the explicitly approved coherent
execution commit `be84bb9702afa75ad07803a678e7fd5a4e6358d7`:
`tests: add adversarial M1 lexer and integration coverage`.
It contains exactly the two test files and 03-03-EXECUTION.md. The parent is
the accepted lexer publication `da92389b5eb6b6b2bbe949072015dd950d8273b3`.
No separate per-file commits, history rewrite or automatic publication occurred.
This summary and final verification documentation are not committed or published.

## Files Created/Modified

- `tests/lexing/token_test.cpp` — returned-token and source snapshot lifetime regression.
- `tests/lexing/lexer_test.cpp` — all-byte, long-run, recovery, location, accumulation and renderer integration regressions.
- [03-03-EXECUTION.md](03-03-EXECUTION.md) — actual execution, owner review, publication and verification checkpoint records.
- This summary — canonical coverage and actual validation evidence for owner review.

## Validation and Evidence

| Gate | Linux developer Debug | Linux developer Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Focused / newly added | 323/323 / 268/268 | 323/323 / 268/268 |
| All-byte / normative | 256/256 / 35/35 | 256/256 / 35/35 |
| Existing source/diagnostic/renderer | 61/61 | 61/61 |
| Full CTest | 419/419 | 419/419 |
| Compiler/directory/C++23 | 13 entries | 13 entries |
| Direct include-policy | PASS | PASS |

All first native regression runs were GREEN. No expected RED check, unexpected
failure, retry or production correction occurred in this plan. Earlier 03-02 RED
evidence remains historical and is not counted as a failure here.

[Build foundation run 39107885454](https://github.com/m4rkens/VeyrLang/actions/runs/39107885454)
completed successfully at the exact execution commit above. Ubuntu Debug,
Ubuntu Release, Windows Debug and Windows Release each executed 419/419 tests
in native test, verify and prepared acceptance. Each execution contains all
323 focused names, byte indices 0–255 and 35 normative names. Each verify and
prepared path reports 13 selected C++23 entries. The include-policy CTest calls
the Python guard directly and passes in all three executions; a separate
standalone workflow invocation is not claimed. All four isolated empty-cache
checks report the expected missing-package/no-remote rejection once, with no
acquisition or retry. A recovered scan or cache hit is never acceptance proof.

Retained local evidence: `.veyr-dev/execution/03-03/REVIEW.md`, evidence.json,
commands.jsonl, validation.json and 16 raw logs. The original 62-entry manifest
and original command logs remain unchanged; superseded execution-record bytes
resolve through checkpoint/accepted-inputs/. Earlier 65-entry 03-02 and 36-entry
Phase 2 packets retain their approved live/baseline/archive mappings.
New verification receipts, raw hosted log, actual Passed name sets and canonical
query results are under `.veyr-dev/execution/03-03/verification/`.
The published pre-reconciliation execution record is separately archived there.

## Decisions Made

All fourteen discussion decisions and the exact accepted profile remain binding
only for experimental M1. The profile's dedicated commit is
`52f598aaf031e549ed3bbe5c598bc43f5f7be6bb`; its bytes are unchanged. Numeric
shape does not enforce semantic range. Every Invalid has one owning Error with
the exact primary span and empty related spans. Callers must consult persistent
DiagnosticEngine::has_errors before any successful compilation handoff.

## Deviations from Plan

No implementation or architecture deviation. Existing behavior satisfied all
new tests without production, API or CMake changes. Explicit owner gates override
generic automatic summary commits, lifecycle advancement and next-phase routing.
The owner has now accepted final Phase 3 verification and authorized bounded
documentation-only closeout. Earlier verification-preparation gates are satisfied;
closeout commit/publication and any next phase remain separately authorized.

## Issues Encountered

No acceptance-blocking local or hosted failures. Optional per-case traces for
looped long-run/EOF fixtures remain a future maintenance improvement, unchanged.
The prior Phase 2 severity-initialization, unsupported-severity and combined
renderer-clipping follow-ups remain non-blocking and outside this work.

## User Setup Required

None. Dependencies and reviewed toolchain policy are unchanged; local packages
were reused without dependency acquisition or rebuild.

## Next Phase Readiness

Hosted validation is complete. Canonical Phase 3 goal verification passed:
five roadmap criteria, 21 plan truths, 22 deduplicated scored truths, 12 artifact
entries, 11 directly inspected links, six requirements and 14 decisions. See
[03-VERIFICATION.md](03-VERIFICATION.md). Canonical verification.status returned
passed. The owner explicitly ACCEPTED Phase 3 and all six LEX requirements.
Plan 03-03 and Phase 3 are complete at 3/3 under bounded canonical bookkeeping;
Phase 1 remains 8/8, Phase 2 4/4, and requirements remain 27 accepted/22 pending.
The supported state.complete-phase --phase 3 and roadmap.update-plan-progress 3
operations preserve Phase 3 as the current closed phase; unrestricted phase.complete
is not used. Actual helper receipts are retained under the ignored closeout/
directory. Closeout commit/publication still require separate owner approval.
Phase 4 is unstarted and unauthorized. Windows hosted CI is validated; Windows
developer-host inspection remains independently pending/non-blocking, and macOS
is deferred. No source, tests, dependencies or accepted findings changed.

## Self-Check: PASSED

The execution commit and parent exist, its exact three-file scope was inspected,
the referenced implementation/evidence files exist, and actual local/hosted
execution records support every coverage entry. Canonical schema, coverage and
summary validation results are retained separately; no final phase acceptance
or publication permission is inferred from this self-check. Final acceptance
comes from the owner's explicit decision recorded above.
