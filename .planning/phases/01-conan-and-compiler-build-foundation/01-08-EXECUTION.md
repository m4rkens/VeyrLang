# Plan 01-08 execution checkpoint

**Date:** 2026-10-07
**Status:** Local Tasks 1–2 drafted and required mocked/static task checks passed.
The owner subsequently authorized checkpoint commits, publication and expanded
observations. Run 37643695998 supplied four successful observations followed by
expected pending-policy failures. The owner now approved the bounded toolchain
policy alignment: both CI candidates are reviewed; CI execution remains explicitly
disabled before tool/bootstrap/package/native work. Task 3 remains a blocking
owner-review/execution-authorization/locks/native-evidence checkpoint, not a completed plan.
Progress remains 7/8 execution plans, zero completed phases. No SUMMARY created.

## Authorization and preserved work

Owner invoked `$gsd-execute-phase 1`, then selected local execution through the
next blocking gate. Work is inline; no sub-agents requested or used. Completed
research, decisions, discussion, all eight plans/waves, implementation and native
Linux evidence were preserved. No Conan reinstall, native/package operation,
cache purge, LLVM/GoogleTest rebuild or successful earlier acceptance rerun.

Plan 01-07 execution/closeout is committed in
`ddec666b43546d8820d7eacd28e6ce4ef2b068ed`; the separately reviewed missed
Support lifetime test is committed in
`eef4780faa9063a50ae5ede89fc40f55408279bb`. Older STATE wording that it remained
uncommitted is now reconciled in the normal current execution checkpoint.

## Task 1 local draft

Created `.github/workflows/build-foundation.yml` with PR, push-main and manual
triggers; Ubuntu 24.04 and Windows 2022 × Debug/Release; fail-fast false and
max-parallel 1; explicit hosted hard timeout 360 minutes.

- Installed runner tools/paths and image/VS candidates are observations only.
  Pending CI policy invokes the existing read-only doctor and fails before
  tool installation, cache restore or any dependency work. No expected identity
  is inferred from runner labels or copied from developer policy.
- Existing pending CI entries gain environment/runner labels and an explicit
  evidence note only. Developer platform entries, shared exact tool/backend
  values and all profiles/locks/recipes remain unchanged. Conan stays 2.33.0;
  LLVM backend stays 23.1.2; Windows candidates stay independently pending.
- For later independently reviewed CI entries, setup-python selects exact base
  Python; conventional venv/isolated pip provides exact tracked CMake/Ninja
  prerequisites. Windows enters the selected Microsoft VS developer shell.
  Installed native compiler/GCC or VS/CRT/SDK/linker identity must pass doctor.
  No host/compiler installer framework or backend SDK acquisition added.
- Project CLI bootstrap uses existing `setup --tools-only` and absolute
  project-owned Conan through existing selection. Doctor is unchanged and
  retains package-metadata-only read-only semantics. Required tracked lock
  absence blocks package work; no CI `--init-lock` or silent lock generation.
- Conan cache restore/save is an optimization after doctor/lock gates, with
  exact baseline/profile/recipe/options/lock/consumer/preset hashes plus actual
  runner image/environment/architecture/configuration. No broad fallback keys
  or cross-OS archives. Only the external unique job/attempt home is cached;
  the action saves on success, not on failed/bootstrap-only runs.
- Manual cold_build skips restore/save and selects a fresh isolated home; no
  active home is deleted. Explicit setup may build genuinely missing packages
  via Conan. Configure/build/test/verify use existing Python/cache-only policy.
  No all-followed-by-duplicate-commands path. Distinct prepared/offline
  foundation checks use existing strict failure handling and supervision.
- Command/doctor/setup wall-time logs and compile/native records are uploaded
  even on bootstrap failure. No CLI venv, package binaries outside Conan cache,
  build tree or foreign SDK publication protocol. Peak resources remain
  unmeasured; setup wall time is not peak memory/disk evidence.

Action definitions at the four immutable revisions were fetched read-only from
upstream GitHub using the web tool; action input/runtime definitions were
reviewed. Exact pins/source links are in CI_MATRIX_POLICY.md. This is source
inspection, not runner execution or a general security/provenance guarantee.

## Task 2 local documentation

Updated DEVELOPER_BOOTSTRAP.md, CI_MATRIX_POLICY.md, FOUNDATION_ACCEPTANCE.md and
README.md against actual argparse, operation implementations, CMake presets and
Conan inputs. Docs describe all eight commands, Debug default/Release override,
explicit setup/acquisition and cache-only chaining, exact observed developer
identities/candidate boundaries, absolute CLI, dedicated home and scoped clean.
Managed/system/custom LLVM discovery retains exact configuration/runtime guards.
Future language Veyr.lock remains separate. Historical accepted Linux native
results/hash observations are retained, not promoted to current CI evidence.

