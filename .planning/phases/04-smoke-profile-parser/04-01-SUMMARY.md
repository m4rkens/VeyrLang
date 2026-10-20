---
phase: 04-smoke-profile-parser
plan: "01"
implementation-commit: 425e5ff456a59cafc04a2fa46ddbaa0f2318e792
subsystem: parsing
tags: [m1, experimental-smoke-profile, checked-parser, source-spans, diagnostics]
requires:
  - phase: "02"
    provides: Immutable source snapshots, checked byte spans and owning ordered diagnostics
  - phase: "03"
    provides: Published experimental M1 profile and accepted conforming Token/Lexer APIs
provides:
  - Complete checked fixed-entry parser with three owned source spans
  - Distinct source, token-stream, lexical and syntax outcomes
  - Baseline native tests with accepted Linux and successful four-configuration hosted evidence
affects: ["04-02", "04-03", "05"]
tech-stack:
  added: []
  patterns: ["whole-stream validation before lexical rejection and syntax", "first-error owning diagnostics", "no borrowed token storage"]
key-files:
  created:
    - include/veyr/parsing/parser.hpp
    - src/parsing/parser.cpp
    - tests/parsing/parser_test.cpp
    - .planning/phases/04-smoke-profile-parser/04-01-EXECUTION.md
  modified: [CMakeLists.txt, tests/CMakeLists.txt, .planning/STATE.md]
key-decisions:
  - Preserve all sixteen accepted Phase 4 decisions and the published non-final M1 profile
  - Keep numeric conversion and range validation outside parsing
  - Separate confirmed technical evidence from owner-gated formal plan and requirement completion
patterns-established:
  - Return a complete ParsedSmokeEntry or a distinct checked rejection, never a partial success
  - Preserve original SourceError payloads and existing DiagnosticEngine state
requirements-completed: []
requirements-delivered: [PARSE-01, PARSE-02, PARSE-03]
requirements-acceptance-scope: Formally accepted Plan 04-01 scope only; PARSE-01 through PARSE-03 remain pending across subsequent Phase 4 plans
requirements-completion-exception: Explicit owner-approved deferral of PARSE-01 through PARSE-03; no generic requirements.mark-complete operation
coverage:
  - id: D1
    description: Complete profile plus EOF yields tight entry, return and integer spans without semantic conversion
    requirement: PARSE-01
    verification:
      - kind: unit
        ref: "tests/parsing/parser_test.cpp#SmokeProfileParser.FixedEntryHasTightSpans and DecimalTokensDeferConversionAndRange; retained Debug/Release focused logs and hosted run 39222510470"
        status: pass
    human_judgment: false
  - id: D2
    description: Checked validation and lexical rejection precede first-error syntax diagnostics without partial output
    requirement: PARSE-02
    verification:
      - kind: unit
        ref: "tests/parsing/parser_test.cpp#SmokeProfileParser missing-semicolon, missing-closer, trailing-token, unknown-source, malformed-stream, Invalid and prior-Error baselines; retained logs and hosted run 39222510470"
        status: pass
    human_judgment: false
  - id: D3
    description: Minimal source-linked value and checked API retain existing component boundaries and value semantics
    requirement: PARSE-03
    verification:
      - kind: unit
        ref: "tests/parsing/parser_test.cpp#SmokeProfileParserValue; four named value tests in each local configuration and all four hosted jobs"
        status: pass
      - kind: integration
        ref: "CMakeLists.txt and tests/CMakeLists.txt; hosted run 39222510470 native test/verify/prepared acceptance, 432/432 and 15 C++23 entries per configuration"
        status: pass
    human_judgment: false
duration: "Active work not measured; 56.032538 seconds summed across 31 retained local commands"
completed: 2026-10-20
formal-acceptance-recorded-at: "2026-10-20T15:50:37.879175+00:00"
status: complete
---

# Phase 4 Plan 01: Checked experimental M1 parser Summary

**A synchronous checked parser recognizes the fixed M1 entry and integer return, owns three original-byte spans and emits precise first-mismatch diagnostics.**

## Acceptance boundary

The owner explicitly accepted formal Plan 04-01 closeout. The local lifecycle
reconciliation was recorded at 2026-10-20T15:50:37.879175+00:00; this is not a backdated
implementation or hosted-validation timestamp. Tasks 1–2 were previously
Linux-owner-accepted on 2026-10-19, published in
425e5ff456a59cafc04a2fa46ddbaa0f2318e792, and validated by four hosted jobs.
Phase 4 is now 1/3 formally complete. Plans 04-02/03 remain unstarted.

**Explicit owner-approved PARSE exception:** Formal completion of 04-01 does not
complete PARSE-01, PARSE-02 or PARSE-03. Full acceptance extends across subsequent
Phase 4 plans. The generic execute-plan instruction to copy every plan requirement
into requirements-completed and invoke requirements.mark-complete is not applied.
All three requirements and their definitions/mappings remain unchanged and pending;
global counts remain 27 accepted/22 pending. requirements-delivered describes
implemented plan-scope functionality, not requirement acceptance.

