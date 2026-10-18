# Plan 03-03 — Adversarial lexer and integration execution checkpoint

**Date:** 2026-10-18
**Invocation:** `$gsd-execute-phase 3 --wave 3`
**Base commit:** `da92389b5eb6b6b2bbe949072015dd950d8273b3` (`main`)
**Status:** Owner ACCEPTED Tasks 1–2 for the validated Linux developer scope. Local implementation/evidence review is complete; publication, hosted validation, canonical phase-goal verification and final plan/phase closeout remain separately gated. No summary or formal 03-03 completion is claimed.
**Lifecycle:** Phase 1 8/8; Phase 2 4/4; Phase 3 remains 2/3, in progress. LEX-01–06 retain their existing accepted scopes; global requirements remain 27 accepted/22 pending. No lifecycle completion mutation occurred.

## Verified entry and canonical execution route

The current branch was main and the worktree was clean at entry. HEAD and tracked
origin/main were the published 03-02 commit above. An actual
`git ls-remote --exit-code --refs origin refs/heads/main` returned exactly that
SHA with exit 0. The dedicated accepted profile commit is
`52f598aaf031e549ed3bbe5c598bc43f5f7be6bb`; it is an ancestor, and current profile
bytes match its published blob exactly. The 03-02 summary, requirement ledger,
owner acceptance and publication were inspected. Fresh owner authorization
permits only 03-03 local testing/validation and an owner-review packet.

Read installed GSD 1.6.1 execute-phase/execute-plan routing, the invoked skill,
approved plan/must_haves and [03-VALIDATION.md](03-VALIDATION.md), the unchanged
profile, existing test patterns and relevant source/diagnostic/build/command/
platform policies. Actual installed queries `init.execute-phase 3`,
`phase-plan-index 3` and `config-get workflow.inline_plan_threshold --default 2`
exited 0. The index reports completed summaries for 03-01/03-02 and only 03-03
incomplete. The installed exact wave-equality filter selects **03-03 only**,
with no incomplete lower wave. Two tasks and threshold 2 select Pattern C inline;
no subagent, worktree or tracking artifact was created. The query's known support-
document/config-key warnings do not change plan selection or repository policy.

The plan's explicit review stop overrides generic automatic commits, summaries,
state mutation, final phase verification and onward routing. No state.begin-phase,
state.advance-plan, requirements.mark-complete, phase.complete or commit helper
was invoked. STATE/ROADMAP/REQUIREMENTS remain the published 03-02 snapshot;
their old publication-gate wording is historical, not evidence that publication
did not occur. This execution checkpoint records current work without rewriting
those accepted records or marking the phase complete.

## Task 1 — Completed test coverage

Only the two approved lexing test files changed; production code and CMake did
not. Existing accepted tests remain intact and the current lexer was tested
through its public API, not modified to satisfy new expectations.

- Added 256 individually discovered `SmokeProfileBytes/SmokeProfileLexerAllBytes`
  cases. Independent specification partitions cover the four whitespace bytes,
  decimal digits, five single-byte punctuation tokens, isolated unsupported words,
  high bytes and all remaining ASCII. Every case checks exact token/diagnostic/
  source-end EOF behavior through the existing assertion helper.
- Added 65,536-byte digits, unsupported words and high-byte runs, plus long closed
  and unterminated block-shaped and line-shaped runs. Digit shape remains valid
  independently of semantic range. Many independent errors retain all 4,096
  delimiters with exactly 8,193 tokens including EOF and 4,096 Errors.
- Added monotonic/nonempty/source-bounded span, whitespace-only gap and final EOF
  properties. Token count is at most source_size+1; new Errors at most source_size.
  These assertions derive from positive consumption, not a new production cap.
- Added exact independent mixed-input oracles, all six independent delimiter/
  arrow boundaries around malformed numbers, double signs, keyword fragments,
  comments containing NUL/ESC/high bytes, nested-looking/overlapping delimiters,
  unterminated tails and adjacent high/ASCII runs. The original 35 normative
  cases still execute unchanged, including `/*/return` with no Return token.
