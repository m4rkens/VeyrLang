# Plan 02-04 — Bounded plain-text diagnostic rendering

Status: Complete and owner-accepted. Tasks 1–2 passed local Linux validation,
architecture/security/evidence review and publication; Task 3's independent
four-job CI and final owner acceptance gates are satisfied. Plan 02-04 and
Phase 2 are complete at 4/4; all seven phase requirements are accepted. Phase 1
remains closed at 8/8. Documentation-only closeout awaits owner review/publication;
Phase 3 has not started. No accepted implementation/test/evidence changes or
validation reruns occurred during closeout.

## Authorization and execution boundary

The owner authorized `$gsd-execute-phase 2 --wave 4` for Tasks 1–2 only, including
local implementation/testing while Plan 02-03 publication CI was pending.
Installed execute-phase.md parses the literal --wave filter, skips completed
summaries and rejects incomplete lower-wave prerequisites. The read-only plan
index confirmed Wave 4 = [02-04], with 02-01/02-02/02-03 summaries present.
No other plan was selected. The explicit Task 3 blocking checkpoint prevented
automatic phase verification, completion, commit/publication or Phase 3 work.
Execution stayed inline; no delegation, implicit flags or automatic commits.

The owner subsequently authorized read-only code/security/evidence review. It
found no local acceptance-blocking defects and recommended acceptance of Tasks
1–2 for the validated Linux scope, preserving the separate publication gates.
The earlier publication-readiness command authorized execution checkpoint
documentation only. The owner then approved publication of the exact eight-file
packet, committed/pushed in ab596c7c721fd673b1716db6a1f6f5681a7ff126. Final
read-only review confirmed all four jobs in 38722486719 passed; the owner now
explicitly accepts Plan 02-04/Phase 2 and authorizes final closeout documentation.
Commit/push of this closeout remains separately gated; Phase 3 is not authorized.
No implementation, test or retained evidence changes occurred during either
documentation step.

## Completed implementation and reviewed contracts

- PlainTextRenderer::render(const Diagnostic&, const SourceManager&) const is
  stateless and synchronous. RenderResult is variant<string, RenderError>;
  success owns LF-terminated ASCII output, while RenderError owns the original
  SourceError plus an optional offending related index (absent for primary).
- Primary validation precedes all related validation in insertion order before
  constructing successful output. Unknown IDs, reversed ranges and beyond-size
  ranges retain original IDs/offsets/size metadata. No partial successful output,
  invented source context, repaired range or silently omitted related reference.
- Checked display_name/location/line_span_at/text operations resolve immutable
  snapshots through their originating SourceManager. Header line/columns retain
  original byte identity, independent of escaped display-cell width. Manager-local
  IDs still cannot prove cross-manager origin by numeric comparison alone.
- Header layout, lowercase severity and ordered labelled related locations match
  the plan. Location-free messages have no fabricated primary/excerpt; related-
  only context remains supplemental. No sorting, deduplication or severity policy.
- All rendered input fields preserve printable ASCII and literal backslashes.
  TAB/CR/LF become visible escapes; remaining controls, DEL and high bytes use
  uppercase hex escapes. No raw input control/ANSI sequence reaches output.
  This presentation is intentionally not a reversible byte encoding.
- Excerpts are sliced before escaping: at most three source rows, 80 original
  bytes and 320 escaped content cells per row. A fixed 81-boundary cell map
  aligns caret/underline intervals. Marker arithmetic and allocation are bounded
  by visible cells, never by the enormous original span. Horizontal/vertical
  clipping leaves the true header location and input span unchanged.
- Empty spans, original newline bytes, EOF and exclusive-end boundaries follow
  the approved source contracts. Terminator bytes remain visible escapes; a span
  ending at a following line start excludes that line. Empty/trailing-break EOF
  receives its own boundary caret without inventing source bytes.
- Renderer source is appended to veyrcore; its tests join existing diagnostic_tests
  with unchanged target policy and PRE_TEST discovery. No LLVM, filesystem,
  terminal, driver, framework or dependency is added.

SourceManager, Diagnostic and DiagnosticEngine APIs remain unchanged. Renderers
retain no input views; callers keep borrowed collections valid while rendering
and reacquire after emit. Output survives destruction of sources/diagnostics.
Rendering errors cannot clear accumulated Error state. Severity must still be
explicitly initialized before reading; no default/constructor policy was added.
Unsupported initialized enum values throw std::invalid_argument. Ordinary C++
allocation exceptions remain ordinary exceptions, not a Veyr language OOM policy.

