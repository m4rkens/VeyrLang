# Phase 1 / Plan 01-01 execution checkpoint

Owner authorization: $gsd-execute-phase 1.
Recorded: 2026-10-04, after Task 1 checks.
Planning commit: f4d8335d0c09eef7eb9ec3bb756e92ff9f7fd5a8.

## Resume position

- Completed: Task 1, capture host baseline and explicit candidate profile inputs.
- Current: Task 2, Omarchy developer candidate selected; bounded planning correction applied for owner review. Prerequisite/package readiness remains pending.
- Not started: Task 3, full doctor/CLI and actual prerequisite validation.
- Completed plans: 0/8. No plan SUMMARY exists because Plan 01-01 is incomplete.
- Execution is inline. No staging, commit, dedicated phase verification or next-phase transition occurred.

Resume from Task 2 after owner review of this bounded planning correction.
Preserve committed discussion and completed research; reuse original host inventory
unless the environment changes. Linux developer candidate is the actual Omarchy
host, not a required Ubuntu host. This correction does not resume execution,
align implementation, install Conan or acquire packages. Candidate selection is
separate from installed prerequisites and native ABI/package readiness.

## Exact files created by Task 1

- conan/profiles/linux-x86_64-clang
- conan/profiles/windows-x86_64-clangcl
- conan/profiles/debug
- conan/profiles/release
- scripts/devtools/baseline.json
- scripts/devtools/doctor.py
- tests/development/test_doctor.py

Execution evidence: 01-01-HOST-INVENTORY.json and this record.
Current progress is reflected in .planning/STATE.md and the ROADMAP progress
row. The owner-approved eleven-file planning correction changes Plan 01-01,
Plan 01-08 and supporting policy/coverage documents only. Original discussion,
raw inventory and Task 1 implementation remain unchanged. Implementation still
uses superseded Linux candidate pins and must be aligned during resumed execution.
scripts/dev.py has not been created.

## Original evidence and superseded candidate comparison

Inventory was collected by sequential, read-only version/identity probes.
No tools were installed, no packages acquired and no compiler/linker build ran.

| Identity | Planned candidate | Observed Linux host |
|---|---|---|
| Distribution | Ubuntu 24.04 | Omarchy 4.0.4, Arch family |
| Python | 3.13.15 | 3.14.7 |
| Conan | 2.33.0 | Missing on PATH |
| CMake | 3.31.8 | 4.4.3 |
| Ninja | 1.13.2 | 1.13.2 |
| Host Clang | 20.1.8 | 22.1.8 |
| Git | 2.55.0 | 2.55.0 |
| GCC / libstdc++ toolchain | 14.2.0 | GCC 16.2.1; headers /usr/include/c++/16 |
| libstdc++ library | Pending candidate path | /usr/lib/gcc/x86_64-pc-linux-gnu/16/libstdc++.so |
| GCC installation | Pending candidate path | /usr/lib/gcc/x86_64-pc-linux-gnu/16 |
| libc | Pending candidate identity | GNU libc 2.44 |
| Linker | Pending candidate identity | GNU ld, Binutils 2.47 |
| Compiler target | Native Linux x86_64 GNU | x86_64-pc-linux-gnu |

The comparison above preserves the original Task 1 checkpoint history; its Ubuntu,
Clang 20/GCC 14 and shared tool pins are superseded for Linux developer selection.
The owner now selected the observed Omarchy identities as candidate evidence.
Native ABI/link/run compatibility has not been tested. Backend LLVM 23.1.2 remains fixed and distinct
from the host Clang candidate; no backend package is present or claimed.

Windows has not been inspected. VS 17.14.37710.0 and SDK 10.0.26100.0 remain
candidates. VCToolsVersion, CRT, linker and actual VS/clang-cl/SDK paths remain
null. No runtime identity was inferred from a redistributable or runner label.

## Task 1 implementation snapshot and remaining alignment

