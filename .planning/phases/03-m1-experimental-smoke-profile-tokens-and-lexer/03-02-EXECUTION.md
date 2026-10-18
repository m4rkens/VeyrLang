# Plan 03-02 — Token and checked lexer execution checkpoint

**Date:** 2026-10-17 implementation/validation; 2026-10-18 owner acceptance and closeout preparation
**Invocation:** `$gsd-execute-phase 3 --wave 2`
**Base commit:** `030fd0b5c17d8228f5c2dc86324c325dba5103f3` (`main`)
**Status:** Tasks 1–2 owner-accepted for validated Linux developer scope; canonical closeout documentation/bookkeeping prepared. Commit/publication remains separately gated.
**Lifecycle:** Phase 1 8/8; Phase 2 4/4; Phase 3 2/3, in progress. LEX-01–06 complete within their recorded scopes. Global ledger 27 accepted/22 pending. 03-02-SUMMARY.md prepared; nothing staged, committed or published. Wave 3 unstarted and unauthorized; new hosted lexer CI and final Phase 3 acceptance pending.

The execution sections below retain the actual 2026-10-17 work and its original
pre-acceptance gates. The dated closeout section records their subsequent owner
resolution; no historical result is silently recast as hosted or phase-wide proof.

## Actual entry gate and canonical execution

The owner explicitly authorized local 03-02 execution only. Read the installed
GSD 1.6.1 execute-phase/execute-plan workflows, approved 03-02 plan/context,
patterns/research/validation and accepted smoke profile, source/diagnostic APIs,
CMake integration and applicable lifecycle/build/include/platform policies.

Ran actual installed `query init.execute-phase 3`, `query phase-plan-index 3`
and `query config-get workflow.inline_plan_threshold --default 2` (exit 0).
Exact wave-equality filtering selects **03-02 only**; 03-01 has its completed
summary and no lower wave is incomplete. 03-03 is Wave 3 and excluded. Two tasks
and threshold 2 select execute-plan Pattern C inline. No subagent/worktree or
agent-tracking artifact is needed or authorized. No partial 03-02 commit, prior
lexer implementation or blocking .continue-here.md existed at entry.

A fresh `git ls-remote --exit-code --refs origin refs/heads/main` exited 0 and
returned exactly `030fd0b5c17d8228f5c2dc86324c325dba5103f3 refs/heads/main`.
The accepted dedicated profile commit is
`52f598aaf031e549ed3bbe5c598bc43f5f7be6bb`. For both HEAD and the actual observed
remote tip, `git merge-base --is-ancestor <profile-SHA> <tip>` exited 0; the
profile blobs at the dedicated SHA and observed tip are identical. Current file
bytes match the accepted blob: 21,726 bytes, SHA-256
`1b5cefb5fa40a4a940ce1c0f4a4d7b8049ce591b81b79ecaf2251e5dfb107864`.
03-01 acceptance, its dedicated publication and published closeout were inspected;
all fourteen decisions remain binding, experimental M1-only contracts. Actual
entry observation/time, scoped authorization and original input hashes are in
ignored `.veyr-dev/execution/03-02/entry.json`.

Ran canonical `query state.begin-phase --phase 3 --name 'M1 Experimental Smoke
Profile, Tokens, and Lexer' --plans 3`; exit 0, actual output retained in
gsd-begin-phase.json. Reconciled its known generic milestone/count inference
within STATE to the actual M1 identity, 15 written/13 accepted plans and two
accepted phases. No completion counter or requirement-acceptance helper ran.
Owner/repository gates override generic automatic task/TDD/summary commits.

## Completed implementation and contracts

- `TokenKind` has exactly Fn, Main, Int32, Return, LeftParen, RightParen, Arrow,
  LeftBrace, RightBrace, Semicolon, IntegerLiteral, Invalid and EndOfFile.
- `Token` is a copyable two-member aggregate: kind and original-byte SourceSpan,
  with value equality. No lexeme copy, numeric payload, source pointer or ABI
  layout promise. Callers initialize both fields and resolve through the
  originating manager; new registrations preserve old views/snapshots.
- `LexResult = std::variant<std::vector<Token>, SourceError>` and synchronous
  `lex(const SourceManager&, SourceId, DiagnosticEngine&)` return owning tokens.
  A recovered vector is not compilation success: callers inspect has_errors().
- One checked source_text lookup precedes scanning. Unknown ID returns its exact
  SourceError and changes neither diagnostics nor Error state; no fabricated EOF.
  Source borrows remain call-local, and no diagnostic view survives emit().
- The scanner implements only the published precedence: four whitespace bytes,
  rejected comment-shaped runs, contiguous arrow, punctuation, whole ASCII
  words, numeric-like runs, consecutive high bytes, then single invalid bytes.
  It uses unsigned bytes, bounded lookahead, advancing branches and one final
  [size,size) EOF for known sources, including empty/error-only input.
