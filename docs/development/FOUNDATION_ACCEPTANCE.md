# Foundation acceptance ledger

**Status:** Plan 01-07 complete for the available Omarchy/Arch Linux developer scope, by explicit owner closeout. Windows developer evidence and independent Windows/Ubuntu CI acceptance remain pending; CI toolchain candidates are owner-reviewed but bootstrap/native/package acceptance is unverified; macOS deferred. Phase 1 remains executing, 7/8 plans complete and zero phases complete.

This ledger records preserved execution-task observations from the current fresh lifecycle. Current alignment uses mocked/static checks only; no native builds, setup or Conan operations run to update it. Dedicated phase verification and four-combination Tier 1 CI acceptance remain separate gates. Exact commands, timestamps, failures, approved corrections and logs are in [01-07-EXECUTION.md](../../.planning/phases/01-conan-and-compiler-build-foundation/01-07-EXECUTION.md).

## Native configuration matrix

| Developer host / consumer | Provider configure/build/link/native tests | Runtime / cache / clean | Offline acceptance | Scope |
|---|---|---|---|---|
| Omarchy/Arch Linux x86_64 / Debug | Managed, explicit system and custom: CTest 13/13 each; four C++23 compile entries each | Runtime 5/5; warm reuse and seven unchanged cache identities; disposable selected clean preservation passed | Passed separately; expected empty-cache/no-remote child exit 1, harness exit 0 | Available Linux developer accepted |
| Omarchy/Arch Linux x86_64 / Release | Managed, explicit system and custom: CTest 13/13 each; four C++23 compile entries each | Runtime 5/5; warm reuse and seven unchanged cache identities; disposable selected clean preservation passed | Passed separately; expected empty-cache/no-remote child exit 1, harness exit 0 | Available Linux developer accepted |
| Windows x86_64 / Debug | Pending actual native host inspection, reviewed runtime identity and execution | Pending native evidence, including Debug CRT/iterator compatibility | Pending | No Linux evidence credited |
| Windows x86_64 / Release | Pending actual native host inspection, reviewed runtime identity and execution | Pending native evidence | Pending | No Linux evidence credited |

Ubuntu and Windows CI acceptance remain independently pending. Ubuntu is not the Linux developer baseline. The owner authorized publication and the automatic observation run 37356855017; all four jobs reported candidates and stopped at pending CI policy before setup/package/build work. No native CI acceptance or cold-runner capacity is claimed. Windows developer candidates remain unchanged and uninspected independently of Windows CI observations.

## Current reviewed CI candidate policy (run 37643695998)

