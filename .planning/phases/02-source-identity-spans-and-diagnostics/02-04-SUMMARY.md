---
phase: 02-source-identity-spans-and-diagnostics
plan: "04"
subsystem: diagnostics
tags: [cpp23, source-spans, plain-text, escaping, bounded-excerpts, native-ci]
requires:
  - phase: "02-01"
    provides: Owned immutable snapshots and checked manager-local byte spans
  - phase: "02-02"
    provides: Checked byte locations and containing-line spans
  - phase: "02-03"
    provides: Owned structured diagnostics and ordered severity accumulation
provides:
  - Stateless checked plain-text diagnostic rendering with owned output
  - Byte-safe escaping, bounded excerpts and byte-to-cell marker mapping
  - Native source-to-diagnostic integration evidence on four Tier 1 CI rows
  - Owner-accepted Plan 02-04 and complete Phase 2 foundation
tech-stack:
  added: []
  patterns: [primary-first checked resolution, owned rendering output, fixed-size byte-to-cell mapping]
key-files:
  created:
    - include/veyr/diagnostics/plain_text_renderer.hpp
    - src/diagnostics/plain_text_renderer.cpp
    - tests/diagnostics/plain_text_renderer_test.cpp
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-04-EXECUTION.md
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-04-SUMMARY.md
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-VERIFICATION.md
  modified: [CMakeLists.txt, tests/CMakeLists.txt, .planning/STATE.md, .planning/ROADMAP.md, .planning/REQUIREMENTS.md, .planning/PROJECT.md]
key-decisions:
  - Preserve all sixteen settled Phase 2 discussion contracts and previously accepted public APIs
  - Validate all primary/related source references before successful output
  - Keep byte locations distinct from escaped display cells and cap source excerpts before allocation
  - Accept actual Ubuntu/Windows Debug/Release evidence separately from Linux developer evidence
  - Keep Windows developer-host inspection independently pending and macOS deferred
requirements-completed: [DIAG-02]
phase-requirements-accepted: [SRC-01, SRC-02, SRC-03, SRC-04, DIAG-01, DIAG-02, DIAG-03]
coverage:
  - id: R1
    description: Checked owned rendering, optional primary and ordered related locations
    requirement: DIAG-02
    verification:
      - kind: unit
        ref: "PlainTextDiagnostic.* — exact headers, ownership, primary/related precedence and original SourceError payload/index; Linux Debug/Release and four hosted jobs passed"
        status: pass
    human_judgment: false
  - id: R2
    description: Hostile bytes, newline/EOF mapping and bounded source markers
    requirement: DIAG-02
    verification:
      - kind: unit
        ref: "27 renderer cases including all 256 byte values, split CRLF, million-byte line and independent clipping/cell bounds; all native configurations passed"
        status: pass
    human_judgment: false
  - id: R3
    description: Complete source-to-diagnostic slice preserves accumulation state
    verification:
      - kind: integration
        ref: "EngineIntegrationKeepsOrderDuplicatesAndErrorsAfterRenderFailure; current full suite contains 12 source, 8 location, 14 diagnostic and 27 renderer tests"
        status: pass
      - kind: integration
        ref: "Run 38722486719 at ab596c7c721fd673b1716db6a1f6f5681a7ff126 — all four jobs successful, CTest 96/96, ten C++23 entries, include/prepared/offline gates passed"
        status: pass
    human_judgment: false
status: complete
completed: 2026-10-15
completion_scope: reviewed-linux-developer-debug-release-and-ubuntu-windows-ci-debug-release
acceptance_run: "38722486719"
tested_commit: "ab596c7c721fd673b1716db6a1f6f5681a7ff126"
duration: "Owner-gated implementation, reviews and closeout on 2026-10-15; no elapsed metric fabricated"
---

# Plan 02-04 — Bounded plain diagnostics and Phase 2 acceptance