- Digit-only strings have no range conversion. Signs/fractions/prefixes/suffixes,
  separators and exponent-like spellings remain grouped Invalid runs. Independent
  delimiters, slash/arrow/high-byte boundaries and subsequent valid tokens survive.
- Block closing search starts after the full opener, at cursor+2. `/*/return`
  produces Invalid [0,9), one Error [0,9) with empty related spans, EOF [9,9),
  and no Return token. Comment contents own embedded control/high bytes; no nesting.
  Line runs exclude LF/CRLF but include lone CR. Original bytes remain immutable.
- Every Invalid has one owning explicitly initialized Error, exact primary span,
  fixed approved M1 wording and empty related spans. Prior diagnostics, insertion
  order, duplicates and existing Error state survive. Ordinary C++ allocation
  exceptions remain possible; no transaction/rollback or new OOM policy is claimed.
- Existing veyrcore gains lexer.cpp; lexer_tests uses existing target policy,
  GTest::gtest_main/GTest::gmock and PRE_TEST discovery. Source/diagnostic APIs,
  renderer and downstream components are unchanged. No parser, semantics, driver,
  final language/type/ABI, LLVM dependency or new framework is introduced.

## Actual task and must-have verification

| Plan criterion / truth | Actual evidence and result |
|---|---|
| Entry proof and fresh Wave 2 authorization | Above live remote/ancestry/blob checks; entry.json; PASS before syntax-dependent edits |
| Exactly 13 kinds, two-member value token | Declaration inspection, static field/aggregate/trivial-copy assertions, two-element structured binding and seven SmokeProfileToken tests; PASS both configurations |
| Checked original lexemes and snapshot lifetime | Byte ranges including NUL/CRLF/high bytes, empty/EOF spans, 3000 fresh registrations and originating-manager tests; 7/7 both |
| Lexical digits without semantic conversion | DecimalShapeHasNoSemanticRangeOrConversion includes 0/255/256/leading zeros/16384 digits; PASS |
| Exact fixed words/punctuation/arrow and boundaries | FixedSample, RecognizesUnitsWithoutParsingProgramStructure, KeywordsAreExactCaseSensitiveWholeWords, numeric/arrow tests and normative rows; PASS |
| Four whitespace bytes and unchanged newline/source behavior | WhitespacePreservesLfCrlfLoneCrAndEofOffsets, normative newline rows, control-byte recovery and unchanged existing source/location suites; PASS |
| Complete Invalid/Error recovery and exact messages/order | All 35 normative rows plus focused class/delimiter/comment/tail tests; compare every expected kind/span, error severity/message/primary/empty related and original bytes; PASS |
| Exactly one original-size EOF and progress | Every expect_scan comparison checks final EOF and unique EOF count; empty/whitespace/error/tail cases; bounded code-path inspection; PASS |
| Original SourceError and persistent engine state | UnknownSource tests compare exact source_text error, including zero/max IDs; existing diagnostics/Errors unchanged; CleanScanCannotErasePriorErrorState and repeated-scan order/duplicate tests; PASS |
| CMake/C++23/include integration and unchanged policy | Both configure/build, focused/full tests, 13 selected compiler/directory/C++23 entries, direct include checker; canonical artifact presence 7/7; PASS |

Task 1 acceptance gate passed independently before Task 2 implementation.
Its scratch enum counter initially omitted Int32 because a letters-only regex
excluded digits; corrected enumeration and the entire acceptance loop passed.
This was a checker error, not a token/native failure; retained task1-acceptance.json
records it. Task 2's gate is recorded in task2-acceptance.json. Artifact presence
alone is not native proof or owner acceptance; the actual logs support each row.

## Independent Linux Debug/Release validation

All commands ran separately and sequentially from
`/home/m4rkens/Projects/VeyrLang`, using `/usr/bin/python3`, explicit
`linux-x86_64-clang` / `linux-developer` / `managed` selections and each
configuration's own doctor-selected directory. Raw merged stdout/stderr was
recorded byte-for-byte, with actual argv/cwd/timestamps/elapsed/exit/hash receipts
in commands.jsonl. The recorder never overwrites existing logs. No duplicate
process, destructive cleanup or silence-based termination occurred.

| Gate | Debug | Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Task 1 configure/build and token suite | PASS; 7/7 | PASS; 7/7 |
| Final explicit configure/build | PASS | PASS |
| Final focused token/lexer suite | 55/55 = 7 token + 13 lexer + 35 normative rows | 55/55, same independent suite |
| Existing source/location/diagnostic/renderer | 61/61 | 61/61 |
| Full dev.py test / CTest | 151/151 | 151/151 |
| dev.py verify | 13 selected compiler/directory/C++23 entries; CTest 151/151 | 13 selected entries; CTest 151/151 |
| Direct check_includes.py | PASS | PASS |

