---
phase: 02-source-identity-spans-and-diagnostics
plan: "01"
subsystem: source
tags: [cpp23, source-manager, snapshots, byte-spans, checked-results, googletest]
requires:
  - phase: "01"
    provides: Accepted C++23/Conan/CMake/Ninja foundation and existing GoogleTest packages
provides:
  - Owned immutable source snapshots with stable lifetime-bound views
  - Fresh manager-local SourceId values and exact caller display metadata
  - Checked half-open original-byte spans with distinct inspectable failures
  - Independent Linux Debug/Release native regression and integration evidence
affects: ["02-02", "02-03", "02-04", "03"]
tech-stack:
  added: []
  patterns: ["heap-stable immutable source records", "local variant result aliases", "checked byte ranges"]
key-files:
  created:
    - include/veyr/source/source_id.hpp
    - include/veyr/source/source_span.hpp
    - include/veyr/source/source_manager.hpp
    - src/source/source_manager.cpp
    - tests/source/source_manager_test.cpp
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-01-EXECUTION.md
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-01-SUMMARY.md
  modified: [CMakeLists.txt, tests/CMakeLists.txt, .planning/STATE.md, .planning/ROADMAP.md]
key-decisions:
  - Follow settled D-01 through D-08 and the reviewed Plan 02-01 API without reopening semantics
  - Close the available Linux developer scope only; Windows Phase 2/native evidence remains pending and non-blocking
  - Preserve separate Windows developer-host inspection and defer macOS
patterns-established:
  - Immutable owned records retain text/name views across registration and container growth
  - Checked validation precedence is unknown source, reversed range, then beyond source size
  - Source infrastructure has no LLVM/driver/filesystem dependency
requirements-completed: []
requirements-covered: [SRC-01, SRC-02, SRC-03]
coverage:
  - id: S1
    description: Owned immutable snapshots, fresh manager-local IDs and exact display metadata
    requirement: SRC-01
    verification:
      - kind: unit
        ref: "tests/source/source_manager_test.cpp — ownership, fresh registration, manager-local IDs, 3000-source view stability and metadata cases; Debug/Release passed"
        status: pass
    human_judgment: false
  - id: S2
    description: Checked half-open byte spans retain distinct invalid-input payloads
    requirement: SRC-02
    verification:
      - kind: unit
        ref: "SourceManager.ValidEmptyFullAndEofSpans, DistinctFailuresRetainOriginalInputs, ExtremeRangesFailClosedWithDeterministicPrecedence and RegistrationCapacityKeepsIdsAndEofColumnsRepresentable — Debug/Release passed"
        status: pass
    human_judgment: false
  - id: S3
    description: Span retrieval preserves every original byte, including NUL and split CRLF
    requirement: SRC-03
    verification:
      - kind: unit
        ref: "SourceManager.EveryInBoundsByteRangePreservesOriginalData and RangesMaySplitCrlfWithoutNormalization — Debug/Release passed"
        status: pass
      - kind: integration
        ref: "02-01-EXECUTION.md — each Linux configuration passed full CTest 47/47 including include-policy and six-entry C++23 verification"
        status: pass
    human_judgment: false
  - id: S4
    description: Independent Windows Phase 2/native acceptance
    verification:
      - kind: integration
        ref: "Pending; Linux evidence and historical Phase 1 CI are not credited as native Windows Phase 2 validation"
        status: unknown
    human_judgment: true
    rationale: "Future Windows CI/native evidence remains pending and non-blocking under the owner-approved Linux-scope closeout"
duration: "Execution and owner-gated closeout on 2026-10-09; no fabricated elapsed metric"
completed: 2026-10-09
status: complete
completion_scope: available-linux-developer-only
---

# Plan 02-01 — Owned source snapshots and checked spans

**SourceManager owns immutable snapshots and stable views, assigns fresh local
IDs, and checks original-byte spans with distinct failures in Debug and Release.**

## Accomplishments