**The owner accepted Plan 02-04 and Phase 2: four plans complete, all seven
requirements accepted. Final four-job evidence is successful. This documentation-
only closeout is uncommitted and awaits owner review/publication; Phase 3 is
unstarted.**

## Completed implementation and public API

PlainTextRenderer::render takes a const Diagnostic and its originating const
SourceManager, returning variant<string, RenderError>. Success owns LF-terminated
ASCII text. RenderError owns the original checked SourceError and optional
related index; absent index identifies the primary. The renderer is stateless,
synchronous and retains neither source nor diagnostic views beyond the call.

Primary validation precedes every ordered related span before output construction.
Headers, lowercase severities, optional primary context and related-only locations
match the plan. Invalid references yield exact infrastructure errors without
clamping, fabricated data, dropped related context or partial successful output.

Printable ASCII, including literal path backslashes, is preserved. TAB/CR/LF
become visible escapes; other controls, DEL and high bytes use uppercase hex.
Locations remain original byte coordinates. Excerpts are limited to three rows,
80 original bytes and 320 escaped content cells per row; an 81-boundary map
aligns ASCII markers. Slicing and marker bounds precede allocation. Empty spans,
EOF, lone CR, LF/CRLF and exclusive next-line ends follow the accepted contracts.
Names/messages and related counts are not globally capped by the excerpt policy.

Production/test sources join existing veyrcore/diagnostic_tests. No new library,
framework, filesystem/terminal API, driver integration or LLVM dependency was
introduced. SourceManager/Diagnostic/DiagnosticEngine APIs remain unchanged.
Source snapshots survive registrations; engine collection views invalidate on
emit, so callers keep views valid during rendering and reacquire afterward.
Owned output survives input destruction; render failures preserve engine Error
state, emission order and duplicate observations. Compiler clients must continue
to use has_errors independently of rendering; driver exit integration is later
scope, as explicitly planned.

## Accepted Linux developer evidence

| Check | Debug | Release |
|-------|-------|---------|
| Read-only doctor | 22/22 | 22/22 |
| Explicit cache-only configure / project build | Passed / passed | Passed / passed |
| PlainTextDiagnostic | 27/27 | 27/27 |
| SourceManager / SourceLocation / Diagnostic | 12/12 + 8/8 + 14/14 | 12/12 + 8/8 + 14/14 |
| Full CTest in test and verify | 96/96 | 96/96 |
| Selected compiler/directory/C++23 entries | 10/10 | 10/10 |
| Direct include-policy verification | Passed | Passed |

Existing LLVM/GoogleTest packages were reused through --no-remote --build=never
configuration. No local dependency download/rebuild, lock regeneration, Conan
CLI reinstall or cleanup occurred. Existing generated-toolchain quotation
warnings remained non-blocking. Omarchy and Ubuntu baselines stay distinct.

## Final hosted native evidence

[Build foundation run 38722486719](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719)
completed successfully at ab596c7c721fd673b1716db6a1f6f5681a7ff126. Actual job logs
were inspected independently before owner acceptance.

| Job / ID | Doctor | Renderer | Full CTest | C++23 | Include / prepared native / controlled offline | Final |
|----------|--------|----------|------------|-------|-----------------------------------------------|-------|
| [Ubuntu Debug / 116380189518](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189518) | 22/22 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |
| [Ubuntu Release / 116380189555](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189555) | 22/22 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |
| [Windows Debug / 116380189082](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189082) | 20/20 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |
| [Windows Release / 116380189595](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189595) | 20/20 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |

Normal test, verify and prepared/native paths each ran the full 96-test suite.
Each job has 27 unique renderer cases passing in all three paths, not 81 unique
cases. Source 12, location 8 and diagnostic 14 cases also passed each time.
The controlled isolated empty-cache rejection is expected success, observed once
without acquisition/retry; it is not a normal dependency/cache failure.

All jobs passed explicit setup/integrity/export/early save and native configure/
build/test/verify. Three archive imports passed; Windows Release import was
skipped. Explicit setup and native validation still passed there. No skipped
step/cache hit is credited as acceptance; no claim that every package was reused
or that new cold-build capacity was established is made.