Task-scoped test-first RED checks were performed deliberately and inspected:
Debug task1 build exited 1 only for missing token.hpp; Task 2 syntax-only Clang
check exited 1 only for missing lexer.hpp. Those are planned local TDD feedback,
not final verification failures or automatic retries. Required declarations/
implementation were then added and independent native GREEN checks passed.
No RED/GREEN commits occurred. All other recorded commands exited 0.

The first parameter presentation exposed raw object addresses. A test-only printer
removed them, but escaped UTF-8 labels made CTest discover only 50 of 55 cases.
The incomplete intermediate run is retained and is not final acceptance evidence.
ASCII hex labels corrected discovery; final logs independently prove all 35 rows
and all 55 focused tests on both configurations. No production behavior changed.
Final focused Debug log is Debug-accepted-focused.log (a selected final evidence
label, not owner acceptance); final Release log is Release-focused.log.

Configure used the existing cache-only --no-remote --build=never path. LLVM and
GoogleTest packages were reused; no explicit dependency setup/acquisition/build
ran. Logs preserve LLVM 23.1.2 RREV aa4cad25b3148c3bdd22ed96fba04654,
GoogleTest 1.18.0 RREV e2507fc4efe2ec9e2db9e5c91eefab9f and build CMake 4.4.3
RREV 8b46ce9dbf1937af1c05f19579c1f105. Existing generated Conan toolchain quotation
warnings recur; no new production/test compiler warning appeared.

## Retained evidence and integrity

Ignored packet: `.veyr-dev/execution/03-02/` contains entry.json,
gsd-begin-phase.json, task1-acceptance.json, task2-acceptance.json, commands.jsonl,
28 raw command logs, prior-ci.json, REVIEW.md, evidence.json and exact copies in
prepared/{Debug,Release}/ of compile_commands.json, CMakeCache.txt, root/tests
CTestTestfile.cmake, LastTest.log and generated Conan toolchain. Baseline copies
preserve the two original CMake inputs before the approved integration changes.

All four locks, accepted profile, Phase 2 public APIs, dependencies, toolchains,
CI workflows, completed plans/closeouts and ignored IDE binding remain unchanged.
The prior 36-entry accepted manifest is untouched: 34 live references still match;
only its two live CMake hash references are superseded by this planned integration,
with both exact original byte sets preserved under baseline/. The new manifest
identifies final source/log/copied metadata bytes; no old acceptance evidence is
edited or silently credited to the new lexer. File-scope/links/whitespace checks
are required before returning the review packet; any discrepancy stops delivery.

## Prior CI and publication gates

