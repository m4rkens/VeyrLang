# Foundation acceptance ledger

**Status:** Plan 01-07 complete for the available Omarchy/Arch Linux developer scope, by explicit owner closeout. Windows developer and independent Windows/Ubuntu CI evidence remain pending; macOS deferred. Phase 1 remains executing, 7/8 plans complete and zero phases complete.

This ledger records preserved execution-task observations from the current fresh lifecycle. No builds, tests, setup or Conan operations ran to create it. Dedicated phase verification and four-combination Tier 1 CI acceptance remain separate gates. Exact commands, timestamps, failures, approved corrections and logs are in [01-07-EXECUTION.md](../../.planning/phases/01-conan-and-compiler-build-foundation/01-07-EXECUTION.md).

## Native configuration matrix

| Developer host / consumer | Provider configure/build/link/native tests | Runtime / cache / clean | Offline acceptance | Scope |
|---|---|---|---|---|
| Omarchy/Arch Linux x86_64 / Debug | Managed, explicit system and custom: CTest 13/13 each; four C++23 compile entries each | Runtime 5/5; warm reuse and seven unchanged cache identities; disposable selected clean preservation passed | Passed separately; expected empty-cache/no-remote child exit 1, harness exit 0 | Available Linux developer accepted |
| Omarchy/Arch Linux x86_64 / Release | Managed, explicit system and custom: CTest 13/13 each; four C++23 compile entries each | Runtime 5/5; warm reuse and seven unchanged cache identities; disposable selected clean preservation passed | Passed separately; expected empty-cache/no-remote child exit 1, harness exit 0 | Available Linux developer accepted |
| Windows x86_64 / Debug | Pending actual native host inspection, reviewed runtime identity and execution | Pending native evidence, including Debug CRT/iterator compatibility | Pending | No Linux evidence credited |
| Windows x86_64 / Release | Pending actual native host inspection, reviewed runtime identity and execution | Pending native evidence | Pending | No Linux evidence credited |

Ubuntu GitHub Actions Linux Debug/Release and Windows GitHub Actions Debug/Release are independently pending. Ubuntu is not the Linux developer baseline. Plan 01-08 local workflow/documentation execution has started; no workflow publication/run or cold-runner capacity is claimed. Windows candidate identities have not been rewritten or promoted to observed evidence.

## Plan 01-08 independent CI evidence gate

The local draft `.github/workflows/build-foundation.yml` implements the four
matrix rows below. Initial CI policy remains pending and stops before CLI or
package setup, after reporting installed runner observations. Creating/reviewing
this YAML does not pass native CI. Its workflow inputs and blocking gates are
documented in [CI_MATRIX_POLICY.md](CI_MATRIX_POLICY.md).

| CI environment / consumer | Observed exact identity / tracked lock | Native configure/build/test/verify | Actual job URL | Cold/warm capacity |
|---|---|---|---|---|
| Ubuntu 24.04 x86_64 / Debug | CI tools/runtime pending; existing Linux lock is not binary/ABI proof | Pending | Pending; no run | Pending |
| Ubuntu 24.04 x86_64 / Release | CI tools/runtime pending; existing Linux lock is not binary/ABI proof | Pending | Pending; no run | Pending |
| Windows 2022 x86_64 / Debug | CI VS/VC/CRT/SDK/linker/tools and Windows Debug lock pending | Pending | Pending; no run | Pending |
| Windows 2022 x86_64 / Release | CI VS/VC/CRT/SDK/linker/tools and Windows Release lock pending | Pending | Pending; no run | Pending |

No runner/tool identities, resource peaks, cache hits or CI URLs are fabricated.
The draft records setup wall time but does not measure peak memory/disk; actual
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

| Authoritative input | Recorded SHA256 |
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

Remaining gaps: actual Windows developer SDK/toolset/CRT/iterator/runtime identities, Windows locks and native Debug/Release execution; independent Windows/Ubuntu CI identities and all four passing CI combinations; cold-runner source-build capacity; broader ABI surfaces; arbitrary external LLVM installations; native spaced-path behavior (existing Linux profile quotation warnings remain). These gaps are not waived by available-Linux closeout. All eight waves and discussion decisions remain unchanged; no phase-wide requirements marked complete.

Platform impact:
- Windows: Native developer/CI Debug/Release evidence remains pending; mocked selection/process checks do not certify CRT/SDK or native execution.
- Linux: Available Omarchy/Arch Debug/Release execution-task evidence accepted; Ubuntu CI stays a separate pending baseline.
- macOS future: Deferred; structured paths/processes and standard Conan/CMake seams preserved without Darwin acceptance claims.
