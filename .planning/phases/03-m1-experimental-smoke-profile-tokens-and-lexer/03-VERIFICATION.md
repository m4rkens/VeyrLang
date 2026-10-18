---
phase: 03-m1-experimental-smoke-profile-tokens-and-lexer
verified: 2026-10-18T08:22:21Z
status: passed
score: 22/22 must-haves verified
roadmap_criteria: 5/5
plan_truths: 21/21
artifacts: 12/12 plan entries
key_links: 11/11 directly verified
requirements: 6/6
decision_coverage:
  honored: 14
  total: 14
  not_honored: []
behavior_unverified: 0
human_verification: []
gaps: []
overrides_applied: 0
verification_mode: inline-canonical-procedure
verified_commit: be84bb9702afa75ad07803a678e7fd5a4e6358d7
hosted_run: 39107885454
completion_authorized: bounded documentation-only closeout; no Phase 4 transition
owner_accepted: true
owner_acceptance_date: 2026-10-18
---

# Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer Verification

**Goal:** The owner-reviewed milestone-local profile defines the syntax boundary
and clients can obtain recovered tokens with precise lexical diagnostics.

**Status:** Passed verification; subsequently owner-accepted for final Phase 3
closeout at 3/3. The original verification findings and evidence remain unchanged.
Only bounded documentation bookkeeping is authorized; this report does not
authorize a commit, publication, unrestricted phase.complete or Phase 4.

## Actual procedure and evidence provenance

This is initial canonical goal verification, not a restatement of an earlier
passed report. No earlier Phase 3 verification report existed. Read the installed
GSD 1.6.1 execute-plan checkpoint/summary procedure, execute-phase verify_phase_goal,
verify-phase workflow, verifier role instructions, verification/report templates,
all three plans and their must_haves, the five roadmap criteria, all available
summaries/execution records, accepted profile, actual token/lexer code and tests,
CMake integration and relevant Phase 2 APIs/evidence. The Codex skill adapter
permits inline work when automatic agent spawning is not authorized; the same
canonical verification steps ran inline without creating a subagent or tracking
artifact. Summary assertions were checked against actual artifacts and logs.

Actual canonical query receipts in
`.veyr-dev/execution/03-03/verification/canonical-queries.json` record:

- init.phase-op 3 and roadmap.get-phase 3.
- agent-skills gsd-verifier (no additional rendered skills).
- execute:post and verify:post render-hooks: both activeHooks arrays empty.
  No code-review, security or TDD capability step is active. No such check is
  claimed executed merely because earlier owner reviews exist.
- frontmatter.get must_haves for all three plans. The return value wraps the
  actual object under must_haves; all 21 truths, 12 artifact entries and 11 links
  were extracted from that object, not inferred from an empty top-level field.
- verify.artifacts: 03-01 2/2, 03-02 7/7, 03-03 3/3; all_passed true.
- verify.key-links: the text heuristic finds only 1/11. Its original results are
  preserved. All 11 semantic relationships were then inspected directly below.

`summary-queries.json` records roadmap.analyze for later-phase/deferred scope,
summary schema validation, installed verify-summary, coverage classification
and check.decision-coverage-verify. The original accepted profile/implementation
reviews and local/native receipts were reused only where actual immutable bytes
and matching execution records support them. No previous report's status was
used as proof of this procedure.

The installed CLI emits its known unknown-config-key warning for veyr_policy.
The repository's explicit policies still apply; helpers ignoring that key do not
grant permission to commit or advance. No lifecycle mutation helper ran during
goal verification; the subsequently authorized closeout is recorded separately below.

## Roadmap acceptance criteria