At the local draft stage the CI ledger had four independently pending rows, no
invented URLs/tool pins or capacity measurements. The observation-only update
below supplies real job URLs and candidate identities. Windows locks
and native/CI acceptance are still missing. Existing foundation JSON
conservatively says CI pending; individual
native task results and actual job URLs need explicit ledger review before
acceptance. No phase-wide requirements marked complete.

## Local execution-task checks performed

All commands ran from `/home/m4rkens/Projects/VeyrLang`:

| Check | Observed result |
|---|---|
| `/usr/bin/python3 -B -m unittest discover -s tests/development -p test_foundation_contract.py` | 16/16 passed; 0.066s suite time; exit 0; Conan/native commands mocked |
| `/usr/bin/python3 -B -m unittest discover -s tests/development -p test_dev_commands.py` | 16/16 passed; 0.075s suite time; exit 0; Conan/native commands mocked |
| Read-only Python/PyYAML BaseLoader + JSON static assertions | Passed: exact four rows/triggers, fail-fast/serial/timeout policy, pending → doctor/lock → cache → package order, cold skip, full cache inputs, 40-hex action pins, shared explicit selections, no all/init-lock, always-upload and unchanged developer/tool/backend identities |
| Diff whitespace and protected-file scope inspection | Passed; final inspection recorded below |

During those local draft checks no real Conan/CMake/Ninja/CTest/compiler/linker
command, dependency operation, installation or build ran locally, and no
push/dispatch/stage/commit/phase verification or advancement occurred. Runner
PowerShell/action execution was then untested. Later owner-approved publication
and observation execution are recorded below; no native CI build has run.
YAML parsing/static inspection is not an Actions or PowerShell execution guarantee.

## Task 3 authorized publication and candidate observations

The owner explicitly authorized the nine-file local draft checkpoint
`01acd22be35226c9c2aa0310da28f89d0a63efcc`, then `git push origin main`.
Automatic run 37355569361 failed workflow validation with zero jobs. The reviewed
one-file correction moved CONAN_HOME from job-level env into the existing
observation step env and persisted it through GITHUB_ENV; the owner committed
it as `c4f27d086370c8d63d0b81990eba04044bdbe98f` and authorized its push.
No manual dispatch or automatic retry occurred.

