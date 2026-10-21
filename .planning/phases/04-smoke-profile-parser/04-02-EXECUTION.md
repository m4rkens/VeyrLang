---
phase: 04-smoke-profile-parser
plan: "02"
status: complete
tasks_locally_complete: 2
plan_completion: complete
owner_review: accepted-linux-tasks-1-and-2
hosted_validation: passed-four-configurations
implementation_commit: 75d712f1931b375a7803a19800539b49fbe84732
formal_acceptance_recorded_at: "2026-10-21T09:06:46.513877+00:00"
requirements_completed: []
---

# Phase 4 Plan 02 — Formal conformance closeout

## Formal acceptance and publication checkpoint

The owner approved formal Plan 04-02 acceptance and the exact four-file
closeout commit/publication. The actual formal reconciliation was recorded at
2026-10-21T09:06:46.513877+00:00. Reviewed preparation bytes and receipts remain
immutable outside discovery; the canonical summary was promoted last in this
controlled operation. Formal acceptance is distinct from earlier task, local
review, implementation publication and hosted completion timestamps.

The owner accepted both local tasks after the independent implementation/evidence
review passed without blocking findings. The accepted test-only packet was
published in 75d712f1931b375a7803a19800539b49fbe84732. Fresh read-only inspection
confirmed run 39493376090 completed/success at that exact commit in all four
Ubuntu/Windows Debug/Release configurations.

Every job executed the exact 529-name suite in native test, strict verify and
prepared acceptance: 110 parser cases, including all 97 additions and 13 original
baselines. Verify/prepared reported fifteen selected C++23 entries; include-policy
passed. The controlled isolated empty-cache rejection occurred exactly once per
job with --no-remote --build=never and without acquisition/retry. Optional warm
snapshot publication skips did not skip native gates. Existing generated-toolchain
quotation and action-runtime deprecation warnings are non-blocking. No unexpected
error or meaningful conformance regression was identified.

The accepted Linux evidence remains unchanged: Debug/Release each doctor 22/22,
parser 110/110, new cases 97/97, selected regressions 93/93, full test/verify 529/529,
fifteen selected C++23 entries and direct includes PASS. Original local manifests,
27 command receipts and earlier prerequisite stops retain their original bytes.
Current closeout evidence is under ignored
.veyr-dev/execution/04-02/closeout-preparation/; the lifetime mapping preserves
this execution document's accepted pre-closeout bytes before later promotion.

The formal result is Phase 4 2/3, seventeen completed plans and 04-03
unstarted. PARSE-01/02/03 remain Pending and global counts remain 27 accepted /
22 pending under the explicit cross-plan exception. Do not run generic requirement
completion, phase.complete or next-wave helpers. Summary promotion and actual
completion timestamp are now recorded under explicit formal owner approval.
Dedicated publication is also approved; actual commit/remote proof is retained
separately. No fresh 04-03 execution authorization is implied.

04-03 integration, final phase-goal verification and final Phase 4 acceptance are
not credited here. The accepted experimental M1 profile, implementation/public APIs,
tests, CMake, all four locks and native acceptance procedures remain unchanged.
The repository is public; the owner abandoned the unpublished phase-end CI trigger
candidate. Published push/PR CI remains unchanged. Stage 1 and Stage 2 candidates
are preserved and excluded; deferred performance remediation does not block language work.

Platform impact:
- Windows: Hosted Debug/Release conformance passed; independent developer-host inspection remains pending/non-blocking.
- Linux: Accepted local and independent Ubuntu Debug/Release evidence preserved.
- macOS future: Deferred; no platform change.

## Preserved published checkpoint history

The following original checkpoint body is retained verbatim. Its earlier pending
review/publication/hosted wording describes those historical stops, not current
technical acceptance. The actual pre-closeout file, including original frontmatter,
is separately archived by the new lifetime receipt.

# Phase 4 Plan 02 — Local execution owner checkpoint

