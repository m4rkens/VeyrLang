---
phase: 04-smoke-profile-parser
plan: "01"
status: complete
tasks_locally_complete: 2
owner_acceptance: Formal Plan 04-01 accepted with established Linux and four-job hosted evidence; PARSE requirement acceptance explicitly deferred
checkpoint_type: human-verify
owner_review: passed
plan_completion: complete
formal_acceptance_recorded_at: "2026-10-20T15:50:37.879175+00:00"
requirements_completed: []
requirements_exception: Owner-approved PARSE-01 through PARSE-03 deferral across subsequent Phase 4 plans
hosted_parser_validation: passed; run 39222510470 for 425e5ff456a59cafc04a2fa46ddbaa0f2318e792
---

# Phase 4 Plan 01 — Checked Smoke Parser Execution

## Formal Plan 04-01 acceptance and local closeout — 2026-10-20

The owner explicitly approved bounded local formal completion, recorded at
2026-10-20T15:50:37.879175+00:00. Published implementation and hosted acceptance were
already established; no native command, dependency operation or new CI run was
performed. Plan 04-01 is complete, Phase 4 is 1/3 formally complete, and 04-02/03
remain unstarted. STATE records 16 completed plans, up from the verified 15.

**Owner-approved requirement exception:** PARSE-01, PARSE-02 and PARSE-03 stay
pending because full acceptance extends across later Phase 4 plans. The generic
requirements.mark-complete step and automatic summary requirements-completed
population are deliberately not applied. Definitions/mappings and global counts
remain 27 accepted/22 pending. This is plan acceptance, not phase-wide acceptance.

The reviewed candidate and original evidence were preserved in separate ignored
formal-closeout/reviewed-inputs archives. Summary discovery was temporarily removed
while the worktree orchestrator reconciled these four documents and finalized the
canonical summary last. Installed schema/coverage/self-check and final phase-index
queries verify the resulting state; their evidence belongs to formal-closeout/.
No unrestricted lifecycle or commit helper ran and no installed GSD code changed.

Remaining gates are independent final lifecycle review, separate closeout commit
and publication authorization, and fresh explicit 04-02/Wave 2 authorization.
Mechanical prerequisite satisfaction is not execution authorization. Phase 4
goal verification, final acceptance and all PARSE completion remain later.
Stage 1 and Stage 2 remain isolated; deferred performance work is not a blocker.

## Historical publication and closeout preparation checkpoint — 2026-10-20

The following section records the reviewed preparation state before the formal
acceptance above. Its pending-closeout wording is historical, not current status.