Task 1 implements the reviewed SourceId/SourceSpan/SourceManager API using local
standard-library variants and fully constructed immutable heap records. Caller
text/name storage may be changed or destroyed after registration. Duplicate names
and bytes still produce fresh IDs; names are exact diagnostic metadata, with no
filesystem lookup or normalization. Manager copy/move is disabled to retain the
reviewed lifetime contract.

Task 2 extends focused coverage to twelve tests: short/long/empty view stability
through 3,000 registrations, duplicate snapshots, manager-local IDs, exact metadata,
all bounded valid byte ranges, empty/interior/EOF spans, embedded NUL/high bytes,
CRLF splits, unknown/maximal IDs and offsets, deterministic error precedence and
numeric representability checks without enormous allocations. Invalid input
returns checked UnknownSource/ReversedRange/BeyondSource payloads in Release as
well as Debug; no clamping, assertion-only check or fabricated text is used.

The existing veyrcore target gains source_manager.cpp. The source_tests target
uses the established target policy and GoogleTest/GoogleMock with PRE_TEST
discovery; it has no LLVM target link dependency. No generic framework, allocator,
package subsystem, language syntax, source-location mapper or diagnostic renderer
is introduced.

## Accepted execution evidence

| Linux developer check | Debug | Release |
|-----------------------|-------|---------|
| Configure/build | Passed | Passed |
| Focused SourceManager tests | 12/12 | 12/12 |
| Full CTest including include-policy | 47/47 | 47/47 |
| Selected C++23 compilation entries | 6/6 | 6/6 |

Read-only Linux prerequisite doctor passed 22/22. The selected Omarchy/Arch build
fingerprint is 3543ddbc0bf7. Exact command selection, sequence and evidence paths
are recorded in 02-01-EXECUTION.md. Successful native checks were not rerun during
closeout: implementation is unchanged and the phase-gate procedure requires no
repeat. Existing final CTest logs and compilation databases were inspected.

Existing LLVM 23.1.2 and GoogleTest packages were reused through cache-only
generation (`--no-remote --build=never`). No dependency downloads/rebuilds, Conan
CLI installation, cache purge or automatic retries occurred. All four locks are
byte-identical to HEAD. Conan 2.33.0, recipes, profiles, baselines, workflows,
dependency versions and accepted Phase 1 behavior remain unchanged. Existing
non-blocking generated-toolchain quotation warnings are retained.

## Decisions and deviations

None beyond the reviewed plan and explicit owner scope approval. D-01 through
D-08 are preserved. Ordinary C++ allocation exceptions remain ordinary C++
failures; no language OOM policy is invented. Capacity checks leave headroom for
the later one-based EOF mapping without implementing Plan 02-02.

The owner explicitly authorized one coherent implementation/closeout commit and
`git push origin main`; tasks are not split into separate commits. The checkpoint
contains the reviewed ten-file execution packet plus this required summary
(eleven files). Resolve its commit hash from Git history; no self-referential
commit hash is fabricated in this artifact.

## Completion scope and remaining gates

Owner-approved Plan 02-01 completion is limited to the validated Linux developer
scope. Phase 2 is now 1/4; Phase 1 remains closed at 8/8. Requirements SRC-01,
SRC-02 and SRC-03 have local evidence, but REQUIREMENTS.md and phase-wide
acceptance remain pending. No dedicated GSD verification stage is claimed.

Windows Phase 2/native evidence remains pending and non-blocking. This is separate
from the independently pending Windows developer-host inspection; neither is
inferred from Linux runs. Any automatically triggered Build foundation run after
publication is reported only; no manual dispatch/retry/duplicate is authorized.
macOS remains deferred.

Platform impact:
- Windows: Standard C++ byte/span API and portable tests; native Phase 2 evidence pending/non-blocking, developer-host inspection independently pending.
- Linux: Primary Omarchy developer host; Debug/Release checks accepted independently; Ubuntu CI remains a separate environment.
- macOS future: Deferred; no POSIX/Win32, filesystem or console dependency introduced.

Plan 02-02 remains unstarted. Stop for owner review after this checkpoint is
published; closeout and commit do not authorize the next plan or phase.
