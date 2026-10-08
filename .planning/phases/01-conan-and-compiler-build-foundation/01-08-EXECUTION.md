# Plan 01-08 execution checkpoint

**Date:** 2026-10-08
**Status:** Complete. Tasks 1–3 and the final independent four-job CI/native
evidence gate passed in run 37826767824 at commit
`2741d6514f5e695d36c20cb792bff4985ac5d754`. Phase 1 is 8/8 complete;
this owner-authorized planning-only closeout awaits review and a separately
approved commit. Phase 2 has not started. Windows developer-host evidence remains
independently pending; macOS deferred.

The final acceptance section below is the current checkpoint. Earlier sections
retain their historical pending/failure/authorization boundaries and are
superseded where the final run supplies evidence; they are not current blockers.
All earlier artifacts, observations and failures remain preserved.

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

Both CI entries have `status: reviewed` for toolchain policy only. At that
alignment checkpoint, the workflow retained an independent
`VEYR_CI_EXECUTION_AUTHORIZED: 'false'` gate before Python,
CMake/Ninja installation, Conan bootstrap, locks/cache and native operations.
Reviewing candidates cannot implicitly authorize any of those steps.

Committed alignment scope (13 files, checkpoint `098430781abae7f07c401786ef89a23bd44be919`):

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

## Historical Task 3 execution-authorization checkpoint

The owner approved the one-line execution authorization change and this bounded
consistency update. `VEYR_CI_EXECUTION_AUTHORIZED` is now true; no other workflow
semantics changed. The mocked foundation test expects authorized execution and
retains the explicit authorization/policy gate, Windows compiler selection and
lock-before-cache/package ordering checks. Current docs/checkpoints distinguish
authorized execution from pending acceptance. Toolchain baselines, profiles,
doctor, dependencies and locks are unchanged; baseline evidence notes preserve
the prior alignment checkpoint rather than defining the current workflow flag.

The current nine-file packet comprises workflow, test_foundation_contract.py,
README.md, DEVELOPER_BOOTSTRAP.md, CI_MATRIX_POLICY.md, FOUNDATION_ACCEPTANCE.md,
STATE.md, ROADMAP.md and this execution record. No Windows lock created. Linux
may proceed after real policy/prerequisite checks; Windows can validate/bootstrap
prerequisites but stops before cache/package/native work until reviewed
Debug/Release locks exist. Cold-build capacity and all four native/package CI
outcomes remain pending; macOS deferred. No CI run, installation, package/native
operation, commit or push has occurred in this authorization/consistency step.

Consistency checks completed from `/home/m4rkens/Projects/VeyrLang`:

- `/usr/bin/python3 -B -m unittest discover -s tests/development -p test_foundation_contract.py`:
  18/18 mocked/static tests passed, exit 0; suite time 0.076s. No real package/native command.
- Read-only YAML/Python AST and input comparisons passed: true is the only
  workflow change; policy/prerequisite/lock-before-cache/package order retained;
  native steps require prior success; no `--init-lock` or Windows lock creation;
  toolchain baseline/profiles/doctor/dependency inputs unchanged; 7/8 plans,
  zero completed phases. Exact diff/scope and whitespace checks passed.

Plan 01-08 remains open and progress stays 7/8; no SUMMARY or phase-completion
claim. Remaining steps:

1. Review this bounded authorization/consistency packet. A dedicated sensitive-file commit
   and publication require separate concrete owner requests.
2. After separately approved publication, the authorized workflow may execute
   the existing reviewed path. Project-owned Conan 2.33.0 and strict real policy/
   prerequisite checks must pass before dependency work. Windows locks require separately approved initialization and
   review; missing locks remain a hard stop, never silent generation or bypass.
3. Under the authorized path after publication and required gates, collect four passing actual job URLs,
   native/runtime/configuration/compile-database evidence, warm-cache benefit,
   deliberate cold-source wall time and actual peak resource/capacity findings.
   Capacity failure requires an adequate runner decision; do not reduce Tier 1
   or introduce a custom SDK/package service.

Current sensitive output: one-line workflow flag change and mocked/static foundation
test consistency. Other outputs: CI_MATRIX_POLICY.md, DEVELOPER_BOOTSTRAP.md,
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


## Final Task 3 acceptance and normal closeout — 2026-10-08