## Evidence integrity and architecture assessment

See 02-04-EXECUTION.md for exact commands, job IDs, gates and chronology.
Ignored .veyr-dev/execution/02-04/ contains 16 raw command logs, twelve copied
generated records, complete implementation.diff, initial-file-hashes.json,
REVIEW.md and the manifest, which hashes 36 entries including implementation.
All accepted entries match; code and evidence were not modified during closeout.

Earlier 02-01/02-02 evidence used replaceable build-tree records. Twelve dedicated
02-03 logs remain valid, while five old generated-build references were superseded.
Current local snapshots and final four-job CI independently cover the whole
accepted source/diagnostic slice. Historical summaries retain their original
scope; they are not rewritten to manufacture retrospective results.

Architecture/security review found no blocker: ownership boundaries are explicit,
source resolution remains checked, ordering is deterministic, hostile controls
are escaped, and excerpt/marker resource use is bounded independently of span
size. Registration/emission/rendering exception safety was inspected structurally;
allocation failures were not fault-injected and no sanitizer/latency SLA is claimed.

## Non-blocking follow-up candidates

- Enforce explicit initialization of Diagnostic::severity before construction
  becomes widespread. Current callers must initialize before reading. No
  arbitrary default, constructor change or new policy is introduced here.
- Add a combined left-clipping, mixed-escape and empty-span regression test.
- Consider documenting/testing unsupported initialized severity values, which
  currently throw std::invalid_argument; this cannot repair an uninitialized read.

No follow-up was implemented during closeout. Ordinary C++ allocation exceptions
remain ordinary exceptions; there is no Veyr language OOM decision. Manager-local
IDs require the originating manager; foreign numeric alias detection is not
promised. Literal backslashes remain printable, so escapes are presentation,
not reversible encoding. These accepted limits do not block the phase.

## Final acceptance and publication boundary

Task 3 is complete after independent four-job evidence review and explicit owner
acceptance. Requirements SRC-01 through SRC-04 and DIAG-01 through DIAG-03 are
accepted. Plan 02-04 is complete, Phase 2 is 4/4, Phase 1 stays 8/8. The required
02-VERIFICATION.md now records the owner-authorized installed GSD 1.6.1
verify_phase_goal procedure actually performed inline: artifacts/substance/wiring,
the union of roadmap and all plan must-haves, requirement traceability, anti-patterns
and named behavioral results from unchanged native logs. Twenty deduplicated truths
are verified, with zero behavior-unverified items or overrides. The canonical
verification.status query returned passed. No independent agent or new native test
execution is claimed. Later requirements remain pending.

This closeout changes exactly STATE, ROADMAP, REQUIREMENTS, PROJECT,
02-04-EXECUTION.md and new 02-04-SUMMARY.md / 02-VERIFICATION.md. PROJECT now
distinguishes validated Phase 1/2 capabilities from active later work, records
actual Linux/Windows hosted evidence and retains separate developer-host/macOS
deferrals. This resolves the owner-reviewed lifecycle documentation gap.
Accepted C++/tests/CMake, tooling,
locks/profiles/recipes/baselines/workflows/dependencies, prior artifacts, ignored
IDE binding and raw evidence are unchanged. Nothing staged, committed or pushed.

Next gate: owner review and explicit publication approval for the seven planning
files. Proposed message: docs: close Phase 2 source and diagnostics acceptance.
Phase 3 discussion/research/planning/implementation needs its own owner command;
no new syntax, smoke-profile decision or milestone completion is inferred.

Platform impact:
- Windows: Hosted Debug/Release source/diagnostic/renderer evidence validated.
  Developer-host inspection remains independently pending/non-blocking.
- Linux: Omarchy local Debug/Release and separate Ubuntu CI evidence accepted.
- macOS future: Deferred; no Darwin execution claimed or OS-specific dependency added.
