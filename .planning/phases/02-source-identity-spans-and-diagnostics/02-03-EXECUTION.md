# Plan 02-03 — Structured diagnostics and ordered accumulation

Status: Complete for validated Linux developer scope after owner acceptance and
documentation-only closeout. Phase 2 is 3/4; 02-04 unstarted. Phase 1 remains
closed at 8/8. The normal SUMMARY is prepared; commit/publication and Wave 4
remain separately owner-gated. No code changes or validation reruns occurred
during closeout.

## Authorization and exact wave selection

The owner accepted 02-02 after four-job Build foundation run 37908907406 at
commit 4c6be3a2e7d4694782c98ac879380458691edd67, then explicitly authorized
`$gsd-execute-phase 2 --wave 3` for Plan 02-03 only. Installed execute-phase.md
parses `--wave N` into WAVE_FILTER, skips other waves and completed summaries,
and rejects missing earlier-wave prerequisites. The read-only plan index reported
Wave 3 = [02-03], earlier summaries present, and Wave 4 = [02-04]. Scope was
confirmed before implementation. No extra flags or delegation were inferred.

The helper's warning about 02-PLAN-REVIEW.md concerns a review document, not an
additional execution plan. Its generic milestone-title output was not used to
rewrite canonical M1 state. Repository owner gates override generic automatic
commit/phase-completion behavior; execution stayed inline with no subagents.

## Implementation and reviewed contracts

- DiagnosticSeverity provides Error, Warning and Note. Diagnostic owns message
  bytes, optional primary SourceSpan and a vector of ordered RelatedSpan values
  with owned labels. Location-free and primary-less related context remain valid.
- DiagnosticEngine::emit takes an owned value. diagnostics returns a read-only
  span in insertion order, retaining duplicates; has_errors scans only for Error.
  Warning/Note alone do not fail compilation. Future clients must use this state
  independently of rendering; the version-only driver remains unchanged.
- Collection views/references must be reacquired after every emission and cannot
  outlive the engine. The plan leaves engine copy/move unspecified; this bounded
  implementation disables both to prevent assignment/movement invalidating views
  outside the documented emit/destruction boundary. Diagnostic values themselves
  remain ordinary owned, copyable/movable values. This C++ API safeguard is
  explicit for owner review and defines no Veyr language ownership semantics.
- The engine performs no source lookup or span repair. Invalid references remain
  exact inputs to checked SourceManager/renderer operations; source errors are
  not replaced with synthetic diagnostics. Manager-local numeric IDs retain their
  existing originating-manager contract.
- veyrcore gains diagnostic_engine.cpp. diagnostic_tests uses the existing target
  policy, veyrcore, GTest::gtest_main/GTest::gmock and PRE_TEST discovery. Neither
  diagnostic API nor engine imports LLVM, SourceManager, driver or rendering APIs.
  Source management has no reverse dependency on diagnostics.

Implementation scope is exactly the six files in 02-03-PLAN.md; bookkeeping adds
STATE, ROADMAP and this execution record (nine changed/new files). No renderer,
warning policy, suppression, sorting, deduplication, clear/reset API, callback,
global singleton, locking, framework or new dependency was introduced.

## Actual execution evidence

Execution date: 2026-10-15. All native commands ran sequentially from
`/home/m4rkens/Projects/VeyrLang`.
Every command exited 0; no retry, duplicate process, cleanup or hard timeout.
No command reached the soft 60-second output-idle liveness checkpoint.

Every dev.py invocation used:

```text
/usr/bin/python3 scripts/dev.py <command> --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
```

Doctor additionally used `--json`. Selected build directories D/R:

```text
D = build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Debug
R = build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Release
```

| Order | Command | Result | Retained log under .veyr-dev/execution/02-03/ |
|-------|---------|--------|----------------------------------------------|
| 1 | dev.py doctor, Debug | Read-only doctor 22/22; Conan 2.33.0 metadata | Observed tool output; no persistent doctor stdout capture claimed |
| 2 | dev.py configure, Debug | Cache-only generation/configure and native ABI probe passed | task1-Debug-configure.log |
| 3 | dev.py build, Debug | Project compile/link passed, 15 completed Ninja steps | task1-Debug-build.log |
| 4 | ctest D, focused Diagnostic | Task 1 tests 6/6 | task1-Debug-focused.log |
| 5 | dev.py configure, Release | Independent cache-only generation/configure and native ABI probe passed | task1-Release-configure.log |
| 6 | dev.py build, Release | Project compile/link passed, 15 completed Ninja steps | task1-Release-build.log |
| 7 | ctest R, focused Diagnostic | Task 1 tests 6/6 | task1-Release-focused.log |
| 8 | After Task 2 extension: dev.py build, Debug | Incremental test compile/link passed, 4 Ninja steps | final-Debug-build.log |
| 9 | ctest D, focused Diagnostic | Final 14/14, 0.21s | final-Debug-focused.log |
| 10 | dev.py verify, Debug | Eight C++23 entries; full CTest 69/69 including include-policy, 1.20s | final-Debug-verify.log |
| 11 | dev.py build, Release | Independent incremental test compile/link passed, 4 Ninja steps | final-Release-build.log |
| 12 | ctest R, focused Diagnostic | Final 14/14, 0.22s | final-Release-focused.log |
| 13 | dev.py verify, Release | Eight C++23 entries; full CTest 69/69 including include-policy, 1.08s | final-Release-verify.log |