The owner explicitly requested inspection of completed run 37826767824 as final
Phase 1 acceptance and authorized planning/checkpoint closeout only if all four
jobs and artifacts support it. No new CI run, retry, dispatch, local package/native
command, implementation edit, staging, commit or push occurred during this review.
No GSD stage or Phase 2 was started automatically.

Run: [37826767824](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824),
conclusion **success**, event push, attempt 1, tested commit
`2741d6514f5e695d36c20cb792bff4985ac5d754`.

| Actual job | Doctor | Native CTest | Verify | Prepared/native acceptance | Acquisition-disabled rejection | Final |
|---|---|---|---|---|---|---|
| [ubuntu-ci-Debug / 113481511231](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511231) | 22/22 | 35/35 | 4 selected C++23 entries; CTest 35/35 | isolated managed configure/build/verify; 35/35 | expected child exit 1; harness success | Success |
| [ubuntu-ci-Release / 113481511547](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511547) | 22/22 | 35/35 | 4 selected C++23 entries; CTest 35/35 | isolated managed configure/build/verify; 35/35 | expected child exit 1; harness success | Success |
| [windows-ci-Debug / 113481511602](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511602) | 20/20 | 35/35 | 4 selected C++23 entries; CTest 35/35 | isolated managed configure/build/verify; 35/35 | raw CRLF preserved; expected child exit 1; harness success | Success |
| [windows-ci-Release / 113481511855](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511855) | 20/20 | 35/35 | 4 selected C++23 entries; CTest 35/35 | isolated managed configure/build/verify; 35/35 | raw CRLF preserved; expected child exit 1; harness success | Success |

In every row, actual step metadata and logs agree that observation, reviewed
policy, exact base-Python selection, auxiliary CMake/Ninja, project-owned Conan
2.33.0 bootstrap, strict read-only doctor and required tracked lock checks passed.
Archive restore/import, explicit dependency setup, integrity/export and early
snapshot save passed before normal configure/build/test/verify. Both native
acceptance steps and always-upload completed successfully; no required gate was
skipped or inferred from another platform/configuration.

### Final package and snapshot evidence

All four dependency setups reused installed LLVM 23.1.2 and GoogleTest packages;
no LLVM/GoogleTest dependency rebuild or download occurred. The recipe test_package
was configured/built/run against the cached selected LLVM package; its native
Support allocation/destruction probe passed. Recipe references remain exactly:

- `llvm/23.1.2@veyr/m1#aa4cad25b3148c3bdd22ed96fba04654`
- `gtest/1.18.0#e2507fc4efe2ec9e2db9e5c91eefab9f`
- build-context `cmake/4.4.3#8b46ce9dbf1937af1c05f19579c1f105`

All four lock JSON files retain Conan version 0.5, those dependency categories/
identities and no new dependencies. Both Linux files SHA-256:
`b76a5f857ba40da4a59d2bdcd8690fba6d12d15d53bd3b0dc6aceb252585f30b`.
Both Windows files SHA-256:
`53d9fc6a882485c51c01a93ce6f9cdd11f277a94377c8d952d2af939b780e9be`.
These match HEAD and final-run prepared/offline `results.json` input identities.
No lock was generated, edited or refreshed by acceptance inspection.

The portable archive alone (`.veyr-dev/conan-cache/dependencies.tgz`) crossed
Actions cache boundaries; each live CONAN_HOME was isolated by run/attempt/job.
Conan import precedes authoritative explicit setup. Integrity and fresh export
both passed, followed by pinned cache/save before project validation.

| Job | Restore/import | Restored snapshot writer | New early-save writer | Successful setup elapsed |
|---|---|---|---|---|
| Ubuntu Debug | Hit / passed | 37820558859-1-Release | 37826767824-1-Debug | 12.5952447s |
| Ubuntu Release | Hit / passed | 37826767824-1-Debug | 37826767824-1-Release | 9.2827918s |
| Windows Debug | Hit / passed | 37820558859-1-Release | 37826767824-1-Debug | 27.840804s |
| Windows Release | Hit / passed | 37826767824-1-Debug | 37826767824-1-Release | 33.4474755s |

Every writer suffix above is appended to its complete platform compatibility
prefix (no broader fallback). Exact prefixes from the completed run:

