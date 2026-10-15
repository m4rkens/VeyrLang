# Plan 02-04 — Bounded plain-text diagnostic rendering

Status: Tasks 1–2 implemented, independently Linux Debug/Release validated and
reviewed without acceptance-blocking defects. Execution checkpoint documentation
prepared under owner authorization. Plan 02-04 remains open; publication, new
four-job renderer CI evidence and final closeout require separate owner approval.
Phase 2 remains 3/4, Phase 1 remains closed at 8/8. No final SUMMARY is created.

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
The latest owner command authorizes CI-prerequisite inspection, completion of
this execution checkpoint and publication-readiness reporting only. It does not
authorize commit/push, final phase closeout or new execution. No implementation,
test or retained evidence changes occurred during this documentation step.

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
historical; the completed CI observation below supersedes it without modifying
accepted evidence. STATE/ROADMAP are the only newly updated original tracked
files beyond the reviewed CMake integration.

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

## Exact publication packet and remaining owner gates

Intended commit contains exactly eight files:

- include/veyr/diagnostics/plain_text_renderer.hpp
- src/diagnostics/plain_text_renderer.cpp
- tests/diagnostics/plain_text_renderer_test.cpp
- CMakeLists.txt
- tests/CMakeLists.txt
- .planning/STATE.md
- .planning/ROADMAP.md
- .planning/phases/02-source-identity-spans-and-diagnostics/02-04-EXECUTION.md

The latest step creates only this record and updates STATE/ROADMAP. The accepted
five implementation/integration/test files and retained evidence remain unchanged.
No REQUIREMENTS, summary, completed discussion/research/plans/earlier execution
artifacts, workflow, profile, baseline, recipe, lock or dependency version changes.
Nothing staged, committed or pushed; no CI dispatch/retry/cancellation.

Next gate: explicit owner authorization for this exact implementation/checkpoint
commit and normal push. Proposed message: diagnostics: add bounded plain-text rendering.
Only after separately authorized publication may the resulting normal four-job CI
supply new renderer evidence. Task 3's independent Tier 1 acceptance/final closeout
remains incomplete. No final summary, Phase 2 acceptance, dedicated phase
verification or Phase 3 transition is authorized. Phase 2 stays 3/4 with all seven
phase-wide requirements pending; Phase 1 stays closed at 8/8.

Platform impact:
- Windows: Structured diagnostics passed both CI configurations in 38705163273;
  renderer hosted/native validation remains pending. Developer-host inspection
  remains independently pending/non-blocking.
- Linux: Renderer validated separately in Omarchy Debug/Release; Ubuntu remains
  an independent CI baseline and has not yet run the new renderer.
- macOS future: Deferred; no filesystem, console, POSIX or Win32 dependency added.