## Current checkpoint — 2026-10-20T20:19:29.190373+00:00

Tasks 1–2 are locally implemented and independently Debug/Release validated.
Owner implementation review, plan acceptance, sensitive-file commit/publication,
fresh hosted validation and formal closeout remain separate gates. No canonical
04-02-SUMMARY.md exists. GSD filename discovery still identifies only 04-01 as
complete; Wave 3 is not authorized.

The owner explicitly superseded the blanket no-shared-cache-mutation restriction:
normal recipe/package usage timestamps, routine metadata and locking are allowed
for offline preparation. Downloads, dependency builds, cache cleanup/replacement,
lock/profile/revision changes remain forbidden. Earlier stop records below are
historical and were not treated as a parser implementation failure.

### Actual isolated preparation

The new Git worktree did not inherit ignored checkout-local tools or generated
outputs. Doctor rejects a redirected Conan tool directory, so no symlink bypass
was used. A standard-library venv was created with
`/usr/bin/python3 -I -m venv --without-pip .veyr-dev/tools/conan-2.33.0`.
The original verified Conan 2.33.0/Python 3.14.7 installation supplied 1,112
installed-distribution files, each copied and SHA-256 checked byte-for-byte.
Only the Conan launcher shebang was relocated to the new venv interpreter.
No pip installation, network operation, Conan upgrade or global tool change.
`offline-tools-provenance.json` records every copied file and launcher hash.

Debug and Release read-only doctors each passed all 22 checks. Each explicit
`dev.py configure` used its tracked lock/profiles and Conan
`install --no-remote --build=never`. Both reused existing LLVM 23.1.2 and
GoogleTest 1.18.0 binaries; logs retain exact recipe/package revisions. All
required binaries reported Cache/Already installed, and the CMake build-tool
binary was skipped, not acquired. Native LLVM builds were not attempted.
Only routine shared-cache bookkeeping was permitted; no package cleanup,
deletion, replacement, lock update or revision selection change was requested.
No foreign CMake build tree was copied. New generators, configurations and build
outputs reside under this worktree's own configuration-specific build directory.

Both configurations used reviewed Clang 22.1.8, CMake 4.4.3 and Ninja 1.13.2.
The managed LLVM remains Release-built for both consumers per the locked profile;
GoogleTest uses its configuration-specific binary. Generated Conan CMake files
emitted quoting warnings at lines 77–80; configure succeeded and subsequent
compiler/directory/C++23 checks and native tests all passed. No generated or
tracked build policy was edited to suppress warnings.

### Test-only implementation and conformance

The only technical change is 465 additive lines in tests/parsing/parser_test.cpp.
The original 13 tests and their source bytes are preserved before the insertion.
No public API, production implementation, native target or CMake change.

| Contract group | Actual new named cases / evidence |
|---|---|
| Basic malformed streams | All 19 plan rows, exact SourceError fields or TokenStreamError kind/selected ID/index |
| Combined-fault priority | 17 additional cases: selected lookup, source/kind/span/EOF priority, earliest index, structure before Invalid/syntax |
| Positive/scan controls | 5 cases: 4,096-token full validation, adjacency, whitespace gaps, empty and whitespace-only actual EOF |
| Invalid placement | 3 cases, each with preserved lexer Errors and a separate initially empty engine |
| Expected syntax positions | 12/12 independently literal messages and primary/related span oracles |
| Incomplete prefixes | 11/11 exact source-end EOF spans and closer context |
| Additional found-label rows | 10/10; Return and EOF labels also covered by position/prefix matrices |
| Additional first-error syntax forms | 9/9 including two entries, unexpected closers and multiple faults |
| Unsupported lexical forms | 7/7, including signs, comments, overlapping delimiter and high byte; no duplicate parser Error |
| Complete/decimal value controls | 4/4: complete prefix, 256, leading zeros and 512 digits; exact three spans and copy/move behavior, no numeric conversion |