Existing implementation entries remain candidate; normal selection rejects them.
Their old Linux tool pins and Clang-major-20 guard have not been changed by this
planning-only correction. Future execution must separate Linux developer policy
from Windows candidates and independently pending CI identities before usage.
read_baseline reads policy without converting unknown identities into support.
select_inputs rejects unreviewed/incomplete identities and unsupported
profile/configuration choices. It selects explicit host profile plus Debug or
Release overlay, separately selecting a native Release build profile.

Profiles require explicit environment paths supplied by selection. Linux pins
libstdc++11 and the reviewed GCC installation/linker. Windows pins dynamic v143
runtime; host runtime type follows Debug/Release, while build context remains
Release. llvm/*:build_type=Release leaves runtime selection intact.

These are Conan-native inputs, not a package graph, installer, SDK normalizer,
package cache or second lock format. Tool paths are prerequisite identities.
Full actual-tool comparison and conventional remedy diagnostics belong to
Task 3 and have not been implemented.

## Checks performed during earlier Task 1 execution

Command:

    PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s tests/development -p test_doctor.py

Result: 11 tests passed in 0.006 seconds on the observed Python 3.14.7 host.
This is a fixture check, not acceptance of Python 3.14.7 as the supported pin.
An earlier 8-test run passed before adding range/missing-identity/policy checks.
Cases cover unknown native inputs, unsupported profiles/configurations, exact
tool versions, separate host/build selections, Windows configuration-specific
runtime and paths with spaces, Linux explicit GCC paths, and malformed JSON.
Fixture identities are synthetic and do not constitute native execution evidence.

Static profile review used official [Conan profile documentation](https://docs.conan.io/2/reference/config_files/profiles.html),
[CMakeToolchain configuration](https://docs.conan.io/2/reference/tools/cmake/cmaketoolchain.html)
and [Clang driver flags](https://clang.llvm.org/docs/ClangCommandLineReference.html).
Conan profile rendering/parsing and native compiler/linker behavior remain
unchecked because Conan is absent and no native support readiness is established.
No builds/tests, inventory re-probes or package operations ran for this correction.

## Owner-approved bounded planning correction — 2026-10-04

The owner explicitly superseded the earlier supply-evidence/retained-Ubuntu
assumption and approved only the eleven identified planning corrections. Adopt
Omarchy/Arch x86_64 as the Linux developer baseline candidate using observed:
Python 3.14.7, CMake 4.4.3, Clang 22.1.8, GCC/libstdc++ 16.2.1, Ninja 1.13.2,
Git 2.55.0, glibc 2.44 and GNU Binutils 2.47. Omarchy release is 4.0.4 in the
unchanged raw inventory. No separate Ubuntu developer host is required.

Conan 2.33.0 remains required and currently missing. LLVM backend target stays
23.1.2. Windows developer candidates remain unchanged and require independent
native host inspection. Ubuntu 24.04 Linux CI identity and Windows CI identity
remain independently pending until their actual workflows are observed; neither
is inferred from developer evidence or public runner labels. macOS stays deferred.

Shared Python commands, exact dependency references/options, Conan ownership,
Debug/Release policy, cache behavior and eight-wave order are preserved. The dated
clarification of D-07/D-14 is recorded in planning documents while committed
01-CONTEXT.md and 01-DISCUSSION-LOG.md remain unchanged. Plan 08 will update the
existing baseline policy with independently observed CI expectations during
its later authorized work; no new infrastructure framework is introduced.

Task 2 can resume review of selected candidate policy after this correction;
it has not been marked complete. Missing Conan, unproven native compatibility
and independent Windows/CI evidence remain readiness gates. No implementation
alignment, installation, package work, builds/tests, commit or phase advancement
was performed by this planning-only turn. Stop for owner review.

Platform impact:
- Windows: Candidate clang-cl/VS/SDK inputs and Debug/Release dynamic-runtime
  selection exist; native identities, profile parsing and ABI validation remain pending.
- Linux: Observed Omarchy/Arch Clang 22/GCC 16 identities are selected as developer
  candidate; Ubuntu CI identity is independently pending. Conan is missing and
  implementation alignment/native compatibility remain pending.
- macOS future: Explicit host/target selection uses platform-gated profiles and
  structured paths. Darwin remains unsupported/deferred without adding an installer.
