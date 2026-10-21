---
phase: 05-minimal-ast-and-semantic-smoke
plan: "01"
wave: 1
status: local_validation_complete_awaiting_owner_review
plan_completion: pending
requirements: [AST-01, AST-02, SEMA-01, SEMA-02]
requirements_completed: []
owner_acceptance: pending
publication: uncommitted_unpublished
hosted_acceptance: pending_for_new_implementation
recorded_at: "2026-10-21T21:08:31.812233+00:00"
---

# Plan 05-01 — Local implementation and evidence checkpoint

## Authorization and canonical execution

The owner invoked `$gsd-execute-phase 5 --wave 1`. Actual remote main, tracked
origin/main and worktree HEAD were checked against planning publication
`20e0685c1e523dcf5716cac34680554204c833a6`. Work stayed in the existing isolated
Phase 4 worktree on `phase4/04-03-parser-integration`; no original main checkout
or held infrastructure candidate was changed.

The installed GSD 1.6.1 execute-phase/execute-plan procedures were inspected.
`init.execute-phase 5` and `phase-plan-index 5`, with the supported `--wave 1`
filter, selected only 05-01. All three Phase 5 plans had no summary. The existing
configuration namespace warning and two planning-support filename warnings do
not alter the executable plan selection. Auto chaining remained disabled.
The execute:post capability query reported no active hooks. Execution used the
inline fallback rather than claiming independent executor-agent review.

Only the two automatic tasks in [05-01-PLAN.md](05-01-PLAN.md) were executed.
The third task is the blocking owner review checkpoint reached by this record.
Repository/owner gates override generic task commits, summary creation and
completion. No canonical SUMMARY, requirement completion, phase verification,
commit, push or subsequent wave was performed.

## Task 1 — One shared AST and restricted semantic public interface

Moved the unchanged `veyr::ParsedSmokeEntry` definition into
[smoke_entry.hpp](../../../include/veyr/ast/smoke_entry.hpp). Its three SourceSpan
fields, equality, value semantics and originating-manager lifetime contract are
preserved. [parser.hpp](../../../include/veyr/parsing/parser.hpp) includes the
shared definition; parse(), ParseResult, parser.cpp and existing tests are unchanged.
The AST public header includes only source_span.hpp among owned headers.

[smoke_validator.hpp](../../../include/veyr/semantics/smoke_validator.hpp) defines
exactly the approved typed SourceError/AstError/semantic rejection/success
alternatives. ValidatedSmokeEntry privately owns the complete AST and signed
int32 constant, has no public default/payload construction, supports copy/move,
and exposes only by-value getters. Only validate_smoke can create its payload.
A default SemanticResult is a rejection. No token/parser/lexer dependency is
introduced into the AST or semantic public interface.

Wrote the eight value/type-trait tests before implementation. Expected RED was
missing new headers; subsequent Debug/Release builds and exact eight-case
discovery/execution passed. Task 1 does not call an undefined validator.
Existing parser tests compile unchanged. Exact identities and receipt are in
`.veyr-dev/execution/05-01/task1-acceptance.json`.

## Task 2 — Complete checked validator and baseline behavior

Wrote the twelve behavioral baseline tests before the validator implementation.
Expected RED was an unresolved validate_smoke linker reference. Added
[smoke_validator.cpp](../../../src/semantics/smoke_validator.cpp) to existing
veyrcore and implemented the complete accepted order:

1. Validate all three source spans in entry/return/integer order and preserve the
   first SourceError exactly, before any AST or language checks.
2. Check common source identity, then nonempty spans, then strict entry/return
   and return/integer nesting; report inspectable AstError without diagnostics.
3. Resolve the original literal bytes, check every byte as unsigned ASCII decimal,
   and preserve the first nondigit's absolute original offset. Continue digit
   checking even after an excessive numeric prefix.
4. Accumulate only while the value remains within 0–255, checking before
   multiply/add. Arbitrarily long decimal text and leading zeros cannot overflow.
