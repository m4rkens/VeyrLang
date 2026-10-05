# Plan 01-08 execution checkpoint

**Date:** 2026-10-05
**Status:** Local Tasks 1–2 drafted and required mocked/static task checks passed.
Task 3 remains a blocking owner publication/identity/native-evidence checkpoint.
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

The CI ledger has four independently pending rows, no invented URLs/tool pins
or capacity measurements. Actual Windows locks and native/CI evidence are still
missing. Existing foundation JSON conservatively says CI pending; individual
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

No real Conan/CMake/Ninja/CTest/compiler/linker command, dependency operation,
installation or build ran locally. No workflow push/dispatch, stage/commit,
phase verification or advancement. PowerShell execution/action execution/native
Windows and Ubuntu bootstrap remain untested. YAML parsing/static inspection
is not an Actions or PowerShell execution guarantee.

## Task 3 blocking owner decision

Local draft is available for sensitive-file review. It cannot claim Plan 01-08
or Phase 1 completion until independent CI identity, four native outcomes and
actual cold-build feasibility are reviewed. Required remaining steps:

1. Review this local workflow/policy/docs packet. A dedicated execution checkpoint
   commit requires the owner's exact request; proposed message:
   `ci: draft gated Windows and Linux foundation workflow`.
2. Separately authorize publication of the reviewed committed workflow. Current
   branch main has origin configured; a candidate publication action after
   that commit is `git push origin main`. Push-main would trigger the four
   pending-identity observation jobs. This action was not executed or authorized
   by local execution. Any manual dispatch also needs separate authorization.
3. Review actual raw CI observations and complete each independent baseline
   only with observed exact tool/native/path identities. Missing Windows CRT/SDK
   details may require another bounded observation step. No baseline selected now.
4. If the observed tools require different profile/doctor inputs, stop for the
   plan's bounded ownership correction. Missing Windows native locks also require
   approved initialization/review; this draft does not create them automatically.
5. Under separately approved CI execution, collect four passing actual job URLs,
   native/runtime/configuration/compile-database evidence, warm-cache benefit,
   deliberate cold-source wall time and actual peak resource/capacity findings.
   Capacity failure requires an adequate runner decision; do not reduce Tier 1
   or introduce a custom SDK/package service.

Sensitive implementation outputs: workflow and baseline.json (CI metadata only).
Other outputs: four contributor/CI/acceptance docs and normal STATE/ROADMAP/this
execution record. No SUMMARY, no REQUIREMENTS edits and no completed-artifact
rewrite beyond necessary current status reconciliation.

Platform impact:
- Windows: PowerShell orchestrates shared Python operations; reviewed installed
  VS/VC/CRT/SDK/linker/clang-cl inputs and independent Debug/Release native/CI
  evidence/locks remain pending. No candidate rewritten or native claim made.
- Linux: Omarchy accepted developer evidence remains intact; Ubuntu CI needs its
  own actual compiler/GCC/libstdc++/libc/linker/tool policy. No developer-policy
  fallback or package/cache reuse across unreviewed identities.
- macOS future: No Darwin job/profile/evidence; standard structured Conan/CMake/
  Python boundaries retained without extending current execution scope.
