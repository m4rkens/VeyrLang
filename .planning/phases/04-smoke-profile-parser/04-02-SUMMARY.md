---
phase: 04-smoke-profile-parser
plan: "02"
implementation-commit: 75d712f1931b375a7803a19800539b49fbe84732
subsystem: parsing
tags: [m1, parser-conformance, independent-oracles, checked-input, syntax-diagnostics]
requires:
  - phase: "04-01"
    provides: Published checked M1 parser, owned three-span result and formally accepted baseline tests
provides:
  - Independent structural and validation-priority conformance matrices
  - Complete syntax-position, truncated-prefix, found-label and unsupported-form coverage
  - Accepted Linux evidence and successful four-configuration hosted validation of all 97 new cases
affects: ["04-03", "05"]
tech-stack:
  added: []
  patterns: [independent exact result/message/span oracles, retained baseline cases, discovered-versus-executed identity reconciliation]
key-files:
  created:
    - .planning/phases/04-smoke-profile-parser/04-02-EXECUTION.md
  modified:
    - tests/parsing/parser_test.cpp
key-decisions:
  - Preserve the accepted parser implementation and all sixteen Phase 4 design decisions
  - Defer PARSE-01 through PARSE-03 completion across remaining integration and phase acceptance
  - Keep formal plan completion, publication and fresh Wave 3 authorization separate
patterns-established:
  - Later structural errors outrank earlier lexical or syntax faults without new diagnostics
  - Syntax rejection has exact first-error fields and never a partial successful value
requirements-completed: []
requirements-delivered: [PARSE-01, PARSE-02, PARSE-03]
requirements-acceptance-scope: Plan 04-02 conformance scope only; all PARSE requirements remain Pending until remaining Phase 4 acceptance
requirements-completion-exception: Owner-directed cross-plan deferral; do not invoke generic requirements.mark-complete
coverage:
  - id: D1
    description: Structural, checked-error and multi-fault precedence conformance without diagnostic side effects
    requirement: PARSE-02
    verification:
      - kind: unit
        ref: "tests/parsing/parser_test.cpp#SmokeProfileParserStructure and SmokeProfileParserInvalidPlacement; 44 Task 1 additions including all 19 structural rows and priority fixtures; retained local identity lists and hosted run 39493376090"
        status: pass
    human_judgment: false
  - id: D2
    description: Exact syntax, prefix, EOF, found-label and unsupported-form rejection contracts
    requirement: PARSE-01
    verification:
      - kind: unit
        ref: "tests/parsing/parser_test.cpp#SmokeProfileParserSyntax, SmokeProfileParserPrefixes, SmokeProfileParserFoundLabels and SmokeProfileParserForms; all 53 Task 2 rows; local and four hosted configurations"
        status: pass
    human_judgment: false
  - id: D3
    description: Complete-entry successes preserve all three source spans without numeric conversion or API expansion
    requirement: PARSE-03
    verification:
      - kind: unit
        ref: "tests/parsing/parser_test.cpp#SmokeProfileParserDecimalForms.CompleteEntryOwnsThreeSpansWithoutNumericConversion; four decimal forms, 13 retained baselines and all 110 parser identities in run 39493376090"
        status: pass
    human_judgment: false
duration: "Active development unmeasured; 75.435790 seconds summed across 27 retained command receipts, including prerequisite history"
implementation-completed: 2026-10-20
completed: 2026-10-21
formal-acceptance-recorded-at: "2026-10-21T09:06:46.513877+00:00"
status: complete
---

# Phase 4 Plan 02: Independent M1 parser conformance Summary

**Ninety-seven independent conformance cases verify checked token streams, deterministic error precedence and exact first-error parser diagnostics while preserving all thirteen baseline cases.**

## Acceptance boundary

The owner explicitly approved formal Plan 04-02 closeout and its dedicated
four-file commit/publication. Formal lifecycle reconciliation was recorded at
2026-10-21T09:06:46.513877+00:00. The reviewed draft remained outside canonical discovery
until this owner-authorized operation; the summary was promoted last. Implementation
and hosted dates below remain their actual historical observations.

The owner accepted Tasks 1–2 following independent implementation/evidence review.
Both tasks were published together in
75d712f1931b375a7803a19800539b49fbe84732,
`test(parser): add M1 parser conformance coverage`. Hosted acceptance is now
independently reconciled from actual completed logs. 04-01/04-02 are now complete, 04-03 remains unstarted, Phase 4 is 2/3 and the global
completed-plan count is 17. This does not accept the whole phase or authorize Wave 3.