| ID | Roadmap contract | Result and concrete evidence |
|---|---|---|
| R1 | Profile defines provisional spelling, examples, supported/unsupported forms, diagnostics, semantic range and non-final status | VERIFIED: published profile sections 1–8 and D-01–D-14 table; all 35 normative rows executed; semantic conversion is absent from lexer.cpp |
| R2 | Owner-reviewed dedicated profile commit precedes syntax-dependent implementation/tests | VERIFIED: profile-publication.json records approved push and actual remote SHA; 03-02 entry records fresh remote/ancestry/content checks and authorization; production first appears in da92389 after 52f598a |
| R3 | Tokens contain only TokenKind/SourceSpan, source owns lexemes and recovered streams end in zero-width EOF | VERIFIED: token.hpp aggregate/static assertions, token_test.cpp lifetime and checked resolution, expect_scan and source-end EOF tests |
| R4 | Recognize profile tokens/ASCII decimals, skip defined whitespace and diagnose unsupported comments/numbers/non-ASCII | VERIFIED: lexer.cpp explicit unsigned-byte branches; 35 exact profile rows, 256 byte cases and named numeric/word/comment tests |
| R5 | Consume the smallest related offending run, report precise source span and continue | VERIFIED: exact normative/mixed token vectors and one-to-one Error assertions, independent delimiters, repeated scans and progress/gap invariants |

All five criteria passed. Their wording was retained as the primary contract;
plan details extend it and do not reduce it.

## All plan-level truths and canonical deduplication

Truth numbering follows each unchanged PLAN frontmatter array. Full source
wording and the merge map are retained in verification/must-have-inventory.json.
Four clear restatements are deduplicated for scoring: 03-01-T1 → R1,
03-02-T2 → R3, 03-02-T4 → R4 and 03-03-T2 → 03-02-T3. All other detail is
retained, including distinct preparation/publication and Wave 2 entry checks.
Thus 5 roadmap plus 21 source truths produce 22 scored truths. Every source row
below is accounted for; deduplication removes no acceptance condition.

| Source truth | Verified contract | Specific evidence |
|---|---|---|
| 03-01-T1 | Experimental P01 one-root/fixed-entry/integer-return profile | R1; profile sections 1–3, no general-language or ABI promise |
| 03-01-T2 | Signed32 / 0–255 belongs to later semantics; huge decimals remain lexical | Profile sections 3–4; DecimalShapeHasNoSemanticRangeOrConversion and LongRunsKeepLexicalShapeAndBoundedProgress |
| 03-01-T3 | Exact experimental signature, braces, one return and semicolon | Profile section 4; FixedSampleHasExactOriginalTokenSpans; lexical program-shape versus future parser scope explicitly distinguished |
| 03-01-T4 | Four whitespace bytes, lowercase whole words and contiguous arrow | Profile 5.2–5.3; KeywordsAreExactCaseSensitiveWholeWords, NumericSignsAndArrowHaveDistinctBoundaries and all-byte tests |
| 03-01-T5 | Invalid/Error, numeric/high-byte/comment grouping and deterministic precedence | Profile 5.3–5.7; exact 35-row recovery table, mixed precedence and hostile comment tests |
| 03-01-T6 | Owner acceptance, dedicated commit, separately approved publication and fresh execution authorization | 03-01 execution/summary and actual profile commit/publication receipts; 03-02 entry authorizes only Wave 2 after verified publication |
| 03-02-T1 | Fresh remote, ancestry and unchanged-profile checks before Wave 2 | 03-02 entry.json: actual remote 030fd0b, query/ancestry exits 0 and equal profile blob; Git ancestry and first-appearance inspection |
| 03-02-T2 | Owned vector, kind/span values and originating-manager lexemes | R3; token.hpp, LexResult, TwoMemberAggregateHasValueSemantics and actual returned-token lifetime test |
| 03-02-T3 | Decimal shape including 256/huge runs has no semantic conversion | lexer.cpp digits_only branch; DecimalShapeHasNoSemanticRangeOrConversion and 65,536-digit regression |
| 03-02-T4 | Approved whole lowercase words, delimiters and arrow | R4; word_kind, punctuation switch, bounded pair_at and complete fixed sample |
| 03-02-T5 | Exact whitespace and unchanged original-byte newline/EOF mapping | WhitespacePreservesLfCrlfLoneCrAndEofOffsets, TokenAndDiagnosticLocationsPreserveOriginalNewlineBytes and eight trailing/empty EOF cases |
| 03-02-T6 | One Invalid and owning Error per rejected run; exact span/order and prior Error preserved | expect_scan; RecoveryAppendsInOrderAndRepeatedScansKeepDuplicates; SharedEnginePreservesEveryPriorSeverityAndRejectsSuccessHandoff |
| 03-02-T7 | Every known scan progresses to exactly one empty source-end EOF | cursor advancement in every branch, bounded lookahead, expect_scan and expect_bounded_scan; empty/error/long-run cases |
| 03-02-T8 | Unknown source returns exact checked error without fabricated diagnostics/EOF; vector is not success | Early source_text/get_if return; UnknownSourceReturnsExactErrorWithoutDiagnosticOrTokens, UnknownSourcePreservesMixedDiagnosticsOnNonemptyManager and success-handoff predicates |
| 03-03-T1 | All 256 values and mixed malformed runs have exact outcomes | Actual byte indices 0–255 and exact expected focused name sets executed in local and every hosted test/verify/prepared stage |
| 03-03-T2 | Huge digits/256 remain lexical; no moved semantic range check | Deduplicated into 03-02-T3; exact corresponding native cases passed |
| 03-03-T3 | Word/arrow/numeric/comment boundaries, original locations and positive recovery | NumericRecoveryRetainsEveryIndependentDelimiter, AdversarialMixedPrecedenceHasExactIndependentOracles, newline/EOF and gap/progress assertions |
| 03-03-T4 | Ordered Errors survive shared engine and separate rendering | SharedEngine, RealLexicalErrorsRenderHostileBytesIntoStableOwnedAscii and RenderingCheckedFailureCannotClearAccumulatedLexicalError |
| 03-03-T5 | Returned tokens/views survive caller destruction and additional snapshots | ReturnedTokensOutliveCallerStorageAndFurtherRegistrations: result/engine/caller scopes end, manager remains; 3,000 additional registrations and saved view identity |
| 03-03-T6 | Independent local Debug/Release test/identity/include records retained | Original 16 commands, selected doctor/build directories, 419/419 local logs, 13 compilation entries and direct include logs; original hash mappings retained |
| 03-03-T7 | New four-job CI and canonical verification/owner gates remain distinct; platforms scoped | Actual hosted.json/run.json and this executed procedure; no completion helper, state/requirement mutation or Phase 4 work; deferrals below |