The installed summary structure and worktree orchestrator's single-writer
STATE/ROADMAP reconciliation are used within this bounded owner authorization.
No automatic task/metadata commit, phase.complete, next-plan execution or requirement
completion helper ran. The reviewed candidate was parked outside canonical summary
discovery while these documents were reconciled, then finalized under its canonical
name last. Filename-based discovery now agrees with actual formal acceptance.
Mechanical readiness never grants fresh Wave 2 execution or publication approval.

## Performance

- **Recorded local command interval:** 2026-10-18T18:58:56.770113+00:00 through
  2026-10-18T19:07:57.198696+00:00; this is not active development duration.
- **Recorded command time:** 56.032538 seconds across 31 command/log receipts.
- **Tasks:** 2/2 implemented, Linux-owner-accepted and now formally closed for Plan 04-01 scope.
- **Implementation files:** Five; the published checkpoint also contains STATE
  and the execution record, seven files in total.
- **Formal closeout recorded:** 2026-10-20T15:50:37.879175+00:00.

## Accomplishments

- ParsedSmokeEntry contains exactly entry_span, return_span and integer_span,
  with ordinary equality/copy/move semantics. It retains no token references,
  source-text copy or converted numeric value. Text resolution requires the
  originating SourceManager to remain alive.
- parse synchronously consumes existing conforming tokens through actual EOF.
  ParseResult distinguishes ParsedSmokeEntry, ParseRejected, original SourceError
  and TokenStreamError. Seven stream-error kinds remain distinct from lexical
  and syntax rejection.
- Selected-source lookup and complete increasing-index structural validation
  precede Invalid rejection and syntax. Source errors preserve their complete
  payload. Infrastructure and lexical rejection emit no parser diagnostic.
- The fixed twelve-position recognizer emits one owning explicitly initialized
  Error at its first mismatch, with the full unexpected token or zero-width EOF
  primary. Only expected ')' or '}' adds the matching opener. Existing diagnostics
  persist and no successful partial parse is exposed.
- Success spans for the 33-byte sample are entry [0,33), return [21,31) and
  integer [28,30). Decimal spellings including 256, leading zeroes and long digits
  receive no parser numeric conversion or range check. A returned syntax value
  alone never establishes compilation success while engine Errors remain.

## Validation and evidence

Retained Linux evidence belongs to the original checkout, not this new worktree:
its ignored .veyr-dev/execution/04-01/ contains evidence.json, commands.jsonl,
validation.json, REVIEW.md, logs, prepared metadata and owner-acceptance/receipt.json.
The original receipt maps superseded review documentation to exact archived bytes;
those historical manifests and receipts have not been rewritten.

| Local gate | Linux Debug | Linux Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Named parser discovery/execution | 13/13 | 13/13 |
| Selected existing regressions | 93/93 | 93/93 |
| Full CTest / strict verify CTest | 432/432 each | 432/432 each |
| Compiler/directory/C++23 entries | 15/15 | 15/15 |
| Direct include policy | PASS | PASS |

The thirteen tests are four SmokeProfileParserValue tests and nine
SmokeProfileParser baselines. All original names were reconciled against actual
discovery and execution. The two expected RED checks were the missing-header
compile failure and missing-parse-definition link failure; neither is a final
validation failure. A sandbox operation-lock restriction was resolved before the
same cache-only configure succeeded, without dependency acquisition or repair.

During this preparation, 75/75 manifest entries, 31/31 command-log hashes, all
five implementation freeze hashes, six immutable receipts and 201 historical
references were revalidated. New read-only hosted-log captures and documentation
checks reside separately in this worktree's ignored
.veyr-dev/execution/04-01/closeout-preparation/; retained-evidence-check.json
records the original checkout root and archival resolution receipt identity.
No native command was repeated.

### Hosted evidence