Structural failures assert the complete expected checked alternative and no
successful value or appended diagnostic. Syntax failures assert exactly one new
initialized Error with its full controlled message, exact primary and matching
opener only where required. Helpers copy owning diagnostic values and reacquire
views after parse/emission; all prior severity/message/primary/related fields and
order are compared. Existing Error state persists; a recovered syntax value
alone is never treated as compilation success. No private parser table supplies
expected values. Token destruction, source lifetime and renderer integration
remain the separately planned 04-03 scope.

### Actual Linux validation

| Check | Debug | Release |
|---|---:|---:|
| Doctor | 22/22 | 22/22 |
| Task 1 discovery/execution | 57/57 | 57/57 |
| Final parser discovery/GoogleTest list/execution | 110/110 | 110/110 |
| Original parser cases preserved/executed | 13/13 | 13/13 |
| New conformance cases executed | 97/97 | 97/97 |
| Selected prior-component regressions | 93/93 | 93/93 |
| Full CTest through test wrapper | 529/529 | 529/529 |
| Full CTest through strict verify | 529/529 | 529/529 |
| Compiler/directory/C++23 selected entries | 15 | 15 |
| Direct include policy | PASS | Same unchanged source tree; standalone PASS |

`validation.json` reconciles exact GoogleTest/CTest discovery names with Passed
names in focused and both full executions. No empty/undiscovered matrix rows are
credited. Task 1 passed both configurations before Task 2 was added. All 26
current native/preparation command receipts succeeded. The one earlier failed
Debug doctor remains a historical prerequisite failure, not an expected RED
parser test or an ignored validation failure. No new RED failure was forced.
The supervised commands were short; no liveness timeout, retry or termination.

### Evidence, isolation and remaining gates

Current owner-review packet: ignored `.veyr-dev/execution/04-02/REVIEW.md`;
current manifest: `.veyr-dev/execution/04-02/local-validation/evidence.json`.
The original prerequisite manifest and offline-stop manifest remain unchanged.
Their superseded live execution/command paths resolve to preserved snapshots,
recorded explicitly by the new manifest. No accepted manifest is rewritten.

Revalidation passed: accepted 04-01 evidence 75/75, command logs 31/31,
historical references 201/201, formal closeout evidence 20/20, held Stage 1
76/76 and Stage 2 correction 19/19. All 290 other tracked entry hashes are
unchanged, including four Conan locks, policies, parser production code, CMake,
STATE/ROADMAP/REQUIREMENTS and published M1 profile. Both held worktrees retain
exact recorded HEAD/status/candidate hashes and empty indices. No Stage 1 or
Stage 2 candidate was incorporated.

Read-only observation of separate run 39453877107 now confirms completed/success
for the exact e35b9e24c23709c9f9e4b2c0e5f1d6af229311b8 baseline, with Ubuntu
Debug/Release and Windows Debug/Release all successful. That prior run is not
hosted validation of these unpublished 04-02 tests. No CI mutation or new run.

Phase 4 remains formally 1/3; PARSE-01/02/03 Pending; global requirements
27 accepted / 22 pending. No completion/requirements helper, summary, commit,
push, phase transition or Wave 3 execution. Next required action is independent
owner implementation/evidence review, then separately authorized acceptance and
publication/closeout. Installed GSD plan artifact and discovery queries are
retained without invoking final phase-goal verification.

Platform impact:
- Linux: Independent Omarchy Debug/Release native evidence above; Ubuntu hosted evidence for these additions remains pending publication.
- Windows: New cases use portable byte/span/value contracts and ASCII names; fresh hosted Debug/Release validation remains pending. Developer-host inspection is independently pending/non-blocking.
- macOS future: Deferred; no new platform dependency or structural barrier.

## Historical prerequisite and offline-preparation checkpoints

The following records describe earlier owner-authorized attempts. Their pending
preparation/authorization wording is superseded by the current checkpoint above;
original receipts and full pre-resumption execution bytes are retained.

