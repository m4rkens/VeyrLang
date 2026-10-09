# Plan 02-02 — Byte locations and newline boundaries

Status: Complete for available Linux developer scope after explicit owner-approved
closeout. Phase 2 is 2/4; 02-03/02-04 unstarted. Phase 1 remains closed at 8/8.
The owner authorized the required SUMMARY and one dedicated execution checkpoint
commit/push. Execution followed the owner's renewed `$gsd-execute-phase 2`;
closeout does not authorize another plan or manual CI action.

## Implementation and settled contracts

- Added `SourceLocation { source_id, offset, line, column }` with value equality.
  Offset remains an original-byte position; lines and columns are one-based.
- Extended immutable source records with an LF-derived line-start vector fully
  built before publishing the snapshot. Binary-search lookup maps LF and both
  CRLF bytes to the preceding line; lone CR is ordinary data.
- Added checked `location` and `line_span_at` local variant results. Unknown ID
  takes precedence; offset beyond size is rejected. Errors retain ID and exact
  point offsets (`begin == end == offset`), plus known source size when available.
  EOF is valid. Containing-line spans retain terminator bytes and trailing-break
  EOF yields an empty final-line span. No clamping, Unicode decoding or lookup
  through filesystem paths is introduced.
- Existing snapshot ownership, fresh manager-local identity, stable views,
  structural span validity and exact caller metadata are preserved. Presentation
  escaping stays with the future renderer, not storage/location mapping.
- The existing source_tests target is extended; no CMake target/integration,
  dependency, driver, LLVM link or generic test framework changes are needed.

Production scope is the four files named in 02-02-PLAN.md: source_location.hpp,
source_manager.hpp, source_manager.cpp and source_manager_test.cpp. Bookkeeping
is this record plus STATE/ROADMAP. Discussion, research, all four plans, validation
strategy, patterns/self-review and completed 02-01 records remain unchanged.

## Actual command ledger

All commands ran sequentially from `/home/m4rkens/Projects/VeyrLang` on
2026-10-09. Selection for every dev.py invocation:

```text
--profile linux-x86_64-clang --environment linux-developer --llvm-provider managed
```

Selected directories (D and R in the table):

```text
D = build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Debug
R = build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Release
```

| Order | Command (selection above on each dev.py command) | Observed result |
|-------|--------------------------------------------------|-----------------|
| 1 | `/usr/bin/python3 scripts/dev.py doctor ... --config Debug --json` | Read-only doctor 22/22; project-owned Conan metadata 2.33.0 |
| 2 | `/usr/bin/python3 scripts/dev.py build ... --config Debug` inside sandbox | Exit 2 before Conan/native launch: operation lock outside workspace could not be created on read-only filesystem |
| 3 | Same authorized Debug command with escalated cache access | Cache-only regeneration/configure passed, native ABI probe passed, project build passed (13 completed Ninja steps) |
| 4 | `ctest --test-dir D -R '^SourceLocation\.' --output-on-failure --stop-on-failure --no-tests=error` | Task 1 location tests 4/4 passed, 0.09s |
| 5 | `/usr/bin/python3 scripts/dev.py build ... --config Release` | Independent incremental project build passed (10 Ninja steps); existing configuration reused |
| 6 | `ctest --test-dir R -R '^SourceLocation\.' --output-on-failure --stop-on-failure --no-tests=error` | Task 1 location tests 4/4 passed, 0.10s |
| 7 | After Task 2 test extension: `/usr/bin/python3 scripts/dev.py build ... --config Debug` | Incremental test compile/link passed, 4 Ninja steps |
| 8 | `ctest --test-dir D -R '^(SourceManager|SourceLocation)\.' --output-on-failure --stop-on-failure --no-tests=error` | Final focused suite 20/20 passed, 0.40s |
| 9 | `/usr/bin/python3 scripts/dev.py verify ... --config Debug` | Six selected C++23 entries verified; full CTest 55/55 including include-policy passed, 1.08s |
| 10 | `/usr/bin/python3 scripts/dev.py build ... --config Release` | Independent incremental test compile/link passed, 4 Ninja steps |
| 11 | `ctest --test-dir R -R '^(SourceManager|SourceLocation)\.' --output-on-failure --stop-on-failure --no-tests=error` | Final focused suite 20/20 passed, 0.31s |
| 12 | `/usr/bin/python3 scripts/dev.py verify ... --config Release` | Six selected C++23 entries verified; full CTest 55/55 including include-policy passed, 1.01s |

