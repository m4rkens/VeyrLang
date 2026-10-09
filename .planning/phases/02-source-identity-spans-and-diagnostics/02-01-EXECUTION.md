# Plan 02-01 — Execution Checkpoint

Owner command: `$gsd-execute-phase 2`, after planning commit
`e9f9adbad0cec21859b8e45b8ffa4f0caf9b8f79`. Execution inline; no delegated agents,
auto commit, push or CI action. Phase 1 remains closed at 8/8. Owner subsequently approved the available-Linux
Plan 02-01 closeout below; Phase 2 is now 1/4.

## Current checkpoint

Tasks 1–2 implemented and independently validated for the available Linux developer
scope. Final focused SourceManager tests passed 12/12 in each configuration; full
CTest passed 47/47 and all six C++23 compilation-database entries verified in each.
Owner reviewed and approved closeout for the validated Linux developer scope.
02-01-SUMMARY.md records this plan complete within that scope (Phase 2 is 1/4);
phase-wide requirement acceptance and Windows Phase 2/native evidence remain
pending/non-blocking. Plan 02-02 has not started.

Read-only Linux Debug doctor passed 22/22, Conan package metadata 2.33.0, current
Omarchy/Arch developer baseline unchanged. Dedicated Conan home exists:
`/home/m4rkens/.cache/veyr/conan/22e2b2472775`.
Selected build fingerprint is `3543ddbc0bf7`, separate from older local output
`5221f1471fc7`. Configure uses the existing reviewed lock/cache only with
`--no-remote --build=never`; all required packages were already cached. The scoped
configure commands were authorized outside the filesystem sandbox because the
existing dedicated Conan home lies outside the workspace. No dependency package
build/download or Conan CLI installation occurred.

## Implementation and decision coverage

Exact implementation/integration files:

- `include/veyr/source/source_id.hpp`
- `include/veyr/source/source_span.hpp`
- `include/veyr/source/source_manager.hpp`
- `src/source/source_manager.cpp`
- `tests/source/source_manager_test.cpp`
- `CMakeLists.txt`
- `tests/CMakeLists.txt`

The implementation follows the reviewed Plan 02-01 API without reopening decisions:

- D-01: Completed immutable heap records own source bytes and display names;
  vector growth moves owning pointers only. Copy/move of the manager is disabled;
  no erase/reset/replace API invalidates views during its lifetime.
- D-02/D-03: Each successful registration receives a fresh nonzero manager-local
  ID, including identical text/name registrations. Separate managers may both
  assign ID 1; no global identity or origin-detection promise is added.
- D-04: Caller display-name bytes are copied/moved unchanged; no filesystem,
  path normalization, canonicalization or deduplication occurs.
- D-05/D-07: Half-open original-byte retrieval permits empty/interior/EOF spans,
  NUL/high bytes and CRLF-split ranges without normalization.
- D-06/D-08: Checked variants distinguish UnknownSource, ReversedRange and
  BeyondSource in that precedence, retaining the supplied ID/offsets and known
  size where available. Release behavior does not depend on assertions.
- Numeric capacity checks preserve ID/index and future one-based EOF headroom;
  the planned private pure helper is tested through the narrow friend accessor
  without enormous allocations or a production synthetic-capacity mode.

`veyrcore` gains the source implementation. `source_tests` uses the existing
target policy, GoogleTest/GoogleMock and PRE_TEST discovery; it links no LLVM
target. No generic result framework or new dependency is introduced.

## Actual command ledger

All commands ran from `/home/m4rkens/Projects/VeyrLang` on 2026-10-09. They were
announced and executed sequentially; every native command exited 0. No failure,
automatic retry, duplicate launch, destructive cleanup or quiet-build termination
occurred. Commands completed without reaching the soft 60-second liveness gate.

Selection for every developer command below:

```text
--profile linux-x86_64-clang --environment linux-developer --llvm-provider managed
```