The owner authorized local `$gsd-execute-phase 4 --wave 2`, scoped to 04-02 only.
Preflight ran on 2026-10-20. No conformance test implementation or native build
began; this record does not complete or accept the plan.

## Verified entry and isolation

- Actual origin refs/heads/main matched published closeout commit
  e35b9e24c23709c9f9e4b2c0e5f1d6af229311b8.
- Installed GSD init.execute-phase and phase-plan-index identify 04-01 complete,
  04-02/03 incomplete, and only 04-02 in the requested wave. The existing
  planning-support filename warning does not change that selection.
- The clean worktree was branched from that exact commit as
  phase4/04-02-parser-conformance. phase4/parser-closeout remains at the published
  closeout; original local main and its held Stage 1 candidate are preserved.
- The 20-entry accepted 04-01 closeout manifest was verified. The accepted public
  API and plan input were inspected; parser_test.cpp and seven planning/config
  inputs were archived byte-for-byte under ignored .veyr-dev/execution/04-02/baseline/.
  entry.json records their hashes and all tracked entry hashes.
- The manual execution chain flag was already false; canonical config-set
  confirmed false to false. No tracked config change or automatic lifecycle
  transition occurred. Execution remains inline and sequential, without agents.

## Actual prerequisite failure

From this worktree, the prescribed command was:

```text
/usr/bin/python3 -B scripts/dev.py doctor --profile linux-x86_64-clang --environment linux-developer --config Debug --llvm-provider managed --json
```

The command ran at 2026-10-20T16:09:21.001015+00:00 and ended at
2026-10-20T16:09:21.185168+00:00 with exit 1. It reported 21/22 checks OK and
prerequisites_ready false. The only missing identity was tools.conan, expected
2.33.0 at this worktree's .veyr-dev/tools/conan-2.33.0/bin/conan. The isolated
worktree also has no build directory or generated dependency/toolchain outputs.
The original checkout's ignored tools/builds were not inherited by Git.

This is missing worktree preparation, not a parser test failure or an effect of
the deferred Stage 2 performance investigation. Existing original-checkout build
outputs are not substitutes for a build configured against this worktree's tests.
The approved plan's Future native verification requires prepared dependencies and
states that missing preparation stops execution. Current owner authorization
also forbids Conan/cache operations, so no setup or configure workaround ran.

No Release doctor, build, discovery, test, verification, include command, Conan
operation, dependency acquisition, retry, cleanup or tool installation followed.
No healthy quiet build was terminated: the doctor returned normally in under one
second. Raw output and the actual command/exit/timing/hash are retained as
Debug-doctor.log and commands.jsonl in ignored .veyr-dev/execution/04-02/.

## Coverage and lifecycle

Tasks 1–2 have not started. All nineteen structural cases, combined-fault and
positive controls, twelve syntax positions, eleven prefixes, found labels and
additional forms remain planned, not executed. There is no test-only diff and
no new parser/native acceptance count. Existing 04-01 evidence remains historical
and unchanged; it is not credited as 04-02 validation.

No canonical 04-02-SUMMARY.md exists. STATE and ROADMAP remain unchanged at Phase
4 1/3 formally complete, sixteen completed plans, 27 accepted/22 pending and all
PARSE requirements pending. 04-03/Wave 3 remains unstarted and unauthorized.
No phase-start/completion, requirement-completion or publication helper ran.

At preflight, independent read-only observation of closeout run 39453877107
reported queued overall, Ubuntu Debug completed/success, Ubuntu Release in
progress and both Windows jobs queued, for the exact published closeout SHA.
That unfinished run is separate from this local preparation stop; no failure or
new CI acceptance was inferred and no workflow was dispatched or changed.

## Required owner decision

Authorize bounded preparation of this worktree's approved Conan tool environment
and cache-only dependency/toolchain outputs, or provide a prepared environment,
before resuming the same 04-02 execution. No dependency source build, download,
lock change or policy change is implied. The supported reuse/preparation method
must be reviewed against the existing bootstrap policy before use. Do not work
around the gate by changing the wrapper or sharing mutable original build outputs.