The owner separately authorized publication of the seven-file implementation
checkpoint, committed and pushed as 425e5ff456a59cafc04a2fa46ddbaa0f2318e792.
[Build foundation run 39222510470](https://github.com/m4rkens/VeyrLang/actions/runs/39222510470)
is independently observed completed/success for that exact commit. All four
Ubuntu/Windows Debug/Release jobs succeeded. Actual logs contain all thirteen
parser test names, 432/432 tests in native test/verify/prepared acceptance and
fifteen selected C++23 entries per configuration. Native configure/build,
include verification, prepared acceptance and acquisition-disabled isolated
empty-cache rejection succeeded. Linux owner acceptance remains distinct from
Windows hosted validation; Windows developer-host inspection is not credited.

The owner now authorized clean Phase 4 worktree preparation and bounded closeout
preparation only. Branch phase4/parser-closeout in .veyr-dev/worktrees/phase4
starts at verified published main b466763b77f8dbf39b4ec8b8ad5efdd0e45332d9.
The original Stage 1 candidate and separate Stage 2 correction are excluded and
preserved. Stage 2 performance remediation remains independent and deferred.

[04-01-SUMMARY.md](04-01-SUMMARY.md) is a pending owner-review candidate using
the installed execute-plan summary structure; it does not close the plan.
No completion date, requirement acceptance, progress advance, commit or push is
authorized here. Phase 4 remains 0/3, PARSE-01–03 pending and global requirements
27 accepted/22 pending. Plans 04-02/03 remain unstarted. The canonical earlier-wave
safety gate still applies; summary filename existence alone is not authorization.

Original Linux evidence remains in the original checkout's ignored
.veyr-dev/execution/04-01/. Its 75-entry manifest, 31 command logs, five accepted
implementation hashes, six immutable receipts and 201 historical references
were checked again through the existing archival mappings. New read-only hosted
captures and checks are separate in this worktree's ignored
.veyr-dev/execution/04-01/closeout-preparation/. Nothing rewrites the original
evidence or claims another native run.

Remaining gates: independent review and explicit formal 04-01 closeout/bookkeeping
approval, separate closeout commit/publication approval, then fresh Wave 2
authorization. Exhaustive malformed/precedence/syntax tests remain 04-02; actual
token-storage destruction, source lifetime and renderer integration remain 04-03.
Canonical phase-goal verification and final Phase 4 acceptance remain later.

## Historical accepted local execution record

The following original record describes the pre-publication Linux checkpoint.
Its then-pending publication and hosted gates were subsequently satisfied as
recorded above; formal closeout and later-wave gates have not been satisfied.

**Date:** 2026-10-18. **Scope:** owner-invoked `$gsd-execute-phase 4 --wave 1`,
04-01 Tasks 1–2 only. **Base:** `5222c585686eecb0eeabca049b6f01d610716a5c`,
shared `main` working tree; sequential executor, no worktree or nested agents.
Both tasks are implemented, locally validated and owner-accepted for the Linux
developer scope on 2026-10-19. Formal plan acceptance/closeout, publication and
later waves remain pending. No SUMMARY, requirement completion, staging, commit,
push or lifecycle advance was performed.

## Implementation

| File | Change |
|---|---|
| `include/veyr/parsing/parser.hpp` | Exact three-span `ParsedSmokeEntry`, two rejection kinds, seven stream-error kinds, selected-source/optional-index stream error, four-way `ParseResult`, synchronous nodiscard `parse` declaration and lifetime/error contracts |
| `src/parsing/parser.cpp` | Complete source/stream validation, Invalid gate, twelve-position fixed recognizer, one owning first-mismatch Error, controlled labels and matching opener context |
| `tests/parsing/parser_test.cpp` | Four public-value tests and nine baseline tests using independent expected data and actual lexer output |
| `CMakeLists.txt` | Add only `src/parsing/parser.cpp` to existing `veyrcore` |
| `tests/CMakeLists.txt` | Add only `parser_tests`, existing target policy, private veyrcore/GTest links and PRE_TEST discovery |

The public value owns exactly `entry_span`, `return_span` and `integer_span`.
`ParseResult` distinguishes that complete value, `ParseRejected`, original
`SourceError`, and `TokenStreamError`. Original errors retain every field.
Malformed streams and Invalid rejection append no parser diagnostic.

The implementation checks selected-source lookup, then every token in increasing
index: existing checked span, source agreement, explicit supported-kind switch,
EOF/non-EOF shape, then order/nonoverlap. Missing EOF follows all existing-token
checks. Complete structural validation precedes remembered Invalid rejection,
which precedes syntax. A later structural error therefore outranks earlier
Invalid or a potential syntax mismatch. No fabricated EOF, clamping, assertion-only
validation, relexing, spelling certification, conversion or numeric range check.

Recognition requires exactly the fixed eleven real tokens and final EOF.
Success retains tight original-byte spans, including [0,33), [21,31), [28,30)
for the fixed sample. Each first mismatch appends one explicitly initialized
Error with its full actual-token or exact EOF primary; only expected `)`/`}`
adds one matching opener. No partial result is exposed. Prior diagnostics and
Error state persist; an independently valid parse value cannot establish
compilation success while an Error remains. Ordinary allocation exceptions and
the accepted conforming-producer/manager-local identity limitations are unchanged.

## Test-first task gates

Task 1 authored the four value tests and registered their target before creating
the header. The authorized Debug RED build exited 1 with exactly
`fatal error: 'veyr/parsing/parser.hpp' file not found`. After the public header
was added, Debug and Release each built and discovered/executed all four tests.
There was no call to an undefined parser and no parser stub in Task 1.
The Task 1 acceptance receipt and exact source snapshot are retained locally.

Task 2 authored the nine baseline tests before creating parser.cpp. Its authorized
Debug RED build compiled the tests and exited 1 at undefined `veyr::parse`
linkage. After the complete implementation and root source-list entry were added,
both configurations built and all thirteen focused tests passed. No source,
test or CMake bytes changed after the final Debug build began. Their recorded
modification times precede both final builds; the final hashes identify the
source validated by both selections.

Expected RED failures are recorded separately from environment failures and
GREEN results. Generic GSD RED/GREEN/per-task/summary commits were intentionally
not performed under the explicit owner and repository commit gates.

## Independent native results

Every native command ran individually from
`/home/m4rkens/Projects/VeyrLang`, using explicit
`--profile linux-x86_64-clang --environment linux-developer --config Debug|Release
--llvm-provider managed` with `/usr/bin/python3 scripts/dev.py`.
Each task/configuration has its own read-only doctor JSON and selected-directory
evidence. Doctor reports prerequisite readiness only; its native/package readiness
fields remain `unverified` and are not relabelled as public acceptance.

| Gate | Debug | Release |
|---|---:|---:|
| Task 1 doctor | 22/22 | 22/22 |
| Task 1 named value discovery/execution | 4/4 | 4/4 |
| Task 2 doctor | 22/22 | 22/22 |
| Task 2 named parser discovery/execution | 13/13 | 13/13 |
| Specified anchored existing-component regression | 93/93 | 93/93 |
| Full `dev.py test` | 432/432 | 432/432 |
| Strict `dev.py verify` CTest | 432/432 | 432/432 |
| Selected compiler/directory/C++23 entries | 15/15 | 15/15 |
| Direct `python -B scripts/check_includes.py --root .` | PASS | PASS |

The exact anchored regression expression selects 93 tests. Parameterized lexer
suites whose names begin with their instantiation prefix are covered by the full
suite: all 323 inherited smoke-token/lexer tests passed there in both configurations.
Historical totals 419 tests/13 compile entries were not used as new evidence.
The actual addition is thirteen tests and two translation units.

Actual absolute directories selected by the respective doctor runs:

- Debug: `/home/m4rkens/Projects/VeyrLang/build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Debug`
- Release: `/home/m4rkens/Projects/VeyrLang/build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Release`

Each fifteen-entry compile database has exactly the expected source membership,
the selected directory, `/usr/bin/clang++`, `-std=c++23`, existing warning policy,
and the appropriate Debug `-g` or Release `-O3 -DNDEBUG` flags. Both new entries
`src/parsing/parser.cpp` and `tests/parsing/parser_test.cpp` are present.
The observed host compiler is Clang 22.1.8 on Omarchy 4.0.4; LLVM backend
23.1.2 is independent. Python 3.14.7, Conan 2.33.0, CMake 4.4.3, Ninja 1.13.2,
GCC/libstdc++ 16.2.1, glibc 2.44 and Binutils 2.47 passed doctor checks.

All thirteen names were independently reconciled between discovery and Passed
output in both configurations:

| Suite | Test |
|---|---|
| SmokeProfileParserValue | ThreeSpansHaveValueSemantics |
| SmokeProfileParserValue | CheckedAlternativesRemainDistinct |
| SmokeProfileParserValue | CopiesResolveOriginalSnapshot |
| SmokeProfileParserValue | ManagerSelectionIsCallerResponsibility |
| SmokeProfileParser | FixedEntryHasTightSpans |
| SmokeProfileParser | DecimalTokensDeferConversionAndRange |
| SmokeProfileParser | MissingSemicolonReportsUnexpectedBrace |
| SmokeProfileParser | MissingBodyCloserReportsEofAndOpener |
| SmokeProfileParser | TrailingTokenCannotProduceValue |
| SmokeProfileParser | UnknownSelectedSourcePreservesOriginalError |
| SmokeProfileParser | MalformedTokenStreamEmitsNothing |
| SmokeProfileParser | InvalidTokenSuppressesParserDiagnostic |
| SmokeProfileParser | EarlierErrorDoesNotGateIndependentValidSyntax |

## Environment gates and observations

The first Debug configure attempt exited 2 before CMake because the sandbox made
the external Conan operation-lock path read-only. Under the runtime's mandatory
sandbox escalation procedure, approval was requested and granted for the same
cache-only configure command. It then succeeded, as did the remaining authorized
commands using the approved operation-lock access. This was not a dependency or
native failure and was not counted as RED. No cache repair, cleanup, dependency
acquisition or alternate package/tool selection occurred.

There were zero unexpected native failures, two intended RED failures, and one
resolved sandbox precondition failure. Existing generated Conan-toolchain author
warnings about whitespace at lines 77–80 were retained without modifying generated
or protected policy. The ignored evidence audit's initial Passed-line extractor
missed CTest's padded `Test  #4` form; its whitespace matching was corrected and
the unchanged raw logs then reconciled completely. No native command was rerun
for that evidence-only correction. No authentication gate occurred.

All commands completed well below the 60-second quiet liveness threshold;
there was no invented hard timeout, duplicate build or cleanup. The ledger has
31 commands and 31 byte-hashed logs. Their summed subprocess time is 56.032538s;
this is not total development or phase duration.

## Preserved evidence and scope

Machine-local packet: `.veyr-dev/execution/04-01/` contains `commands.jsonl`,
`logs/`, `entry.json`, `task1-acceptance.json`, `task1-snapshot/`,
`archive-resolution.json`, `validation.json`, `prepared/`, `implementation.diff`,
`REVIEW.md` and `evidence.json`. The manifest excludes itself and mutable
parent-owned live STATE; it creates no self-hash cycle. Prepared outputs are
local evidence and do not establish hosted/native acceptance for another host.

All accepted 62/65/36/18/20-entry manifests remain unchanged and resolve their
original bytes. Current CMake bytes needed by older evidence resolve through the
verified entry baseline; accepted planning inputs, including pre-execution STATE,
were archived as ten exact byte sets. The accepted planning integrity receipt
was preserved. Original archives and historical raw logs were not overwritten.

All 281 protected tracked inputs match entry hashes. Existing source/token/lexer/
diagnostic/renderer APIs, four Conan locks, recipes, profiles, baselines, wrappers,
workflow and accepted plans/context/research/patterns remain unchanged.
The experimental profile is byte-identical to dedicated commit
`52f598aaf031e549ed3bbe5c598bc43f5f7be6bb` (SHA-256
`1b5cefb5fa40a4a940ce1c0f4a4d7b8049ce591b81b79ecaf2251e5dfb107864`).
Ignored `CMakeUserPresets.json` retains SHA-256
`6ff016306538af61f6ec3f6054a4f6b6ab892044b40d41f622532c95fb8c65de`.
The executor changed only the five approved production/test/configuration files
and this execution record; the parent owns the separately authorized STATE update.
Git HEAD remains the entry commit, the index is empty, and diff whitespace checks
pass. Source review found no known stub or security surface beyond the plan's
in-process token/source/diagnostic trust boundaries.

## Owner review and remaining gates

The independent read-only implementation/evidence review inspected all seven
review candidates against the accepted CONTEXT and 04-01 plan. It found no
acceptance-blocking implementation, lifetime, test-quality or evidence defects
and recommended ACCEPT for Tasks 1–2 in the validated Linux developer scope.
The owner explicitly accepted that scope on 2026-10-19. This is local task
acceptance, not formal Plan 04-01 completion, PARSE requirement acceptance or
hosted validation of the uncommitted parser.

Accepted Debug and Release results are independently 22/22 doctor checks,
13/13 focused parser tests, 93/93 selected regressions, 432/432 full CTest and
432/432 verify CTest, strict verification PASS, 15/15 compiler/directory/C++23
entries and direct include-policy PASS. Review confirmed 75/75 manifest entries,
31/31 command-log hashes, 201 historical entries and ten archived planning inputs.
The two planned RED results remain expected implementation history; neither is
final validation failure or evidence of hosted acceptance. No native command was
rerun to record this acceptance.

The accepted architecture retains exactly three owned, copyable/movable spans,
no borrowed token storage or copied source text, and originating-SourceManager
resolution. Whole-stream structural validation precedes Invalid rejection and
syntax; original checked errors and accumulated diagnostics are preserved.
Syntax failure exposes no successful partial result. Numeric conversion/range
validation remains outside the parser. Exhaustive malformed-stream, precedence
and syntax matrices remain 04-02; actual token-destruction/source-lifetime,
newline/location and renderer integration remain 04-03. These are planned later
checks, not defects or validation credited to this baseline.

Installed GSD 1.6.1 execute-plan checkpoint_protocol/checkpoint_return_for_orchestrator
and execute-phase checkpoint_handling were inspected. The owner's actual review
response is recorded here; supported frontmatter.merge, state.update and
state.record-session operations retain the bounded checkpoint. Generic summary,
commit, advance-plan, update-progress and requirements-completion operations were
not entered: the accepted plan and explicit owner gate reserve them for separately
authorized closeout. No additional lifecycle mutation is needed for this record.

Original REVIEW.md, evidence.json, integrity.json and parent-checkpoint.json remain
immutable historical records of the pre-acceptance review packet. Before these
two documentation edits, exact reviewed STATE/EXECUTION bytes were preserved in
`.veyr-dev/execution/04-01/owner-acceptance/accepted-inputs/`. The separate
`owner-acceptance/receipt.json` records original-to-archive hash resolution and
current checkpoint hashes; original manifests, receipts and logs are unchanged.

Sensitive-file commit approval, separate publication approval and owner-approved
formal 04-01 closeout/SUMMARY remain pending. A fresh exact Wave 2 command is
required after the approved boundary.
Fresh Ubuntu/Windows hosted evidence, canonical phase-goal verification and final
owner acceptance remain later gates. PARSE-01–03 remain pending; no final grammar,
general AST, numeric semantics, stable ABI, LLVM/driver or end-to-end success is
accepted by this checkpoint.

Platform impact:

- Windows: Standard C++23 byte/span/variant behavior and existing target/runtime/
  PRE_TEST policy are preserved; fresh hosted Debug/Release remains later.
  Windows developer-host inspection stays independently pending/non-blocking.

- Linux: Both authorized Omarchy Debug/Release selections passed. Ubuntu CI is
  independent; no POSIX behavior or newline normalization enters the parser.

- macOS future: Deferred; no parser API or implementation introduces an OS
  restriction. Existing build-platform policy is unchanged.

## Self-check

PASS: all five implementation files exist; exact thirteen names execute in each
configuration; full tests/verify/includes pass; fifteen compile entries include
both new units; 31 command-log hashes and all historical mappings validate;
protected inputs/profile/IDE and empty index are confirmed. Owner review of
Tasks 1–2 is accepted for Linux developer scope; formal completion, commit and
publication remain pending. Phase 1/2/3 remain 8/8, 4/4 and 3/3; Phase 4 remains
0/3 formally complete with 27 accepted/22 pending requirements and all PARSE
requirements pending.