Selected build directories:

```text
build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Debug
build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Release
```

| Order | Command (selection above included for each dev.py invocation) | Observed result |
|-------|---------------------------------------------------------------|-----------------|
| 1 | `/usr/bin/python3 scripts/dev.py doctor ... --config Debug --json` | Read-only doctor 22/22; exact project-owned Conan distribution 2.33.0 |
| 2 | `/usr/bin/python3 scripts/dev.py configure ... --config Debug` | Cache-only Conan generation and CMake configure passed; native runtime probe passed |
| 3 | `/usr/bin/python3 scripts/dev.py build ... --config Debug` | Initial project compile/link passed, 22 Ninja steps |
| 4 | `ctest --test-dir <Debug-dir> -R '^SourceManager\.' --output-on-failure --stop-on-failure --no-tests=error` | Initial Task 1 SourceManager tests 6/6 passed |
| 5 | `/usr/bin/python3 scripts/dev.py configure ... --config Release` | Independent cache-only generation/configure passed; native runtime probe passed |
| 6 | `/usr/bin/python3 scripts/dev.py build ... --config Release` | Initial project compile/link passed, 22 Ninja steps |
| 7 | `ctest --test-dir <Release-dir> -R '^SourceManager\.' --output-on-failure --stop-on-failure --no-tests=error` | Initial Task 1 SourceManager tests 6/6 passed |
| 8 | Extend only `tests/source/source_manager_test.cpp` for Task 2 | Twelve focused tests now defined; production implementation unchanged |
| 9 | `/usr/bin/python3 scripts/dev.py build ... --config Debug` | Incremental source-test compile/link passed, four Ninja steps |
| 10 | `ctest --test-dir <Debug-dir> -R '^SourceManager\.' --output-on-failure --stop-on-failure --no-tests=error` | Final SourceManager 12/12 passed |
| 11 | `/usr/bin/python3 scripts/dev.py verify ... --config Debug` | Six selected C++23 entries verified; full CTest 47/47 including include-policy passed |
| 12 | `/usr/bin/python3 scripts/dev.py build ... --config Release` | Independent incremental source-test compile/link passed, four Ninja steps |
| 13 | `ctest --test-dir <Release-dir> -R '^SourceManager\.' --output-on-failure --stop-on-failure --no-tests=error` | Final SourceManager 12/12 passed |
| 14 | `/usr/bin/python3 scripts/dev.py verify ... --config Release` | Six selected C++23 entries verified; full CTest 47/47 including include-policy passed |

The exact developer-command form was, for example:

```sh
/usr/bin/python3 scripts/dev.py verify --profile linux-x86_64-clang --environment linux-developer --config Release --llvm-provider managed
```

Generated evidence remains untracked under the selected build directories:
`compile_commands.json`, `CMakeCache.txt` and `Testing/Temporary/LastTest.log`.
Configure/build/verification stdout and exit codes were observed in the execution
tool results; no separate persistent command-log files are claimed. Each final
CTest log records all 47 tests passing, including the twelve newly implemented
SourceManager tests. Driver.Version records a successful required-regex match as
`Test Pass Reason`, while the other 46 record `Test Passed.`. The initial
six-case runs are historical Task 1 checks; final evidence uses the twelve-case
Task 2 suite. Historical Phase 1 evidence is not substituted for these new runs.

## Focused regression coverage

Twelve native tests cover caller mutation/destruction, fresh duplicate snapshots,
manager-local numeric IDs, short/long/empty text and name pointer/content stability
after 3,000 additional sources, empty/full/interior/EOF spans, distinct error payloads,
every in-bounds range of bounded byte fixtures, CRLF splitting, unknown and extreme
IDs/ranges, deterministic failure precedence, exact path/synthetic/control-byte
display metadata, and representability boundaries without huge allocations.

