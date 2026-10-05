# Plan 01-08 execution checkpoint

**Date:** 2026-10-05
**Status:** Local Tasks 1–2 drafted and required mocked/static task checks passed.
The owner subsequently authorized checkpoint commits and publication. Run
37356855017 supplied four successful observations followed by pending-policy
failures; both CI candidates remain pending/unselected. Task 3 remains a blocking
identity/profile/doctor/locks/native-evidence checkpoint, not a completed plan.
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
below now supplies real job URLs and partial candidate identities. Windows locks
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
Windows CI entries remain pending/unselected; the owner authorized recording
observations only. Omarchy developer policy and the independently pending
Windows developer candidate are unchanged. Conan 2.33.0 and LLVM 23.1.2 remain
required; progress is still 7/8 with all eight waves preserved and macOS deferred.

Remaining Ubuntu gaps: Clang 18 conflicts with the existing Clang-22
developer/profile/doctor policy; resolved libstdc++ runtime/path, standard-library
header identity and exact linker path are not established. The locator reported
only `libstdc++`. Current doctor libc/linker parsers do not accept observed Ubuntu
banner formats. No profile or parser was changed to bypass these gaps.

Remaining Windows gaps: VC tools, CRT, Windows SDK, exact linker identity/path
and selected developer-shell/toolchain environment are not established. VS
installation alone does not prove these identities. Both platforms still need
project-owned Conan validation, native ABI/package acceptance and cold-build
capacity evidence; Windows Debug/Release locks remain pending.

Current edit scope is exactly baseline.json, CI_MATRIX_POLICY.md,
FOUNDATION_ACCEPTANCE.md, README.md, STATE.md, ROADMAP.md and this execution
record. Workflow/profiles/doctor and all other files are unchanged. No additional
CI run, setup/package/native operation, commit or phase advancement is authorized
or performed by this candidate-recording step.

## Task 3 blocking owner decision

Publication and initial observations are complete under explicit owner requests;
the seven-file recording packet is now available for owner review. It cannot
claim Plan 01-08 or Phase 1 completion until independent CI identity, four native
outcomes and actual cold-build feasibility are reviewed. Remaining steps:

1. Review the pending candidate/evidence record. A dedicated commit requires a
   separate exact owner request; recording does not authorize commit/publication.
2. Separately approve a bounded observation-step extension to collect missing
   Ubuntu library/header/linker paths and Windows VC/CRT/SDK/linker/environment
   identities before the existing pending-policy gate. No setup/package enablement.
   Any observation publication/run requires its own concrete authorization.
3. Review those exact identities and decide the Ubuntu Clang/profile ownership
   correction before selecting either CI baseline. A narrow doctor libc/linker
   banner parsing correction needs its own scope/approval; neither developer
   baseline nor profile detection becomes CI authority. Windows locks require
   separately approved initialization/review and are never generated silently.
4. Under separately approved CI execution, collect four passing actual job URLs,
   native/runtime/configuration/compile-database evidence, warm-cache benefit,
   deliberate cold-source wall time and actual peak resource/capacity findings.
   Capacity failure requires an adequate runner decision; do not reduce Tier 1
   or introduce a custom SDK/package service.

Current sensitive output: baseline.json (pending CI observation metadata only).
Other current outputs: CI_MATRIX_POLICY.md, FOUNDATION_ACCEPTANCE.md, README.md
and normal STATE/ROADMAP/this execution record. No SUMMARY, REQUIREMENTS changes
or phase completion; completed native evidence and discussion/plans preserved.

Platform impact:
- Windows: PowerShell orchestrates shared Python operations; reviewed installed
  VS/VC/CRT/SDK/linker/clang-cl inputs and independent Debug/Release native/CI
  acceptance/locks remain pending. CI observations recorded without rewriting
  the independent Windows developer candidate or making a native claim.
- Linux: Omarchy accepted developer evidence remains intact; Ubuntu candidate
  observations are recorded pending/unselected, with compiler/profile and
  libstdc++/header/linker/parser gaps. No developer-policy fallback or package/cache
  reuse across unreviewed identities.
- macOS future: No Darwin job/profile/evidence; standard structured Conan/CMake/
  Python boundaries retained without extending current execution scope.
