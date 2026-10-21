---
phase: 04-smoke-profile-parser
plan: "03"
wave: 3
status: local_validation_complete_owner_review_pending
plan_completion: pending
requirements: [PARSE-01, PARSE-02, PARSE-03]
requirements_completed: []
owner_acceptance: pending
publication: not_authorized
hosted_acceptance: pending_for_this_packet
phase_goal_verification: not_performed
recorded_at: "2026-10-21T09:34:22.377089+00:00"
---

# Plan 04-03 — Local integration execution checkpoint

## Authorization and canonical selection

The owner explicitly authorized `$gsd-execute-phase 4 --wave 3`, Tasks 1–2
only, at published baseline `02e01063f4b5c4a70a9cfbc68b1749a8b87a2437`.
The installed GSD execute-phase argument parser supports `--wave`; its actual
init/index queries find three sequential plans, 04-01/04-02 summaries and only
04-03 incomplete in wave 3. Work is isolated on `phase4/04-03-parser-integration`
in the existing Phase 4 worktree. The prior branch is preserved.

Execution followed the installed execute-plan task/checkpoint procedure inline,
as permitted by its Codex adapter and the repository's no-automatic-subagent
policy. The existing veyr_policy configuration warning and two planning-support
filename warnings do not change the three executable plans or selected wave.
No lifecycle mutation, summary creation, requirement completion, automatic commit,
phase verification or next-stage transition was performed. The explicit owner
checkpoint overrides generic automatic final-summary and phase-completion steps.

## Task 1 — Real component integration

Created [parser_integration_test.cpp](../../../tests/parsing/parser_integration_test.cpp)
and added it to the existing parser_tests source list in
[tests/CMakeLists.txt](../../../tests/CMakeLists.txt), preserving original tests,
links, target policy and PRE_TEST discovery. No production/API change was needed.

The 32 new native test identities consist of seven standalone cases, six decimal
rows, eleven lexical-handoff rows and eight EOF/location rows. All execute in both
configurations. Independent expectations come from the approved 04-03 fixtures,
accepted M1 profile and D-01–D-16; no production expectation table is imported.
Decimal offsets use the literal fixture prefix/suffix lengths, without conversion.

| Coverage / decisions | Actual integration case identities |
|---|---|
| Owned spans, source lifetime, copy/move, tight byte bounds; D-01–04, D-10 | `SmokeProfileParserIntegration.OwnedThreeSpanResultOutlivesTokens`; `WhitespaceAndPunctuationKeepTightSyntaxBounds` in the same suite |
| Decimal syntax only; D-02/03/16 | `DecimalShapeDoesNotPerformSemantics/{Zero,FortyTwo,ByteMaximum,BeyondByteRange,LeadingZeros,SixteenThousandDigits}` |
| Lexical suppression and no partial value; D-06/08/10/16 | `InvalidStreamPreservesEveryLexicalDiagnostic/{BeforeEntry,PositiveSign,NegativeSign,NonDecimal,BetweenIntegers,AfterEntry,HighBytes,EmbeddedNul,OverlappingBlockDelimiter,EarlierSyntaxMismatch,OrderedDuplicateErrors}` |
| Persistent prior state and ordered duplicates; D-05–09/12/13 | `SmokeProfileParserIntegration.EarlierErrorsDoNotGateValidSyntax`; `RepeatedSyntaxErrorsPreserveOrderedDuplicates` |
| Original EOF/locations and one matching opener; D-05–07/12–15 | `OriginalNewlineEofLocations/{BodyLF,BodyCRLF,BodyLoneCR,ParenLF,ParenCRLF,ParenLoneCR,Empty,WhitespaceOnly}` |
| Separate owning safe rendering; D-01/07/12–15 | `SmokeProfileParserIntegration.ParserErrorsRenderWithEscapedOwningMetadata` |
| Checked alternatives/full-stream precedence; D-06/07/11 | `SmokeProfileParserIntegration.UnknownSourcePropagatesWithoutDiagnosticsOrValue`; `CheckedCorruptionOutranksRealLexicalRejection` |

The parameterized identities have prefix
`SmokeProfileParserIntegrationCases/SmokeProfileParserIntegrationDecimal.`,
`...Lexical.` or `...Eof.` respectively; coverage.json and validation.json retain
all exact full names, discovery records and executed-name reconciliation.
Each loop has a subcase trace. Rendering separately exercises Paren, Body and
Semicolon, including literal complete escaped output and raw diagnostic fields.

The retained result resolves original entry [1,39), return [23,36), integer
[30,35) after caller strings, token vector, LexResult, ParseResult and local
engine destruction, copy/move operations and 3,000 further registrations.
The originating SourceManager remains alive. The 16,384-digit case is syntax
only. No numeric range, AST, source ownership or cross-manager guarantee is added.

The `/*/return` fixture asserts exactly Invalid [0,9), EOF [9,9), one lexical
Error, no Return token and no appended parser diagnostic. Other lexical rows
preserve every owning diagnostic field/order and existing Error state. Separate
valid syntax with prior Errors yields a value but fails a test-local successful
handoff predicate; recovered or parsed values never imply compilation success.

## Task 2 — Independent final-code native verification