[Build foundation run 39222510470](https://github.com/m4rkens/VeyrLang/actions/runs/39222510470)
is completed/success for the exact published parser commit above. Actual job logs
contain all thirteen parser test names, 432/432 CTest totals in native test,
verify and prepared acceptance, and fifteen selected C++23 entries per job.

| Job | Conclusion | Actual completedAt (UTC) |
|---|---|---|
| ubuntu-ci-Debug | success | 2026-10-19T08:30:22Z |
| ubuntu-ci-Release | success | 2026-10-19T08:35:08Z |
| windows-ci-Debug | success | 2026-10-19T08:45:32Z |
| windows-ci-Release | success | 2026-10-19T08:55:27Z |

All four native configure/build/test/verify, prepared acceptance and isolated
acquisition-disabled empty-cache rejection steps concluded success. Include
checks are part of the verified native/prepared procedures. These are hosted
results, not Windows developer-host acceptance or new execution in this checkout.

## Requirement traceability

| Requirement | Delivered evidence | Remaining owner-gated scope |
|---|---|---|
| PARSE-01 | Complete profile/EOF matcher; FixedEntryHasTightSpans and DecimalTokensDeferConversionAndRange; local and four-job hosted results | Formal requirement acceptance; 04-02 conformance and 04-03 integration evidence |
| PARSE-02 | Original checked errors, Invalid suppression, first syntax Error/EOF/opener and trailing-token baselines; local and hosted results | Full malformed/precedence, twelve-position and eleven-prefix matrices in 04-02; integration in 04-03 |
| PARSE-03 | Three-span value and synchronous checked API; four value tests; existing-component CMake wiring; local and hosted results | Actual token-destruction/source-lifetime/renderer integration in 04-03 and final owner acceptance |

All sixteen accepted discussion decisions remain binding for experimental M1.
No new general syntax, AST framework, semantic conversion, final type system,
ABI, driver or LLVM dependency is introduced.

## Task Commits

The owner separately approved one seven-file implementation checkpoint:
`425e5ff456a59cafc04a2fa46ddbaa0f2318e792`,
`parser: add checked M1 smoke profile parser`. Both task implementations are
in that commit. Generic RED/GREEN/per-task commits were not made under repository
policy. This preparation creates no closeout commit and publishes nothing.

## Files Created/Modified

- `include/veyr/parsing/parser.hpp` — owned value and checked public outcomes.
- `src/parsing/parser.cpp` — validation, lexical gate, fixed matcher and diagnostics.
- `tests/parsing/parser_test.cpp` — four value and nine baseline native tests.
- `CMakeLists.txt` — parser.cpp in existing veyrcore.
- `tests/CMakeLists.txt` — parser_tests with existing policy and PRE_TEST discovery.
- [04-01-EXECUTION.md](04-01-EXECUTION.md) and [STATE.md](../../STATE.md) — published checkpoint records, reconciled for owner-authorized formal Plan 04-01 closeout.

## Decisions Made

None beyond the accepted plan and sixteen discussion decisions. The profile
remains byte-identical to dedicated commit
52f598aaf031e549ed3bbe5c598bc43f5f7be6bb. Producer conformance and manager-local
source identity remain caller responsibilities; matching numeric IDs cannot
detect a foreign manager. Ordinary allocation exceptions propagate without batch
rollback. No source spelling, omitted whitespace or numeric meaning is rechecked.

## Deviations from Plan

No implementation scope deviation. The explicit owner-approved PARSE exception
overrides generic requirement-completion metadata/helpers for this plan; all three
requirements remain pending. Generic automatic commits and phase transitions are
also withheld under repository policy. Formal plan progress is reconciled to 1/3,
without final phase verification or phase acceptance. The published Stage 2 CI
baseline is inherited unchanged; held Stage 1 and unpublished Stage 2 corrections
are excluded.

## Issues Encountered

Expected RED results and the resolved sandbox configure restriction are retained
as history. Existing generated-toolchain quotation warnings remain non-blocking;
an evidence extractor was corrected for padded CTest output without rerunning
native commands. No acceptance-blocking parser defect was identified by the
accepted implementation review. Optional diagnostic/renderer improvements from
earlier phases remain deferred and are not implemented here.

## User Setup Required

None for documentation preparation. Future native work must select and verify
the correct worktree/build directory through the approved doctor/toolchain policy;
original build outputs are evidence, not a new worktree's configured outputs.

## Next Phase Readiness

The next implementation unit is 04-02, not another parser implementation.
Formal 04-01 closeout and its summary/acceptance record are now available.
04-02 execution still requires preserved baseline test/evidence snapshots,
resolved closeout publication gates and fresh owner authorization. Its only implementation edit is the independent
test matrices in parser_test.cpp. A production defect requires separate approval.
Wave 3 remains later and unstarted. Canonical phase-goal verification and final
Phase 4 acceptance remain separate gates after the later plans.

The deferred Stage 2 performance investigation does not block compiler work.
Stage 1 remains held and unpublished. Neither is part of this closeout candidate.

Platform impact:
- Windows: Hosted Debug/Release parser validation passed; developer-host inspection independently pending/non-blocking.
- Linux: Accepted Omarchy Debug/Release and successful independent Ubuntu hosted evidence are preserved.
- macOS future: Deferred; no new platform-specific parser or build behavior.

## Self-Check: PASSED

Published artifacts, retained evidence, exact hosted results and owner-approved
formal Plan 04-01 acceptance support local closeout. Phase 4 is 1/3; both later
plans are unstarted. The explicit PARSE exception preserves 27 accepted/22 pending.
Final lifecycle review, commit/publication and fresh Wave 2 authorization remain
separate owner gates. No native validation or implementation was repeated.