- Added actual lexer-result lifetime coverage after caller text/name mutation,
  destruction of input/result/engine scopes and 3,000 further registrations.
  Token values and saved source views still resolve through the live original
  manager; no foreign-manager detection or destroyed-manager access is claimed.
- Added exact source locations across LF, both CRLF bytes, lone CR and empty/
  trailing-break EOF; original source bytes and diagnostic/token spans are checked.
- Added fresh-engine determinism, prior Warning/Note/Error preservation, repeated
  shared-engine duplicate/order preservation and checked unknown-source behavior
  on a nonempty manager. Explicitly initialized severity is used at each new
  emission. Caller-level predicates show recovered vectors with Errors are not
  eligible for successful compilation; no driver API is introduced.
- Added SourceManager → lex → DiagnosticEngine → PlainTextRenderer integration
  with hostile names and source bytes. Exact owned ASCII/LF output is checked,
  copied owning diagnostics survive collection growth, and saved rendering stays
  stable after further registrations. Borrowed collection views end before emit.
  A separate invalid reference preserves the original RenderError/SourceError
  and cannot clear already accumulated lexical Error state.

This is regression coverage of already implemented behavior: all first native
builds and test runs were GREEN. No artificial RED failure was manufactured,
and no production defect or owner-scope exception was encountered.

## Task 2 — Independent Linux validation and actual counts

Working directory for all commands: /home/m4rkens/Projects/VeyrLang.
Used /usr/bin/python3 and explicit linux-x86_64-clang / linux-developer / managed
selections, independently for Debug and Release. Each absolute build directory
was read from that configuration's doctor JSON. Both doctors preceded native
validation; each configuration then ran configure → build → focused → existing
components → full dev.py test → dev.py verify → direct includes, sequentially.

| Gate | Debug | Release |
|---|---|---|
| Doctor | 22/22 | 22/22 |
| Cache-only configure / incremental build | PASS | PASS |
| Focused native tests | 323/323 | 323/323 |
| Newly added cases actually executed | 268/268 | 268/268 |
| Single-byte cases actually executed | 256/256, every index 0–255 | 256/256, every index 0–255 |
| Normative profile cases actually executed | 35/35 exact names | 35/35 exact names |
| Existing source/location/diagnostic/renderer | 61/61 | 61/61 |
| Full CTest / dev.py test | 419/419 | 419/419 |
| dev.py verify | 13 selected compiler/directory/C++23 entries; CTest 419/419 | 13 entries; CTest 419/419 |
| Direct include-policy | PASS | PASS |

Focused total: 8 token tests + 24 named lexer tests + 35 normative rows + 256
single-byte rows = 323. This adds 268 cases to the prior 55; all prior 96 other
CTest cases remain, yielding 419 overall. Per-byte labels are ordinary numeric
indices, and normative labels retain deterministic ASCII hex. Actual Passed
name sets were audited for uniqueness and completeness in each focused/full/
verify log; successful CTest exit alone was not used as discovery proof.

All 16 retained native/static command receipts exited 0, with raw byte-for-byte
merged stdout/stderr, argv/cwd/time/duration/exit/log hash. Summed command time
is 52.230 seconds, not active development time or wall-clock plan duration.
No duplicate command, automatic retry, hard timeout or quiet-build termination
occurred. No command reached the soft quiet threshold.

Configure used the existing cache-only --no-remote --build=never path. LLVM
23.1.2 and GoogleTest 1.18.0 were already installed; no dependency acquisition or
package rebuild occurred. Reviewed identities remain LLVM RREV
aa4cad25b3148c3bdd22ed96fba04654, GoogleTest RREV
e2507fc4efe2ec9e2db9e5c91eefab9f and build-context CMake 4.4.3 RREV
8b46ce9dbf1937af1c05f19579c1f105. Conan remains 2.33.0. Known generated-toolchain
quotation warnings recur; native builds introduce no new compiler warning.
No local prepared/offline acceptance or new hosted acceptance is inferred.

## Must-have / threat coverage and retained evidence

