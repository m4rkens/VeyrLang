---
phase: 05-minimal-ast-and-semantic-smoke
plan: "01"
implementation-commit: aab44db8fc63749d5186d79b516412438b8bd1a1
subsystem: semantics
tags: [m1, shared-ast, checked-semantics, decimal-range, source-spans, diagnostics]
requires:
  - phase: "02"
    provides: Immutable source snapshots, checked source spans and owning ordered diagnostics
  - phase: "03"
    provides: Published non-final M1 profile and accepted lexer boundary
  - phase: "04"
    provides: Accepted fixed-entry parser and owned ParsedSmokeEntry contract
provides:
  - One independently consumable three-span AST shared with the unchanged parser API
  - Complete checked decimal semantic validator and restricted owning validated success
  - Twenty baseline tests with accepted Linux and successful four-configuration hosted evidence
affects: ["05-02", "05-03", "06"]
tech-stack:
  added: []
  patterns: [shared source-only AST boundary, ordered checked validation, bounded decimal accumulation, privately constructed owning success]
key-files:
  created:
    - include/veyr/ast/smoke_entry.hpp
    - include/veyr/semantics/smoke_validator.hpp
    - src/semantics/smoke_validator.cpp
    - tests/ast/smoke_entry_test.cpp
    - tests/semantics/smoke_validator_test.cpp
    - .planning/phases/05-minimal-ast-and-semantic-smoke/05-01-EXECUTION.md
  modified:
    - include/veyr/parsing/parser.hpp
    - CMakeLists.txt
    - tests/CMakeLists.txt
    - .planning/STATE.md
key-decisions:
  - Follow all sixteen approved Phase 5 decisions and the published experimental M1 boundaries
  - Keep shared syntax AST public while only validation constructs successful semantic payloads
  - Preserve source failures before AST relationships and complete digit conformance before range rejection
  - Defer AST-01/02 and SEMA-01/02 acceptance until remaining Phase 5 verification and explicit owner acceptance
patterns-established:
  - Return original checked SourceError or inspectable AstError without adding a language diagnostic
  - Emit one fixed full-literal range Error and gate successful output on persistent accumulated Error state
  - Preserve all three original-byte spans without borrowing tokens or input AST storage
requirements-completed: []
requirements-delivered: [AST-01, AST-02, SEMA-01, SEMA-02]
requirements-acceptance-scope: Implemented baseline plan scope only; all four requirements remain Pending across 05-02, 05-03 and final phase acceptance
requirements-completion-exception: Owner-directed cross-plan deferral; no generic requirements.mark-complete or phase.complete operation
coverage:
  - id: C1
    description: One source-only three-span AST preserves the accepted parser contract and original snapshot resolution
    requirement: AST-01
    verification:
      - kind: unit
        ref: "tests/ast/smoke_entry_test.cpp#SmokeProfileAst; four named cases, accepted local logs and all four jobs of run 39678721428"
        status: pass
    human_judgment: false
  - id: C2
    description: Shared AST boundary and restricted owning success expose no public arbitrary construction or mutable payload aliases
    requirement: AST-02
    verification:
      - kind: unit
        ref: "tests/semantics/smoke_validator_test.cpp#SmokeProfileSemanticValue and LeadingZerosAreDecimal; compile-time traits and runtime copy/move/value isolation, run 39678721428"
        status: pass
    human_judgment: false
  - id: C3
    description: Decimal zero through 255, leading zeros and long literals use complete ASCII checking and bounded arithmetic
    requirement: SEMA-01
    verification:
      - kind: unit
        ref: "tests/semantics/smoke_validator_test.cpp#AcceptsZero, Accepts255, LeadingZerosAreDecimal, LongLeadingZerosRemainInRange, Rejects256WithExactError and HugeDecimalUsesFixedError; local/four-job hosted execution"
        status: pass
    human_judgment: false
  - id: C4
    description: Checked failures and exact range diagnostics preserve prior state and prevent partial or accumulated-Error success
    requirement: SEMA-02
    verification:
      - kind: unit
        ref: "tests/semantics/smoke_validator_test.cpp#UnknownSourcePreservesPayload, InvalidRelationshipIsChecked, MalformedDigitsAreChecked, PriorErrorBlocksValidOutput, PriorErrorDoesNotSuppressRangeError, WarningsAndNotesPermitSuccess and exact range tests; run 39678721428"
        status: pass
    human_judgment: false
duration: "Active work not measured; 68.946830 seconds summed across 29 retained local commands"
implementation-completed: 2026-10-21
completed: 2026-10-22
formal-completion-recorded-at: "2026-10-22T09:48:24.303525+00:00"
status: complete
---

# Phase 5 Plan 01: Shared AST and checked semantic smoke Summary

