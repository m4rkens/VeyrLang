---
phase: 02-source-identity-spans-and-diagnostics
plan: "03"
subsystem: diagnostics
tags: [cpp23, owned-diagnostics, severity, ordered-accumulation, googletest]
requires:
  - phase: "02-01"
    provides: Owned immutable source snapshots and checked manager-local spans
  - phase: "02-02"
    provides: Checked byte locations and containing-line spans
provides:
  - LLVM-independent owned structured diagnostics and related spans
  - Rendering-independent Error/Warning/Note accumulation
  - Ordered read-only diagnostic collection with explicit view lifetime
  - Owner-accepted Linux Debug/Release execution evidence
affects: ["02-04", "03"]
tech-stack:
  added: []
  patterns: ["owning diagnostic values", "insertion-order accumulation", "borrowed const collection views"]
key-files:
  created:
    - include/veyr/diagnostics/diagnostic.hpp
    - include/veyr/diagnostics/diagnostic_engine.hpp
    - src/diagnostics/diagnostic_engine.cpp
    - tests/diagnostics/diagnostic_test.cpp
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-03-EXECUTION.md
    - .planning/phases/02-source-identity-spans-and-diagnostics/02-03-SUMMARY.md
  modified: [CMakeLists.txt, tests/CMakeLists.txt, .planning/STATE.md, .planning/ROADMAP.md]
key-decisions:
  - Preserve D-01/D-03/D-08/D-13/D-14/D-16 and the reviewed structured diagnostic model
  - Accept engine copy/move restrictions to preserve its collection-view lifetime contract
  - Require callers to explicitly initialize severity; record hardening as a non-blocking improvement only
  - Close validated Linux developer scope, with new Windows evidence independently pending/non-blocking
patterns-established:
  - Diagnostic values own messages/labels while source spans remain manager-local references
  - Error state depends only on stored Error severity and survives checked source-resolution failure
  - Read-only collection views must be reacquired after emission
requirements-completed: []
requirements-covered: [DIAG-01, DIAG-03]
coverage:
  - id: D1
    description: Owned optional-primary and ordered labelled related diagnostic values
    requirement: DIAG-01
    verification:
      - kind: unit
        ref: "tests/diagnostics/diagnostic_test.cpp — owning-type, primary, related-source, caller mutation/destruction and byte-payload cases; Linux Debug/Release passed"
        status: pass
    human_judgment: false
  - id: D2
    description: Severity semantics, emission order, duplicates and read-only collection lifetime
    requirement: DIAG-03
    verification:
      - kind: unit
        ref: "Diagnostic.* — fourteen native cases including 4096 emissions and invalid-reference preservation; Linux Debug/Release passed"
        status: pass
      - kind: integration
        ref: "02-03-EXECUTION.md and retained final-Debug/Release-verify.log — full CTest 69/69 and eight C++23 entries per configuration"
        status: pass
    human_judgment: false
  - id: D3
    description: New diagnostic Windows CI/native acceptance
    verification:
      - kind: integration
        ref: "Pending/non-blocking; run 37908907406 predates this diagnostic implementation"
        status: unknown
    human_judgment: true
    rationale: "Linux-scope owner acceptance does not replace separate Windows evidence or developer-host inspection"
duration: "Execution, owner review and documentation closeout on 2026-10-15; no fabricated elapsed metric"
completed: 2026-10-15
status: complete
completion_scope: available-linux-developer-only
---

# Plan 02-03 — Structured diagnostics and ordered accumulation

**The owner accepted Plan 02-03 for validated Linux developer scope. Structured
diagnostics own their data, preserve emission order and expose independent error
state. Diagnostic rendering remains outside Plan 02-03.**

## Completed implementation and public API

DiagnosticSeverity provides Error, Warning and Note. Diagnostic stores severity,
an owned std::string message, optional primary SourceSpan and ordered vector of
RelatedSpan values, each with a span and owned label. Location-free diagnostics
need no dummy source. Related-only context and same/different registered source
spans are preserved without replacing the primary when one exists.

DiagnosticEngine::emit(Diagnostic) accepts an owned value. diagnostics() const
returns std::span<const Diagnostic> in insertion order, retaining duplicates.
has_errors() const scans for Error; Warning/Note alone remain non-errors. No
sorting, suppression, warnings-as-errors, clear/reset, callback or global state
is introduced. The version-only driver is unchanged; later compiler clients must
treat accumulated Error state as failed compilation independently of rendering.

The engine owns no SourceManager and performs no source lookup or span repair.
References resolve only through their originating manager; unknown/reversed/
out-of-bounds spans remain exact inputs to checked consumers. Diagnostic APIs
have no LLVM, driver or renderer dependency, and source APIs have no reverse
dependency on diagnostics.

veyrcore gains diagnostic_engine.cpp. The new diagnostic_tests target follows
existing target policy and GTest::gtest_main/GTest::gmock PRE_TEST discovery.
No external library, build-policy change or infrastructure framework was added.

## Ownership and collection lifetime

