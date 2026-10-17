---
phase: 03
slug: m1-experimental-smoke-profile-tokens-and-lexer
status: draft
nyquist_compliant: true
wave_0_complete: false
created: 2026-10-15
---

# Phase 3 — Validation Strategy

Future execution contract, not validation evidence. Research and planning run no
native build/test/compiler/Conan commands. The first documentation-only plan
must stop for owner review and an explicitly authorized dedicated smoke-profile
commit and separately approved, live-verified remote publication before any
syntax-dependent code or tests. Research recovery proposals
become execution requirements only after that profile gate. No task or stage
commit is automatic.

## Test Infrastructure

| Property | Value |
|---|---|
| Framework | Existing GoogleTest/GoogleMock 1.18.0 and CTest PRE_TEST discovery |
| Configuration | Root CMakeLists.txt and tests/CMakeLists.txt; existing C++23 target policy |
| Future target | lexer_tests; proposed SmokeProfileToken and SmokeProfileLexer suites |
| Quick run | `ctest --test-dir <selected-build-dir> -R '^SmokeProfile' --output-on-failure --stop-on-failure --no-tests=error` |
| Full suite | Explicit selected `scripts/dev.py test`, independently in Debug and Release |
| Runtime | Small in-memory cases expected to run in seconds after incremental build; not measured. Build/CI latency remains liveness-supervised |

## Exact execution selection and commands

Use the reviewed base Python and explicit selection on every command. On Omarchy
the interpreter is `/usr/bin/python3`. The configuration placeholder is replaced
independently with Debug and Release. Read the absolute build directory from each
doctor JSON selection; do not guess its fingerprint or reuse the other configuration.

```text
/usr/bin/python3 scripts/dev.py doctor --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed --json
/usr/bin/python3 scripts/dev.py configure --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py build --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
ctest --test-dir <selected-build-dir> -R '^SmokeProfile' --output-on-failure --stop-on-failure --no-tests=error
ctest --test-dir <selected-build-dir> -R '^(SourceManager|SourceLocation|Diagnostic|PlainTextDiagnostic)\.' --output-on-failure --stop-on-failure --no-tests=error
/usr/bin/python3 scripts/dev.py test --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py verify --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 -B scripts/check_includes.py --root .
```

Commands are future sequential gates, never a chained retry script. Stop on the
first nonzero result. Configure explicitly after target/source-list changes;
build may configure and test may build according to existing wrapper semantics.
Ordinary commands do not implicitly set up dependencies. Reuse prepared Conan
packages and the selected dedicated home. Stop for separate owner review if
preparation is absent/incompatible; this phase adds no dependency or package work.
Do not use `all` merely for incremental validation because it includes setup.

For hosted validation, use the unchanged Build foundation four-row matrix and
reviewed ubuntu-ci/windows-ci selections with VEYR_BASE_PYTHON. Publication/CI
requires separate owner authorization. No workflow modification, manual dispatch
or retry is implied. Historical Phase 2 CI is regression evidence only; new Phase 3
code needs its own native evidence before phase acceptance. Windows developer-host
inspection is independently pending/non-blocking, and macOS remains deferred.

## Sampling Rate

- After each implementation task: incremental build and focused tests in both
  configurations, reusing accepted unchanged gates only where justified.
- After every implementation wave: full selected test/verify and include checks.
- Before final owner acceptance: actual four-job hosted native/prepared acceptance
  evidence and canonical phase-goal verification, separately authorized.
- Feedback latency: focused tests target seconds, not a hard timeout; no latency
  measurement is claimed. Quiet healthy builds receive liveness supervision.

## Per-Task Verification Map

Concrete task IDs refer to the three owner-approved sequential plans. Every
missing test reference has a same-task or earlier producer. `nyquist_compliant`
denotes checked **planned feedback coverage only**, never successful execution.
`wave_0_complete: false` records that no native test/profile implementation has
been performed; no separate scaffolding/framework wave is required or authorized.