**Result:** 21/21 original plan truths covered; 22/22 merged scored truths VERIFIED.
No behavioral truth was accepted on symbol presence alone. Named tests and
actual Passed records support each runtime invariant; no behavior-unverified
item or formal must_haves.prohibitions descriptor exists in these plans.

## Artifact verification — 12/12 entries, 10 distinct paths

Canonical verify.artifacts checks existence/substance. Direct inspection and
runtime evidence establish wiring, rather than treating the query as sufficient.

| Plan | Artifact | Substantive implementation and wiring |
|---|---|---|
| 03-01 | docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md | Complete 14-decision experimental contract; exact published blob governs the implementation/tests |
| 03-01 | 03-01-EXECUTION.md | Owner decisions, dedicated SHA, real publication and final bookkeeping receipts; referenced by summary and Wave 2 entry |
| 03-02 | include/veyr/lexing/token.hpp | Thirteen kinds, exactly kind/span value aggregate; included and consumed by lexer/tests |
| 03-02 | include/veyr/lexing/lexer.hpp | Checked synchronous LexResult API with ownership/error/exception contract; used by implementation and native clients |
| 03-02 | src/lexing/lexer.cpp | Complete checked byte scanner, fixed owning Errors and EOF; compiled into veyrcore and invoked by tests |
| 03-02 | tests/lexing/token_test.cpp | Value, checked resolution and actual snapshot lifetime assertions; discovered in lexer_tests |
| 03-02 | tests/lexing/lexer_test.cpp | Exact tokens/spans/messages, 35 normative and 256 byte cases plus integration; all discovered and executed |
| 03-02 | CMakeLists.txt | src/lexing/lexer.cpp is a real veyrcore member, subject to existing target policy |
| 03-02 | tests/CMakeLists.txt | lexer_tests contains both test files, links veyrcore/GTest/GMock and uses PRE_TEST discovery |
| 03-03 | tests/lexing/token_test.cpp | Additional returned-token/caller-storage/registration regression executes through public lex API |
| 03-03 | tests/lexing/lexer_test.cpp | Additional all-byte, large-run, newline, accumulation and renderer regressions execute unchanged production code |
| 03-03 | 03-03-EXECUTION.md | Actual local review/publication/hosted/canonical checkpoint evidence and explicit remaining owner gates |

## Key links — 11/11 direct inspections