Read-only gh run view confirmed Build foundation run
[39047427666](https://github.com/m4rkens/VeyrLang/actions/runs/39047427666)
completed/success at the published closeout commit
`030fd0b5c17d8228f5c2dc86324c325dba5103f3`. Ubuntu Debug/Release and Windows
Debug/Release all completed/success. This satisfies the prior-CI prerequisite
for a future publication decision; it does not test uncommitted lexer code.
No CI dispatch, cancellation or retry occurred.

At the end of local execution, the owner-review checkpoint blocked closeout and
publication. The owner subsequently accepted Tasks 1–2 and authorized the bounded
documentation/bookkeeping below. Sensitive-file commit/publication is still
separately gated. 03-03/Wave 3, phase-goal verification, final Phase 3 acceptance
and Phase 4 remain outside this work. No implementation blocker was identified.

## Owner acceptance and canonical closeout preparation — 2026-10-18

The owner accepted the architecture, implementation, security and evidence review
for Linux developer scope. That review independently inspected all ten original
changed/new files and verified the 65-entry manifest, 28 raw command-log hashes,
both final configuration results and actual execution of all 35 normative cases.
No acceptance-blocking defect or new language decision was introduced.

Re-read installed GSD 1.6.1 VERSION, execute-plan.md, its summary template and Git
integration reference, repository GSD/commit policy, the actual plan frontmatter
and current state. The plan declares exactly LEX-02, LEX-03, LEX-04, LEX-05 and
LEX-06. Accepted evidence supports each for the explicit Linux scope; its eight
must-have truths and seven artifact checks remain supported by actual artifacts,
the entry receipt and retained native logs. No user_setup block or codebase map
exists, so those conditional workflow branches do not require another artifact.

Performed these actual canonical operations through the installed gsd-tools.cjs:

1. Created [03-02-SUMMARY.md](03-02-SUMMARY.md) using the installed template,
   including all five requirement IDs, six classified deliverables, API/ownership
   contracts, actual verification counts, expected RED checks and platform gates.
2. `query state.advance-plan` advanced the administrative pointer from 2 to 3;
   it did not execute 03-03 or authorize Wave 3.
3. `query state.update-progress` counted 14 summaries against 20 apparent plans
   and returned 70%. The generic metadata writer also inferred an incorrect
   milestone name and zero completed phases. Reconciled the existing authorized
   STATE document against numbered PLAN/SUMMARY files: 15 written plans, 14
   completed, two accepted phases of ten (20%), actual M1 identity and Phase 3 2/3.
   The helper's raw output is preserved rather than claimed as accurate evidence.
4. `query state.record-metric --phase 03 --plan 02` recorded two tasks and seven
   implementation/test/CMake files. Duration explicitly remains unmeasured active
   work; 38.856 seconds is only the sum of 28 retained command durations.
5. `query state.add-decision` recorded the summary's three existing contracts:
   preserve the experimental profile, require persistent Error-state inspection,
   and separate local acceptance from hosted/Wave 3/phase-wide acceptance.
6. `query state.record-session` recorded the publication-review stop and existing
   execution resume file. No automatic phase transition was invoked.
7. `query roadmap.update-plan-progress 3` observed three plans/two summaries,
   checked 03-02 and reported In Progress, complete=false. Restored detailed
   scope/owner safeguards that the generic table/count replacement omits.
8. `query requirements.mark-complete LEX-02 LEX-03 LEX-04 LEX-05 LEX-06`
   completed exactly 5/5 IDs, with none missing or previously complete. Reconciled
   prose totals to 27 accepted/22 pending; definitions, mappings, earlier accepted
   requirements and all later pending requirements remain unchanged.

Actual helper commands, timestamps, output and exit statuses are retained under
ignored .veyr-dev/execution/03-02/closeout/. Mandatory automatic task/summary
commits in the generic workflow remain deferred by explicit owner/repository
policy; no commit helper, staging or publication was performed. No automatic
next-plan command, final phase verification or phase-completion helper ran.

Before the first closeout mutation, independently verified all 65 manifest entries
and 28 log hashes again. The accepted evidence.json, REVIEW.md and logs remain
unchanged. The three superseded planning documents (STATE, ROADMAP and this
execution record) are archived byte-for-byte under closeout/accepted-inputs/.
This retains all 65 original accepted byte sets rather than rewriting the old
manifest to fit new documents. All seven implementation/test/CMake files retain
their accepted hashes. The prior 36-entry packet likewise retains 34 live matches
and its two original CMake inputs under baseline/.

Closeout verification uses canonical summary schema/summary/artifact/coverage
and lifecycle queries plus direct documentation/link/frontmatter/whitespace,
requirement-count and hash checks. Actual results are recorded under closeout/;
the initial summary check treated failure-related prose after a heading named
Verification as a failed self-check. Renaming that heading to Validation makes
the checker inspect the actual final Self-Check section; the initial result is
retained, and the final canonical result must pass. This is a documentation
checker issue, not a native validation failure or implementation change.
Final results: summary schema valid; summary/self-check passed with six referenced
files found, no errors and prerequisite commits found; plan artifacts 7/7; all
six local-scope deliverables classified as covered. The phase-plan-index query
reports completed summaries for 03-01/03-02 and only 03-03 incomplete. The
state-snapshot query retains the correct phase/status/session but reports null
for some legacy numeric fields in this compound STATE format; direct numbered
artifact enumeration and explicit state/frontmatter checks supply those counts.
Its output is retained, not presented as stronger verification than it provides.
No native builds, tests or dependency operations are repeated. The accepted
profile remains byte-identical to dedicated commit
52f598aaf031e549ed3bbe5c598bc43f5f7be6bb; all four Conan locks, Phase 2 APIs,
toolchain/dependency policy and ignored IDE bindings remain unchanged.

Remaining work is explicitly retained: 03-03's all-256-byte coverage, long
malformed runs, progress/whitespace-gap invariants, returned-token lifetime,
renderer integration and adversarial cases. Those are planned verification,
not new defects. New Ubuntu/Windows hosted lexer evidence, canonical phase-goal
verification and final owner acceptance remain pending. The prior CI result
above is not relabelled as a lexer run. This five-file documentation closeout
joins seven unchanged accepted implementation files, for twelve tracked/new
candidates overall. Owner review precedes any staging, commit or publication.

Platform impact:
- Windows: Standard C++23 original-byte implementation; compiler/runtime/DevShell,
  profiles and locks unchanged. New hosted/native lexer evidence pending;
  developer-host inspection independently pending/non-blocking.
- Linux: Primary Omarchy Debug/Release passes all recorded gates; Ubuntu remains
  an independent baseline, awaiting publication/CI of this new code.
- macOS future: Deferred; no POSIX/Win32, filesystem, terminal or encoding API added.
