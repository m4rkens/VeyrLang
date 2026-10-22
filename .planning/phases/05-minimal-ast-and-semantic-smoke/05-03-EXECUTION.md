---
phase: 05-minimal-ast-and-semantic-smoke
plan: "03"
wave: 3
status: local_validated_owner_checkpoint
plan_completion: pending
requirements: [AST-01, AST-02, SEMA-01, SEMA-02]
requirements_completed: []
owner_acceptance: pending
publication: not_authorized
hosted_acceptance: pending_for_this_candidate
recorded_at: "2026-10-22T15:03:45.069744+00:00"
---

# Plan 05-03 — Real component integration local execution checkpoint

## Authority and prerequisites

The owner explicitly authorized `$gsd-execute-phase 5 --wave 3`, Tasks 1–2 of
[05-03-PLAN.md](05-03-PLAN.md), ending at independent owner review. Actual remote
main, tracked origin/main and HEAD were independently confirmed at published
05-02 closeout `24009c88899dfaebc2d8d436bc6c479272338b1c`. The existing isolated
Phase 4 worktree and `phase4/04-03-parser-integration` branch are preserved.
The index was empty and no unrelated candidate was inherited.

Installed GSD 1.6.1 init and phase-plan-index queries find 05-01/02 summaries and
only 05-03 incomplete. The literal wave filter selects 05-03 alone. Execution is
inline under the installed skill's spawn-restriction fallback, not an independent
executor-agent result. Canonical begin-phase/update/record-session operations and
bounded frontmatter reconciliation record the authorized execution/checkpoint,
preserving curated M1 metadata, four completed phases and 20/21 completed plans.
Existing support-filename/configuration warnings do not change plan discovery.
Auto chaining stays disabled; execute:post has no active hooks.