Platform impact:
- Linux: All observed host/compiler identities passed except the missing worktree-local Conan tool; no new native acceptance.
- Windows: Hosted and developer-host identities remain separate; no new Windows result claimed.
- macOS future: Deferred; no platform policy changed.

## Offline preparation inspection — updated prerequisite stop

Recorded at 2026-10-20T16:22:00.677090+00:00. The subsequent owner authorization allows offline
preparation only without shared Conan-cache mutation. The original checkpoint
and failed doctor receipt above remain historical; the following finding
supersedes the earlier requested preparation approval.

Conan 2.33.0 and Python 3.14.7 distribution metadata exists in the original
checkout's `.veyr-dev/tools/conan-2.33.0`. No tool or package was reused in a
Conan operation. The new worktree still lacks its checkout-local CLI environment
and configuration-specific generated outputs. No foreign CMake tree was copied.

Static inspection confirms the supported `prepare_dependencies` path invokes
Conan install with `--no-remote --build=never` and the tracked lock. Those flags
prevent acquisition/building but do not prohibit cache metadata writes:

- Pinned `conan/internal/graph/graph_builder.py:75` updates recipe usage metadata;
  `conan/internal/cache/cache.py:317-320` implements it with `os.utime`.
- Pinned `conan/internal/graph/installer.py:352-358` calls
  `os.utime(package_layout.base_folder, None)` for already-installed binaries.

This conflicts with the explicit shared-cache-mutation prohibition. No Conan,
setup, configure, installation, dependency download/build or native command was
run after this discovery. Debug doctor remains the previous 21/22 result;
Release was not run. Both required passes remain unestablished and Tasks 1–2
remain unstarted. No wrapper/tool-path check was bypassed.

Normal missing-tool bootstrap invokes pip against PyPI, and the doctor rejects
redirected tool environments. That bootstrap was not run. Offline checkout-local
CLI provisioning still needs a reviewed method independently of the cache-write
conflict. No alternate dependency store, symlink workaround or shared build tree
was introduced.

The original 12-entry prerequisite manifest passed before this update. Its bytes
and original execution document are preserved in ignored
`.veyr-dev/execution/04-02/offline-preparation-stop/`. The companion inspection
receipt maps its superseded live execution path to `previous-execution.md`.
The original manifest and command log were not rewritten. All 291 tracked entry
hashes and the 20-entry accepted 04-01 closeout manifest passed. Stage 1 and Stage
2 HEAD/status/candidate hashes and their empty indices matched the retained
isolation record. All four tracked Conan locks remain unchanged.

Actual remote main still resolved to
`e35b9e24c23709c9f9e4b2c0e5f1d6af229311b8`. Read-only observation of run
39453877107 reported both Ubuntu jobs successful, Windows Debug in progress and
Windows Release queued; overall status queued, without a conclusion. This is not
new completed hosted acceptance. Detailed observations and pinned implementation
excerpts/hashes are in `offline-preparation-stop/inspection.json`.

Required owner decision: authorize the normal bounded cache-usage metadata writes
needed for offline cache-only preparation, together with a reviewed offline CLI
provisioning method; alternatively authorize a separately reviewed supported
isolated-home procedure. Downloads, dependency rebuilds, lock/profile changes and
package-identity changes remain prohibited. No parser implementation may resume
until both configuration prerequisite checks pass.

Phase 4 remains 1/3 complete, PARSE-01/02/03 Pending, and global requirements
27 accepted / 22 pending. No canonical 04-02 summary or lifecycle mutation.

Platform impact:
- Linux: Confirmed Conan usage-metadata writes block preparation under the current boundary.
- Windows: No new runtime/developer-host validation; preparation safety needs independent verification.
- macOS future: Deferred; no policy or implementation changes.