| Plan must-have | Concrete support |
|---|---|
| All bytes and exact mixed recovery | AllBytes parameter suite, AdversarialMixedPrecedenceHasExactIndependentOracles, numeric delimiter cases, all 35 normative rows |
| Huge digits retain lexical shape | LongRunsKeepLexicalShapeAndBoundedProgress; existing DecimalShapeHasNoSemanticRangeOrConversion |
| Original bytes, boundaries and progress | expect_bounded_scan, ManyIndependentErrorsPreserveEveryDelimiterAndWhitespaceGap, explicit newline/EOF location tests |
| Invalid/Error ownership and persistent state through rendering | FreshScans, SharedEngine, UnknownSource and real-render/synthetic-render-error integration tests |
| Returned tokens and views retain original snapshots | ReturnedTokensOutliveCallerStorageAndFurtherRegistrations and existing token snapshot tests |
| Independent native identity/include evidence | Actual Debug/Release logs, generated metadata copies, commands.jsonl and evidence.json |
| Hosted/canonical final acceptance remains gated | Prior CI observation below and explicit publication/phase acceptance stop |

T-03-07 is covered by large runs and structural progress bounds, without timing
microbenchmarks. T-03-08 uses actual escaped rendering of hostile lexer Errors.
T-03-09 uses source lifetime, copied owning diagnostics, view scope and persistent
Error/order checks. T-03-10 uses actual command/count/hash evidence with platform
and owner-gate distinctions. No new allocation/OOM policy, final encoding,
parser, semantic range enforcement or stable ABI commitment was introduced.

Ignored packet: .veyr-dev/execution/03-03/ contains entry.json, commands.jsonl,
16 raw logs, validation.json with actual Passed test-name sets, prior-ci.json,
implementation.diff, REVIEW.md, integrity.json, evidence.json and generated
metadata under prepared/{Debug,Release}/. Copies include compilation databases,
CMakeCache, CTest registration files, LastTest.log and generated Conan toolchain.
The command recorder is temporary orchestration outside tracked source, not a
new repository evidence framework. Canonical artifact results are retained.
At the original review freeze, integrity checks passed: 62/62 manifest entries, 16/16 raw command-log hashes,
canonical artifact presence 3/3, exact three-file scope, documentation links and
whitespace. The manifest excludes itself and preserves earlier evidence unchanged.

The owner-accepted execution record was archived before this documentation-only
checkpoint update at checkpoint/accepted-inputs/.planning/phases/
03-m1-experimental-smoke-profile-tokens-and-lexer/03-03-EXECUTION.md within the
ignored packet. checkpoint/entry.json records that retention mapping. The original
evidence.json remains byte-identical: its 62 original byte sets resolve as 61 live
files and this one archived record. All 16 command logs remain unchanged. The
updated checkpoint and integrity results have a separate checkpoint/verification.json
receipt; historical acceptance evidence is not rewritten to describe later work.

The two original lexing test files are preserved byte-for-byte under baseline/.
All other initially tracked inputs, the accepted profile, production APIs/CMake,
four locks, dependency/toolchain/CI policy and ignored IDE configuration remain
unchanged. Earlier evidence manifests and logs are not rewritten: superseded
test references resolve to these baseline copies and earlier superseded planning
references retain their approved historical copies. Current hashes describe this
new candidate; they do not silently replace earlier acceptance evidence.

## Prior CI and mandatory owner stop