The explicit owner-directed PARSE exception remains effective:
requirements-completed is empty; PARSE-01, PARSE-02 and PARSE-03 remain Pending;
global requirements remain 27 accepted/22 pending. Full acceptance still needs
04-03 integration, phase-goal verification and final owner acceptance. No generic
requirement-completion or automatic phase-transition helper is authorized.

## Performance

- Tasks: 2/2 implemented and locally accepted; four hosted configurations passed.
- New tests: 44 Task 1 plus 53 Task 2 cases, 97 total; 13 original cases retained.
- Published files: the test translation unit and execution/checkpoint record.
- Local command receipts: 27, from 2026-10-20T16:09:21.001015+00:00 through
  2026-10-20T20:15:55.930839+00:00; this interval includes environment preparation
  and gaps and is not active development duration.
- Summed recorded command time: 75.435790 seconds, not CI or active work duration.
- Formal closeout recorded: 2026-10-21T09:06:46.513877+00:00.

## Accomplishments

- Task 1 adds 44 structural/priority cases, including 19 malformed-stream rows,
  exact SourceError/TokenStreamError payloads, earliest-index/same-index priority,
  and later structural faults outranking earlier Invalid or syntax faults.
- Task 2 adds 53 cases: twelve syntax positions, eleven incomplete prefixes,
  ten found labels, nine unsupported syntax forms, seven lexical forms and four
  decimal success forms. Exact full messages, primary/related spans, prior engine
  state and absence of partial success are checked against independent oracles.
- Complete successes preserve the entry/return/integer span relationships and
  defer numeric conversion/range checking. No production parser, public API,
  test framework, CMake, accepted M1 profile or dependency correction was required.

## Validation and evidence

| Local gate | Linux Debug | Linux Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Parser discovery/execution | 110/110 | 110/110 |
| New conformance / retained baseline | 97/97 + 13/13 | 97/97 + 13/13 |
| Selected regressions | 93/93 | 93/93 |
| Full CTest / strict verify CTest | 529/529 each | 529/529 each |
| Selected compiler/directory/C++23 entries | 15/15 | 15/15 |
| Direct include policy | PASS | PASS |

Accepted local evidence is retained under ignored .veyr-dev/execution/04-02/:
local-validation/evidence.json has 70 entries; commands.jsonl has 27 hash-verified
receipts. Earlier prerequisite stops and their immutable manifests resolve through
the existing historical mappings. The permitted offline resumption used pinned
Conan 2.33.0, LLVM 23.1.2 and GoogleTest 1.18.0 with --no-remote --build=never;
ordinary cache usage bookkeeping was owner-authorized. No dependency download,
rebuild, revision or lock change was part of 04-02. No new native command ran
during closeout preparation.

### Hosted acceptance

