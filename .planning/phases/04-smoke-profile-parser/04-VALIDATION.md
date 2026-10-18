---
phase: 04
slug: smoke-profile-parser
status: draft
nyquist_compliant: true
wave_0_complete: false
created: 2026-10-18
---

# Phase 4 — Validation Strategy

Future execution contract, not completed validation. Planning runs no native
build, test, compiler or Conan commands. The sixteen decisions in
[04-CONTEXT.md](04-CONTEXT.md) and the published experimental M1 profile bind
execution. Implementation, stage publication and final acceptance each require
their separate owner authorization. No task commit is automatic.

## Test Infrastructure

| Property | Value |
|---|---|
| Framework | Existing GoogleTest/GoogleMock 1.18.0; CTest PRE_TEST discovery |
| Configuration | Root CMakeLists.txt and tests/CMakeLists.txt; existing C++23 target policy |
| Future target | parser_tests; SmokeProfileParser-prefixed contract/integration suites |
| Quick run | `ctest --test-dir <selected-build-dir> -R '^SmokeProfileParser' --output-on-failure --stop-on-failure --no-tests=error` |
| Full suite | Explicit selected `scripts/dev.py test`, independently in Debug and Release |
| Estimated runtime | Focused fixtures target under 30 seconds after incremental build; unmeasured, not a hard timeout |

## Exact Execution Selection and Commands

During separately authorized execution use the reviewed base Python and explicit
selection on every wrapper command. On the current Omarchy developer host:

```text
/usr/bin/python3 scripts/dev.py doctor --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed --json
/usr/bin/python3 scripts/dev.py configure --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py build --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
ctest --test-dir <selected-build-dir> -R '^SmokeProfileParser' --output-on-failure --stop-on-failure --no-tests=error
ctest --test-dir <selected-build-dir> -R '^(SourceManager|SourceLocation|Diagnostic|PlainTextDiagnostic|SmokeProfileToken|SmokeProfileLexer)' --output-on-failure --stop-on-failure --no-tests=error
/usr/bin/python3 scripts/dev.py test --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py verify --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 -B scripts/check_includes.py --root .
```

Substitute each configuration independently. Obtain its absolute build directory
from doctor selection evidence; do not guess the fingerprint or reuse the other
configuration's directory. Run commands individually, supervised under the
watchdog policy; stop on the first unexpected failure. Quiet healthy builds are
not failures. Configure after target/source-list changes. Reuse prepared Conan
dependencies; do not use setup or all for ordinary validation. Missing or
incompatible preparation requires owner review, not unapproved acquisition.

## Sampling Rate

- After each coherent implementation task: incremental build and focused tests
  in both configurations; check actual names/counts and reject zero selection.
- After each implementation wave: full selected test/verify and direct includes.
- Before final phase acceptance: separately authorized four-job Ubuntu/Windows
  hosted acceptance and canonical GSD phase-goal verification.
- Preserve command logs, discovered/executed names, configuration identities,
  source hashes and archive-aware evidence manifests. A historical passing count
  or successful exit alone does not prove new fixtures executed.
- Maximum focused feedback target: 30 seconds after build, unmeasured; no new
  timeout, resource quota or latency requirement is imposed on native builds.

## Per-Task Verification Map

Task identifiers below match the owner-approved three-plan/three-wave breakdown.
All rows are pending execution. Independent checker cycle 2 passed planned
feedback coverage; this is not native or hosted validation evidence.

| Task scope | Requirement | Secure behavior | Test type | Automated verification | Producer / status |
|---|---|---|---|---|---|
| 04-01-01 Public value/outcome contract and native value target | PARSE-02, PARSE-03 | Exactly three independent spans; distinct checked alternatives; manager-local lifetime contract | Native value unit | Focused discovery/execution restricted to SmokeProfileParserValue | Same task creates parser.hpp/parser_test.cpp and parser_tests; pending |
| 04-01-02 Complete checked parser and baseline behavior | PARSE-01, PARSE-02, PARSE-03 | Whole-stream validation before Invalid/syntax; original errors; exact shape/EOF; one initialized Error; no partial value | Native baseline unit | Focused command plus wave full checks | Same task creates parser.cpp and extends existing parser_test.cpp; pending |
| 04-02-01 Independent structure/combined-fault matrix | PARSE-02, PARSE-03 | Exact source/stream error kind/payload/index; deterministic priority; no diagnostic emission | Native adversarial unit | Focused discovery/execution and named matrix reconciliation | Extends parser_test.cpp from 04-01; pending |
| 04-02-02 Complete syntax/EOF/label matrix | PARSE-01, PARSE-02, PARSE-03 | Twelve positions; eleven incomplete prefixes; complete expected/found/primary/opener fields; first error only | Native parameterized unit | Focused discovery/execution plus wave full checks | Extends parser_test.cpp from 04-01/04-02-01; pending |
| 04-03-01 Real component/lifetime integration | PARSE-01, PARSE-02, PARSE-03 | No token borrows; persistent Error/history; lexical suppression; exact newline/EOF and safe owning rendering | Native integration | Focused discovery/execution and mandatory subcase mapping | Same task creates parser_integration_test.cpp and wires it when present; pending |
| 04-03-02 Final independent local evidence and owner packet | PARSE-01, PARSE-02, PARSE-03 | Actual executed names/counts; strict compiler/directory/C++23; archive-aware original hashes; no self-acceptance/publication | Full suite and static/evidence review | Full selected commands and hash-resolving evidence audit | Existing framework and all prior tests; pending |