The installed text helper reports 1/11 because it searches target-path spellings;
canonical angle includes omit the include/ prefix, CMake paths are relative and
some links represent semantic/documentary contracts. Original helper failures
remain in canonical-queries.json. The following direct checks resolve those
heuristic misses without altering code or weakening the contract.

| Link | Direct evidence |
|---|---|
| Profile → CONTEXT | Profile's D-01–D-14 table and normative sections correspond to the actual accepted decisions; canonical decision-coverage query reports 14/14 |
| 03-01 execution → profile | Accepted path/SHA/hash and actual origin/main publication receipt; unchanged profile blob verified |
| token.hpp → source_span.hpp | `#include <veyr/source/source_span.hpp>` and `SourceSpan span`; aggregate/type assertions and checked lexeme tests |
| lexer.cpp → SourceManager | lexer.hpp includes the manager; lex calls source_text exactly once and handles SourceError before any scan/emission |
| lexer.cpp → DiagnosticEngine | lexer.hpp imports engine; Invalid branch calls emit with explicit Error, owning string, exact span and empty related vector |
| lexer.cpp → profile | Inspected precedence, words, numeric/comment/high-byte grouping and fixed messages match the published profile; all 35 exact normative oracles execute |
| tests/CMakeLists → lexer_test.cpp | `lexing/lexer_test.cpp` listed inside lexer_tests; PRE_TEST discovery, runtime Passed names and compilation database entries demonstrate actual use |
| lexer_test.cpp → checked lexer API | Canonical lexer.hpp include and repeated real lex calls, asserted result variants and complete expected vectors |
| lexer_test.cpp → PlainTextRenderer | Canonical renderer include, real render calls, exact escaped owned output and preserved original RenderError/SourceError |
| lexer_test.cpp → SourceManager | Real registration/text/location calls, byte spans, caller-storage lifetime and further registrations |
| 03-03 execution → validation contract | Relative 03-VALIDATION.md link resolves; actual selected commands, raw receipts, counts, hashes and platform gates implement its independent validation rows |

The SourceManager → lex → owning DiagnosticEngine → separate PlainTextRenderer
data flow is exercised with actual hostile bytes and names. Assertions compare
complete owned output, not presence flags. Borrowed diagnostic views end before
emit; copied owning diagnostics and stored rendering remain valid afterward.

## Requirements and decision coverage

| Requirement | Result | Implementation and runtime evidence |
|---|---|---|
| LEX-01 | SATISFIED | Published owner-reviewed profile; exact commit/content/publication-before-code history and documentary checks |
| LEX-02 | SATISFIED | token.hpp, checked SourceManager lexemes, type/value and lifetime tests |
| LEX-03 | SATISFIED | Fixed recognition and numeric scanner; exact sample/word/arrow/numeric tests and all-byte outcomes |
| LEX-04 | SATISFIED | Explicit four whitespace bytes, comment/high-byte grouping and original location tests |
| LEX-05 | SATISFIED | One final zero-width EOF for known sources, no fabricated EOF on infrastructure error |
| LEX-06 | SATISFIED | Exact Invalid/owned Error pairing, recovery, diagnostic insertion order and persistent Error state |

Canonical check.decision-coverage-verify actually ran: **14 honored / 14 total**,
not_honored empty. Its warning-only textual result is supplemented by direct
inspection of the accepted profile and implementation/tests:

| Decisions | Preserved behavior |
|---|---|
| D-01 / D-02 | Experimental 0–255/signed32 semantics remain later work; 256 and huge digit strings lex successfully |
| D-03 / D-04 | Exact provisional fn/main and int32 spellings; no final grammar/type/ABI guarantee |
| D-05 / D-06 | Documented fixed signature, braces, return and semicolon; lexer recognizes units, not program structure |
| D-07 | Only space/TAB/LF/CR skipped; byte-based source locations unchanged |
| D-08 / D-09 | Four lowercase whole-word matches; unsupported words remain whole Invalid runs |
| D-10 | Contiguous arrow precedes numeric handling; spaced fragments remain invalid |
| D-11 | Invalid plus Error, insertion order, EOF and Error-preserving caller success contract |
| D-12 | Whole unsupported numeric-like grouping; no salvaged fragments or swallowed independent delimiters |
| D-13 | Consecutive high-byte runs without Unicode decoding |
| D-14 | Rejected comment-shaped runs own their contents; first eligible closer/EOF, no nesting; `/*/return` has no Return token |