## Actual Linux execution evidence

All commands ran sequentially from /home/m4rkens/Projects/VeyrLang on the reviewed
Omarchy/Arch developer baseline. Each exited 0, with no automatic retry, duplicate
native process, cleanup or output-idle liveness stop. No validation was rerun for
this documentation update. Raw merged command output is retained through
pipefail/tee under ignored .veyr-dev/execution/02-04/.

Every developer command used:

```text
/usr/bin/python3 scripts/dev.py <command> --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
```

Doctor additionally used --json. Selected build directories:

```text
build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Debug
build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Release
```

Debug's eight commands completed before Release's independent eight commands.
Each <config>-<stage>.log below exists with its hash in evidence.json.

| Stage | Debug | Release | Log suffix |
|-------|-------|---------|------------|
| Read-only doctor | 22/22 | 22/22 | doctor.log |
| Explicit cache-only configure | Passed | Passed | configure.log |
| Project/test build | Passed, 15 completed Ninja steps | Passed, 15 completed Ninja steps | build.log |
| Focused PlainTextDiagnostic | 27/27, 0.52s | 27/27, 0.41s | renderer.log |
| Existing SourceManager/SourceLocation/Diagnostic | 34/34, 0.57s | 34/34, 0.49s | existing.log |
| dev.py test | Full CTest 96/96, 1.71s | Full CTest 96/96, 1.52s | test.log |
| dev.py verify | 10 C++23 entries; CTest 96/96, 1.70s | 10 C++23 entries; CTest 96/96, 1.52s | verify.log |
| Direct include policy | Passed | Passed | includes.log |

Focused CTest commands used the selected build directory with:

```text
ctest --test-dir <selected-build-dir> -R '^PlainTextDiagnostic\.' --output-on-failure --stop-on-failure --no-tests=error
ctest --test-dir <selected-build-dir> -R '^(SourceManager|SourceLocation|Diagnostic)\.' --output-on-failure --stop-on-failure --no-tests=error
/usr/bin/python3 -B scripts/check_includes.py --root /home/m4rkens/Projects/VeyrLang
```

Full counts comprise the prior 69 tests plus 27 renderer cases. Existing focused
counts are SourceManager 12, SourceLocation 8 and Diagnostic 14. Both full suites
include include-policy and all 22 prior CRT fixtures. Both compile databases
contain the two new translation units and ten selected C++23 entries; Debug
uses -g and Release -O3/-DNDEBUG, with separate exact build directories.

The 16 command logs, twelve copied generated records under prepared/Debug and
prepared/Release, five implementation hashes, complete implementation.diff,
initial-file-hashes.json and REVIEW.md form 36 evidence-manifest entries.
Prepared copies include compilation databases, CMake caches, root/tests CTest
registration, LastTest.log and Conan toolchain. Read-only review and the current
publication-readiness check found no missing/hash/size mismatches; the retained
implementation diff still matches current code. Manifest exit codes are retained
execution records, corroborated by successful command output. No new test run
is claimed from hash inspection. Original REVIEW.md's CI-pending snapshot is
historical; the completed CI observations below supersede it without modifying
accepted evidence. At the publication-readiness checkpoint, STATE/ROADMAP were
the only newly updated original tracked files beyond reviewed CMake integration;
the later documentation-only closeout scope is recorded below.

Both configurations used Conan --no-remote --build=never and reused LLVM/GoogleTest.
No dependency download/rebuild occurred in local 02-04 execution. Reviewed recipe
identities remain LLVM aa4cad25b3148c3bdd22ed96fba04654, GoogleTest
e2507fc4efe2ec9e2db9e5c91eefab9f and build CMake 4.4.3
8b46ce9dbf1937af1c05f19579c1f105. All four lock bytes and the ignored local IDE
preset remain unchanged. Previously documented generated toolchain quotation
warnings remain non-blocking; no new compiler warning was observed.

## Native regression and security coverage

Twenty-seven PlainTextDiagnostic cases cover exact location-free/header/severity
output, ordered cross-source/duplicate/related-only context, checked original
primary/related failures and validation precedence, LF/CRLF/lone CR and split
CRLF spans, multiline exclusive ends, interior/empty/trailing/nontrailing EOF,
byte columns versus expanded tab/UTF8 cells, hostile metadata/message/label/source
bytes, all 256 byte values, literal Windows/UNC/relative spelling, million-byte
logical lines, 100,000-line spans, clipping edges, multidigit gutters and independent
row/byte/cell/marker bounds. Owned-output and immutable-input cases plus engine
integration cover output lifetime, insertion order, duplicates and Error state
preservation on render failure.