Diagnostic values own message/label bytes and may be copied or moved. Caller
mutation/destruction after emission cannot alter accumulated data. Source spans
remain lightweight non-owning identities, not owned text or cross-manager IDs.
The originating SourceManager must remain available when resolving them.

Engine collection views/references are read-only and valid only until the next
emit or engine destruction; callers must reacquire views after every emission.
They do not have SourceManager text-view append stability. Engine copy/move
operations are deleted to prevent assignment/movement introducing another
invalidation boundary. This bounded C++ API choice was explicitly reviewed and
accepted; it defines no Veyr language ownership semantics.

## Accepted Linux developer verification

| Check | Debug | Release |
|-------|-------|---------|
| Explicit cache-only configure and incremental build | Passed | Passed |
| Diagnostic tests | 14/14 | 14/14 |
| Full CTest including include-policy | 69/69 | 69/69 |
| Selected compiler/directory/C++23 compilation entries | 8/8 | 8/8 |

Execution-time doctor passed 22/22. Fourteen tests cover empty collections,
location-free severities, primary/related spans, short/long string ownership,
moved caller values, insertion order, duplicate retention, 4,096 emissions with
safely reacquired views, exact invalid-reference payloads, empty/control/UTF-8
bytes and manager-local identity. Existing source/location tests remain intact.

Both configurations reused the existing LLVM 23.1.2 and GoogleTest packages using
the established Conan `--no-remote --build=never` configuration path. No dependency
downloads/rebuilds, CLI installation, acquisition setup, lock regeneration or
cleanup occurred. Existing generated Conan toolchain quotation warnings remain
non-blocking. Conan 2.33.0, locks, recipes, profiles, baselines and workflows are
unchanged.

## Retained evidence

[02-03-EXECUTION.md](02-03-EXECUTION.md) records authorization, exact Wave 3
selection, commands, selected build directories and outcomes. Ignored
`.veyr-dev/execution/02-03/` contains twelve command logs and evidence.json:

- `task1-{Debug,Release}-{configure,build,focused}.log`: initial configure/build
  and six-case Task 1 evidence.
- `final-{Debug,Release}-{build,focused,verify}.log`: final incremental builds,
  fourteen diagnostic cases, full CTest and compilation-database verification.
- `evidence.json`: hashes for those twelve logs and the selected Debug/Release
  compile_commands.json, CMakeCache.txt and Testing/Temporary/LastTest.log files.

The owner review checked all eighteen manifest entries without missing files or
hash mismatches. Raw doctor stdout was not retained; its count is an observed
execution record, not an independently replayed artifact. No builds/tests or
dependency operations were rerun during this documentation closeout. No dedicated
GSD verification stage or new hosted diagnostic acceptance is claimed.

## Known limitations and non-blocking improvements

Current callers must explicitly initialize Diagnostic::severity before its value
is read. `Diagnostic d;` without assigning severity leaves it indeterminate.
The owner accepted this as a non-blocking design improvement. Closeout introduces
no arbitrary default severity, constructor change or new initialization policy.
A compile-time enforced initialization policy may be considered separately.

Allocation-failure behavior was reviewed structurally, without fault injection;
ordinary C++ allocation exceptions remain ordinary C++ failures, not a Veyr OOM
policy. A future nonthrowing-move assertion and retained doctor stdout are optional
hardening/evidence improvements. Rendering failure integration, escaping, source
excerpts and caret layout are reserved for Plan 02-04 and were not implemented.

## Completion scope and remaining gates

Phase 2 is now 3/4 complete for validated Linux developer scope; Phase 1 stays
closed at 8/8. DIAG-01/DIAG-03 have accepted local coverage, while all seven Phase 2
requirements remain pending in REQUIREMENTS.md until phase-wide acceptance.
Plan 02-04 / Wave 4 remains unstarted and requires separate owner authorization.

New 02-03 Windows CI/native evidence remains pending/non-blocking. Earlier
four-job run 37908907406 validates the completed source plans and predates the
diagnostic implementation; it is not credited here. Windows developer-host
inspection remains independently pending/non-blocking. macOS remains deferred.

Closeout changes only STATE, ROADMAP and the execution record, and adds this
summary. The accepted nine-file packet plus summary totals ten files. All six
accepted implementation/integration/test files, retained evidence, completed
planning artifacts, four locks and the ignored local IDE preset remain unchanged.
No implementation fix, validation rerun, commit, push or CI action occurred.

Platform impact:
- Windows: Portable owned values and source references; new native/CI evidence
  pending/non-blocking, developer-host inspection independently pending.
- Linux: Primary Omarchy developer scope accepted independently in Debug/Release;
  Ubuntu hosted baseline stays separate.
- macOS future: Deferred; no new filesystem, terminal, POSIX or Win32 dependency.

Next gate: explicit owner approval before the dedicated execution/closeout commit
or publication. Stop after preparing these artifacts; acceptance does not grant
commit/push permission or authorize diagnostic rendering or Wave 4.