The original read-only observation of [run 39105535767](https://github.com/m4rkens/VeyrLang/actions/runs/39105535767)
for the published 03-02 base commit found Ubuntu Debug and Release completed/
success, Windows Debug in_progress and Windows Release queued. The API reports
overall queued and no conclusion; it is not treated as success. No reported
failure was observed. Exact output is retained in prior-ci.json. No CI run was
dispatched, retried, cancelled or otherwise modified.

The owner has now accepted the exact three-file candidate and retained
architecture/security/evidence packet for the validated Linux developer scope.
Publication additionally requires prior 03-02 CI success or explicit owner
resolution and separate commit/push authorization. No summary or formal 03-03
closeout is prepared yet.

After an authorized normal push, real four-job Ubuntu/Windows CI must validate
the exact new commit and actual test counts, verify/includes and prepared/offline
acceptance. Canonical phase-goal verification must later evaluate all plan
must_haves and all six Phase 3 requirements under explicit owner authorization.
Final owner acceptance/closeout is still required; Phase 4 remains unstarted.

## Owner local-acceptance checkpoint — 2026-10-18

The owner ACCEPTED Tasks 1–2 after independent implementation, test-quality,
integration and evidence review. No acceptance-blocking defects were found;
all required local adversarial/integration criteria are satisfied. No production
lexer, public API or CMake change, or architectural redesign, was required.
Additional per-case traces in looped long-run/EOF fixtures are an optional future
maintenance improvement, not an outstanding local acceptance criterion. They
were not implemented during this checkpoint.

Applied the installed GSD 1.6.1 execute-plan checkpoint_protocol: consumed the
explicit owner response, checked accepted evidence integrity and recorded the
passed local review while stopping at the next authorized boundary. Re-read the
approved plan's Task 2, must_haves and output gate. Its explicit separation of
local review, publication, hosted validation and final verification means generic
create_summary, update_current_position, update_requirements and metadata-commit
operations are not due at this checkpoint. No completion helper was invoked.
03-03-SUMMARY.md remains deferred until separately authorized closeout after real
hosted evidence. No final Phase 3 verification artifact was created.

At 2026-10-18 07:20:33 UTC, a read-only GitHub CLI query of the existing
[Build foundation run 39105535767](https://github.com/m4rkens/VeyrLang/actions/runs/39105535767)
confirmed commit da92389b5eb6b6b2bbe949072015dd950d8273b3:

| Prior 03-02 hosted job | Observed status | Conclusion |
|---|---|---|
| ubuntu-ci-Debug | completed | success |
| ubuntu-ci-Release | completed | success |
| windows-ci-Debug | completed | success |
| windows-ci-Release | in_progress | none yet |

The run remains in_progress with no conclusion. Windows Release was executing
"Check dependency integrity and export portable Conan archive". This is not a
failed job and is not inferred to be successful. The compact actual CLI result,
command and observation time are retained separately in checkpoint/prior-ci.json;
the earlier prior-ci.json is unchanged. These results concern the published
03-02 commit and cannot validate the unpublished 03-03 tests.

Remaining gates, in order:

1. Prior 03-02 hosted CI success, or separately owner-reviewed resolution.
2. Separate explicit owner authorization for the 03-03 test/checkpoint commit and
   publication; local review acceptance alone does not authorize either.
3. Actual Ubuntu/Windows Debug/Release hosted validation of the new published
   tests, including their counts and required native/prepared/offline gates.
4. Separately authorized canonical GSD Phase 3 goal verification of the combined
   roadmap acceptance criteria, all plan must_haves and requirement traceability.
5. Final owner acceptance and formal plan/phase closeout.

Phase 1 remains 8/8, Phase 2 4/4 and Phase 3 2/3 pending final acceptance;
03-01/03-02 remain completed and 03-03 is not fully closed. Global requirements
remain 27 accepted/22 pending. No additional native validation or hosted success
for the unpublished 03-03 tests was claimed, and no state, roadmap or requirement
ledger was changed at this checkpoint.
Accepted test bytes, profile, production code, APIs, CMake, four locks, toolchain
policies, historical evidence and the ignored IDE binding remain unchanged.
Only this execution record changed in the tracked planning candidate; new
ignored checkpoint receipts preserve its pre-update version and integrity proof.
No builds, tests, dependency operations, staging, commits, pushes or CI mutations
were performed during checkpoint preparation. Phase 4 remains unstarted.

Platform impact:
- Windows: Portable C++23 in-memory byte tests; existing compiler/runtime/DevShell,
  linker/profile/locks unchanged. New test hosted/native evidence remains pending;
  developer-host inspection independently pending/non-blocking.
- Linux: Omarchy Debug/Release locally validated; Ubuntu is an independent hosted
  baseline, and prior 03-02 Ubuntu success does not validate these new tests.
- macOS future: Deferred; no OS, filesystem, console, encoding or ABI dependency added.