The resulting automatic push run
[37356855017](https://github.com/m4rkens/VeyrLang/actions/runs/37356855017)
completed the four observation steps and artifact uploads, then all four jobs
failed at the expected pending CI-policy gate. No tools-only/CLI installation,
cache restore/save, dependency setup, native build/test or package operation ran.
No Conan installation/version was observed. These are identity observations,
not successful native CI rows or package/ABI readiness evidence.

| Observed identity | Ubuntu Debug/Release | Windows Debug/Release |
|---|---|---|
| Runner / ImageOS / image version | ubuntu-24.04 / ubuntu24 / 20260927.320.1 | windows-2022 / win22 / 20260927.320.1 |
| Runner agent | 2.337.0 | 2.337.0 |
| OS / architecture | Ubuntu 24.04.5 LTS / x86_64 | Windows Server 2022 Datacenter 10.0.20348 / x86_64 |
| Python / CMake / Ninja | 3.12.3 / 3.31.6 / 1.13.2 | 3.12.10 / 3.31.6 / 1.13.2 |
| Clang / clang++ / clang-cl | 18.1.3 / 18.1.3 / missing | 20.1.8 / 20.1.8 / 20.1.8 |
| Target | x86_64-pc-linux-gnu | x86_64-pc-windows-msvc |
| GCC / G++ / glibc / Binutils | 13.3.0 / 13.3.0 / 2.39 / 2.42 | Not applicable |
| Visual Studio Enterprise 2022 | Not applicable | 17.14.37710.0 |
| Git | 2.55.0 | 2.55.0.windows.5 |

Debug and Release share the observed platform identities, while their homes
remain isolated by run/attempt/environment/configuration. Exact observed paths
and four job URLs are in baseline.json and FOUNDATION_ACCEPTANCE.md. Ubuntu and
Windows CI entries remained pending/unselected at this historical checkpoint;
the owner authorized recording observations only. Omarchy developer policy and the independently pending
Windows developer candidate are unchanged. Conan 2.33.0 and LLVM 23.1.2 remain
required; progress is still 7/8 with all eight waves preserved and macOS deferred.

At that initial checkpoint, Clang 18 conflicted with the existing Clang-22
developer/profile/doctor policy; resolved libstdc++ runtime/path, standard-library
header identity and exact linker path were not established. The locator reported
only `libstdc++`. The then-current doctor libc/linker parsers did not accept observed Ubuntu
banner formats. No profile or parser was changed to bypass these gaps.

At that initial checkpoint, VC tools, CRT, Windows SDK, exact linker identity/path
and selected developer-shell/toolchain environment were not established. VS
installation alone does not prove these identities. Both platforms still need
project-owned Conan validation, native ABI/package acceptance and cold-build
capacity evidence; Windows Debug/Release locks remain pending.

That candidate-recording edit scope was exactly baseline.json, CI_MATRIX_POLICY.md,
FOUNDATION_ACCEPTANCE.md, README.md, STATE.md, ROADMAP.md and this execution
record. Workflow/profiles/doctor and all other files were unchanged. No additional
CI run, setup/package/native operation, commit or phase advancement was authorized
or performed by that candidate-recording step. The seven-file record was separately
committed in `c4447eb3668eab0e62f0d0a2edd0e0cba76e45fe`.

## Task 3 expanded observations and reviewed policy alignment

Expanded observation publication and infrastructure-only reruns were separately
owner-authorized. Windows Release evidence from run 37361465857 is preserved;
jobs that acquired no runner supplied no project/toolchain failure evidence.
The Linux multiple-command lookup defect was corrected in the observation-only
checkpoint `160d550`, preserving Windows observations and partial Linux evidence.
Run 37634442631 completed all four expanded observations but left the default
Linux GCC selection and Windows DevShell compiler precedence decisions unresolved.
The owner separately approved observation-only explicit GCC-13 and standalone
Windows Clang checks, committed in
`63c275d175e727830a6c30ba8204dcc3e102bdca`, and publication.

[Run 37643695998](https://github.com/m4rkens/VeyrLang/actions/runs/37643695998)
acquired all four hosted runners. Every expanded observation and artifact upload
succeeded; every job then stopped with exit 1 at the expected pending-policy gate.
No CI tool installation, Conan bootstrap, cache restore/save, package operation
or native build/test ran. All previous observations and remote artifacts remain
historical evidence, not acceptance claims.

Owner-approved contracts now recorded in baseline.json:

- Ubuntu CI: Python 3.12.3, CMake 3.31.6, Ninja 1.13.2, Clang/clang++ 18.1.3,
  native GNU target, explicit `/usr/lib/gcc/x86_64-linux-gnu/13`, GCC/G++ 13.3.0,
  GCC-13 C++ headers, raw `_GLIBCXX_RELEASE=13` and `_GLIBCXX_USE_CXX11_ABI=1`,
  runtime resolving to `libstdc++.so.6.0.33`, glibc 2.39, GNU ld.bfd/Binutils 2.42.
  Debug/Release header/search/runtime/linker observations match; driver trace
  differs only by temporary object name. Raw default-GCC observations are retained.
- Windows CI: Python 3.12.10, CMake 3.31.6, Ninja 1.13.2, absolute standalone
  LLVM 20.1.8 tools under `C:\Program Files\LLVM\bin`, native MSVC target,
  VS Enterprise 2022 17.14 family (observed 17.14.37710.0), VC tools 14.44.35207,
  CRT header 14.44.35220.0, SDK/UCRT SDK 10.0.26100.0, native linker 14.44.35229.0.
  DevShell supplies VC/CRT/SDK/linker variables; future execution selects absolute
  standalone clang-cl for both C/C++, validates all three standalone executables
  and restores job-local standalone PATH precedence. Bundled LLVM 19.1.5 is an
  observation only. Debug/Release core toolchain identities match despite image,
  Git/PATH and VS installation metadata differences.
- Git is required and recorded with its observed version/path; no exact version
  or patch gate and no ABI/build identity contribution. No prior minimum version
  requirement was found. Windows Debug observed 2.56.0.windows.1 while Release
  observed 2.55.0.windows.5; that utility drift does not invalidate the toolchain.

The Linux profile takes its compiler major from the explicitly reviewed selection:
Omarchy remains 22; Ubuntu CI is intentionally 18. No profile detection, developer
fallback, global PATH mutation or second package-management path was introduced.
Doctor accepts the observed Ubuntu GLIBC/GNU linker banners, retains strict native
comparisons, and keeps project-owned Conan checks package-metadata-only/read-only.
Computed build fingerprints now exclude observation metadata and Git; historical
Linux build paths/results are retained, not revalidated under the new fingerprint.

Both CI entries have `status: reviewed` for toolchain policy only. The workflow
retains an independent `VEYR_CI_EXECUTION_AUTHORIZED: 'false'` gate before Python,
CMake/Ninja installation, Conan bootstrap, locks/cache and native operations.
Reviewing candidates cannot implicitly authorize any of those steps.

Current bounded edit scope (13 files):

- `.github/workflows/build-foundation.yml`
- `scripts/devtools/baseline.json`
- `scripts/devtools/doctor.py`
- `conan/profiles/linux-x86_64-clang`
- `tests/development/test_doctor.py`
- `tests/development/test_foundation_contract.py`
- `docs/development/CI_MATRIX_POLICY.md`
- `docs/development/DEVELOPER_BOOTSTRAP.md`
- `docs/development/FOUNDATION_ACCEPTANCE.md`
- `README.md`
- `.planning/STATE.md`
- `.planning/ROADMAP.md`
- this `01-08-EXECUTION.md`

Bounded checks from `/home/m4rkens/Projects/VeyrLang`:

| Mocked/static unit suite (`/usr/bin/python3 -B -m unittest discover -s tests/development -p ...`) | Result |
|---|---|
| `test_doctor.py` | 43/43 passed; exit 0 |
| `test_foundation_contract.py` | 18/18 passed; exit 0 |
| `test_dev_commands.py` | 16/16 passed; exit 0 |
| `test_conan_ops.py` | 22/22 passed; exit 0 |

Final static checks passed: JSON parsing; unchanged developer/global tool/backend
policy and preserved initial observations; YAML parsing with the four matrix rows,
triggers, raw observation/cache/isolation steps unchanged; independent false
execution stop; Python AST parsing; Jinja-only Linux profile renders for Clang
22/GCC-16 and Clang 18/GCC-13 (no Conan import/CLI); all eight plan files and
discussion/locks/recipes preserved; diff whitespace and exact scope review.
PowerShell additions were inspected statically; no local PowerShell parser or
runner execution was available, so runtime validation is not claimed.

These 99 checks do not establish real CI prerequisite, ABI or package acceptance.
No real Conan, compiler, linker, CMake, Ninja or CTest operation ran in this alignment
step. No installation, CI rerun, commit, push or phase advancement occurred.

## Task 3 blocking owner decision

The reviewed policy alignment is ready for owner review. Plan 01-08 remains open
and progress stays 7/8; no SUMMARY or phase-completion claim. Remaining steps:

1. Review this bounded policy/toolchain packet. A dedicated sensitive-file commit
   and publication require separate concrete owner requests.
2. Separately authorize the CI execution-gate change and the exact prerequisite
   bootstrap/doctor scope; publication or observations alone do not grant this.
   Project-owned Conan 2.33.0 and strict real prerequisite checks must pass before
   dependency work. Windows locks require separately approved initialization and
   review; missing locks remain a hard stop, never silent generation or bypass.
3. Under separately approved package/native CI execution, collect four passing actual job URLs,
   native/runtime/configuration/compile-database evidence, warm-cache benefit,
   deliberate cold-source wall time and actual peak resource/capacity findings.
   Capacity failure requires an adequate runner decision; do not reduce Tier 1
   or introduce a custom SDK/package service.

Current sensitive output: workflow, baseline, Linux profile, doctor and mocked/static
tests. Other outputs: CI_MATRIX_POLICY.md, DEVELOPER_BOOTSTRAP.md,
FOUNDATION_ACCEPTANCE.md, README.md and normal STATE/ROADMAP/this execution record.
No SUMMARY, REQUIREMENTS changes
or phase completion; completed native evidence and discussion/plans preserved.

Platform impact:
- Windows: PowerShell orchestrates shared Python operations; reviewed installed
  VS/VC/CRT/SDK/linker and standalone clang-cl inputs now define the CI candidate;
  independent Debug/Release prerequisite/native/CI acceptance/locks remain pending.
  The independent Windows developer candidate is preserved without a native claim.
- Linux: Omarchy accepted developer evidence remains intact; Ubuntu candidate
  policy independently selects Clang 18/GCC-13 with Ubuntu parser support.
  No developer-policy fallback; CI execution and native/package readiness remain gated.
- macOS future: No Darwin job/profile/evidence; standard structured Conan/CMake/
  Python boundaries retained without extending current execution scope.
