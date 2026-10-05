# CI Matrix Policy

**Status:** Plan 01-08 published workflow; run 37356855017 supplied four
successful observation steps, followed by pending-policy failures. Both CI
candidates remain pending/unselected. Complete identity review, all four native
results and cold-runner capacity remain blocking evidence gaps; progress 7/8.

## Required matrix and entry points

`.github/workflows/build-foundation.yml` has `pull_request`, pushes to `main`
and `workflow_dispatch` triggers. Its matrix is exactly:

| Runner family | Environment / profile | Consumer |
|---|---|---|
| ubuntu-24.04 x86_64 | ubuntu-ci / linux-x86_64-clang | Debug |
| ubuntu-24.04 x86_64 | ubuntu-ci / linux-x86_64-clang | Release |
| windows-2022 x86_64 | windows-ci / windows-x86_64-clangcl | Debug |
| windows-2022 x86_64 | windows-ci / windows-x86_64-clangcl | Release |

`fail-fast: false` retains independent row failures; `max-parallel: 1` preserves
the approved serial policy. Windows and Linux are Tier 1; macOS remains deferred.

## Independent bootstrap and native stages

Publication/run authorization is separate from local implementation. The
owner-approved pushes published the draft and its startup correction. Run
37355569361 failed workflow validation with zero jobs; the committed correction
in `c4f27d086370c8d63d0b81990eba04044bdbe98f` moved CONAN_HOME into step-level
env and persisted it through GITHUB_ENV. Its automatic push run
[37356855017](https://github.com/m4rkens/VeyrLang/actions/runs/37356855017)
reported installed runner tools, paths, image version and VS candidates.
All four observation steps and artifact uploads succeeded; all four jobs then
failed at the expected pending-policy gate. Both tracked `ci_environments` remain `pending`:
the existing read-only doctor fails before CLI installation, cache restore or
dependency setup. A pending-policy observation run is not a passing native row.

## Observed CI candidates, not selected policy

Debug and Release reported the same observed identities within each platform.
`baseline.json` stores them under `observed_candidate`, separate from executable
policy inputs; this evidence does not select either CI entry or enable setup.

| Identity | Ubuntu CI candidate | Windows CI candidate |
|---|---|---|
| Runner / ImageOS / image version | ubuntu-24.04 / ubuntu24 / 20260927.320.1 | windows-2022 / win22 / 20260927.320.1 |
| Runner agent | 2.337.0 | 2.337.0 |
| OS / architecture | Ubuntu 24.04.5 LTS / x86_64 | Windows Server 2022 Datacenter 10.0.20348 / x86_64 |
| Python | 3.12.3 | 3.12.10 |
| CMake / Ninja | 3.31.6 / 1.13.2 | 3.31.6 / 1.13.2 |
| Clang / clang++ / clang-cl | 18.1.3 / 18.1.3 / missing | 20.1.8 / 20.1.8 / 20.1.8 |
| Compiler target | x86_64-pc-linux-gnu | x86_64-pc-windows-msvc |
| GCC / G++ | 13.3.0 / 13.3.0 | Not applicable |
| glibc / Binutils | 2.39 / 2.42 | Not applicable |
| Visual Studio installation | Not applicable | Enterprise 2022 / 17.14.37710.0 |
| Git | 2.55.0 | 2.55.0.windows.5 |
| Project-owned Conan | Validation pending; installation/version unobserved | Validation pending; installation/version unobserved |

Exact observed tool paths and the four job URLs are in
[FOUNDATION_ACCEPTANCE.md](FOUNDATION_ACCEPTANCE.md) and baseline.json. Conan
2.33.0 remains required; LLVM backend remains 23.1.2. No ambient Conan was
accepted or used. Windows developer candidates and the Omarchy baseline are
unchanged and independent of these CI observations.

Remaining Ubuntu gaps: observed Clang 18 conflicts with the existing Clang-22
developer/profile/doctor policy; resolved libstdc++ runtime/path, standard-library
header identity and exact linker path are not established. The libstdc++ locator
returned only `libstdc++`. Current doctor libc/linker parsers do not accept the
observed Ubuntu banner formats. Neither profile settings nor parsers changed.

Remaining Windows gaps: VC tools, CRT, Windows SDK, exact linker identity/path
and selected developer-shell/toolchain environment are not established. The
observed VS installation is not evidence of selected runtime/SDK/toolset values.

Both environments still lack project-owned Conan validation, native ABI/package
acceptance and cold-build capacity evidence. Windows Debug/Release locks remain
pending. Shared platform identity across configurations does not waive separate
native Debug/Release and Windows runtime validation. No additional run or setup
is authorized by recording these candidates.

## Requirements before policy activation

After actual observations and owner review, each CI entry must have its own
complete `environment`, `runner`, `profile`, `os`, `arch`, `target`, `git`, exact
`tools`, `native` and `paths` values compatible with the existing doctor schema.
Runner labels are scheduling choices, not compiler/CRT/SDK identities. Never
copy Omarchy developer policy into Ubuntu CI or promote Windows developer
candidates as CI evidence. Windows developer inspection remains independent.

The existing Linux profile fixes Clang major 22/libstdc++11; Windows fixes Clang
major 20/v143 dynamic CRT with separate Debug/Release runtime selection. Actual
CI tools outside these inputs require owner review and a bounded profile/doctor
ownership correction before use. No `profile detect`, automatic candidate
promotion, extra profile or compiler-major change is authorized by this draft.

Once reviewed, upstream setup-python selects the exact CI base Python. Standard
venv/isolated pip installs only exact tracked CMake/Ninja prerequisites into
`.veyr-dev/tools/ci-prerequisites`; no system Python/global pip changes. Installed
host Clang/GCC or VS/SDK must match reviewed native identities. Windows uses the
selected Microsoft VS developer shell; strict doctor checks actual VC tools,
CRT/SDK/linker/paths. Missing compiler provisioning remains an owner gate; no
backend SDK acquisition or automatic drift acceptance. Explicit tools-only setup
creates project-owned Conan 2.33.0; doctor reads isolated package metadata without
executing Conan or mutating its home.

A required native lock must already be tracked. Both Linux locks exist;
Windows Debug/Release locks remain pending. Missing locks stop before cache
restore or package work. CI never runs `--init-lock`; lock initialization and
review require a separately approved bounded execution step.

## Shared commands and Conan ownership

Each command passes explicit `--environment`, `--profile`, `--config` and the
job-owned `--conan-home` to existing Python tooling:

```text
python scripts/dev.py setup --tools-only ...
python scripts/dev.py doctor --json ...
python scripts/dev.py setup ...
python scripts/dev.py configure ...
python scripts/dev.py build ...
python scripts/dev.py test ...
python scripts/dev.py verify ...
python scripts/check_foundation.py --case prepared ...
python scripts/check_foundation.py --case offline ...
```

Distinct named steps preserve merged stdout/stderr logs and stop on first
nonzero exit without retry. The offline case recognizes only its strict
expected child failure; other failures stop the job. Configure/build/test/verify
never acquire dependencies. Setup visibly permits Conan downloads and genuinely
missing source builds; dependency versions/options remain in Conan inputs.
The workflow never runs `all` followed by duplicate build/test/verify. Separate
test/verify invocations each perform their documented checks; the foundation
prepared case deliberately validates an isolated native probe output.

Conan owns resolution, acquisition, packages/revisions/storage and locks. No
custom package graph, binary publication, SDK normalization, host provisioning
framework or language-level `Veyr.lock` behavior is introduced.

## Cache and deliberate source build

Each job uses a fresh external home beneath `runner.temp`, isolated by run ID,
attempt, environment and consumer configuration. Only Conan's home is cached.
Keys include OS/architecture, environment/configuration, actual image version and
hashes of baseline, profiles, recipes/options, locks, root consumer and CMake
presets. Doctor first matches actual tool/runtime identities to baseline inputs.
No broad restore prefixes or cross-OS archives; save occurs only after job
success. Failed/bootstrap-only runs publish no package cache.

Manual `cold_build: true` skips restore and save. The new job-owned home has no
dependency packages; explicit setup uses standard source-built LLVM 23.1.2 and
`--build=missing`. No forced rebuild, active-home deletion, cache purge or foreign
backend archive. Isolated CMake/Ninja and Conan CLI tools are distinct from LLVM
package state. No CLI venv or build directory is cached/published as a package.
Normal developer clean preserves tools, package home and other selections.

## Supervision, capacity and evidence

The job has an explicit 360-minute hard limit within the
[GitHub-hosted six-hour limit](https://docs.github.com/en/actions/reference/limits).
About 60 seconds quiet remains a soft liveness checkpoint in the existing
wrapper: inspect children/CPU/I/O/artifacts and continue healthy work. No
quiet-time termination, duplicate execution or repair.

The draft records setup exit/wall time and uploads runner observations, doctor,
command logs, compile databases, CTest logs and foundation results. These are
evidence, not transferable build/package truth. Existing foundation results
retain their conservative `windows_and_ci: independently pending` field;
specific task results and actual job URLs require ledger review before CI
acceptance. A JSON file does not mark a phase complete.

Process-tree peak memory/disk, useful compiler/link parallelism, feasible cold
source builds, four passing job URLs and warm-cache benefit remain unmeasured
on actual runners. Setup wall time is not peak-resource measurement. Collect
those observations during authorized CI work and record failures honestly. If
resources/time are inadequate, report the bottleneck and request adequate
larger/self-hosted capacity; retain all four combinations and Conan ownership.

## Reviewed immutable action inputs

Action definitions were inspected read-only during local execution:

| Action | Exact revision / upstream definition |
|---|---|
| checkout v4.2.2 | [11bd71901bbe5b1630ceea73d27597364c9af683](https://github.com/actions/checkout/blob/11bd71901bbe5b1630ceea73d27597364c9af683/action.yml) |
| setup-python v5.6.0 | [a26af69be951a213d495a4c3e4e4022e16d87065](https://github.com/actions/setup-python/blob/a26af69be951a213d495a4c3e4e4022e16d87065/action.yml) |
| cache v4.2.3 | [5a3ec84eff668545956fd18022155c47e93e2684](https://github.com/actions/cache/blob/5a3ec84eff668545956fd18022155c47e93e2684/action.yml) |
| upload-artifact v4.6.2 | [ea165f8d65b6e75b540449e92b4886f43607fa02](https://github.com/actions/upload-artifact/blob/ea165f8d65b6e75b540449e92b4886f43607fa02/action.yml) |

Pins establish selected upstream code/inputs, not successful runner execution.
Remote publication/dispatch, sensitive commit, dedicated phase verification and
Phase 2 require their own exact owner actions.

Platform impact:
- Windows: Independent VS/VC/CRT/SDK/linker and clang-cl identities, native
  Debug/Release runtime evidence and locks remain pending; PowerShell calls
  shared Python operations with the selected absolute Conan executable.
- Linux: Ubuntu CI remains distinct from validated Omarchy developer evidence;
  exact compiler/GCC/libstdc++/libc/linker review precedes packages.
- macOS future: No Darwin matrix or acceptance; Conan/CMake/Python boundaries
  remain without extending active platform scope.
