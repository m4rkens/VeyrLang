---
phase: 05-minimal-ast-and-semantic-smoke
plan: "02"
implementation-commit: 244a2eec0c73005e2ad09fcd513d37c3d8895957
subsystem: semantics
tags: [m1, semantic-conformance, checked-errors, decimal-range, diagnostic-history]
requires:
  - phase: "05-01"
    provides: Shared source-only AST, complete checked semantic validator and twenty accepted baseline tests
  - phase: "04"
    provides: Accepted parser, exact original-byte spans and retained component integration
provides:
  - Eighteen independently executed structural AST failure rows with complete checked payloads
  - Eight combined-fault precedence rows and eighteen decimal/content rows including long literals
  - Five persistent diagnostic-state rows with exact owning history and no invalid success
  - Accepted Linux Debug/Release and four-configuration hosted evidence retaining every previous case
affects: ["05-03", "06"]
tech-stack:
  added: []
  patterns: [literal public-contract oracles, named parameter rows, exact checked alternatives, archive-aware evidence retention]
key-files:
  created:
    - .planning/phases/05-minimal-ast-and-semantic-smoke/05-02-EXECUTION.md
  modified:
    - tests/semantics/smoke_validator_test.cpp
    - .planning/STATE.md
key-decisions:
  - Preserve all sixteen Phase 5 decisions and the accepted Phase 3/4/05-01 production contracts
  - Conformance expectations come from approved contracts and literal fixtures, never implementation tables or numeric conversion
  - Source and structural failures outrank range and prior-Error gating; malformed bytes outrank numeric excess
  - Keep all four AST/SEMA requirements Pending through remaining integration and canonical phase acceptance
patterns-established:
  - Individually named and executed conformance rows assert complete payloads and absence of successful partial output
  - Seed owning Note/Warning/Error history and reacquire diagnostic views after emission
requirements-completed: []
requirements-delivered: [AST-01, AST-02, SEMA-01, SEMA-02]
requirements-acceptance-scope: Plan-level conformance evidence only; all four requirements remain Pending until remaining integration and final phase acceptance
requirements-completion-exception: Owner-directed cross-plan deferral; no generic requirements.mark-complete or phase.complete
coverage:
  - id: C1
    description: Caller-created AST structural misuse returns exact source or AST errors without diagnostics or partial success
    requirement: AST-01
    verification:
      - kind: unit
        ref: "tests/semantics/smoke_validator_test.cpp#SmokeProfileSemanticStructure.ExactCheckedFailure; 18 named rows, Linux Debug/Release and all four jobs of run 39775049283"
        status: pass
    human_judgment: false
  - id: C2
    description: Shared three-span AST and restricted owning success contracts remain intact across the unchanged public API
    requirement: AST-02
    verification:
      - kind: unit
        ref: "tests/ast/smoke_entry_test.cpp#SmokeProfileAst; retained SmokeProfileSemanticValue and successful complete-AST assertions, all 20 baselines in run 39775049283"
        status: pass
    human_judgment: false
  - id: C3
    description: Decimal range and malformed-content outcomes cover boundaries, all in-range values and long decimal input shapes without a conversion oracle
    requirement: SEMA-01
    verification:
      - kind: unit
        ref: "tests/semantics/smoke_validator_test.cpp#SmokeProfileSemanticDecimal.DecimalContentAndRange; 18 named rows, 256 traced in-range subcases and 65536-byte fixtures, run 39775049283"
        status: pass
    human_judgment: false
  - id: C4
    description: Two-fault precedence and persistent diagnostic state preserve complete history and prevent all invalid success
    requirement: SEMA-02
    verification:
      - kind: unit
        ref: "tests/semantics/smoke_validator_test.cpp#SmokeProfileSemanticPriority and SmokeProfileSemanticDiagnostics; 8 priority and 5 diagnostic rows, exact fixed Error, run 39775049283"
        status: pass
    human_judgment: false
duration: "Active work unmeasured; 79.203719 seconds summed across 22 supervised local commands"
implementation-completed: 2026-10-22
completed: 2026-10-22
formal-completion-recorded-at: "2026-10-22T12:44:37.808730+00:00"
status: complete
---

# Phase 5 Plan 02: Independent semantic conformance Summary

**Forty-nine independently named conformance rows prove checked AST failures, deterministic precedence, long decimal handling and persistent diagnostic-state behavior without production changes.**

## Acceptance boundary

Tasks 1–2 passed independent owner review and were explicitly accepted before
publication in `244a2eec0c73005e2ad09fcd513d37c3d8895957`. The three-file execution
commit contains only appended semantic tests, factual execution and STATE
bookkeeping. All accepted baseline source bytes are archived and preserved.