The owner approved policy alignment from
[run 37643695998](https://github.com/m4rkens/VeyrLang/actions/runs/37643695998),
commit `63c275d175e727830a6c30ba8204dcc3e102bdca`. All four jobs acquired hosted
runners, completed both raw and explicitly selected toolchain observations,
uploaded artifacts and stopped at the expected pending-policy gate, exit 1.
No CLI/bootstrap, cache/package operation or native build/test ran.

| Configuration | Observed toolchain | Observation job | Native acceptance |
|---|---|---|---|
| Ubuntu Debug | Clang 18.1.3, explicit GCC-13; image 20260927.320.1 | [112868833273](https://github.com/m4rkens/VeyrLang/actions/runs/37643695998/job/112868833273) | Pending |
| Ubuntu Release | Same exact toolchain; driver trace differs only by random temporary object | [112868833146](https://github.com/m4rkens/VeyrLang/actions/runs/37643695998/job/112868833146) | Pending |
| Windows Debug | Standalone 20.1.8 after DevShell; image 20261004.326.1; observed Git 2.56.0.windows.1 | [112868833261](https://github.com/m4rkens/VeyrLang/actions/runs/37643695998/job/112868833261) | Pending |
| Windows Release | Same compiler/native inputs; image 20260927.320.1; observed Git 2.55.0.windows.5 | [112868832679](https://github.com/m4rkens/VeyrLang/actions/runs/37643695998/job/112868832679) | Pending |

Reviewed Ubuntu candidate: Python 3.12.3, CMake 3.31.6, Ninja 1.13.2,
Clang/clang++ 18.1.3, target x86_64-pc-linux-gnu, GCC/G++ 13.3.0 at explicit
`/usr/lib/gcc/x86_64-linux-gnu/13`, GCC-13 C++ headers, glibc 2.39, and
`/usr/bin/ld` resolving to `/usr/bin/x86_64-linux-gnu-ld.bfd` / Binutils 2.42.
Header `/usr/include/x86_64-linux-gnu/c++/13/bits/c++config.h` records release 13,
`__GLIBCXX__ 20240904`, and raw `_GLIBCXX_USE_CXX11_ABI 1`; SHA256
`335E3585A6D6626CC9C9264C8C19716EB827CBCBC777516B0CE5BA1840A59F1C`.
The selected library `/usr/lib/gcc/x86_64-linux-gnu/13/libstdc++.so` resolves to
`/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.33`, package
`libstdc++6:amd64 14.2.0-4ubuntu2~24.04.1`; SHA256
`1FD75FE70354A416D75AEF22BCAE68C47BD25D20E2D0568C30B1A9838CF62F11`.
These files/search paths match across Debug/Release. Default GCC-14 raw
observations remain preserved. Raw definitions/dry-runs do not prove compiled ABI.

Reviewed Windows candidate: Python 3.12.10, CMake 3.31.6, Ninja 1.13.2;
standalone clang/clang++/clang-cl 20.1.8 in `C:\Program Files\LLVM\bin`, target
x86_64-pc-windows-msvc; VS Enterprise 2022 17.14.37710.0, DevShell 17.14.41;
VC tools 14.44.35207, CRT header 14.44.35220.0, SDK/UCRT SDK 10.0.26100.0,
MSVC link.exe 14.44.35229.0. OS ucrtbase.dll is separately observed as
10.0.20348.5622. Compiler/CRT/SDK/linker and runtime file hashes match across
configurations. Image/Git/PATH and VS installation metadata differ. Git remains
an available utility with observed version/path, not an exact ABI/package pin.
Bundled Clang 19.1.5 observations remain; absolute standalone queries after
DevShell retain its VC/SDK/linker environment without selecting the bundled tools.

Both baseline entries say `reviewed` as candidate policy only. Under separate
owner authorization the workflow now sets `VEYR_CI_EXECUTION_AUTHORIZED: 'true'`.
After separately approved publication, Linux may proceed through setup/package/
build/test only after policy/prerequisite checks pass. Windows remains blocked
before cache/package/native work until reviewed Debug/Release locks exist.
This authorizes execution, not successful native/package acceptance.
Omarchy and Windows developer baselines are unchanged. Baseline evidence notes
retain the prior policy-alignment checkpoint; the workflow flag records the
current execution authorization. No baseline identities were changed.
Project-owned Conan 2.33.0 validation, Windows locks, four native CI outcomes,
ABI/package acceptance and actual cold/warm capacity remain pending. Backend
LLVM stays 23.1.2; no native CI row, plan or phase is closed by this policy update.
All earlier observation artifacts/history are retained.

## Historical initial CI evidence gate (run 37356855017)

The published `.github/workflows/build-foundation.yml` implements the four
matrix rows below. At the initial run, CI policy was pending and stopped before CLI or
package setup, after reporting installed runner observations. Publication and
observation do not pass native CI. Its workflow inputs and blocking gates are
documented in [CI_MATRIX_POLICY.md](CI_MATRIX_POLICY.md).

| CI environment / consumer | Observed exact identity / tracked lock | Native configure/build/test/verify | Actual job URL | Cold/warm capacity |
|---|---|---|---|---|
| Ubuntu 24.04 x86_64 / Debug | Observed candidate below; pending/unselected; existing Linux lock is not CI ABI proof | Not run; pending-policy failure before setup | [Observation job 111921336171](https://github.com/m4rkens/VeyrLang/actions/runs/37356855017/job/111921336171) | Pending |
| Ubuntu 24.04 x86_64 / Release | Same observed candidate as Debug; pending/unselected | Not run; pending-policy failure before setup | [Observation job 111921336214](https://github.com/m4rkens/VeyrLang/actions/runs/37356855017/job/111921336214) | Pending |
| Windows 2022 x86_64 / Debug | Observed candidate below; runtime/toolchain identity and Windows lock incomplete | Not run; pending-policy failure before setup | [Observation job 111921336225](https://github.com/m4rkens/VeyrLang/actions/runs/37356855017/job/111921336225) | Pending |
| Windows 2022 x86_64 / Release | Same observed candidate as Debug; runtime/toolchain identity and Windows lock incomplete | Not run; pending-policy failure before setup | [Observation job 111921336313](https://github.com/m4rkens/VeyrLang/actions/runs/37356855017/job/111921336313) | Pending |

Run [37356855017](https://github.com/m4rkens/VeyrLang/actions/runs/37356855017)
observed commit `c4f27d086370c8d63d0b81990eba04044bdbe98f`, triggered by the
owner-approved push. All four observation steps and artifact uploads succeeded;
all four jobs failed at the pending-policy gate. No tools-only/bootstrap,
dependency setup, cache restore/save or native configure/build/test work ran. Conan installation
and version were not observed; exact project-owned Conan 2.33.0 validation is
still pending. LLVM backend remains 23.1.2.

### Observed candidates shared by Debug/Release within each platform

| Identity | Ubuntu CI candidate | Windows CI candidate |
|---|---|---|
| Runner / ImageOS / image version | ubuntu-24.04 / ubuntu24 / 20260927.320.1 | windows-2022 / win22 / 20260927.320.1 |
| Runner agent | 2.337.0 | 2.337.0 |
| OS / architecture | Ubuntu 24.04.5 LTS / x86_64 | Windows Server 2022 Datacenter 10.0.20348 / x86_64 |
| Python | 3.12.3 at `/usr/bin/python` | 3.12.10 at `C:\hostedtoolcache\windows\Python\3.12.10\x64\python.exe` |
| CMake | 3.31.6 at `/usr/local/bin/cmake` | 3.31.6 at `C:\Program Files\CMake\bin\cmake.exe` |
| Ninja | 1.13.2 at `/usr/local/bin/ninja` | 1.13.2 at `C:\ProgramData\Chocolatey\bin\ninja.exe` |
| Clang / clang++ | 18.1.3 at `/usr/bin/clang` and `/usr/bin/clang++` | 20.1.8 at `C:\Program Files\LLVM\bin\clang.exe` and `clang++.exe` |
| clang-cl | Missing; Linux native profile uses clang/clang++ | 20.1.8 at `C:\Program Files\LLVM\bin\clang-cl.exe` |
| Compiler target | x86_64-pc-linux-gnu | x86_64-pc-windows-msvc |
| GCC / G++ | 13.3.0 / 13.3.0; Ubuntu package 13.3.0-6ubuntu2~24.04.1 | Not applicable |
| glibc / Binutils | 2.39 / 2.42 | Not applicable |
| Visual Studio installation | Not applicable | Enterprise 2022 17.14.37710.0 at `C:\Program Files\Microsoft Visual Studio\2022\Enterprise` |
| Git | 2.55.0 at `/usr/bin/git` | 2.55.0.windows.5 at `C:\Program Files\Git\bin\git.exe` |

During the initial run, Ubuntu reported `/usr/lib/gcc/x86_64-linux-gnu/13/libgcc.a`, but its libstdc++
locator returned only `libstdc++`. Runtime/header/linker identities and the
Clang-18 profile decision were then missing; Ubuntu banner parsing was unsupported.
The current approved alignment above supersedes those historical identity gaps.

At that initial run, Windows VC tools, CRT, SDK, linker and selected shell
identities were unobserved; the expanded current observations above resolve them. Both platforms retain
project-owned Conan validation, native ABI/package acceptance and cold-build
capacity gaps; Windows Debug/Release locks remain pending. At that run both CI entries remained
pending/unselected; current reviewed policy is recorded above. These observations do not modify either developer baseline,
authorize setup, or prove Debug/Release runtime compatibility.

No runner/tool identities, resource peaks, cache hits or CI URLs are fabricated.
The workflow is designed to record setup wall time when enabled, but setup was
skipped in this run. It does not measure peak memory/disk; actual
cold-source feasibility, useful compiler/link parallelism and warm cache benefit
need separately authorized observation. Historical Omarchy cold timings below
are not Ubuntu/Windows runner measurements. Missing native profile/tool/lock
inputs require owner review before package work. Capacity failure requires an
owner runner choice while retaining Tier 1 and Conan ownership.

The missed Plan 01-07 Task 1 Support lifetime extension is now committed as
`eef4780faa9063a50ae5ede89fc40f55408279bb`, after the nine-file execution/closeout
checkpoint `ddec666b43546d8820d7eacd28e6ce4ef2b068ed`. Its preserved native
13/13 results below were not rerun. Plan 01-08 completion and phase-wide
acceptance remain blocked on actual reviewed CI evidence.

## Observed Linux selection

| Identity | Observed selection |
|---|---|
| Developer environment | linux-developer; Omarchy 4.0.4 / Arch x86_64 |
| Host and target identity | linux-x86_64-gnu; native compiler target x86_64-pc-linux-gnu |
| Python / Conan CLI | Python 3.14.7; project-owned Conan 2.33.0 |
| CMake / Ninja / Git | 4.4.3 / 1.13.2 / 2.55.0 |
| C and C++ compiler | Clang 22.1.8 at /usr/bin/clang and /usr/bin/clang++ |
| Standard library/toolchain | GCC/libstdc++ 16.2.1; libstdc++11 ABI=1; /usr/lib/gcc/x86_64-pc-linux-gnu/16 |
| Headers / library | /usr/include/c++/16; /usr/lib/gcc/x86_64-pc-linux-gnu/16/libstdc++.so |
| libc / linker | glibc 2.44; GNU Binutils 2.47 at /usr/bin/ld |
| Host profiles | conan/profiles/linux-x86_64-clang plus debug or release overlay |
| Build profiles | Same native profile plus release overlay, independently from consumer configuration |
| Project Conan executable | /home/m4rkens/Projects/VeyrLang/.veyr-dev/tools/conan-2.33.0/bin/conan |
| Dedicated Conan home | /home/m4rkens/.cache/veyr/conan/22e2b2472775 |

Read-only doctor passed 22/22, including isolated venv package metadata, without launching Conan or initializing its home. Explicit warm setup reused the validated CLI environment, performed CLI launch validation and pip check, and did not install/upgrade it. Commands use the absolute project CLI and selected dedicated home; no activation or persistent PATH change.

LLVM policy observed in both consumer configurations: exact LLVM 23.1.2, static Release, X86, native imported LLVMSupport, LLVM_ABI_BREAKING_CHECKS=FORCE_OFF, generated header LLVM_ENABLE_ABI_BREAKING_CHECKS=0 and exported/consumer libstdc++11 ABI=1. LLVM host/target triple is x86_64-unknown-linux-gnu. This is native GNU compatibility under the reviewed policy, not a claim that the compiler and package triple spellings are identical. Both Support tests passed, including heap-backed string growth, move, clear, reuse and destruction.

All three provider modes selected the real installed config at `/home/m4rkens/.cache/veyr/conan/22e2b2472775/p/b/llvm3c80d92357c17/p/lib/cmake/llvm`. System/custom were explicitly supplied this same compatible installation. Their successful configure/link/run does not establish arbitrary distro or foreign SDK compatibility. WrongVersion, WrongConfiguration, WrongTriple, WrongRuntime and WrongHeaderABI CTest rejection assertions passed; fabricated rejection configs never count as positive native evidence. Tested Support surfaces do not establish a general LLVM/native ABI guarantee.

## Conan-owned identities and compilation records

Conan remains the owner of dependency solving, acquisition, package storage/revisions and locks. The Python layer selects and supervises existing commands. These observations are documentation, not a new package graph, provisioning schema, SDK normalization format or future language-level Veyr.lock.

| Conan reference | Recipe revision | Package ID / package revision |
|---|---|---|
| llvm/23.1.2@veyr/m1 | aa4cad25b3148c3bdd22ed96fba04654 | a214d896e569f9b075f68e0b581ce595d09d80df / cd9a88c23979fe65c10c97b796943a64 |
| gtest/1.18.0, Debug | e2507fc4efe2ec9e2db9e5c91eefab9f | 77ee4a6c9da2ff6b1df8eac330ceb33aad0d1d69 / 3604fb7abde2c4493bc7897a104b3995 |
| gtest/1.18.0, Release | e2507fc4efe2ec9e2db9e5c91eefab9f | 4c47d11dc631ceaef479f42595e1124ab519c480 / f244293f6c361ee557e76d8fed0392f4 |
| cmake/4.4.3, existing cache observation | 8b46ce9dbf1937af1c05f19579c1f105 | 63fead0844576fc02943e16909f08fcdddd6f44b / e75518647d197cbde7df069c10071164 |

Both configurations reused the same LLVM package. GoogleTest configurations have distinct Conan package identities. Raw before/after observations from Conan list contain seven recipe/package identities, all unchanged across each separately explicit warm setup. Existing cached CMake identity is also retained; no dependency build is inferred from its presence.

| Historical Plan 01-07 input | Recorded SHA256 |
|---|---|
| conan/locks/linux-x86_64-clang-Debug.lock and linux-x86_64-clang-Release.lock, each | b76a5f857ba40da4a59d2bdcd8690fba6d12d15d53bd3b0dc6aceb252585f30b |
| conan/profiles/linux-x86_64-clang | f929ae7dd71317c578cb34d3954e01412cb81c87b24b0f1a1829291a8afa2ed9 |
| conan/profiles/debug | c9abfdab1dba1320dc62cb26ba2eb33db8a7b979f7eaead3f839d21bcf4ee510 |
| conan/profiles/release | de43178f58dccd6f215f279d7f06a854919a7ad5f1edd5681c5a67ebb3ed3ea8 |
| scripts/devtools/baseline.json | 43b2f5bad7b1d17ac8d5157eb3c112774c835416905fed1f78443c1f586eaf8b |

LLVM package conanmanifest.txt SHA256 remained `2eba146976c0f4460a0fb60fdf7363eb9c91a00b9a9926b47b893f0dd6cb297e`. Locks, profiles and recipes were preserved; no package state was copied into Veyr.lock.

The independent provider runs retained successful results.json and exact compile database paths, relative to the checkout:

| Consumer / provider | Actual compile database |
|---|---|
| Debug / managed | build/foundation-acceptance/linux-x86_64-clang-Debug-zrhfimen/managed/Debug/compile_commands.json |
| Debug / system | build/foundation-acceptance/linux-x86_64-clang-Debug-zrhfimen/system/Debug/compile_commands.json |
| Debug / custom | build/foundation-acceptance/linux-x86_64-clang-Debug-zrhfimen/custom/Debug/compile_commands.json |
| Release / managed | build/foundation-acceptance/linux-x86_64-clang-Release-xx4twfor/managed/Release/compile_commands.json |
| Release / system | build/foundation-acceptance/linux-x86_64-clang-Release-xx4twfor/system/Release/compile_commands.json |
| Release / custom | build/foundation-acceptance/linux-x86_64-clang-Release-xx4twfor/custom/Release/compile_commands.json |

Each database had four selected Clang/C++23 entries; verify reused equivalent CTest already completed in its invocation. Generated databases, logs, tool/cache state and acceptance results stay untracked.

## Preserved command evidence and bounded recovery

All native invocations used cwd /home/m4rkens/Projects/VeyrLang. Ignored logs below record actual structured argv, child output, UTC start, final exit and elapsed time. Full commands are preserved in the execution checkpoint; nothing was rerun for this closeout.

| Operation | Start UTC, 2026-10-05 | Outer exit / elapsed | Log under .veyr-dev/logs/ |
|---|---|---|---|
| Debug independent providers | 14:31:50.646284 | 0 / 17.504s | 01-07-providers-Debug.log |
| Release independent providers | 14:32:26.297694 | 0 / 17.998s | 01-07-providers-Release.log |
| Explicit Debug warm setup | 14:41:49.793255 | 0 / 3.555s | 01-07-setup-Debug.log |
| Debug all-checks, historical matcher stop | 14:42:22.113757 | 1 / 61.640s | 01-07-all-checks-Debug.log |
| Approved Debug offline-only recovery | 14:54:07.410925 | 0 / 0.589s | 01-07-offline-recovery-Debug.log |
| Explicit Release warm setup | 14:55:55.494725 | 0 / 3.912s | 01-07-setup-Release.log |
| Release all-checks, historical runner stop | 14:56:24.548342 | 1 / 63.672s | 01-07-all-checks-Release.log |
| Approved Release offline-only recovery | 15:22:44.554234 | 0 / 0.455s | 01-07-offline-recovery-Release.log |

Each historical all-checks run completed provider positives (13/13 each), runtime (5/5), cache comparison and disposable clean before its offline handling stop. Successful earlier gates remain accepted. Their final combined results.json was never written; this ledger does not change their exit status or manufacture a successful combined rerun.

Raw cache evidence: `.veyr-dev/observations/01-07-cache-before-Debug.json` and `01-07-cache-before-Release.json`, compared with `build/foundation-acceptance/linux-x86_64-clang-Debug-ld_u43zk/cache-after.json` and `linux-x86_64-clang-Release-gthgciz3/cache-after.json`, respectively. Before observations precede the separately logged setup commands and were not overwritten.

Disposable clean removed only each acceptance workspace's selected clean/Debug or clean/Release tree. Other disposable configuration, normal Debug/Release output, project Conan tools and actual package cache matched before/after read-only metadata inventories. Access time was excluded, links/junctions not traversed. Detailed clean digests were held in memory and not persisted after the offline stops; no digest values or byte-content hash of the full cache are invented. Normal Debug has only regenerated setup output; it was not silently configured/built after Plan 01-06 clean.

Offline-only recovery retained successful results.json and rejection logs at `build/foundation-acceptance/linux-x86_64-clang-Debug-yd7zhm3i/` and `linux-x86_64-clang-Release-qn3kxwxt/`. Distinct empty homes `/tmp/veyr-foundation-empty-conan-89e1rm95` and `/tmp/veyr-foundation-empty-conan-9pdckgj3` were used, never purging the real cache. Each ran one Conan install with tracked lock/profiles, --no-remote --build=never. Expected child exit 1 was correctly reported and matched only with both observed lines:

```text
gtest/1.18.0: Not found in local cache, looking in remotes...
ERROR: Package 'gtest/1.18.0' not resolved: No remote defined. Required by ''
```

The generic remotes wording did not enable a remote or cause acquisition. Wrong exit/diagnostic, missing either line, unrelated package, download/build activity or unexpected success remains failure. This covers missing recipe/package cache, not a separate binary-only cache-miss scenario. No dependency download, remote enablement, dependency rebuild, forced build, purge, automatic retry or policy relaxation occurred in these final checks. Explicit setup's small consumer probe and isolated Veyr acceptance targets did compile/link; they are not dependency package rebuilds. Both recoveries followed explicit owner decisions, without duplicate active execution.

The approved runner correction checks final returncode after EOF before returning a completed result. Five deterministic mocked race tests plus the existing mocked Windows success fixture passed 6/6; foundation fixtures passed 16/16. Drained merged stdout/stderr, successful exit 0, exact nonzero propagation, one launch/stop and no retry were preserved. Streaming and liveness behavior were unchanged. Last test durations were 0.107s and 0.063s; no tests were repeated during closeout.

## Earlier current-lifecycle evidence and limits

[Plan 01-02](../../.planning/phases/01-conan-and-compiler-build-foundation/01-02-SUMMARY.md) records the original LLVM 23.1.2 Conan source/checksum/build/install/package operation: official llvm-project-23.1.2.src.tar.xz SHA256 c98bbef08a2b4c2613cd50e9aa9ae7b69b1fe6c16b2c40373bc0ab6116fdf78a, approximately 11m11s for initial create, and separate final cached Debug/Release probes approximately 2s each. Initial package creation stopped at its consumer FORCE_OFF guard; the approved expectation correction and subsequent probes passed without rebuilding LLVM. Its preflight observed 12 logical CPUs, approximately 53 GiB available RAM and 382 GiB filesystem space, with Conan jobs=8. These are current-lifecycle historical host observations, not peak resource measurements or CI capacity proof. No cold build repeated in Plan 01-07.

[Plan 01-05](../../.planning/phases/01-conan-and-compiler-build-foundation/01-05-SUMMARY.md) retains 20 include fixtures and independent Linux Debug/Release include-check acceptance. Plan 01-07's native CTest suites also included the canonical owned-header check. [Plan 01-06](../../.planning/phases/01-conan-and-compiler-build-foundation/01-06-SUMMARY.md) retains developer chaining/clean/liveness fixtures (30 new and 60 regression checks), successful native Debug/Release all and selected Debug clean preservation. Ordinary setup remains explicit; build may configure, test may build, all includes setup and equivalent steps are reused. These earlier suites were not rerun for this ledger.

Approximately 60 quiet seconds remains a soft liveness checkpoint. Plan 01-07 combined durations include read-only metadata scans, not an expensive quiet dependency build. No healthy command was automatically terminated. Existing supervision fixtures establish policy behavior; native Windows process-tree handling and expensive-build quiet telemetry are not established by these short native commands.

Remaining gaps: actual Windows developer SDK/toolset/CRT/iterator/runtime identities, Windows locks and native Debug/Release execution; real prerequisite validation of the independently reviewed Windows/Ubuntu CI candidates and all four passing CI combinations; cold-runner source-build capacity; broader ABI surfaces; arbitrary external LLVM installations; native spaced-path behavior (historical Linux profile quotation warnings remain). These gaps are not waived by available-Linux closeout or execution authorization. Linux execution is authorized subject to policy/prerequisite checks; Windows remains blocked before package/native work by missing reviewed locks. All eight waves and discussion decisions remain unchanged; progress 7/8, macOS deferred, no phase-wide requirements marked complete.

Platform impact:
- Windows: Native developer/CI Debug/Release evidence remains pending; mocked selection/process checks do not certify CRT/SDK or native execution.
- Linux: Available Omarchy/Arch Debug/Release execution-task evidence accepted; Ubuntu CI stays a separate pending baseline.
- macOS future: Deferred; structured paths/processes and standard Conan/CMake seams preserved without Darwin acceptance claims.
