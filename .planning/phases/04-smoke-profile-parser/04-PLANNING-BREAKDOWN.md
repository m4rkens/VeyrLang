# Phase 4 — Approved Planning Breakdown and Source Audit

**Scope:** Smoke Profile Parser, PARSE-01–03, P01 only.
**Approval:** The owner explicitly approved this Phase 4 three-plan/three-wave
breakdown after the installed interactive confirm_breakdown checkpoint. Phase 3
approval was not reused. Approval authorizes detailed plans and plan checking,
not implementation, native commands, commits, publication or lifecycle changes.
**Execution:** 0/3. Independent checker cycle 2 passed with zero blockers/warnings
after one bounded research-resolution annotation; owner review of the complete
planning packet remains pending.

## Plans, dependencies and ownership

| Plan / wave | Needs | Two approved tasks | Requirements |
|---|---|---|---|
| 04-01 / 1 | Accepted Phase 3/profile, reviewed plans and fresh Wave 1 execution command | 1 public three-span/outcome API and native value tests/target; 2 complete checked parser/recognizer, baseline tests and root wiring | PARSE-01, PARSE-02, PARSE-03 |
| 04-02 / 2 | Actual reviewed 04-01 local evidence, resolved commit boundary, fresh Wave 2 command | 1 independent malformed-stream/combined-fault matrix; 2 complete syntax/EOF/found-label/opener matrix | PARSE-01, PARSE-02, PARSE-03 |
| 04-03 / 3 | Actual reviewed 04-02, resolved commit boundary, fresh Wave 3 command | 1 real lexer/parser/renderer/lifetime integration and test wiring; 2 independent final Debug/Release evidence and owner review packet | PARSE-01, PARSE-02, PARSE-03 |

Edges are exactly **04-01 → 04-02 → 04-03**. Each plan has two XML tasks and
precomputed wave/depends_on metadata. Shared parser_test.cpp and tests CMake
changes are sequential; no same-wave file ownership overlap exists. Wave 1
delivers the complete parser, Wave 2 scrutinizes it independently, and Wave 3
integrates existing components and records evidence. The six-file native surface
fits three focused plans (approximately 30–50% context each); fine granularity
does not require artificial extra plans.

All plans set autonomous:false and stop after local work for owner review.
Human checks are in verification/output rather than extra checkpoint tasks,
preserving the approved task count. No generic task/RED/GREEN/summary commit is
authorized. Later commits, publication, hosted/canonical acceptance and next wave
each require their own explicit owner instruction.

| Implementation file | Creation owner | Later changes |
|---|---|---|
| include/veyr/parsing/parser.hpp | 04-01-01 | None planned |
| tests/parsing/parser_test.cpp | 04-01-01 value tests; 04-01-02 baseline | 04-02-01/02 independent conformance |
| tests/CMakeLists.txt | 04-01-01 parser_tests with existing policy/PRE_TEST | 04-03-01 adds integration source when created |
| src/parsing/parser.cpp | 04-01-02 complete implementation | None planned; discovered defects need bounded owner correction |
| CMakeLists.txt | 04-01-02 existing veyrcore source list | None planned |
| tests/parsing/parser_integration_test.cpp | 04-03-01 | None planned |

04-03-02 produces its factual execution record and ignored commands/evidence/
review files. Other execution/summary records follow explicit owner gates; they
are future outputs, not planning-time runtime evidence.

## Settled bounded implementation details

04-01 names every new public symbol, field and enum value: ParsedSmokeEntry,
ParseRejectionKind, ParseRejected, TokenStreamErrorKind, TokenStreamError,
ParseResult and parse. Success owns exactly entry_span/return_span/integer_span.
ParseResult distinguishes complete value, lexical/syntax rejection, original
SourceError and parser-specific TokenStreamError. The public signature uses
const SourceManager&, SourceId, span<const Token> and DiagnosticEngine&.

The exact tables in 04-01 settle selected lookup → per-index checked span →
source agreement → supported kind → EOF/non-EOF shape → order → required final
EOF → any Invalid → fixed syntax. Entire stream structure precedes lexical or
syntax rejection. Error priority is earliest index, then the stated same-index
order. Missing EOF has no token_index; original source-error payloads survive.