[Build foundation run 39493376090](https://github.com/m4rkens/VeyrLang/actions/runs/39493376090)
is completed/success at the exact implementation commit. All four jobs passed.
Actual CTest Start indices 1–529 and Passed names reconcile exactly with the
accepted name sets in native test, strict verify and prepared acceptance.
This demonstrates hosted discovery/execution; no standalone hosted ctest -N
invocation is claimed. All 110 parser identities, 97 new cases and 13 baselines
occur in each of the three complete suites per job.

| Job | Conclusion | Actual completedAt (UTC) |
|---|---|---|
| ubuntu-ci-Debug | success | 2026-10-20T20:41:03Z |
| ubuntu-ci-Release | success | 2026-10-20T20:45:18Z |
| windows-ci-Debug | success | 2026-10-20T20:53:55Z |
| windows-ci-Release | success | 2026-10-20T21:06:27Z |

Every job passed 529/529 in native test, verify and prepared acceptance, fifteen
selected C++23 entries in verify/prepared, and include-policy. Configure/build,
mandatory dependency setup/integrity and controlled isolated empty-cache rejection
passed. The isolated negative case reports exactly one expected rejection without
acquisition/retry. Optional warm-snapshot publication steps being skipped do not
skip native acceptance. Generated Conan-toolchain quotation warnings and action
Node/runtime deprecations are non-blocking; no unexpected error was identified.

Fresh read-only captures and exact job/test reconciliation are retained in
.veyr-dev/execution/04-02/closeout-preparation/. New evidence is separate from
the accepted local receipts. Superseded planning bytes are archived before
promotion; original manifests, command logs and historical receipts are preserved.

## Requirement traceability

| Requirement | Delivered 04-02 evidence | Remaining scope |
|---|---|---|
| PARSE-01 | Complete shape/EOF successes; all syntax/prefix/unsupported-form matrices; local and four-job hosted execution | 04-03 real component integration and final phase acceptance |
| PARSE-02 | Exact structure/source errors, precedence, first-error messages and spans, preserved diagnostics, no partial success | 04-03 lexer/renderer/newline/hostile-input integration and phase verification |
| PARSE-03 | Public checked outcome assertions and three owned success spans without production/API changes | Actual token-destruction/source-registration/renderer integration in 04-03; final owner acceptance |

All sixteen accepted design decisions and the published experimental M1 profile
remain binding. This test-only plan adds no general grammar, parser recovery,
AST, semantic conversion, stable type system or ABI commitment.

## Task Commits

Tasks 1–2 share the separately owner-approved two-file implementation commit
75d712f1931b375a7803a19800539b49fbe84732. No automatic per-test task commits were
created. The owner separately authorized the dedicated formal closeout commit/publication.
Its actual identity and remote verification are recorded in ignored publication
evidence rather than a self-referential follow-up commit.

## Files Created/Modified

- `tests/parsing/parser_test.cpp` — 97 new independent cases; 13 baselines preserved.
- `.planning/phases/04-smoke-profile-parser/04-02-EXECUTION.md` — original preparation,
  validation and owner checkpoint history, reconciled with publication/hosted evidence.
- `.planning/STATE.md` and `.planning/ROADMAP.md` — formal 2/3 progress,
  17 completed plans and the next separate owner gate; no requirement acceptance.
- This summary became canonical last during the explicitly approved formal closeout.

## Decisions Made

None beyond the accepted plan and explicit cross-plan PARSE exception. The owner
abandoned the held phase-end CI-trigger candidate after public visibility; automatic
push/PR CI remains the published policy. No CI file or held candidate is changed.

## Deviations from Plan

No implementation deviation or production correction. Repository owner gates
override automatic task/metadata commits and next-plan execution. The explicit
PARSE exception overrides generic copying of plan requirements into completed
requirements and requirements.mark-complete. The reviewed summary stayed outside
canonical discovery until the owner approved formal promotion; status fields alone
were not used as an execution gate.

## Issues Encountered

The initial worktree lacked its local Conan CLI and prepared outputs. Earlier
stops are preserved; bounded offline preparation succeeded after owner permission
for ordinary shared-cache usage bookkeeping. Generated toolchain quotation
warnings remain non-blocking. Hosted reconciliation normalizes CTest's optional
GetParam() object-byte descriptions while preserving every stable test identity
and the original logs. No accepted test expectation was changed during closeout.

## User Setup Required

None for documentation preparation. Future execution uses the prescribed
worktree-specific prepared configurations and doctor checks; no acquisition or
cache operation is implied by formal closeout.

## Next Phase Readiness

04-03 is the next planned unit. Formal 04-02 closeout is complete; its dedicated
publication is owner-authorized and actual remote proof must be checked before
fresh owner execution authorization. It covers real lexer/parser/renderer
integration, token-storage destruction, source lifetime/registration, newline/EOF
and hostile input, with independent final Debug/Release evidence. No part of that
work or final Phase 4 goal verification/acceptance is credited here.

Stage 1 and the abandoned phase-end CI candidate remain unpublished and isolated;
Stage 2 performance remediation remains deferred and independent of compiler work.

Platform impact:
- Windows: Hosted Debug/Release conformance acceptance passed; developer-host inspection remains independently pending/non-blocking.
- Linux: Accepted local Debug/Release and independent Ubuntu hosted acceptance preserved.
- macOS future: Deferred; no new platform dependency.

## Self-Check: PASSED

All summary-preparation, hosted identity/count, retained-evidence and scope checks
passed. Owner-approved formal acceptance was recorded at 2026-10-21T09:06:46.513877+00:00;
canonical discovery agrees with Phase 4 2/3 and seventeen completed plans.
Dedicated publication is explicitly approved; actual commit/remote receipts are
retained separately. Fresh 04-03 execution remains an owner gate. No native
execution was repeated.