```text
conan-v2-Linux-X64-ubuntu-ci-linux-x86_64-clang-20261004.327.1-d0f26a40065fdfad5f46b075a752a45dfbfe58d7a97af4f49f85886f2ef27a76-456ff82915ec38682f20a108eb0f999fd91fe7497a6bff7fad9764b9272f53ac-
conan-v2-Windows-X64-windows-ci-windows-x86_64-clangcl-20261004.326.1-3ee01c658391531e0b2d89e6b7a897c1cba8bdf2b299759a3e94e2546282bb28-fee6e3dcbb82f0e6e55aa8026b6bec13d4ede861859c5d026c9ca98f52c29780-
```

Debug/Release share compatible snapshots while distinct GoogleTest configurations
and Windows LLVM CRT package IDs coexist. Snapshot reuse is actual evidence here,
not a replacement for the independently successful project/native checks.

### Windows Release probe proof

Both normal and isolated prepared `CMakeConfigureLog.yaml` events show
`VEYR_CONSUMER_RUNTIME_COMPATIBLE` returning 0. The tested function sets
Windows-only, function-local `CMAKE_TRY_COMPILE_CONFIGURATION` to the selected
Release `CMAKE_BUILD_TYPE` before the check. This normal variable need not appear
as a CMakeCache entry. The actual scratch commands independently prove the chosen
configuration:

```text
clang-cl.exe ... /O2 /Ob2 /DNDEBUG -clang:-std=c++23 -MD ... -c ...src.cxx
cmake.exe -E vs_link_exe ... link.exe ... /INCREMENTAL:NO ...
```

The probe records `CMAKE_CXX_FLAGS_RELEASE=/O2 /Ob2 /DNDEBUG` and
`CMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL`. Neither Release ABI event contains
`-MDd`. Both compile and link complete. Windows Debug events retain
`MultiThreadedDebugDLL` and `-MDd`, returning 0 in normal and prepared trees.
All existing _DLL/_MT/_DEBUG/iterator-level checks remain strict. The actual
Release build, 35/35 tests, compilation-database verify and both acceptance gates
subsequently passed; there is no remaining Release evidence gap.

### Final reviewed corrections and regression outcome

| Correction | Reviewed implementation checkpoint | Final evidence |
|---|---|---|
| Canonical LF recipe bytes and reviewed Linux seed identities for Windows locks | a662e625f41b5e8e4aad9e5433d42374bba04f1b; locks imported in b666a6159f8480e5bb31dd783c2d6f3fb6fba48d | Original LLVM/GoogleTest/CMake recipe revisions respected |
| Reuse validated DevShell; no Conan VCVars reactivation | 9e41c960dada40f814307a25e1a0c988c9faa875 | No vcvars_ver=14.3 defect; both Windows setups passed |
| Forward-slash CMAKE_LINKER serialization, same native linker identity | df71007ea6464b5e40dad388edfcf0d3e29b7a1d | No raw-backslash parsing failure; configure/link passed |
| Exact supported dynamic CRT generator expressions | 0030bd3078dde74d56527fa93ae5d986f0906ff1 | Normal Debug/Release consumer guard passed |
| CMake 3.31 policy baseline in CRT fixture runner | 15434d4cf158cd35a1ed6f336b6937552981b8a8 | All 22 CRT fixtures passed in all four jobs; no CMP0012 regression |
| Native Windows short/long compiler identity and exact clang-cl C++23 forwarding | cd211234ad292955353590445afa01d758bbeeb4 | Four-entry compile database verification passed in both Windows jobs |
| Matching-only CRLF normalization and configure evidence capture | a971d79e1ad47d623462e17629266d1299647eac | Windows raw offline logs retain 59 CRLF pairs each; controlled rejection passed |
| Function-local Windows try-compile configuration | 2741d6514f5e695d36c20cb792bff4985ac5d754 | Release -MD and Debug -MDd probes both compile/link successfully |

Shared earlier fixes also remain effective: setup-python's reviewed base
interpreter remains authoritative despite the auxiliary venv; read-only Ninja/
Windows target/linker probes pass; exact quoted/bracket-quoted include-policy
registration verification passes; early portable snapshot preservation succeeds.
No baseline/version/policy weakening or custom dependency/package system was added.

### Controlled offline semantics and warnings

