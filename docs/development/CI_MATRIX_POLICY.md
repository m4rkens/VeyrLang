# CI Matrix Policy

**Status:** Plan 01-08 remains open, progress 7/8. Owner-reviewed CI toolchain
candidates are based on run 37643695998. Native/package acceptance and CI
prerequisite bootstrap remain unverified. `VEYR_CI_EXECUTION_AUTHORIZED: 'false'`
blocks all installation/package/native steps, independently of baseline review.

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

## Reviewed toolchain candidates and separate execution gate

The owner approved policy alignment after
[run 37643695998](https://github.com/m4rkens/VeyrLang/actions/runs/37643695998),
commit `63c275d175e727830a6c30ba8204dcc3e102bdca`. All four hosted runners
were acquired; all expanded observations and uploads succeeded; every job then
stopped at the expected pending-policy gate. No bootstrap/package/native work ran.
The earlier run 37356855017 observations are retained in baseline.json's
`prior_observations` and the acceptance/checkpoint history; they are not current
selected toolchain inputs. Startup and observation fixes remain separate commits.

| Contract | ubuntu-ci | windows-ci |
|---|---|---|
| Runner / native target | ubuntu-24.04 x86_64 / x86_64-pc-linux-gnu | windows-2022 x64 / x86_64-pc-windows-msvc |
| Python / CMake / Ninja | 3.12.3 / 3.31.6 / 1.13.2 | 3.12.10 / 3.31.6 / 1.13.2 |
| Host Clang | 18.1.3 at /usr/bin/clang and /usr/bin/clang++ | Standalone 20.1.8 at C:\Program Files\LLVM\bin; C/C++ driver is absolute clang-cl.exe |
| GCC / library inputs | GCC/G++ 13.3.0; explicit /usr/lib/gcc/x86_64-linux-gnu/13; GCC-13 headers; libstdc++11 | VS Enterprise 2022 17.14.37710.0; VC tools 14.44.35207; dynamic v143 CRT |
| Runtime / SDK | libstdc++.so.6.0.33, libstdc++6 14.2.0-4ubuntu2~24.04.1; glibc 2.39 | CRT header 14.44.35220.0; Windows/UCRT SDK 10.0.26100.0 |
| Linker | /usr/bin/ld resolves to x86_64-linux-gnu-ld.bfd; GNU Binutils 2.42 | Native MSVC link.exe 14.44.35229.0 under the selected VC toolset |

Both entries have `status: reviewed`: exact candidate policy, not installed CI
Conan or native/package readiness. Conan remains exactly 2.33.0; backend LLVM
remains 23.1.2. Omarchy developer Clang 22/GCC 16 and the independently pending
Windows developer candidate are unchanged. No developer baseline certifies CI.

Linux's shared explicit profile receives `VEYR_CLANG_MAJOR` from the reviewed
environment: 22 for Omarchy, 18 for Ubuntu. The wrapper supplies the explicit GCC
root and compiler/linker paths; there is no profile detection or arbitrary
compiler selection. GCC-13 configuration records `_GLIBCXX_RELEASE 13`,
`__GLIBCXX__ 20240904`, and raw `_GLIBCXX_USE_CXX11_ABI 1`. These header/dry-run
observations are not an effective compiled ABI proof. Doctor now accepts the
observed Ubuntu GLIBC and Binutils banners while retaining strict exact-version
comparison and the existing GNU banner support.

Windows DevShell supplies VC/CRT/SDK/linker inputs only. After entering the
selected shell, the future prerequisite step queries standalone clang, clang++
and clang-cl by absolute paths, verifies 20.1.8/native target, and puts the
standalone directory first in the job-local PATH. C/C++ Conan/CMake inputs remain
absolute clang-cl.exe paths. Bundled 19.1.5 is retained as an observation, never
selected through DevShell precedence. No persistent machine PATH modification.

## Git is an observed utility

Git must be available for repository identity queries. Record its actual version
and path; do not require an exact Git version or include Git observations in the
build selection's ABI/toolchain fingerprint. No pre-existing minimum Git version
requirement was found. Compiler/runtime mismatches still fail; Git version drift
alone does not reject CI. Image version remains observed cache-isolation metadata.

Run 37643695998 observed Linux Git 2.55.0. Windows Debug used image
20261004.326.1/Git 2.56.0.windows.1; Windows Release used image
20260927.320.1/Git 2.55.0.windows.5. Windows compiler/VC/CRT/SDK/linker inputs and
runtime file hashes matched. Image/Git/PATH and VS installation metadata differed;
this is not a native ABI acceptance result.

## Required next owner gate

The workflow's independent execution flag remains **false**. The first policy
step throws before setup-python, CMake/Ninja tool installation, project-owned
Conan bootstrap, cache or dependency work even though the candidates are reviewed.
Review/commit/publication and any later execution flag change each require exact
owner authorization. This policy update does not authorize a new CI run.

Under later separately authorized execution, strict read-only doctor must match
actual Python/Conan/CMake/Ninja/compiler/native identities; Git reports availability
and observations. Conan is inspected via isolated venv metadata, never launched
by doctor. Actual tools-only setup is the sole Conan CLI validation/bootstrap path.

Required tracked native locks must exist before package/cache work. Linux locks
exist as reference/recipe pins, not CI package/ABI proof. Windows Debug/Release
locks remain pending; lock initialization/review is a separately authorized
Conan operation, never silently generated by CI. Four native acceptance results
and measured cold/warm runner capacity remain pending. macOS remains deferred.

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
- Windows: Reviewed CI VS/VC/CRT/SDK/linker and standalone clang-cl candidate identities; native
  Debug/Release evidence, developer inspection and locks remain pending; PowerShell calls
  shared Python operations with the selected absolute Conan executable.
- Linux: Ubuntu CI remains distinct from validated Omarchy developer evidence;
  reviewed Clang 18/GCC-13 candidate policy stays separate; execution/acceptance precede any readiness claim.
- macOS future: No Darwin matrix or acceptance; Conan/CMake/Python boundaries
  remain without extending active platform scope.