The initial sandbox denial was a filesystem permission boundary, not a native
or package failure. The required escalation was requested for the same authorized
command; no package/build failure was retried, no duplicate process was launched,
and no cleanup occurred. Every subsequently launched native/package command
exited 0. Commands completed before a 60-second output-idle liveness checkpoint.

Debug stale configuration used the unchanged explicit preset and Conan
`install --no-remote --build=never` path. It reused existing LLVM/GoogleTest;
this is cache-only generation, not acquisition setup. Release configuration was
already current and remained independently selected. Existing generated Conan
toolchain quotation warnings appeared during Debug configure; no new blocker.

Generated evidence remains untracked in D/R: compile_commands.json,
CMakeCache.txt and Testing/Temporary/LastTest.log. Final CTest logs contain all
55 tests, including eight SourceLocation and twelve SourceManager cases. Command
stdout/exit codes were observed in execution tool results; no separate persistent
stdout capture is claimed.

## Coverage and scope preservation

Task 1 covers the approved empty/no-break/LF/CRLF/repeated-break/lone-CR boundary
table, containing lines with original terminators, valid EOF and explicit unknown
or out-of-bounds point payloads, including maximal numeric offsets/IDs.

Task 2 compares every offset of all 2,801 strings of length zero through four
over seven ordinary/newline/tab/NUL/UTF-8 bytes against an independent scanning
oracle. Additional mixed UTF-8/NUL/tab/newline fixtures, exact drive/UNC/relative/
synthetic/control-byte names, and retained text/name/line views through 3,000
further indexed registrations pass in both Debug and Release. Existing twelve
snapshot/span tests remain unchanged and pass; the full suite also retains all
35 foundation tests. Synthetic Windows CRT tests on Linux are not native Windows
Phase 2 evidence.

SRC-04 has local observable coverage alongside SRC-01–SRC-03 regressions; all
seven Phase 2 requirements remain pending for phase-wide acceptance. No settled
D-01–D-16 decision was reopened; rendering/diagnostics remain later-plan work.

Conan 2.33.0 and LLVM 23.1.2 remain fixed. Debug reused cached LLVM package
`a214d896e569f9b075f68e0b581ce595d09d80df#cd9a88c23979fe65c10c97b796943a64`
and GoogleTest package
`77ee4a6c9da2ff6b1df8eac330ceb33aad0d1d69#3604fb7abde2c4493bc7897a104b3995`.
Release continued using its prepared cached packages. No dependency downloads,
builds, CLI installation, cache purge, lock generation or acquisition setup ran.
All four locks, profiles, recipes, baselines, workflows, scripts and completed
planning artifacts remain byte-identical to the starting checkpoint. The ignored
Code - OSS CMakeUserPresets.json is preserved unchanged.

Platform impact:
- Windows: Same byte/path-metadata contracts and portable C++23 tests; native
  Phase 2 evidence pending/non-blocking. Developer-host inspection independently
  pending. No Windows runtime/toolchain/CI policy change or native claim.
- Linux: Primary Omarchy host; Debug/Release gates passed independently. Ubuntu
  CI remains separate; this execution does not collect hosted CI evidence.
- macOS future: Deferred. Standard C++ byte/string/index APIs only; no POSIX,
  Win32, filesystem, locale or terminal dependency.

## Owner-approved closeout and publication boundary

The owner approved Plan 02-02 closeout for validated Linux developer scope and
instructed one dedicated implementation/closeout commit followed by
`git push origin main`. The reviewed seven-file execution packet is preserved;
the normal 02-02-SUMMARY.md adds one file, yielding eight files in this checkpoint.
The four implementation/test files remain byte-identical to their reviewed
versions. All four locks match HEAD; unrelated tracked files and the ignored
local IDE preset are unchanged. Completed discussion/research/plans and 02-01
execution/summary are preserved.

No phase-gate rule requires repeating successful checks for unchanged code.
Closeout inspected existing final CTest logs and compilation databases, preserving
20/20 focused, 55/55 full CTest and 6/6 C++23 results for each Linux configuration.
No additional build/test, dependency setup, lock generation or implementation
edit occurred. No dedicated GSD verification stage or hosted Phase 2 acceptance
is claimed. All seven requirements remain pending for phase-wide acceptance.

Phase 2 is now 2/4; Phase 1 stays closed at 8/8. Windows Phase 2/native evidence
remains pending/non-blocking, separately from independently pending Windows
developer-host inspection. macOS remains deferred. Resolve the dedicated
checkpoint hash from Git history; any push-triggered Build foundation run is
observed only, without retry/duplicate/manual dispatch. Stop for owner review
after publication; Plan 02-03 remains unstarted.