**One shared original-byte AST feeds a checked decimal validator that owns complete validated values and rejects malformed, out-of-range or accumulated-Error inputs.**

## Acceptance boundary

The owner explicitly authorized formal Plan 05-01 completion and this four-file
publication. Formal completion was recorded at 2026-10-22T09:48:24.303525+00:00. The accepted
off-discovery preparation packet remains immutable; this timestamp records the
actual lifecycle operation, not implementation or hosted execution time.

Tasks 1–2 and the independent implementation/evidence review were owner-accepted
before publication. The exact ten-file implementation was published in
`aab44db8fc63749d5186d79b516412438b8bd1a1`. Hosted [run 39678721428](https://github.com/m4rkens/VeyrLang/actions/runs/39678721428)
completed successfully in Ubuntu/Windows Debug/Release on that exact SHA.
The final job completed at 2026-10-21T21:47:58Z; this is hosted completion, not a
formal plan-completion timestamp. Existing planning STATE/execution wording is a
historical implementation checkpoint, reconciled by the accompanying proposal.

Formal closeout establishes Phase 5 at 1/3 plans complete and written
plans are 19/21 complete. Plans 05-02 and 05-03 remain unstarted. Mechanical readiness
does not authorize execution. Whole Phase 5 and its four requirements are not accepted.

**Requirement-completion exception:** AST-01, AST-02, SEMA-01 and SEMA-02 remain
Pending. requirements-delivered records baseline functionality, not final acceptance;
remaining exhaustive conformance, real integration and canonical whole-phase
verification are still planned. Do not copy these IDs into requirements-completed
or invoke generic requirement completion. Global counts remain 30 accepted / 19 pending.

## Performance

- **Started:** 2026-10-21T17:40:28.810802+00:00 (retained execution entry).
- **Implementation validation:** Final include check completed at 2026-10-21T21:03:06.947569+00:00.
- **Duration:** Active work unmeasured; 68.946830 seconds summed over 29 retained command receipts, including two deliberate RED checks and one sandbox-boundary configure stop.
- **Formal completion:** 2026-10-22T09:48:24.303525+00:00; not backdated to implementation or hosted execution.
- **Tasks:** 2 implementation tasks; blocking owner implementation/evidence checkpoint accepted separately.
- **Files:** 8 technical paths plus execution and STATE bookkeeping (10 implementation-commit paths). This proposed closeout has four planning paths.

## Accomplishments

- Moved the unchanged ParsedSmokeEntry definition into the source-only shared AST
  header; parser namespace, fields, equality, API and implementation behavior remain
  unchanged. No duplicate AST, token borrow or general expression framework exists.
- Added synchronous validate_smoke with exact SourceError, AstError,
  SemanticRejected and privately constructed ValidatedSmokeEntry alternatives.
  Default result construction is rejection; success owns the complete AST and int32
  constant, with copy/move support and by-value accessors.
- Implemented source entry/return/integer validation before common identity,
  nonempty/strictly nested spans, checked text, complete ASCII decimal conformance,
  inclusive 0–255 range and final accumulated Error-state gate. A late nondigit
  outranks a known excessive numeric prefix. No whole-input host conversion is used.
- Added four AST, four value/API and twelve semantic baseline cases, including
  65,536 leading zeros plus 42, 65,536 nines, exact diagnostics, original errors,
  owning history, runtime copy/move and detached getter-copy mutation.

Exact range diagnostic: `return value must be between 0 and 255 in M1 Experimental Smoke Profile`.
It is one explicitly initialized Error over the full literal with no related spans.
Prior diagnostics and Error state persist; valid current input with earlier Errors
returns BlockedByErrors without emission. Source/AST failures add no language Error.

## Task Commits

1. **Task 1: Shared AST/public semantic boundary and value tests** — `aab44db8fc63749d5186d79b516412438b8bd1a1`.
2. **Task 2: Complete validator and baseline behavior** — the same coherent owner-approved implementation commit.

Tests-first RED/GREEN logs are retained; separate TDD commits were not made because
repository policy requires owner-reviewed stage publication. No task/metadata
commit was fabricated. Formal closeout publication is explicitly authorized for this commit; actual
remote confirmation is retained separately.

## Files Created/Modified

- Created: `include/veyr/ast/smoke_entry.hpp` — exact common three-span definition.
- [parser.hpp](../../../include/veyr/parsing/parser.hpp) — includes common AST; no other public declaration changed.
- Created: `include/veyr/semantics/smoke_validator.hpp` — checked outcomes and restricted owned success.
- Created: `src/semantics/smoke_validator.cpp` — complete ordered decimal validation.
- Created: `tests/ast/smoke_entry_test.cpp` — four source/value and independent-header cases.
- Created: `tests/semantics/smoke_validator_test.cpp` — four value and twelve semantic cases.
- Root/tests CMake — existing veyrcore registration and policy/PRE_TEST ast_tests/semantic_tests.
- [05-01-EXECUTION.md](05-01-EXECUTION.md) and STATE — factual checkpoints; no unrelated technical change.

## Validation and hosted evidence

| Check | Linux Debug | Linux Release | Each Ubuntu/Windows hosted job |
|-------|-------------|---------------|---------------------------------|
| Local doctor | 22/22 | 22/22 | required hosted doctor passed |
| New tests discovered/executed | 20/20 | 20/20 | 20/20 |
| Retained parser identities | 142/142 | 142/142 | 142/142 |
| Selected local regressions | 235/235 | 235/235 | complete historical set in full suite |
| Full CTest | 581/581 | 581/581 | 581/581 |
| Strict verification CTest | 581/581 | 581/581 | 581/581 |
| Prepared acceptance CTest | not newly rerun locally | not newly rerun locally | 581/581 |
| Compiler/directory/C++23 entries | 19/19 | 19/19 | 19/19 |
| Includes | PASS | PASS | PASS |

All 561 prior full-suite identities are preserved. Hosted CTest Start indices
1–581 match exact Passed identities in all three stages; no separate hosted
ctest -N command is claimed. Artifact compilation databases independently include
all three new TUs plus existing parser TUs under C++23 in native and prepared trees.
All jobs passed configure/build, integrity, foundation prepared and controlled
isolated empty-cache rejection exactly once without acquisition/retry. Optional
snapshot save/confirmation skips did not bypass acceptance.

Ignored evidence: `.veyr-dev/execution/05-01/` (74-entry original manifest and 29
command-log hashes); `.veyr-dev/execution/05-01/closeout-preparation/hosted-acceptance.json`, full run log,
four artifact sets, current packet manifest and immutable archive mappings.
Original 04-01/02/03, Phase 2/3, discussion/planning and held-candidate evidence
remain preserved through exact historical hash resolution.

## Decisions Made

No new language or architectural decision. Followed D-01–D-16 and the approved
05-01 contract. Source resolution still requires the live originating manager;
manager-local numeric IDs do not certify foreign provenance. Caller-AST checks do
not reparse surrounding syntax. Success gates current engine state and does not
certify Errors emitted later. Ordinary C++ allocation exceptions retain existing
policy. No final numeric semantics, AST framework, ABI, LLVM or lowering is added.

## Deviations from Plan

The published plan lists nine files. Required canonical STATE execution/session
bookkeeping is the tenth path; it was independently reviewed and explicitly
approved for publication. Supported bounded STATE reconciliation restored curated
milestone/plan counts after helper inference. No unplanned technical file changed.
Inline GSD execution was recorded honestly; no independent executor-agent claim.

## Issues Encountered

- Two expected tests-first RED checks: missing new headers, then undefined validator.
- Initial configure could not create a shared-cache lock inside the sandbox;
  permitted offline rerun passed with existing pinned packages and no dependency
  build/download. Normal Conan bookkeeping was allowed.
- Generated Conan quoting warnings and action-runtime deprecation warnings are
  retained, pre-existing/non-blocking. The four error lines in hosted output are
  the required isolated empty-cache rejection, not failed native validation.
- Windows baseline.json artifact input hash uses CRLF checkout; the exact hash
  matches that representation. Force-LF recipes, profiles and all locks match
  canonical bytes. This is not a dependency identity or policy change.

No outstanding implementation defect or evidence blocker remains.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness

The preceding implementation/hosted prerequisites for 05-02 are satisfied. Formal
05-01 closeout is recorded here. After publication, a fresh
`$gsd-execute-phase 5 --wave 2` authorization remains required; do not execute it
automatically. Plan 05-02 adds 49 exhaustive
structural/precedence/decimal/diagnostic cases; 05-03 adds 16 real-component
lifetime/rendering cases. Those results are not claimed here. Final phase-goal
verification and owner requirement acceptance remain pending.

Platform impact:
- Windows: hosted Debug/Release passed; developer-host inspection independently pending/non-blocking. Existing CRT/profile/locks unchanged.
- Linux: independent Omarchy Debug/Release plus Ubuntu hosted Debug/Release passed.
- macOS future: deferred; no Darwin acceptance or final ABI claimed.

## Self-Check: PASSED

Implementation commit and all created technical paths exist; retained local and
four-job hosted identities/results are reconciled. Canonical summary schema,
coverage and self-check command receipts are stored in the closeout packet. This
self-check does not authorize formal lifecycle mutation or next-wave execution.