| Gate | Linux Debug | Linux Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Configure / incremental build | PASS | PASS |
| Parser discovered / executed | 142/142 | 142/142 |
| New integration / retained parser cases | 32/32 + 110/110 | 32/32 + 110/110 |
| Retained original / 04-02 conformance cases | 13/13 + 97/97 | 13/13 + 97/97 |
| Selected source/diagnostic/lexer regressions | 93/93 | 93/93 |
| Full native CTest | 561/561 | 561/561 |
| Strict verification, including full CTest | PASS; 561/561 | PASS; 561/561 |
| Compiler/directory/C++23 entries | 16/16 | 16/16 |
| Direct include policy | PASS | PASS |

All 529 prior full-suite identities plus exactly 32 new identities appear in
each final test and strict-verify output. Exact discovery matches execution,
not just an aggregate total. All three parser translation units are selected
with `/usr/bin/clang++`, `-std=c++23` and their configuration-specific directory.
Focused CTest times were 1.95 seconds Debug and 1.83 seconds Release.

The exact commands, cwd, selections, UTC start/end, exit codes and raw-log hashes
are retained in ignored `.veyr-dev/execution/04-03/commands.jsonl`: 19 receipts,
18 successful commands and one sandbox-blocked initial configure. The latter
could not acquire the normal Conan lock on a read-only sandbox path; its approved
permitted rerun succeeded. It was an environment restriction, not an expected
RED test or compiler defect. No native test failed. No healthy command was killed
or duplicated; no hard timeout or cache cleanup was used.

Both configure logs record pinned Conan 2.33.0 `--no-remote --build=never`, existing
LLVM 23.1.2 and GoogleTest 1.18.0 packages. Normal usage metadata/locking remained
allowed. No dependency download, source build, revision, recipe, lock, profile or
toolchain change occurred. Existing generated Conan-toolchain quotation warnings
also appear in accepted earlier logs; no warning-policy suppression was added.

## Evidence integrity and unchanged scope

Ignored review evidence is under `.veyr-dev/execution/04-03/`: REVIEW.md,
evidence.json, commands.jsonl, coverage.json, validation.json, prepared Debug/Release
metadata/compile databases, native logs, baseline archives and the exact candidate
diff. Baseline tests/CMakeLists.txt and parser_test.cpp were archived before edits.
Source hashes are below; final evidence.json covers this checkpoint without a
self-hash cycle. Historical manifests and receipts were never rewritten.

Archive-aware revalidation passed: 04-02 local 70/70 and command logs 27/27,
preparation 29/29, formal closeout 16/16; 04-01 local 75/75 and command logs 31/31,
formal closeout 20/20 and earlier preparation 11/11; all 201 historical entries.
Held Stage 1 evidence 76/76 and Stage 2 correction evidence 19/19 remain valid.
Original local main and the held Stage 1, Stage 2 and abandoned phase-end CI
candidates retain their baseline HEADs, file hashes and empty indices.

The only tracked publication candidates are the new integration test, its existing
CMake source list and this execution record. Original parser_test.cpp, production
code/APIs, published M1 profile, four Conan locks, recipes, profiles, workflow,
policies, STATE, ROADMAP and REQUIREMENTS remain unchanged.

## Owner checkpoint and outstanding gates

Tasks 1–2 are locally implemented and validated; independent owner review and
acceptance remain pending. No acceptance blocker or production defect was found
by these tests. This is implementation evidence, not final phase acceptance.

Formal state remains: 04-01/04-02 complete; 04-03 incomplete; Phase 4 2/3,
overall 17/18. PARSE-01/02/03 remain Pending, global 27 accepted/22 pending.
No canonical 04-03-SUMMARY.md was created. The documented cross-plan requirement
exception remains effective until separately authorized final phase acceptance.

Next gates are independent implementation/evidence owner review; explicit
sensitive-file commit and publication approval; fresh Ubuntu/Windows four-job
hosted validation at the actual published implementation; separately authorized
canonical phase-goal verification and final owner acceptance/closeout. No manual
workflow operation is authorized here. The prior documentation-only run
[39574720208](https://github.com/m4rkens/VeyrLang/actions/runs/39574720208) is
observed separately in prior-ci.json; pending work there is not a local execution
prerequisite and does not establish hosted acceptance of these new tests. The
latest read-only capture in prior-ci-latest.json now reports completed/success
for that exact baseline commit, with all four jobs successful. No new test
packet was published or hosted run dispatched.

Stage 2 performance remediation remains deferred and independent of compiler
work. No Stage 1/Stage 2 or held CI-policy change is incorporated.

Platform impact:
- Windows: New exact-byte, lifetime, diagnostics and rendering tests require fresh hosted Debug/Release acceptance after separately authorized publication. Developer-host inspection remains independently pending/non-blocking.
- Linux: Independent local Debug/Release evidence passed; this does not replace Ubuntu hosted acceptance of the published packet.
- macOS future: Deferred; no filesystem/terminal/OS-specific parser behavior introduced.

## Final technical-source SHA-256

- `tests/parsing/parser_integration_test.cpp`: `6717598e8f727dc680c0f29fd6bda8d8e9942def0d5cc2bfb8949642f9a5d7a9`
- `tests/CMakeLists.txt`: `32e8fd7db8a3368292f2098792bf05b3268292e84f835b5934005f30bf63cf78`
- `tests/parsing/parser_test.cpp`: `f9fd7899506a0c2aa227c3dcfe72d3a1da5cedb9ffb76fc963f010ff00256693`
- `src/parsing/parser.cpp`: `91cb9612310dcebf4eb8018353e15fddbb1ddb5dbb9a469a401e41972af6ac8c`
- `include/veyr/parsing/parser.hpp`: `6f9c9cf002619cc4db40d16fe686e92018dfcdd9e7a7c05c4282384535e3a476`