| Task ID | Requirement / ownership | Secure behavior | Verification | Test/artifact producer | Execution status |
|---|---|---|---|---|---|
| 03-01-01 | LEX-01 primary | Non-final signed32/0–255 contract, all14 decisions and exact mixed-input proposal | Documentation/Git whitespace and complete profile-content review | Same task creates future filled profile; no native test applies | pending |
| 03-01-02 | LEX-01 mandatory owner gate | Owner review; separately approved dedicated commit and publication; actual remote/branch containment, ancestry/content if advanced; fresh scoped execution instruction | Record commit SHA, remote/branch, successful authorized push and fresh ls-remote tip/time; verify required ancestry and unchanged profile; stop if unverifiable | 03-01-01 profile/checkpoint; local commit, prepared push and stale tracking refs are insufficient | pending |
| 03-02-01 | LEX-02 primary | Live profile-publication gate before edits, fresh Wave 2 approval; exact kind/span token, checked original lexemes, manager-local ownership/lifetime | Fresh authorized-branch ls-remote; if advanced, merge-base --is-ancestor and profile blob identity; then independent doctor/configure/build and `^SmokeProfileToken\.` CTest | Profile publication gate from 03-01; same task creates token_test.cpp and lexer_tests before tests | pending |
| 03-02-02 | LEX-03/04/05/06 primary; LEX-02 API integration | Complete ASCII scanner, Invalid/Error pairing, exact EOF, progress, checked SourceError, no Error clearing | Focused `^SmokeProfile` and existing suites; full selected test/verify/include in both configurations | Same task creates lexer.hpp/lexer.cpp/lexer_test.cpp and adds existing target memberships | pending |
| 03-03-01 | LEX-02–06 regression/integration | All256 byte values, mixed numeric/comment/high-byte boundaries, long runs, source locations, lifetime and renderer/Error integration | Focused profile + existing source/diagnostic/renderer CTest in Debug/Release | Extends two test files created by 03-02; no new harness | pending |
| 03-03-02 | LEX-01–06 evidence/owner review; no competing implementation owner | Final-code evidence identity, strict selected compiler/directory/C++23/includes and honest platform claims | Complete final-state independent command sequence; file/byte/SHA-256 manifest checks; owner packet | Existing wrappers/CTest plus 03-02/03-03-01 tests; same task retains ignored raw evidence and normal execution record | pending |

No three implementation tasks can run without automated feedback. Documentation
uses static checks plus a mandatory manual semantic/commit gate. Code tasks
create their missing tests before their focused native verification. Planned
first implementation outputs must not be materialized during research/planning.

## Exact regression expectations

- `03-02-01`: TokenKind/SourceSpan field types, two-member token aggregate,
  checked lexeme retrieval, fresh IDs and stable old snapshot views.
- `03-02-02`: Fixed sample, lowercase whole words, delimiters/arrow, four
  whitespace bytes, 0/255/256/long digits, all rejected spelling classes,
  original SourceError, exact Invalid/Error spans/order and EOF once at size.
- `03-03-01`: All256 single-byte outcomes; +12/1..2/exponent-like rejection;
  numeric/comment adjacency; high bytes inside/outside comment-shaped runs;
  LF/CRLF/lone-CR/trailing EOF; first block terminator/non-nesting; all malformed
  tails; bounds/progress; original snapshot/input ownership; shared engine,
  duplicates, preexisting Error and separate safe renderer handoff.
- `03-03-02`: Actual final-source native counts/commands/hashes, four unchanged
  locks, unchanged accepted APIs/profile/IDE state and explicit pending hosted/
  canonical verification gates. No historical count or cache hit is proof.

See 03-PLANNING-BREAKDOWN.md for the complete GOAL/REQ/RESEARCH/CONTEXT audit and
single primary requirement ownership. A profile review that changes proposed
mixed boundaries requires bounded dependent-plan correction before code.

## Wave 0 Requirements

Existing infrastructure is sufficient; no new framework or preliminary test wave.
Create missing native tests only in their owning authorized implementation tasks,
after the reviewed committed profile. Do not create test stubs during planning.
All automated future references must have an earlier/same-task producer.

## Manual-Only Verifications

| Behavior | Requirement | Why manual | Instructions |
|---|---|---|---|
| Profile acceptance and dedicated publication | LEX-01 | Owner controls milestone-local semantics and separate commit/publication/execution approvals | Review all14 decisions; explicitly approve dedicated commit, then separately approve publication destination; require actual successful push and fresh remote containment/ancestry/content evidence before fresh Wave 2 authorization |
| Sensitive-file publication and phase acceptance | All | Existing owner gates | Review exact implementation/evidence packet; independently authorize commit/push/closeout |

## Validation Sign-Off

- [x] Every actual task has automated verification or an explicit manual owner gate.
- [x] No three consecutive implementation tasks lack automated feedback.
- [x] Missing tests have same-task/earlier creation ownership.
- [x] Deterministic recovery proposals are carried into the profile approval gate.
- [x] Independent Debug/Release, original bytes and fail-closed Release behavior covered in planned tasks.
- [x] No watch modes, automatic retry/commit, dependency acquisition or phase hopping.
- [x] Owner-requested live remote-publication gate correction 1 independently passed checker cycle 3 with zero blockers/warnings; canonical decision coverage and post-planning gap checks were rerun at 14/14 and 20/20.
- [x] `nyquist_compliant: true` denotes checked planned coverage, never tests passed; initial revision 1 passed cycle 2 and owner gate correction 1 passed independent cycle 3. Owner planning review remains pending.
- [ ] Execution evidence and actual native counts/latency remain pending.

Approval: pending owner review of planning packet. Phase 3 execution has not started.

Platform impact:
- Windows: Same unsigned-byte lexer and CRLF spans; independent hosted evidence required, developer inspection deferred/non-blocking.
- Linux: Primary Omarchy developer validation; independent Ubuntu CI baseline preserved.
- macOS future: Deferred; no platform API is introduced by the proposed scanner.