5. Out of range emits exactly one initialized Error on the complete integer span,
   with empty related list and this exact message:

   `return value must be between 0 and 255 in M1 Experimental Smoke Profile`

6. A structurally/digit/range-valid current input with accumulated Error state
   returns BlockedByErrors without emission. Notes and Warnings permit success.
   Only otherwise construct the owning validated result.

Earlier diagnostic fields/order/related labels persist. No failure contains a
successful partial value. No source-text copy, token borrow, parser invocation,
whole-input numeric conversion, general AST, LLVM type, lowering or driver was added.
The originating SourceManager must remain alive for resolution; manager-local
numeric IDs do not prove provenance across a foreign-manager collision. Public
AST validation does not reparse arbitrary surrounding source syntax. Success
certifies the current engine state, not later diagnostic emissions.

## Independent test identities and oracles

The tests use literal contract-derived expected values and caller-created ASTs,
without importing production validation tables or parser helpers. Fixed `42`
spans are entry [0,33), return [21,31), integer [28,30). All outcomes are inspected
by exact variant alternative and payload. Diagnostic assertions compare owning
severity/message/primary/related values after emissions. Runtime copy/move and
assignment of successful values and detached getter-copy mutation are checked.

| Suite | Exact cases |
|-------|-------------|
| SmokeProfileAst (4) | ThreeSpansHaveValueSemantics; ExactFieldTypesAndEquality; CopiesResolveOriginalSnapshot; HeaderIsIndependentOfParserTokens |
| SmokeProfileSemanticValue (4) | CheckedAlternativesRemainDistinct; SuccessCannotBePubliclyForged; SuccessSupportsCopyMove; AccessorsExposeOwnedValuesOnly |
| SmokeProfileSemantic (12) | AcceptsZero; Accepts255; LeadingZerosAreDecimal; LongLeadingZerosRemainInRange; Rejects256WithExactError; HugeDecimalUsesFixedError; UnknownSourcePreservesPayload; InvalidRelationshipIsChecked; MalformedDigitsAreChecked; PriorErrorBlocksValidOutput; PriorErrorDoesNotSuppressRangeError; WarningsAndNotesPermitSuccess |

Long-input fixtures use 65,536 leading zeros plus 42 and 65,536 nines. The existing
142 parser cases and all prior 561 full-suite identities are retained. Exhaustive
49-case malformed/precedence conformance remains planned 05-02 work; sixteen real
lexer/parser/semantic lifetime/rendering cases remain 05-03 work, not reported as
executed here.

## Local environment and native validation

Each command ran separately under watchdog supervision with exact argv, cwd,
timestamps, exit status and full log hash. No duplicate build/test was launched
for silence. Receipt durations measure commands, not active implementation time.

Pinned Conan 2.33.0, managed LLVM 23.1.2 and GoogleTest 1.18.0 were reused with
Clang 22.1.8 at `/usr/bin/clang++`. Configure used `--no-remote --build=never` and
already installed locked revisions. No dependency acquisition, dependency rebuild,
cache cleanup, recipe/profile/lock update or archive operation occurred. Normal
Conan usage metadata/locking was permitted. The selected configuration directories
are independently under:

`build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/{Debug,Release}`

| Check | Linux Debug | Linux Release |
|-------|-------------|---------------|
| Doctor mandatory prerequisites | 22/22 | 22/22 |
| Task 1 discovered/executed | 8/8 | 8/8 |
| Final AST/value/semantic discovered/executed | 20/20 | 20/20 |
| Selected source/diagnostic/lexer/parser regressions | 235/235 | 235/235 |
| Full native CTest | 581/581 | 581/581 |
| Strict verification CTest | 581/581 | 581/581 |
| Compiler/directory/C++23 entries | 19/19 | 19/19 |
| Direct include-policy | PASS | PASS |