## Exact Regression Obligations

- Independent literal source/message/span oracles for the fixed eleven-token
  entry plus EOF, every expectation position and truncated prefixes.
- Complete success value contains only entry, return and integer spans; accept
  lexically valid 256, leading zeros and long digits without semantic conversion.
- Structural matrix: unknown selected/token source, original reversed/beyond
  errors, source mismatch, unsupported kind, empty non-EOF, order/overlap,
  missing/early/duplicate/malformed EOF and valid adjacency/whitespace gaps.
- Multi-fault fixtures prove the selected deterministic validation priority and
  that later structural faults outrank earlier Invalid or syntax mismatches.
- Invalid anywhere prevents syntax parsing and adds no parser diagnostics;
  checked infrastructure failures also add none. Neither returns partial data.
- One initialized Error on first syntax mismatch; exact full-token or EOF
  primary span; exactly one matching opener related span only for expected
  closing parenthesis/brace; all other related collections empty.
- Preserve insertion order, prior Error state and duplicate emissions. Parsing a
  valid stream with prior Errors may yield a value but cannot imply successful
  compilation. Copy diagnostic data before later emits invalidate borrowed views.
- Destroy input token storage, copy/move saved values, add sources and resolve
  original spans while the originating SourceManager remains alive.
- Real lexer/parser/renderer tests cover LF, CRLF, lone CR, empty/trailing EOF,
  comment/high-byte/numeric lexical rejection and hostile display-name escaping.
- Inspect includes/dataflow: no lexer-to-parser dependency, duplicate lexer,
  general AST/semantics, LLVM coupling or changed Phase 2/3 public API.

See [04-RESEARCH.md](04-RESEARCH.md#validation-architecture) for the complete
independent test strategy. Test expectations must not import the parser's own
expected-kind/message tables. Test-loop subcases require useful trace identities.

## Wave 0 Requirements

Existing infrastructure covers framework/dependency requirements. No framework
installation, scaffolding wave or planning-time stubs are authorized. During their
own implementation tasks create parser.hpp, parser.cpp, parser_test.cpp and
parser_integration_test.cpp, and register parser_tests with PRE_TEST discovery.
Every missing test must have a same-task or earlier producer before execution.

## Manual-Only Verifications

| Behavior | Requirement | Why manual | Instructions |
|---|---|---|---|
| Architectural boundary and producer precondition | PARSE-03 | Include checker alone cannot establish responsibility separation | Inspect actual parser/lexer APIs and dependency wiring; no relex/spelling certification |
| Sensitive-file publication and final acceptance | All | Explicit owner gates | Review exact implementation/evidence packet; authorize commit/publication and later phase acceptance separately |

## Validation Sign-Off

- [x] Actual plan tasks mapped to automated checks and test producers.
- [x] Sampling continuity: every implementation/test task includes native focused feedback during later authorized execution.
- [x] Missing test references have same-task or earlier producers; integration source is registered only when created.
- [x] No watch mode, automatic retry, dependency acquisition or automatic commit.
- [x] Focused latency is a planning target, not fabricated timing evidence.
- [x] Independent checker cycle 2 verified planned Nyquist coverage for all six tasks; zero blockers/warnings. `nyquist_compliant: true` denotes this planning result only.
- [ ] Native execution and four-job hosted evidence remain pending.

Approval: pending owner review of the completed planning packet.

Platform impact:
- Windows: Identical byte/span and checked-parser contracts; hosted Debug/Release
  acceptance required. Developer-host inspection remains independently pending.
- Linux: Primary Omarchy developer validation with independent Ubuntu hosted CI.
- macOS future: Deferred; proposed parser uses no platform-specific facilities.