Accepted 05-01/02 implementation, reviews, summaries, local and four-job hosted
evidence are reused. Documentation run
[39781264045](https://github.com/m4rkens/VeyrLang/actions/runs/39781264045) is now
observed completed/success in all four configurations on this baseline. It is
separate from validation of the new uncommitted integration tests. No workflow
was dispatched, retried or cancelled.

## Task 1 — Sixteen real integration identities

The new [integration source](../../../tests/semantics/smoke_validator_integration_test.cpp)
uses actual SourceManager, Lexer, Parser, shared ParsedSmokeEntry, DiagnosticEngine,
validate_smoke and PlainTextRenderer components. It contains precisely sixteen
SmokeProfileSemanticIntegration identities from the approved plan. Test-local
helpers check explicit result alternatives, the complete AST/constant, checked
SourceError payloads, and owning diagnostic fields/history. Independent literal
source bytes, spans, locations, messages and complete rendering strings are the
oracles; no production conversion/table/private helper, mock or second parser is
used to calculate expected results.

Coverage and actual identities are retained in ignored
`.veyr-dev/execution/05-03/COVERAGE.md`, coverage.json and discovery/execution logs:

- RealFixedEntryValidates: exact [0,33), [21,31), [28,30), signed 42, original
  entry/return/literal text and no diagnostic.
- RealLeadingZerosAreDecimal and RealBoundaryValues: actual 00042 becomes 42;
  separately traced 0 and 255 succeed with their complete original spans.
- RealHugeDecimalRejects: 65,536 nines are a valid parser decimal shape; only
  semantics rejects them, with one initialized full-literal Error and no success.
- OwnedOutputOutlivesInputs: caller strings, token vector, LexResult, ParseResult,
  input AST and temporary diagnostic engine actually die before inspection of
  the retained validated value. Its complete three-span AST and constant survive.
- CopiesAndMovesKeepCompleteContext: copy/move construction and assignment retain
  original context; assignment replaces a different already-validated source/value.
  Only usable destination objects are inspected, never moved-from assumptions.
- ThreeThousandRegistrationsPreserveSnapshot: 3,000 additional registrations retain
  original source view, full entry bytes, three spans and constant. The originating
  SourceManager remains alive throughout every resolution/lifetime fixture.
- LexicalInvalidCannotReachSuccess: -1, 0x10, comment and high-byte subcases retain
  exact Invalid/Error values. Parser rejects LexicalErrors without another Error;
  an explicit ParseResult gate leaves semantic invocation/result absent.
- SyntaxFailureCannotReachSuccess: missing semicolon, missing brace and trailing
  token subcases reject with exact primary/opener-related parser diagnostics.
  Failed parse results never become default ASTs or semantic successes.
- PriorErrorBlocksValidPipeline: separate earlier-source real lexer and parser
  Errors seed the same engine. A later valid parse succeeds syntactically, then
  semantics returns BlockedByErrors without altering history or emitting an Error.
- PriorWarningNoteAllowsPipeline: owning Note/Warning fields and order survive
  an otherwise successful real pipeline.
- DuplicateRangeErrorsRemainOrdered: prior Error persists; two validations append
  distinct owning identical range Errors in order with the full 000256 span.
- OriginalSpansAcrossLineEndings: literal LF/CRLF/lone-CR byte/span/location/EOF
  oracles remain original. CRLF [0,36), [22,33), [29,32), literal line 2 column 8;
  lone CR remains on line 1, as required by accepted source-location policy.
- CheckedSourceFailureRemainsDistinct: forged unknown-source AST preserves the
  exact SourceError and all prior diagnostics; real lexer/parser checked failures
  stay distinct, with no fabricated language Error or successful value.
- RangeDiagnosticRendersIndependently: copied owning Error survives engine/input
  destruction and renders again through the live source manager. Complete literal
  output matches; the independently owned rendered string survives diagnostic death.
- HostileNameIsEscaped: ESC/newline/path-like/backslash display metadata produces
  exactly escaped safe-ASCII rendering. Original metadata is retained, not opened.

Looped input/operation variants have SCOPED_TRACE and explicit coverage records;
bounded boundary, lexical, syntax, prior-Error, duplicate, newline and registration
loops assert their execution counts. Subcases are not inflated into CTest identities.

[tests/CMakeLists.txt](../../../tests/CMakeLists.txt) adds only this source to
existing semantic_tests. Existing links, target policy and PRE_TEST discovery are
unchanged. Every accepted AST/semantic/parser/lexer test file remains byte-identical.

Tests were written before first compilation/execution. They passed existing
accepted production behavior immediately; no artificial RED or production mutation
was introduced. Task 1 finished both configurations' build/discovery/focused and
selected regression checks before Task 2 full/strict verification began. The exact
set and ordering proof is task1-acceptance.json. Passing focused receipts are reused,
not repeated solely for Task 2.

## Task 2 — Independent native verification

| Check | Debug | Release |
|-------|-------|---------|
| Doctor |22/22|22/22|
| Focused discovered / executed |85 /85|85 /85|
| New integration / retained AST-semantic |16 /69|16 /69|
| Selected regressions |235/235|235/235|
| Retained parser identities |142/142|142/142|
| Full CTest |646/646|646/646|
| Strict verification CTest |646/646|646/646|
| Selected compiler/directory/C++23 entries |20/20|20/20|
| Direct include policy |PASS|PASS|

Exact discovery/execution sets match separately in Debug/Release. Each full/strict
set equals the prior 630 identities plus exactly sixteen integration additions;
all earlier 581 and 561 identities remain included. All 69 previous AST/semantic
cases (20 original and 49 conformance) and 142 parser cases remain intact. No
required identity is disabled, skipped or inferred merely from an aggregate count.

Both doctors select linux-developer/linux-x86_64-clang, managed LLVM 23.1.2,
GoogleTest 1.18.0, Conan 2.33.0 and Clang 22.1.8 `/usr/bin/clang++`. Independent
absolute build directories are under
`build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/{Debug,Release}`.
All twenty compile-database entries match the selected compiler/build/C++23 and
exact prior nineteen plus the new integration translation unit. Retained snapshots
include compilation databases, CMake caches, toolchains, PRE_TEST registrations and
LastTest logs; validation.json records exact members and semantic binary hashes.

The CMake source-list edit required configure in both configurations. Actual
commands use the accepted locks/profiles and `--no-remote --build=never`, reusing
existing complete pinned package revisions. Only normal supported cache bookkeeping
and worktree-specific generated outputs occur: no download, dependency build,
cleanup, package replacement, revision/profile/lock or global toolchain change.
The permitted offline configure commands succeeded; there is no sandbox-blocked
attempt in this execution. Existing generated conan_toolchain.cmake quoting warnings
at lines 77–80 recur; the runtime compatibility probes and both strict checks pass.
These generated warnings are retained, not silently suppressed or corrected here.

All eighteen supervised native receipts succeeded: two doctors and separate
configure/build/discovery/focused/regression/test/verify/include commands for each
configuration. Exact cwd/argv/timestamps/status/bytes/SHA-256 remain in commands.jsonl.
The watchdog records quiet liveness rather than killing healthy work. No unexpected
native failure or production defect was observed. Command duration is not active
implementation duration. Local success is not fresh Ubuntu/Windows acceptance.

## Evidence, scope and owner checkpoint

Pre-edit STATE and tests/CMakeLists bytes were archived before changes. All 27
retained manifest sets, totaling 1,772 references, validate against original hashes
and exact archive mappings. They include 05-02 local72/logs22/preparation284/formal24,
05-01 local74/logs29/preparation280/formal23, accepted Phase 4 records, Phase 2/3
historical201, Stage1 retained76 and Stage2 correction19. Earlier immutable receipts
are not rewritten when their live shared paths change; new baseline mappings resolve
those paths. Current evidence counts/hashes are in manifest-validation.json.

The complete candidate has four tracked paths: new integration source, existing
semantic target source-list registration, this factual execution record, and the
owner-permitted canonical STATE execution/session checkpoint. The published plan
enumerates the first three; installed GSD bookkeeping requires the fourth.
ROADMAP/REQUIREMENTS/PROJECT, production and public headers, existing tests, root
CMake, published M1 profile, all four locks, recipes/profiles/policies and workflows
remain unchanged. Original main and held Stage1/Stage2/abandoned CI-policy worktrees
preserve their HEAD, empty indices and exact candidate hashes. This index is empty.

The ignored owner packet is `.veyr-dev/execution/05-03/REVIEW.md`, with COVERAGE.md,
candidate.diff, validation.json, evidence.json, commands.jsonl, integrity.json and
archive-resolution.json. Links, whitespace, protected-source hashes, requirement
ledger and installed GSD discovery are checked at sealing.

Tasks 1–2 are locally validated and await independent owner review/acceptance.
05-01/02 remain complete, 05-03 formally incomplete; no canonical 05-03-SUMMARY.md
exists. Phase 5 stays 2/3, overall plans 20/21, four whole phases accepted. AST-01/02
and SEMA-01/02 remain Pending at 30 accepted /19 pending. No staging, commit, push,
workflow dispatch, requirement completion, canonical phase verification or Phase 6
operation occurred. Independent review, separate publication, fresh four-job hosted
acceptance, formal plan closeout and whole-phase verification/acceptance remain
subsequent owner gates. Execution stops at the plan's blocking owner checkpoint.

Platform impact:
- Windows: portable C++ values, explicit original LF/CRLF/lone-CR bytes and safe
  metadata rendering; target/CRT/PRE_TEST and locks unchanged. Fresh Windows
  Debug/Release hosted acceptance and developer-host inspection remain pending.
- Linux: independent local Debug/Release results above; new Ubuntu hosted
  acceptance remains pending. No process-exit or filesystem lookup assumption.
- macOS future: deferred; no OS-specific integration contract introduced.