Selected regressions include all 142 parser cases plus the earlier 93 selected
component regressions. This selection is not claimed to include all parameterized
lexer cases; the full suite preserves every prior 561 identity. Exact new and
retained identities are reconciled in validation.json, not inferred from totals.
All three new TUs occur in each independent compilation database. Both targets
retain existing policy, private links and PRE_TEST discovery.

## Expected RED, environment boundary and non-blocking warnings

There are 29 native command receipts: 26 successful, two deliberate tests-first
RED results, and one sandbox-blocked configure. The initial Debug configure could
not create Conan's normal lock under the sandbox's read-only shared cache. Its
permitted cache-only offline rerun passed. This was not a parser/semantic defect,
dependency failure, download, or accepted failed validation.

Existing generated Conan-toolchain lines 77–80 emit quoting syntax warnings.
The same warnings occur in accepted earlier configure evidence. Actual compiler
compatibility, selected C++23 entries, native tests and strict checks passed.
No generated-toolchain warning was hidden or fixed through unrelated policy changes.
No unexpected final build or test failure occurred.

## Evidence, history and exact scope

Ignored review packet: `.veyr-dev/execution/05-01/REVIEW.md`.
Current receipt set: `.veyr-dev/execution/05-01/commands.jsonl` (29/29 hashes).
Current manifest: `.veyr-dev/execution/05-01/evidence.json`; exact verified count
is recorded in manifest-validation.json. Source/build snapshots, discovery JSON,
full command logs, coverage and historical resolution are retained there.

Original shared parser header/root and tests CMake/STATE bytes were archived
before editing under `baseline/`. Earlier accepted manifests were not rewritten:
04-01 local 75, 04-02 local 70, 04-03 local 83, Phase 4 preparation 287 and
verification 50, five historical sets totaling 201, Stage 1 76, Stage 2 correction
19, Phase 5 discussion 11 and planning 20 all hash-resolve using archived mappings.
Earlier native command logs (31/27/19) and four Phase 4 verification logs remain
intact. Superseded live paths resolve to exact accepted archives rather than being
incorrectly required to keep old bytes at current mutable paths.

The technical scope is the eight approved AST/semantic/header/test/CMake paths.
The factual execution record and canonical STATE checkpoint bring the complete
candidate to ten files. STATE uses supported state.begin-phase, state.patch,
state.record-session and bounded frontmatter merge. The installed begin-phase
helper inferred historical milestone/support-file counts; curated M1 identity,
four completed phases and 18/21 written execution plans were restored without any
plan/requirement completion. ROADMAP, REQUIREMENTS and PROJECT remain unchanged.

The published M1 profile remains byte-identical to its dedicated accepted commit.
All four locks, policies, workflow, existing implementations/tests and held worktrees
are unchanged. The staging index remains empty. Nothing is committed or published.

## Owner checkpoint and remaining gates

Tasks 1–2 are locally validated, not self-accepted. Plan 05-01 remains formally
incomplete; no canonical summary exists. Phase 5 remains 0/3; Phases 1–4 stay
accepted with 18 prior completed plans. AST-01/02 and SEMA-01/02 remain Pending;
global requirements remain 30 accepted / 19 pending.

Next gate is independent owner implementation/evidence review, followed only by
explicitly authorized acceptance, sensitive-file publication and formal closeout.
Fresh Ubuntu/Windows Debug/Release hosted evidence and final canonical Phase 5
verification remain subsequent acceptance gates. Neither recovered parser/lexer
output nor semantic success after a later Error is successful compilation proof.
05-02/05-03 execution requires its own concrete owner authorization and preceding
canonical closeout. No next-wave or phase transition is performed.

Platform impact:
- Windows: portable C++23 values/bytes and existing target/CRT/PRE_TEST policies;
  fresh hosted Debug/Release validation of this slice is pending. Developer-host
  inspection remains independently pending/non-blocking.
- Linux: independent developer Debug/Release evidence above; no locale/path/process
  dependency introduced. Ubuntu hosted evidence for this slice remains pending.
- macOS future: deferred; no OS, final ABI, LLVM or host-integer dependency added.