Hosted [run 39775049283](https://github.com/m4rkens/VeyrLang/actions/runs/39775049283)
completed/success on that exact SHA in Ubuntu/Windows Debug/Release. The final job
completed at 2026-10-22T12:25:38Z. This is hosted completion, not formal lifecycle time.
The separately owner-authorized formal closeout records actual completion at
2026-10-22T12:44:37.808730+00:00; no event is invented or backdated.

This plan closeout establishes Phase 5 at 2/3 plans and overall written execution
plans at 20/21. 05-01 remains complete; 05-03 is unstarted. Mechanical readiness does
not authorize Wave 3 execution. Phase 5 as a whole is not yet accepted.

**Requirement-completion exception:** AST-01, AST-02, SEMA-01 and SEMA-02 remain
Pending at 30 accepted / 19 pending. Plan-level functionality/conformance is delivered,
while 05-03 real-component integration and canonical whole-phase verification and
explicit owner acceptance remain. Do not run generic requirement/phase completion.

## Performance

- **Started:** 2026-10-22T09:58:53.919931+00:00 (retained entry).
- **Final local check:** 2026-10-22T10:06:46.057451+00:00.
- **Duration:** Active work unmeasured; 79.203719 seconds summed over 22 supervised commands. Command time is not development or total elapsed time.
- **Formal completion:** 2026-10-22T12:44:37.808730+00:00.
- **Tasks:** 2 automatic tasks; independent owner implementation/evidence checkpoint accepted separately.
- **Files:** One technical test file plus factual execution and required STATE bookkeeping (three implementation paths); four planning paths in this closeout.

## Accomplishments

- Eighteen structural rows cover unknown/reversed/beyond-source spans for all
  fields, known distinct source identities, three empty spans and four touching
  nesting boundaries. Complete SourceError fields and AstError kind/field/span/
  optional byte offset are checked; no invalid result exposes a validated value.
- Eight combined-fault rows independently establish both fault premises and check
  exact priority. Later source errors beat earlier relationship faults; source
  mismatch beats emptiness; emptiness beats nesting; Return relationship errors
  beat Integer errors. Four nesting combinations are traced and counted. Prior
  engine Errors do not mask checked failures.
- Eighteen decimal/content rows cover 0, 1, 254, 255, 256; at least 65,536 digits or
  leading zeros; long zero-prefixed 42/255/256; signs, letters, spaces, NUL, high
  bytes and dots. A 65,536-nine prefix followed by x gives NonDecimalLiteral at
  original offset 65564 without a range diagnostic. A traced/counted subloop also
  checks every value 0..255. These subcases are not additional CTest identities.
- Five diagnostic-state rows prove prior Errors block success, current range
  rejection still appends exactly one initialized Error, Notes/Warnings permit
  success, duplicate Errors preserve order and malformed ASTs preserve every
  owning prior field. Views are reacquired after emissions.

Exact range message: `return value must be between 0 and 255 in M1 Experimental Smoke Profile`.
Severity is Error, primary is the entire original literal (including leading zeros),
and related spans are empty. Expectations use literal public-contract fixtures;
no production tables/helpers or whole-input numeric conversion supply an oracle.

## Task Commits

1. **Task 1: Structural and combined-fault precedence matrix** — `244a2eec0c73005e2ad09fcd513d37c3d8895957`.
2. **Task 2: Decimal/content and diagnostic matrix** — the same coherent owner-reviewed execution commit.

Task 1 passed 46/46 focused cases in both configurations before Task 2 was added.
These conformance tests exercised accepted existing behavior and were immediately
GREEN; no artificial RED or production mutation was introduced. No separate task
or metadata commit is fabricated. Formal closeout publication is explicitly owner-authorized for this commit;
actual remote confirmation is retained separately.

## Files Created/Modified

- Modified: `tests/semantics/smoke_validator_test.cpp` ([source](../../../tests/semantics/smoke_validator_test.cpp)) — 457 appended lines; original 271-line file byte-identical.
- Created during implementation: `.planning/phases/05-minimal-ast-and-semantic-smoke/05-02-EXECUTION.md` ([record](05-02-EXECUTION.md)) — original execution history and closeout reconciliation.
- Modified: `.planning/STATE.md` ([state](../../STATE.md)) — bounded execution/continuity bookkeeping and formal 2/3 proposal.
- Modified: `.planning/ROADMAP.md` ([roadmap](../../ROADMAP.md)) — marks only 05-02 newly complete; leaves 05-03 unstarted.
- Unchanged contract references: `include/veyr/semantics/smoke_validator.hpp` and `include/veyr/ast/smoke_entry.hpp`.

No production, shared AST/parser API, other test, CMake, dependency, lock, profile
or workflow changed. Original local main and held infrastructure candidates remain
isolated. The shared AST and semantic architecture from [05-01-SUMMARY.md](05-01-SUMMARY.md)
remain unchanged.

## Validation and hosted evidence

| Gate | Linux Debug | Linux Release | Each Ubuntu/Windows hosted configuration |
|------|-------------|---------------|-------------------------------------------|
| Local doctor |22/22|22/22|required hosted doctor passed|
| AST/semantic discovered/executed |69/69|69/69|69/69|
| New conformance / retained baseline |49 /20|49 /20|49 /20|
| Retained parser identities |142/142|142/142|142/142|
| Selected local regressions |235/235|235/235|all historical identities retained in full suite|
| Full native CTest |630/630|630/630|630/630|
| Strict CTest |630/630|630/630|630/630|
| Prepared acceptance CTest |not rerun|not rerun|630/630|
| Compiler/directory/C++23 entries |19/19|19/19|19/19|
| Include policy |PASS|PASS|PASS|

Actual hosted CTest Start indices 1..630 and Passed identities match the accepted
local full set in native test, strict verify and prepared acceptance for every
job, including every prior 581 identity. No separate hosted ctest -N command is
claimed. Artifact native/prepared compilation databases each contain all 19
expected TUs with C++23, configuration-specific build directories and reviewed
compilers; no extra source or component was introduced.

All four jobs passed setup/integrity/configure/build, foundation prepared and
isolated acquisition-disabled empty-cache rejection exactly once without retry.
Optional snapshot save/confirmation skips are not native acceptance evidence.
The 140 retained warning lines are the previously observed generated Conan quoting
and Node/action-runtime warnings; they are not 140 distinct defects. Only four
expected no-remote rejection error lines occur; no unexpected native failure.
Windows baseline.json hashes match exact CRLF checkout bytes; force-LF locks,
recipes and profiles match canonical bytes.

Ignored evidence: `.veyr-dev/execution/05-02/` (original 72-entry manifest, 22 command
receipts, exact49-row COVERAGE.md and archive mappings), and
`.veyr-dev/execution/05-02/closeout-preparation/` (full hosted log, four artifacts,
hosted-acceptance.json, canonical checks and prospective publication diff).
All accepted 05-01, earlier Phase 4/2/3 and held-candidate manifests remain immutable.

## Decisions Made

None beyond the approved D-01–D-16 and exact [05-02-PLAN.md](05-02-PLAN.md) contract.
No new grammar, numeric semantics, general AST, ABI, LLVM or lowering is added.
Source resolution still needs the live originating SourceManager; manager-local
numeric SourceIds do not certify foreign provenance. Future integration and
phase-wide acceptance are not inferred from unit-test conformance.

## Deviations from Plan

The plan enumerates the test and execution paths; the owner explicitly permits
and approved mandatory STATE bookkeeping as the third execution path. Installed
helper metadata inference was bounded to preserve accepted milestone/count values.
No technical scope deviation, production repair or independent executor-agent claim.

## Issues Encountered

No unexpected local/native failure or production defect. Prepared pinned
Conan 2.33.0/LLVM 23.1.2/GoogleTest 1.18.0 inputs were reused; no new configure,
package acquisition/build or cache operation was necessary for this test-only plan.
One sandbox-boundary remote SSH check during publication was repeated successfully
as a permitted direct read; it did not affect implementation or validation.
Earlier 05-01 RED/sandbox records are historical and not 05-02 failures.
Known hosted warnings and Windows CRLF representation are retained above.

## User Setup Required

None - no external service configuration required.

## Next Phase Readiness

Formal 05-02 closeout is recorded here. After verified publication, request a fresh
`$gsd-execute-phase 5 --wave 3` for [05-03-PLAN.md](05-03-PLAN.md). It adds sixteen
real-component integration, ownership/lifetime and renderer cases with independent
local evidence. Do not execute it automatically. Canonical Phase 5 verification
and explicit whole-phase/requirement owner acceptance still follow that work.

Platform impact:
- Windows: hosted Debug/Release passed; developer-host inspection independently pending/non-blocking; CRT/profiles/locks unchanged.
- Linux: independent developer Debug/Release and Ubuntu hosted Debug/Release passed.
- macOS future: deferred; no Darwin execution, final language numeric policy or ABI claim.

## Self-Check: PASSED

The actual implementation commit, all referenced source paths, original baseline
bytes, local identities and four-job hosted results are verified. Installed summary
schema/coverage/self-check results and link checks are retained in the review
packet. This check does not itself authorize lifecycle promotion or another wave.