Requirement satisfaction here is verification evidence, not new ledger mutation.
Existing accepted scopes/counts remain 27 accepted/22 pending until separately
authorized final owner reconciliation. No future parser, AST, semantics, module,
backend or compiler-driver success capability is credited to this phase.

## Native, hosted and prior-phase behavioral evidence

Original independent Omarchy Debug/Release local validation remains intact:
doctor 22/22, focused 323/323, new 268/268, byte cases 256/256, normative 35/35,
existing components 61/61, full CTest 419/419, 13 selected compiler/directory/C++23
entries and direct includes PASS per configuration. There was no package
acquisition/rebuild or production correction. The 16 command-log hashes and
62-entry manifest retain their original bytes and approved archive resolution.

| Hosted job at be84bb9702afa75ad07803a678e7fd5a4e6358d7 | Job | Native test | Verify | Prepared acceptance | Empty-cache rejection |
|---|---|---|---|---|---|
| ubuntu-ci-Debug | success | 419/419 | 419/419; 13 entries | 419/419; 13 entries | expected, once |
| ubuntu-ci-Release | success | 419/419 | 419/419; 13 entries | 419/419; 13 entries | expected, once |
| windows-ci-Debug | success | 419/419 | 419/419; 13 entries | 419/419; 13 entries | expected, once |
| windows-ci-Release | success | 419/419 | 419/419; 13 entries | 419/419; 13 entries | expected, once |

All stages include exactly the expected 323 focused names, all byte indices
0–255 and all 35 normative names. include-policy directly invokes the Python
guard through CTest and passed in each of the three executions per job; no
separate standalone hosted invocation is claimed. All normal/prepared paths
passed; missing-gtest/no-remote diagnostics occur in the intentionally isolated
empty-cache case, whose checker confirms no acquisition/retry.
Actual results and raw-log SHA-256 are retained in verification/hosted.json and
hosted-run.log; run.json independently confirms completed/success and job/commit
association. This is new Phase 3 hosted evidence, not old Phase 2 or cache-hit
inference. Run URL: https://github.com/m4rkens/VeyrLang/actions/runs/39107885454.

The canonical prior-phase regression/behavior gate additionally ran one fresh
complete prepared Debug CTest suite during this verification: 419/419, exit 0,
6.968 seconds recorded command time. Its actual Passed set includes all 96
original Phase 2 native test names; none disappeared. It also directly executes
the named ordering, lifetime, checked-error and renderer state tests referenced
above. Release reuses independently accepted unchanged local and current hosted
evidence; no second full suite was redundantly run per truth. The direct command
`/usr/bin/python3 -B scripts/check_includes.py --root .` also exited 0.
Actual argv/cwd/time/exit/log hashes are in verification/native-commands.json.
No project configure/build, Conan setup, download or dependency rebuild ran.

P01 is design-probe pressure for the later compiler walking skeleton. No
standalone runnable compiler probe/driver command is declared for Phase 3;
no native executable-compilation capability or fabricated probe result is claimed.

## Architecture, anti-patterns and integrity

- Token values contain only kind/span; SourceManager owns immutable source bytes.
- Lex obtains one checked source view and retains none after return. Unknown
  input preserves its exact SourceError without emission or fabricated token stream.
- Pair lookahead subtracts remaining length before addition; each branch advances.
  Comment searches start after the complete opener, with checked closing-pair bounds.
  Classification uses unsigned bytes and explicit ASCII ranges, with no locale.
- Every rejected run emits exactly one initialized owning Error and empty related
  spans. No prior diagnostic is cleared, sorted or deduplicated. Allocation exceptions
  retain documented ordinary C++ behavior, not a new rollback/OOM policy.
- Rendering remains a separate checked test consumer. No LLVM/parser/AST/semantic,
  filesystem, terminal, Unicode-decoder or new package dependency enters the lexer.
- No unreferenced TBD/FIXME/XXX or TODO/HACK/PLACEHOLDER marker was found in the
  profile, token/lexer implementation, tests or CMake integration changed by this phase.
  No missing production branch, placeholder return or unused public entry was found.
- Canonical native discovery and actual Passed names demonstrate target/source wiring;
  no cache hit, registration alone or successful dependency setup is treated as proof.