The twelve-position expected/found table, complete controlled message form and
two related-opener labels are explicit. Unexpected real tokens retain full spans;
EOF uses exact source end. No partial value, relexing, kind/spelling certification,
numeric conversion, source API change or final grammar/ABI decision is implied.
04-02 supplies independent literal message/span tables rather than importing
implementation tables. 04-03 covers actual component/lifetime/rendering effects.

## Multi-source coverage audit

All rows mean **COVERED by planned tasks**, not implemented or owner-accepted.
Independent checker cycle 2 confirmed coverage. Requirement statuses remain pending. Deferred/later-phase items
are excluded according to accepted scope, not silently dropped.

### GOAL

| Item | Outcome | Coverage | Status |
|---|---|---|---|
| Goal | Clients parse fixed M1 entry/return and receive useful syntax failures | 04-01-01/02; 04-02-01/02; 04-03-01/02 | COVERED |
| Criterion 1 | Exact entry/single integer return and final EOF | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| Criterion 2 | Precise malformed/incomplete/trailing/unsupported failure contract | 04-01-02; 04-02-01/02; 04-03-01 | COVERED |
| Criterion 3 | Source relationships/error state and independent lexer | 04-01-01/02; 04-02-01; 04-03-01/02 | COVERED |
| Criterion 4 | Tests/messages explicitly identify experimental M1 scope | All implementation/test actions and 04-03-02 review | COVERED |

### REQ

| Requirement | Primary production owner | Supplemental proof | Status |
|---|---|---|---|
| PARSE-01 | 04-01-02 complete recognizer | 04-02-02; 04-03-01/02 | COVERED |
| PARSE-02 | 04-01-02 complete checked failure/diagnostics | 04-02-01/02; 04-03-01/02 | COVERED |
| PARSE-03 | 04-01-01 API/value; 04-01-02 source/dataflow wiring | 04-02-01/02; 04-03-01/02 | COVERED |

Each plan's frontmatter lists all three requirements because each delivers or
independently verifies those contracts. This repeats evidence coverage, not
competing production ownership.

### CONTEXT

| Decision | Preserved contract | Task coverage | Status |
|---|---|---|---|
| D-01 | Independent value, no token borrow/text copy; live source manager for resolution | 04-01-01/02; 04-03-01 | COVERED |
| D-02 | Exactly entry/return/integer spans | 04-01-01/02; 04-02-02; 04-03-01 | COVERED |
| D-03 | Tight original half-open bounds, surrounding whitespace excluded | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| D-04 | Ordinary copy/move values, no language ownership decision | 04-01-01; 04-03-01 | COVERED |
| D-05 | First syntax mismatch, one Error, no recovery/cascade | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| D-06 | No partial result; final EOF before success | 04-01-02; 04-02-01/02; 04-03-01 | COVERED |
| D-07 | Checked syntax rejection distinct from infrastructure/ordinary allocation behavior | 04-01-01/02; 04-02-01/02 | COVERED |
| D-08 | After structure, Invalid rejects without parser emission | 04-01-02; 04-02-01; 04-03-01 | COVERED |
| D-09 | Prior Error preserves state but does not block independent valid syntax | 04-01-02; 04-02-01; 04-03-01 | COVERED |
| D-10 | Caller supplies completed tokens; no existing API changes | 04-01-01/02; 04-02-01/02; 04-03-01/02 | COVERED |
| D-11 | Whole-stream checked identity/bounds/kind/order/EOF with original errors | 04-01-01/02; 04-02-01; 04-03-02 | COVERED |
| D-12 | Controlled exact expected/found M1 messages, owning initialized Error | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| D-13 | Complete unexpected real-token primary | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| D-14 | Exact EOF primary and original location rules | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| D-15 | One matching recognized opener only for expected closers | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| D-16 | Conforming producer precondition; no second lexer/spelling guards | 04-01-01/02; 04-02-01/02; 04-03-01/02 | COVERED |

### RESEARCH