Focused CTest command for each selected directory:

```text
ctest --test-dir <D|R> -R '^Diagnostic\.' --output-on-failure --stop-on-failure --no-tests=error
```

Native command stdout/stderr was streamed and retained with
`set -o pipefail` and `2>&1 | tee <log>` so log capture cannot hide failure.
The twelve ignored logs and evidence.json hash manifest retain this execution's
results. Generated D/R compile_commands.json, CMakeCache.txt and
Testing/Temporary/LastTest.log remain untracked. Future builds may replace their
contents; the dedicated command logs retain the observed results.

## Regression coverage and limits

Fourteen Diagnostic tests cover empty accumulation; all location-free severities;
optional primary; labelled related spans on same/different sources; short/long
caller-string mutation/destruction; moving and reusing caller values; mixed
severity/source/offset insertion order; repeated diagnostics/related spans;
4,096 emissions with safely reacquired read-only views; invalid unknown/reversed/
out-of-bounds references preserving exact inputs and Error state after checked
resolution failure; empty/NUL/CRLF/tab/ESC/UTF-8 message/label bytes; primary-less
related context and manager-local identity. Static assertions check owning field
types, const collection views and engine copy/move restrictions. No dangling
collection view is accessed, and no renderer is introduced to simulate failure.

Each final full suite comprises the existing 55 tests plus fourteen Diagnostic
cases. Source/span/location tests and implementation remain unchanged. The two
new translation units account for eight compilation-database entries, up from
six; strict selected directory/compiler/C++23 validation passed in both builds.
DIAG-01/DIAG-03 have Linux evidence, not phase-wide acceptance. All seven Phase 2
requirements remain pending; the checked renderer and integration are 02-04 work.

Both explicit configurations used Conan `install --no-remote --build=never`
through the established cache-only wrapper. LLVM package
`a214d896e569f9b075f68e0b581ce595d09d80df#cd9a88c23979fe65c10c97b796943a64`
was reused in both. GoogleTest Debug package
`77ee4a6c9da2ff6b1df8eac330ceb33aad0d1d69#3604fb7abde2c4493bc7897a104b3995`
and Release package
`4c47d11dc631ceaef479f42595e1124ab519c480#f244293f6c361ee557e76d8fed0392f4`
were reused. Recipe revisions remain LLVM aa4cad25b3148c3bdd22ed96fba04654,
GoogleTest e2507fc4efe2ec9e2db9e5c91eefab9f and build CMake 4.4.3
8b46ce9dbf1937af1c05f19579c1f105. No downloads or dependency builds occurred.
Existing generated Conan toolchain quotation warnings remain non-blocking.

All four locks, profiles, recipes, baselines, workflows, scripts, driver and
completed planning/execution artifacts remain byte-identical to the starting
checkpoint. The ignored local CMakeUserPresets.json is unchanged. No Conan CLI
installation, acquisition setup, cache purge or lock generation occurred.

Platform impact:
- Windows: Earlier source-plan hosted acceptance from 37908907406 is preserved;
  this new diagnostic implementation's native/CI evidence remains pending and
  non-blocking. Developer-host inspection stays independently pending.
- Linux: Primary Omarchy developer host; Debug/Release gates passed separately.
  Ubuntu hosted baseline remains distinct; no new CI execution was requested.
- macOS future: Deferred; standard C++ owned values/containers only, no required
  filesystem, terminal, POSIX or Win32 API.

## Owner-approved closeout and remaining gates

The owner completed architecture/code/evidence review and accepted Plan 02-03
for validated Linux developer scope with no acceptance-blocking defects. Normal
closeout changes only STATE, ROADMAP and this record, and creates 02-03-SUMMARY.md.
The accepted nine-file execution packet plus the summary totals ten files. All
six implementation/integration/test files remain unchanged; retained evidence,
locks, profiles, recipes, baselines and the ignored IDE binding are preserved.
No builds/tests or dependency operations were rerun.

Accepted API observation, non-blocking: current callers must explicitly initialize
Diagnostic::severity before its value is read. Default construction without
assignment leaves severity indeterminate. This closeout introduces no arbitrary
default, constructor change or new initialization policy. A compile-time enforced
initialization policy may be considered separately. Engine copy/move restrictions
and the emit/destruction view-lifetime contract were reviewed and accepted.
Raw doctor stdout is unavailable as already documented; allocation-failure
behavior was reviewed structurally without fault injection. These are evidence/
hardening opportunities, not acceptance blockers or language OOM decisions.

Phase 2 is now 3/4; Phase 1 stays 8/8. DIAG-01/DIAG-03 have accepted Linux scope
coverage, while all seven Phase 2 requirements await phase-wide acceptance.
New diagnostic Windows CI/native evidence remains pending/non-blocking,
independently of pending Windows developer-host inspection; macOS deferred.
Earlier run 37908907406 is source-plan evidence and is not credited for this
new diagnostic implementation. Diagnostic rendering remains outside 02-03.

No commit, push, CI dispatch/retry, Wave 4 or Phase 3 execution is authorized.
Next gate: owner approval of the prepared implementation/closeout commit and
publication, followed separately by authorization for Plan 02-04. Stop after
preparing closeout; do not infer publication permission from owner acceptance.