- The accepted profile remains byte-identical to 52f598a, SHA-256
  1b5cefb5fa40a4a940ce1c0f4a4d7b8049ce591b81b79ecaf2251e5dfb107864.
  Its original draft/pending prose is the immutable owner-accepted snapshot;
  later approved execution/summary/publication records supply actual gate status.
- Actual history places profile acceptance/publication before lexer implementation;
  production first appears in da92389. Historical publication and fresh Wave 2
  remote/ancestry/content receipts were inspected, not replaced by a stale ref.
- Accepted source, tests, CMake, locks and toolchain/dependency policies remain unchanged.
  Earlier 65-entry and 36-entry packets retain their approved historical mapping.

## Findings, human verification and platform scope

**BLOCKER:** None. **Unverified behavior / human verification items:** None for
the authorized Phase 3 scope. Existing explicit owner profile/local acceptance
has occurred; final owner phase acceptance remains a procedural gate after this
verification and is not fabricated as a runtime verification result.

**Non-blocking follow-ups, unchanged and not implemented:**

- Explicitly enforce Diagnostic::severity initialization before widespread use;
  all new lexer/test emissions initialize it today.
- Add combined left-clipping/mixed-escaping/empty-span renderer regression.
- Consider documenting/testing unsupported initialized severity values.
- Optional per-case traces for looped long-run/EOF test fixtures.

**Tooling limits:** GSD key-link helper misses were resolved by direct wiring
inspection above; raw helper results remain unchanged. The known config-key
warning is administrative. Historical STATE/ROADMAP/PROJECT prose is due for
authorized closeout reconciliation and is not interpreted as current execution
truth. 03-VALIDATION.md remains the approved historical planning strategy rather
than a falsely updated execution report. No absent summary or old draft flag
was used to infer either implementation failure or success.

Platform impact:
- Windows: Hosted Debug/Release native, verification and acceptance now validated.
  Developer-host inspection remains independently pending/non-blocking; hosted
  success is not local-host evidence. Existing compiler/CRT/DevShell policy unchanged.
- Linux: Accepted independent Omarchy Debug/Release and Ubuntu hosted Debug/Release
  remain distinct reviewed environments; fresh canonical Debug regressions passed.
- macOS future: Deferred. No Darwin validation, CI or support is claimed.

## Final verification boundary

All five roadmap criteria, all 21 original plan truths, 22 deduplicated scored
truths, 12 artifact entries, 11 directly inspected links, six requirements and
14 accepted decisions are supported. No gap was hidden by later-phase deferral
or override. The actual canonical verification.status query ran against this
completed report and returned the following (exit 0; raw receipt in
verification/verification-status.json):

```json
{
  "status": "passed",
  "next_action": "Verification passed — continue.",
  "next_command": ""
}
```

The verification frontmatter schema query also returned valid true, with no
missing fields. These query receipts complement the actual inspection and
execution evidence; report existence or a frontmatter string alone is not
treated as execution of verification. The generic next_action does not override
the remaining owner gates.

At the verification checkpoint, no state.advance-plan, roadmap.update-plan-progress,
requirements.mark-complete, phase.complete, metadata commit, automatic UAT
transition or Phase 4 routing had run. Phase 3 remained 2/3 at that checkpoint;
those historical verification limits are preserved in the accepted archived report.

## Subsequent owner acceptance and closeout preparation

The owner ACCEPTED Phase 3 and all six requirements after reviewing the findings
above. Authorized bounded documentation-only bookkeeping now completes Plan 03-03
and Phase 3 at 3/3, preserving Phase 1 8/8, Phase 2 4/4 and requirements
27 accepted/22 pending. Supported state.complete-phase --phase 3 and
roadmap.update-plan-progress 3 operations preserve the closed Phase 3 pointer;
unrestricted phase.complete and any Phase 4 work remain forbidden. Actual
closeout receipts are separate from the original verification records.

This acceptance annotation changes no score, truth, artifact, link, test result,
gap, deferral or technical finding. The original accepted report and the other
pre-closeout planning inputs are archived under .veyr-dev/execution/03-03/closeout/
accepted-inputs/; the accepted verification manifest is unchanged. Completion
documentation awaits explicit owner commit/publication approval. No new test or
verification outcome is inferred from owner acceptance or this bookkeeping.