Read-only security review found no terminal injection, arithmetic overflow or
out-of-bounds indexing under the checked SourceManager contract. Window endpoints
stay within the original line, mapped indices within 0..80, and underline/padding
lengths within bounded visible cells. Message/name sizes and related counts remain
uncapped as permitted; source-excerpt bounds are not a global output-size policy.

Non-blocking opportunities remain unimplemented: combine left-clipping with mixed
expanded escapes/empty-span alignment in one fixture; document/test unsupported
initialized enum handling; consider a separately reviewed compile-time severity
initialization policy. Allocation failures were not fault-injected and no latency
SLA/allocation instrumentation is claimed. These do not block local acceptance.

## Plan 02-03 publication CI prerequisite — now satisfied

Read-only GitHub CLI run metadata and completed job logs independently confirmed:
[Build foundation run 38705163273](https://github.com/m4rkens/VeyrLang/actions/runs/38705163273)
is completed/success at a9d356efaf651eae1dd6c8fb97942f274b632f6f.

| Job | ID | Status / conclusion |
|-----|----|---------------------|
| ubuntu-ci-Debug | 116322660847 | completed / success |
| ubuntu-ci-Release | 116322660930 | completed / success |
| windows-ci-Debug | 116322660645 | completed / success |
| windows-ci-Release | 116322660363 | completed / success |

Job URLs follow the run URL plus /job/<ID>. All four passed reviewed policy,
bootstrap/doctor, dependency setup/integrity/export/early save, native configure,
build/test/verify, prepared native acceptance and controlled acquisition-disabled
empty-cache rejection. Logs show the fourteen Diagnostic cases passing, full CTest
69/69 and eight selected C++23 entries per job. Expected isolated rejection was
observed once without acquisition/retry. Ubuntu Debug archive import was skipped;
the other three imports passed. Skipped import is not credited as cache reuse or
acceptance evidence. No new cache-capacity/resource claim is made here.

These results satisfy 02-03's post-publication CI requirement and remove that
prerequisite for separately authorizing 02-04 publication. This commit predates
the renderer: none of these results is credited as 02-04 hosted validation.
Windows developer-host inspection remains separate from Windows CI.

## Final Task 3 CI evidence and owner acceptance

Final [Build foundation run 38722486719](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719)
completed successfully at published commit ab596c7c721fd673b1716db6a1f6f5681a7ff126.
Read-only metadata and actual completed job logs were inspected; no run was
retried, cancelled, dispatched or inferred from the owner's report alone.

| Job / ID | Doctor | Renderer | Full CTest | C++23 entries | Include / prepared native / controlled offline | Final |
|----------|--------|----------|------------|---------------|-----------------------------------------------|-------|
| [ubuntu-ci-Debug / 116380189518](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189518) | 22/22 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |
| [ubuntu-ci-Release / 116380189555](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189555) | 22/22 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |
| [windows-ci-Debug / 116380189082](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189082) | 20/20 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |
| [windows-ci-Release / 116380189595](https://github.com/m4rkens/VeyrLang/actions/runs/38722486719/job/116380189595) | 20/20 | 27/27 | 96/96 | 10 | Passed / passed / passed | Success |

Each job's normal test, verify and prepared/native paths separately passed
CTest 96/96, including SourceManager 12/12, SourceLocation 8/8, Diagnostic 14/14,
PlainTextDiagnostic 27/27 and include-policy. Logs contain 81 successful renderer
case occurrences per job: 27 unique cases in each of those three paths, not
81 distinct tests. Strict verification reports ten selected C++23 entries in
normal and prepared paths. All expected component cases passed in Release as
well as Debug; no success is inferred from assertions, skipped work or cache hits.

Minimal decisive actual log results, observed on every job:

```text
100% tests passed, 0 tests failed out of 96
Compilation database: 10 selected C++23 entries
Expected isolated empty-cache rejection observed once; no acquisition/retry
```

All four passed policy/bootstrap/doctor/lock checks, explicit dependency setup,
integrity/export/early snapshot save and configure/build/test/verify. Archive
import passed for both Ubuntu jobs and Windows Debug; Windows Release import
was skipped. Its subsequent explicit setup/native/acceptance gates passed.
Skipped import is not evidence of reuse; this closeout makes no new cold-build,
resource or all-packages-reused claim. Windows developer-host inspection is
independent from the exercised Windows hosted environment.

The integrated architecture/security review found no implementation blocker.
The focused final evidence review recommended ACCEPT; the owner explicitly
accepted Phase 2, all seven requirements and final Plan 02-04 closeout. Task 3
is complete. SRC-01/SRC-02/SRC-03/SRC-04 and DIAG-01/DIAG-02/DIAG-03 are accepted
in REQUIREMENTS; 02-04-SUMMARY.md and required 02-VERIFICATION.md record the final
scope. No extra runtime verification is claimed from preparing those documents.

## Publication history, closeout integrity and next gate

Implementation/checkpoint publication ab596c7c721fd673b1716db6a1f6f5681a7ff126
contains exactly five reviewed implementation/integration/test files plus STATE,
ROADMAP and this execution record (eight files). The accepted implementation
and retained 36-entry evidence packet remain unchanged.

The initial six-file documentation closeout recorded prior accepted reviews but
did not establish actual execution of canonical verify_phase_goal. The owner's
GSD compliance review identified that gap and stale PROJECT.md as incomplete;
the owner then explicitly authorized bounded lifecycle reconciliation.

Canonical GSD 1.6.1 verify_phase_goal has now executed inline under the installed
adapter: loaded plans/summaries/roadmap/requirements, inspected actual production
and test artifacts, traced every declared key link and complete source-to-renderer
data flow, cross-checked all requirements, scanned anti-patterns and confirmed
named behavioral tests in unchanged local and actual completed CI logs. Four
roadmap criteria plus twenty plan truths deduplicate to twenty verified truths;
the report maps every input, so no extra plan-level requirement is omitted.
All 22 plan artifact references passed the canonical query. The key-link query's
literal repository-path heuristic matched only one link; manual C++/CMake tracing
resolved all twelve, as detailed in 02-VERIFICATION.md. No implementation gap or
outstanding human item was found; no override was needed. Rehashing all 36
accepted entries succeeded. Read-only GitHub metadata and all four job logs
again confirmed successful native evidence at the accepted commit.

After recording those findings, the canonical command:

```text
node /home/m4rkens/.codex/gsd-core/bin/gsd-tools.cjs query verification.status .planning/phases/02-source-identity-spans-and-diagnostics
```

exited 0 with status passed, next_action "Verification passed — continue." and
empty next_command. This is not permission to cross the owner publication gate.
The owner authorized evidence reuse; no new native build/test or independent
verifier-agent execution is claimed. The actual installed procedure, rather than
the previous report's existence, is now the basis for verification completion.

The applicable update_project_md document reconciliation was performed: completed
Phase 1/2 capabilities moved Active → Validated with phase notes, current state
and hosted evidence updated, stale initialization-only outcomes corrected, future
work kept active, known improvements/deferrals retained and last-updated refreshed.
Its automatic commit was withheld under the explicit owner no-commit constraint.

The resulting closeout changes exactly seven planning files: STATE, ROADMAP,
REQUIREMENTS, PROJECT and this execution record; new 02-04-SUMMARY.md and
02-VERIFICATION.md. The generic phase completion helper was not run because it
would advance STATE to the next phase; owner policy keeps current_phase=2 and
publication separate. No staging or commit command was invoked.

No C++/tests/CMake/scripts/workflows/profiles/recipes/baselines/dependency versions,
four locks, prior execution/summaries, discussion/research/plans, ignored IDE
binding or accepted raw evidence are modified. The planning-time VALIDATION.md
retains its historical future-tense strategy; final report/summary/state supply
current observed acceptance. No new test, acquisition, CI action or follow-up
implementation occurred. All three non-blocking follow-up candidates described
above remain deferred; initialization must still be explicit in current callers.

Phase 2 is accepted at 4/4 with all seven requirements; Phase 1 stays closed at
8/8. Final closeout is uncommitted. Next gate is owner review and explicit approval
for its planning-only commit/publication. No staging, commit/push, final milestone
completion or Phase 3 discussion/research/planning/implementation is authorized.
Proposed message: docs: close Phase 2 source and diagnostics acceptance.

Platform impact:
- Windows: Hosted Debug/Release complete source/diagnostic/rendering gates passed.
  Developer-host inspection remains independently pending/non-blocking.
- Linux: Primary Omarchy Debug/Release local evidence and independent Ubuntu
  Debug/Release CI accepted; baseline identities remain separate.
- macOS future: Deferred; no Darwin execution claimed or OS-specific dependency added.
