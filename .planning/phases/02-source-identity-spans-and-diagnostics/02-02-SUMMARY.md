---
phase: 02-source-identity-spans-and-diagnostics
plan: "02"
subsystem: source
tags: [cpp23, byte-locations, newline-boundaries, checked-results, googletest]
requires:
  - phase: "02-01"
    provides: Owned immutable source snapshots, fresh manager-local IDs and checked spans
provides:
  - Checked one-based original-byte source locations
  - Immutable LF-derived line index and checked containing-line spans
  - Exact approved LF/CRLF/lone-CR and EOF boundary behavior
  - Independent Linux Debug/Release regression and integration evidence
affects: ["02-03", "02-04", "03"]
tech-stack:
  added: []
  patterns: ["immutable per-snapshot line index", "checked point lookup", "bounded byte-scanning test oracle"]
key-files:
  created:
    - include/veyr/source/source_location.hpp
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-02-EXECUTION.md
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-02-SUMMARY.md
  modified:
    - include/veyr/source/source_manager.hpp
    - src/source/source_manager.cpp
    - tests/source/source_manager_test.cpp
    - .planning/STATE.md
    - .planning/ROADMAP.md
key-decisions:
  - Preserve settled D-01/D-04/D-06/D-07/D-09–D-12 and the reviewed Plan 02-02 API
  - Close validated Linux developer scope only; Windows native Phase 2 evidence remains pending/non-blocking
  - Keep Windows developer-host inspection independent and macOS deferred
patterns-established:
  - Build the complete line index before publishing an immutable source snapshot
  - Checked point lookup retains original ID/offset and permits EOF
  - Containing-line spans preserve terminator bytes without text normalization
requirements-completed: []
requirements-covered: [SRC-04]
coverage:
  - id: L1
    description: Exact byte locations and containing-line spans across newline and EOF boundaries
    requirement: SRC-04
    verification:
      - kind: unit
        ref: "SourceLocation.* — eight tests; Debug/Release passed independently"
        status: pass
    human_judgment: false
  - id: L2
    description: Original bytes, metadata and lifetime contracts survive indexed lookup
    requirement: SRC-04
    verification:
      - kind: unit
        ref: "Byte-scanning oracle covers all offsets of 2801 bounded byte strings; metadata and 3000-registration lifetime regressions passed"
        status: pass
      - kind: integration
        ref: "02-02-EXECUTION.md — each Linux configuration passed source/location 20/20, full CTest 55/55 and six-entry C++23 verification"
        status: pass
    human_judgment: false
  - id: L3
    description: Independent Windows native Phase 2 acceptance
    verification:
      - kind: integration
        ref: "Pending/non-blocking; no Windows Phase 2 result inferred from Linux or historical Phase 1 CI"
        status: unknown
    human_judgment: true
    rationale: "Separate hosted/native evidence remains pending under owner-approved Linux-scope closeout"
duration: "Execution and owner-gated closeout on 2026-10-09; no fabricated elapsed metric"
completed: 2026-10-09
status: complete
completion_scope: available-linux-developer-only
---

# Plan 02-02 — Byte locations and newline boundaries

**SourceManager maps checked original-byte offsets to one-based locations and
containing-line spans with the approved LF/CRLF/EOF behavior.**

## Accomplishments

Task 1 adds SourceLocation value equality, local LocationResult/SpanResult
variants and checked location/line_span_at APIs. Each immutable source record
receives a fully constructed LF-derived index before publication. Binary-search
lookup treats LF and both CRLF bytes as part of the preceding line, advances once
after LF, and treats lone CR as ordinary data. Empty EOF is 1:1; trailing-break
EOF is next-line:1; other EOF is one byte beyond final contents. Line spans
include original terminators; trailing-break EOF has an empty final-line span.

Unknown sources and out-of-bounds offsets fail explicitly with exact point
payloads in Release as well as Debug. Existing structural span validation,
snapshot ownership, fresh manager-local identity and stable text/name views are
unchanged. Caller display names stay byte-exact metadata, including Windows/UNC/
relative spelling and control bytes. No filesystem operation, Unicode decoding,
visual-width policy, lexer rule or diagnostic rendering is introduced.

Task 2 adds an independent scanning oracle for every offset of all 2,801 strings
up to four bytes over a seven-byte alphabet, mixed LF/CRLF/lone-CR/tab/UTF-8/NUL
fixtures, exact metadata tests and indexed view stability through 3,000 additional
registrations. Eight new SourceLocation tests join the unchanged twelve
SourceManager tests in the existing target. No target, dependency, LLVM/driver
coupling or generic framework was added.

## Accepted Linux developer evidence

| Check | Debug | Release |
|-------|-------|---------|
| Configure/build state | Cache-only configure and incremental build passed | Existing configuration and incremental build passed |
| SourceManager/SourceLocation | 20/20 | 20/20 |
| Full CTest including include-policy | 55/55 | 55/55 |
| Selected C++23 compilation entries | 6/6 | 6/6 |

Read-only Debug doctor passed 22/22; build/verify also enforced prerequisites.
Exact selected commands, initial four-case Task 1 gates and final evidence are
recorded in 02-02-EXECUTION.md. The initial sandbox denied the Conan operation
lock before launching package/native work; the same authorized command succeeded
with escalated cache access. No actual native/package failure was retried.

Existing LLVM 23.1.2/GoogleTest packages were reused; Debug stale generation used
`--no-remote --build=never`. No downloads, dependency rebuilds, CLI installation,
acquisition setup, lock generation or cleanup occurred. Existing generated Conan
toolchain quotation warnings remain non-blocking. All four locks, profiles,
recipes, baselines, workflows, scripts and dependency versions are unchanged.

## Owner-approved closeout

The owner approved Plan 02-02 closeout for validated Linux developer scope and
one dedicated execution checkpoint commit followed by `git push origin main`.
The reviewed seven-file packet plus this required summary totals eight files.
The four implementation/test files remain byte-identical to their reviewed
versions. Completed planning and 02-01 records are preserved; the ignored local
CMakeUserPresets.json IDE binding remains unchanged and outside lifecycle scope.

No procedure requires repeating accepted checks for unchanged code. Closeout
inspected the existing final CTest logs and compilation databases and ran no
additional builds/tests or dependency operations. No dedicated GSD verification
stage or hosted Phase 2 acceptance is claimed. No plan deviation or settled
discussion decision was reopened.

Phase 2 is now 2/4; Phase 1 remains closed at 8/8. SRC-04 has accepted local
coverage alongside source/span regressions; all seven Phase 2 requirements remain
pending in REQUIREMENTS.md until phase-wide acceptance. Windows native Phase 2
evidence remains pending/non-blocking, independently of pending Windows
developer-host inspection. macOS remains deferred.

Platform impact:
- Windows: Same portable byte/location contracts and tests; native Phase 2
  evidence pending/non-blocking, developer-host evidence independently pending.
- Linux: Primary Omarchy developer host; Debug/Release evidence accepted
  independently. Ubuntu CI remains a separate environment.
- macOS future: Deferred; no required filesystem, POSIX, Win32 or terminal API.

Resolve this checkpoint's hash from Git history. Observe any push-triggered Build
foundation run only; no manual dispatch, retry or duplicate is authorized.
Plan 02-03 remains unstarted. Stop for owner review after publication.