| Item | In-scope feature / constraint | Task coverage | Status |
|---|---|---|---|
| R-01 | Fixed recognizer and independent four-way checked result | 04-01-01/02 | COVERED |
| R-02 | Exact deterministic full validation and original SourceError priority/payload | 04-01-02; 04-02-01 | COVERED |
| R-03 | Invalid precedence, no diagnostics on infrastructure/lexical rejection | 04-01-02; 04-02-01; 04-03-01 | COVERED |
| R-04 | Twelve positions, fixed labels/message style, tight success spans | 04-01-02; 04-02-02 | COVERED |
| R-05 | Precise EOF versus missing-token-storage sentinel distinction | 04-01-02; 04-02-01/02; 04-03-01 | COVERED |
| R-06 | Copy/move/token destruction and stable manager-owned snapshots | 04-01-01; 04-03-01 | COVERED |
| R-07 | Prior Errors, duplicate diagnostics, owning strings and view invalidation | 04-01-02; 04-02-01; 04-03-01 | COVERED |
| R-08 | Conforming producer/manager-local ID limitation and one-way layering | 04-01-01/02; 04-02-01; 04-03-02 | COVERED |
| R-09 | Semantic independence of 256/leading zeros/long decimal | 04-01-02; 04-02-02; 04-03-01 | COVERED |
| R-10 | Independent real lexer/newline/hostile-display renderer integration | 04-03-01 | COVERED |
| R-11 | Nineteen structure rows, multifaults, syntax/prefix/found-label forms and traces | 04-02-01/02; 04-03-01 | COVERED |
| R-12 | veyrcore/parser_tests/GoogleTest PRE_TEST, C++23/includes, no dependencies | 04-01-01/02; 04-03-01/02 | COVERED |
| R-13 | Prepared dependency selections, supervised single commands and no retries | All task verification; 04-03-02 | COVERED |
| R-14 | Actual discovery/execution, independent Debug/Release and strict compiler/directory evidence | All verification; 04-03-02 | COVERED |
| R-15 | Archive-aware original hashes/manifests and source/evidence review | Every plan entry/output; 04-03-02 | COVERED |
| R-16 | Security trust boundaries, bounded cursor, controlled messages, no new OOM policy | Every threat_model; 04-01-02; 04-02-01/02; 04-03-01/02 | COVERED |
| R-17 | Separate publication, fresh four-job hosted and canonical/owner acceptance gates | Every owner stop; 04-03-02 pending-gate packet | COVERED |
| R-18 | No general AST/grammar/driver/LLVM/source API/package/CI expansion | Every scope/action/review | COVERED |

No uncovered in-scope source item remains. Broader AST consolidation/decimal
semantics, final language design, Phase 2 hardening, Windows developer-host
inspection and macOS bring-up remain explicitly outside this phase's delivery.

## Discovery, validation and remaining gates

Existing 04-RESEARCH and 04-PATTERNS supply current API/pattern discovery. Direct
planning inspection confirmed existing headers/source/CMake/test analogs and no
parser implementation. No new library/integration selection or package install
requires additional discovery. No ORM/database/schema applies to this native
parser; no database push task. Knowledge graph, codebase map and retrospective
are absent; no graph freshness claim is made.

04-VALIDATION maps the six actual tasks to same-task test producers and future
automated checks. Installed frontmatter/structure checks are planning validation,
not native acceptance. Independent checker cycle 2 confirmed all three
requirements, four roadmap criteria, sixteen decisions and planned Nyquist
coverage; nyquist_compliant is true for planned feedback coverage only.
No native count/timing or fresh hosted outcome is
asserted. The task maps preserve baseline correctness versus adversarial proof
versus real integration/evidence as separate responsibilities.

Platform impact:
- Windows: Identical byte/span/failure behavior, existing targets/discovery;
  fresh hosted Debug/Release acceptance after separate publication. Independent
  developer-host inspection remains pending/non-blocking.
- Linux: Primary Omarchy native checks and separate Ubuntu hosted acceptance;
  explicit reviewed profile/environment/provider and cache-only preparation.
- macOS future: Deferred; no parser/source lifetime contract depends on an OS.

Next gates: owner review of completed planning packet
→ separately approved planning commit → concrete Wave 1 execution command.
Each later wave and publication/hosted/phase acceptance remains separately gated.
No arrow grants automatic transition or commit authority.