The Debug and Release full suites each include the unchanged 35 foundation tests
plus these twelve SourceManager tests. Existing Windows CRT fixtures also pass
under their synthetic inputs on Linux; this is not native Windows evidence.

## Dependency and scope preservation

Both configurations reused LLVM 23.1.2 package
`a214d896e569f9b075f68e0b581ce595d09d80df#cd9a88c23979fe65c10c97b796943a64`.
Debug used cached GoogleTest package
`77ee4a6c9da2ff6b1df8eac330ceb33aad0d1d69#3604fb7abde2c4493bc7897a104b3995`;
Release likewise used its existing cached configuration package. Reviewed recipe
identities remain LLVM `aa4cad25b3148c3bdd22ed96fba04654`, GoogleTest
`e2507fc4efe2ec9e2db9e5c91eefab9f` and build-context CMake 4.4.3
`8b46ce9dbf1937af1c05f19579c1f105`. No new package acceptance surface is claimed.

All four tracked lock bytes, recipes, profiles, baselines, workflows, scripts,
driver and completed Phase 1 artifacts remain unchanged. Discussion/research and
all four Phase 2 plans are preserved. Execution bookkeeping is limited to this
record, SUMMARY, STATE and ROADMAP; REQUIREMENTS.md remains unchanged pending
phase-wide acceptance.

Existing non-blocking generated Conan toolchain quotation warnings were observed
during Linux configuration; they did not prevent configure/build/tests/verify and
are retained as evidence rather than changing Phase 1 policy.

## Boundaries

No changes to plans/discussion/research, Conan inputs, locks, profiles, recipes,
baselines, workflow, scripts, LLVM policy, driver or dependency versions.
No dependency package acquisition/build or Phase 2 CI yet. Veyr/source-test native
builds above were explicitly authorized execution. No automatic retry or cleanup.
No commit, push, plan closeout, dedicated verification stage or next-plan execution
is inferred from successful checks. Source locations, diagnostics, rendering and
Phase 3 remain unstarted. The owner explicitly approved available-Linux closeout
and one dedicated checkpoint commit/push. Native Windows evidence remains pending
and non-blocking; no manual CI dispatch/retry or Plan 02-02 start is authorized.

Platform impact:
- Windows: Same byte API/tests; native Phase 2 CI evidence pending, developer-host evidence independently pending/non-blocking.
- Linux: Primary Omarchy developer host; independent Debug/Release gates passed, separate from Ubuntu CI.
- macOS future: Deferred; only standard C++ byte/string storage APIs.

## Owner-approved closeout and publication boundary

On 2026-10-09 the owner approved Plan 02-01 closeout for the validated Linux
developer scope and instructed a single dedicated implementation/closeout commit
followed by `git push origin main`. This records Phase 2 progress 1/4; Phase 1 stays
closed at 8/8. The reviewed ten-file packet was confirmed before closeout; the
normal 02-01-SUMMARY.md adds one file, yielding eleven files for this checkpoint.
Seven implementation/integration files remain byte-identical to their reviewed
versions; all four locks match HEAD, and unrelated tracked files are unchanged.

No phase-gate rule requires rerunning successful native checks for unchanged
implementation. Existing final Debug/Release CTest logs and compilation databases
were inspected; accepted 12/12 focused, 47/47 full CTest and 6/6 C++23 results are
preserved. Closeout performs no additional dependency setup, native builds/tests,
lock generation or implementation edits. No separate GSD verification stage is
claimed. Requirements remain pending for phase-wide acceptance; this summary
records SRC-01/SRC-02/SRC-03 Linux evidence only.

Windows Phase 2/native evidence remains pending/non-blocking, separate from the
independently pending Windows developer-host inspection. macOS remains deferred.
Resolve the dedicated checkpoint hash from Git history; any push-triggered Build
foundation run is observed only, with no retry/duplicate/manual dispatch. Stop for
owner review after publication; Plan 02-02 remains unstarted.