For every job, missing-gtest / No remote defined messages appear only in the
intentionally empty, separate Conan home used by the acquisition-disabled
negative check. That single child exits 1, while the harness/workflow accepts the
exact expected rejection. No retry, download, package build or acquisition fallback
occurs in that negative gate. Normal setup/configure/build/test/verify/prepared
logs do not report those cache failures. Linux LF and Windows CRLF evidence remain
raw and untouched. This covers missing recipe/package cache, not a binary-only
cache-miss proof or an all-provider Windows acceptance claim.

Action-runtime Node deprecations and existing Linux generated-toolchain quotation
warnings remain visible and non-blocking. No new blocker or reappearance of any
reviewed defect is evidenced; all 35 tests include both positive and rejecting
CRT fixtures and the include-policy registration.

### Source-build capacity and honest evidence limits

Final run 37826767824 is warm-cache acceptance. Earlier actual source builds
establish feasibility of missing-package construction on the hosted runners:

- Run [37670754053](https://github.com/m4rkens/VeyrLang/actions/runs/37670754053):
  Ubuntu Debug dependency step 18:57:57–19:49:39 UTC (~51m42s), Release
  19:51:05–20:31:34 UTC (~40m29s), both successful. Logs show LLVM and GoogleTest
  built from source in both jobs; later project verify failures did not negate
  successful dependency construction. Those original packages were not saved by
  the old cache policy; subsequent snapshot-enabled runs populated reusable caches.
- Run [37786566284](https://github.com/m4rkens/VeyrLang/actions/runs/37786566284):
  Windows Debug-runtime LLVM build/package 13:59:51–14:29:03 UTC (~29m12s),
  Release-runtime variant 14:39:06–15:03:08 (~24m02s); GoogleTest variants built
  immediately afterward. Debug started from an Actions miss; Release imported
  Debug state but built the genuinely missing distinct runtime variant.
  Dependency setup/snapshot preservation succeeded before later consumer guards.

These successful builds fit the hosted six-hour limit and establish observed
capacity. Peak RAM/disk/CPU utilization was not measured. This review does not
claim a manual cold_build=true dispatch, a resource ceiling, arbitrary future
image capacity, broader ABI guarantees or independently inspected Windows
developer-host acceptance. None is silently inferred from this warm final run.

### Evidence set and lifecycle boundary

Uploaded artifacts are `foundation-ubuntu-ci-Debug-1`,
`foundation-ubuntu-ci-Release-1`, `foundation-windows-ci-Debug-1`, and
`foundation-windows-ci-Release-1`. Per artifact the decisive files are:

- `.veyr-dev/ci/doctor.json`, setup/tools-only/configure/build/test/verify logs,
  `foundation.log`, `offline.log`, cache-import/integrity/export logs and setup timing;
- normal/prepared compilation databases and LastTest.log;
- normal/prepared CMakeConfigureLog.yaml, CMakeCache.txt and Conan toolchain;
- isolated prepared/offline `results.json` and raw `offline-rejection.log`.

The harness's generic `windows_and_ci: independently pending` and
`phase_verification: not performed` labels are conservative fixed metadata;
actual environment/configuration-specific task results and successful job/step
outcomes establish this owner-authorized acceptance. Artifacts were inspected,
not rewritten. No automatic separate GSD verification command was executed.

Plan 01-08 is complete, Phase 1 is 8/8 complete, and BUILD-01 through BUILD-14
are accepted for the documented foundation using preserved earlier execution plus
this final native matrix. All eight plans, earlier summaries and discussion are
unchanged. Normal closeout edits only REQUIREMENTS.md, ROADMAP.md, STATE.md,
this execution record and new 01-08-SUMMARY.md. Canonical implementation docs
retain their earlier historical checkpoints; this final planning record supplies
the current evidence without modifying them in this bounded closeout.

Platform impact:
- Windows: Hosted CI Debug/Release native/package/verify/offline foundation accepted;
  independent developer-host VS/VC/CRT/SDK/linker inspection remains pending.
- Linux: Ubuntu CI Debug/Release accepted independently of preserved Omarchy
  developer evidence; Linux behavior/identities/profiles remain unchanged.
- macOS future: Deferred; no execution, profile promotion or acceptance claimed.

Closeout commit/push require separate explicit owner authorization. Phase 2 and
any further lifecycle command remain unstarted. Proposed dedicated message:
`docs: close Phase 1 build-foundation acceptance`.
