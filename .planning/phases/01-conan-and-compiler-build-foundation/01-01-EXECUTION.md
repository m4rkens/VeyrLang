# Phase 1 / Plan 01-01 execution checkpoint

Owner authorization: $gsd-execute-phase 1.
Recorded: 2026-10-04, after Task 1 checks.
Planning commit: f4d8335d0c09eef7eb9ec3bb756e92ff9f7fd5a8.

## Resume position

- Completed: Task 1, capture host baseline and explicit candidate profile inputs; Task 2 candidate-policy review using prior explicit owner approval, with readiness gaps reported.
- Current: Conan CLI 2.33.0 installed; setup exited 0 and saved native doctor passed 22/22 identities. Plan 01-01 acceptance waits on the read-only doctor contract correction decision, not tool installation.
- Not started: Wave 2 and all package/native build work. Windows and CI observations remain independently pending.
- Completed plans: 0/8. No plan SUMMARY exists because Plan 01-01 is incomplete.
- Execution is inline. No staging, commit, dedicated phase verification or next-phase transition occurred.

Resume from the read-only doctor correction decision recorded below. The real
Conan CLI prerequisite is satisfied; do not repeat installation or completed
Tasks 1–3. Plan acceptance and later package/build authorization remain pending.
Preserve committed discussion and completed research; reuse original host inventory
unless the environment changes. Linux developer candidate is the actual Omarchy
host, not a required Ubuntu host. The bounded planning correction did not execute
implementation; separately authorized Task 3 alignment is recorded below.
Candidate selection remains separate from installed prerequisites and native
ABI/package readiness. No Conan installation or package operation occurred.

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
raw inventory and Task 1 implementation remained unchanged during that correction.
Task 3's separately authorized alignment supersedes the old Linux candidate pins;
the original implementation snapshot below is historical.
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

## Resumed Task 2 review and next owner decision — 2026-10-04

Owner explicitly resumed execution at Task 2 after the separate correction commit
8336e13e7a85b2bf4661828a7bdf4aa6b1211937, requiring exact preservation of all
eight untracked Task 1 outputs and a stop at the next required owner decision.
Reviewed the saved inventory and candidate/profile selection code without
restarting Task 1, repeating research/planning, re-probing tools or running tests.

The prior explicit owner selection satisfies Task 2's candidate-policy decision:
Omarchy/Arch Linux developer identities remain the approved candidate. Required
Conan 2.33.0 is missing; LLVM backend target remains 23.1.2. Windows developer
candidates are unchanged with native evidence pending. Ubuntu Linux CI and
Windows CI observations remain independently pending; macOS remains deferred.
Task 2 candidate review is complete, not prerequisite/package/ABI acceptance.
No pending value or candidate implementation status was promoted to ready.

Next owner decision: allow Task 3's planned non-package implementation to edit
scripts/devtools/baseline.json, scripts/devtools/doctor.py,
tests/development/test_doctor.py and conan/profiles/linux-x86_64-clang, or retain
exact preservation of all eight outputs and hold execution. Task 3 also creates
scripts/dev.py as planned. Baseline policy still combines old tool versions and
Ubuntu/GCC 14 Linux expectations; profile and selection code still require Clang
major 20. Aligning these with Omarchy/Clang 22 requires edits to the preserved
files, so no Task 3 work was started and no decision was assumed.

Authorizing those bounded edits need not authorize Conan installation or package
operations. Non-package doctor/profile work can proceed while Conan is missing;
actual doctor must exit nonzero and report that missing prerequisite. Package
commands remain blocked until concrete reviewed identities and matching tools
are available. Native compatibility and CI acceptance remain later evidence gates.

Changed only this execution record and .planning/STATE.md. All eight existing
untracked outputs remain byte-for-byte unchanged. No staging, commit, build/test,
installation, package operation, implementation edit, later wave or phase
advancement occurred during the resumed review. Stop for the owner decision.

## Authorized Task 3 alignment and checks — 2026-10-04

Owner authorized edits only to scripts/devtools/baseline.json,
scripts/devtools/doctor.py, tests/development/test_doctor.py and
conan/profiles/linux-x86_64-clang, plus creation of scripts/dev.py. Task 1 was
not restarted. The raw inventory, Windows profile and Debug/Release overlays
remain byte-for-byte unchanged. No completed research/planning was repeated.

Linux developer policy now selects the observed Omarchy/Arch x86_64 candidate:
Python 3.14.7, CMake 4.4.3, Clang 22.1.8, GCC/libstdc++ 16.2.1, Ninja 1.13.2,
Git 2.55.0, glibc 2.44 and Binutils 2.47. Native profile selects Clang major 22;
libstdc++11 remains the Conan ABI setting, not a library-version pin. Windows
candidate identities are unchanged and have separate tool expectations. Ubuntu
and Windows CI entries remain pending; the CLI rejects substituting developer
identity for CI, including when GITHUB_ACTIONS=true. Backend LLVM stays 23.1.2.

The CLI implements only doctor at this checkpoint. Native Debug is default;
Release and explicit native build-profile/provider/home options are supported.
BuildSelection records separate canonical host/target identity, native Release
build profile, environment, configuration, provider, LLVM config path, build and
lock paths, and dedicated external Conan home. These paths are computed only;
no build directory, cache, lock or receipt is created. No custom graph, installer,
source acquisition, SDK normalization or package storage layer was added.

Doctor sequentially observes exact tools and native paths without compilation or
linking. It reports expected/actual/missing values and conventional remedies;
missing or mismatched inputs cause nonzero exit. External providers require an
explicit LLVMConfig.cmake path; no silent fallback. Windows SDK/CRT/linker/VS
checks are implemented but lack native Windows evidence. Fixture results cannot
certify those Windows identities. Even a Conan version probe uses the selected
dedicated home; the default shared .conan2 home is rejected. No package command
is exposed. No installed or package-ready status is persisted by doctor.

Final fixture command:

    PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s tests/development -p test_doctor.py

Result: 22 tests passed in 0.052 seconds. Fixtures cover missing/mismatched exact
tools and native identities/paths, Windows SDK/runtime and case/space handling,
Debug/Release host versus native Release build selection, independently pending
CI identities, default/override CLI behavior, dedicated home isolation and
rejection of shared/in-checkout homes, build/provider identity isolation,
external-config absence, one version probe with no package command, and failure
diagnostics. These are synthetic checks, not native ABI/package acceptance.
The independently selected CI fixture uses separate synthetic CI identities to
prove that future Plan 08 baseline updates cannot overwrite developer tool
expectations. Actual CI records remain pending and rejected; no CI evidence
was fabricated or inferred from that fixture.

Final native command:

    PYTHONDONTWRITEBYTECODE=1 python3 scripts/dev.py doctor --json

Result: exit 1 as required. 21 identity/path checks passed; the sole failed check
is tools.conan, expected 2.33.0, actual missing. Selected host/target is
linux-x86_64-gnu, compiler target x86_64-pc-linux-gnu, configuration Debug,
native build configuration Release and provider managed. A preceding human-readable
doctor invocation reported the same result. No native compile/link/test ran.
Native ABI compatibility and package readiness are explicitly unverified.

Task 3 implementation/diagnostics are complete for owner review, while native
prerequisite acceptance remains blocked. Next owner decision: provide Conan
2.33.0 through conventional host installation, or separately authorize a scoped
tool installation. Current instruction forbids installation; no method or
permission is assumed. Rerun doctor only after the missing prerequisite is
provided. No wave 2 work, SUMMARY/plan completion marker, requirement acceptance,
package operation, stage verification, staging, commit or phase advancement.
Updated only the five authorized implementation paths plus STATE.md and this
execution checkpoint record. Stop for the owner decision.

Platform impact:
- Windows: Candidate clang-cl/VS/SDK inputs and Debug/Release dynamic-runtime
  selection exist; native identities, profile parsing and ABI validation remain pending.
- Linux: Observed Omarchy/Arch Clang 22/GCC 16 identities are selected as developer
  candidate; Ubuntu CI identity is independently pending. Conan is missing and
  doctor correctly fails after implementation alignment; native ABI/package
  compatibility remains unverified.
- macOS future: Explicit host/target selection uses platform-gated profiles and
  structured paths. Darwin remains unsupported/deferred without adding an installer.


## Owner-approved CLI-bootstrap correction and implementation — 2026-10-05

The owner approved the revised CLI contract and its bounded implementation, but
explicitly withheld permission for real installation, package work or builds.
Plan 01-01 Task 3 now owns the standard project-local Conan 2.33.0 CLI prerequisite
before Wave 2; Plan 01-03 retains Conan-owned dependency/package setup and will
ensure the CLI through the same function before package work. Plan 01-06 normal
clean preserves both the CLI tool environment and dedicated package home/cache.
The eight waves, eighteen tasks and dependency edges remain unchanged. Coverage
updates are limited to VALIDATION, PLANNING-BREAKDOWN and PLAN-REVIEW; no new
research, framework, package graph, lifecycle completion or readiness claim.

Bounded implementation changes only scripts/devtools/doctor.py, scripts/dev.py,
tests/development/test_doctor.py and docs/development/DEVELOPER_BOOTSTRAP.md.
setup --tools-only creates .veyr-dev/tools/conan-2.33.0 only when missing using
reviewed base Python, standard venv and pinned isolated pip. It validates the
isolated runtime/exact Conan version and runs pip check. Valid state is reused
without install/upgrade; broken or mismatched state fails once with exact-path
manual removal/rerun guidance, without deletion, repair or retry. Plain setup
is deferred until Plan 03 extends it with dependency setup. Doctor inspects only
the absolute project-owned executable and remains read-only. BuildSelection now
exposes conan_executable; every Conan identity probe receives the selected
external CONAN_HOME. No activation or persistent PATH change is used.

Final bounded check:

    PYTHONDONTWRITEBYTECODE=1 python3 -m unittest discover -s tests/development -p test_doctor.py

Result: 33 tests passed in 0.065 seconds. All installer, pip, Conan and native
identity commands in this suite are mocked. Added cases cover missing bootstrap,
exact isolated install arguments, validated reuse without install/upgrade,
broken/non-isolated/wrong-Conan/wrong-Python environments, unrelated PATH Conan,
venv/install/pip-check failures without retry, Windows executable paths and
tools-only stopping before package work. Temporary fixtures contain inert files,
not real Python environments. Earlier 22-test/native-doctor results above are
historical and were not repeated as native acceptance during this correction.

AST syntax checks passed for the three Python files; baseline JSON parses.
Static checks confirm eight waves and all four protected Task 1 file hashes are
unchanged. Committed discussion artifacts remain unchanged. Exact diff review
and whitespace checks passed. The real .veyr-dev/tools/conan-2.33.0 location does
not exist. Conan 2.33.0 is still required/missing; LLVM remains 23.1.2. Windows
developer/CI and Ubuntu CI evidence remain independently pending.

Next exact owner instruction:

    Run /usr/bin/python3 scripts/dev.py setup --tools-only on the current Omarchy host. Authorize real Conan CLI installation only; then stop for owner review.

No real Conan installation, system Python modification, Conan package operation,
LLVM/dependency build, native build/test, commit, plan completion or phase
advancement occurred. Task 3 remains the active prerequisite-acceptance gate;
0/8 plans are accepted complete. Stop for owner review.

Platform impact:
- Windows: Scripts/python.exe and Scripts/conan.exe use structured absolute
  paths; observed native developer/CI evidence remains pending.
- Linux: bin/python and bin/conan use the reviewed Omarchy Python; Ubuntu CI
  identity remains independent. No installation or native acceptance now.
- macOS future: Standard venv/Conan seams remain available; Darwin is deferred.


## Resumed Plan 01-01 acceptance review — 2026-10-05

Owner resumed $gsd-execute-phase 1 with the real CLI prerequisite explicitly
satisfied. The preceding authorized tools-only invocation completed successfully:

    /usr/bin/python3 scripts/dev.py setup --tools-only

Result: exit 0; created .veyr-dev/tools/conan-2.33.0 with exact Conan 2.33.0;
venv isolation/runtime/CLI checks and pip check passed. Absolute executable:
/home/m4rkens/Projects/VeyrLang/.veyr-dev/tools/conan-2.33.0/bin/conan.
Only pip installed the CLI and its Python runtime requirements inside that venv;
no Conan dependency/package command, LLVM/dependency build or system Python
package modification occurred.

The preceding post-installation identity command was:

    PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 scripts/dev.py doctor --json

Saved result: exit 0; all 22 identity/path checks passed for linux-developer,
Debug host configuration and native Release build profile. Dedicated home:
/home/m4rkens/.cache/veyr/conan/22e2b2472775. Native ABI and package readiness
were explicitly unverified; Windows developer, Ubuntu CI and Windows CI were
independently pending. The earlier missing-Conan records above are historical.
This resume did not reinstall Conan, rerun doctor or repeat the completed unit
checks/implementation. Installed dist-info metadata was inspected read-only and
confirms Version: 2.33.0. Four protected Task 1 hashes remain unchanged.

### Acceptance finding: read-only doctor probe

scripts/devtools/doctor.py inspect_conan_tool invokes the absolute Conan CLI
with --version. Installed Conan 2.33.0 cli.py constructs ConanAPI before parsing
that flag. ConanAPI initializes its cache helpers and invokes migrations, so a
fresh selected CONAN_HOME receives settings/configuration/cache metadata even
for a version command. This initialization was observed during the preceding
explicit tools-only setup. No dependency packages were acquired, but this same
probe cannot guarantee the approved read-only doctor contract for a fresh or
migration-requiring home. Passing the 22 identity comparisons does not prove
that invariant. Do not mark Plan 01-01 accepted or silently waive the contract.

Proposed bounded correction for owner decision: doctor reads the Conan version
from installed distribution metadata through the isolated project venv Python,
without importing/running the Conan CLI/API. Keep actual absolute CLI launch and
version validation in explicit setup, where Conan metadata initialization is
visible and permitted. Preserve the selected dedicated home and executable
contract; no temporary cache, new bootstrap framework, new wave or package graph.
Likely correction paths: scripts/devtools/doctor.py, scripts/dev.py,
tests/development/test_doctor.py and docs/development/DEVELOPER_BOOTSTRAP.md.
A meaningful regression must cover an absent selected home remaining absent
and an existing home remaining unchanged after doctor. No implementation edit
or correction decision is assumed in this resume.

Next owner instruction, if approved:

    Approve the bounded read-only doctor correction in those four files. Keep CLI launch validation in explicit setup; run relevant mocked/unit checks only. Do not reinstall Conan, run package operations/builds, commit or advance Phase 1.

Changed only STATE.md, the ROADMAP current readiness/status wording and this
execution checkpoint. No SUMMARY/completed-plan/requirement acceptance marker.
All eight waves and 0/8 accepted plans remain unchanged. No implementation,
Conan operation, build/test, commit or phase advancement occurred this resume.
Stop for owner decision.

Platform impact:
- Windows: The same CLI-initialization concern applies; developer/CI identities
  remain independently pending, and no native evidence is fabricated.
- Linux: Omarchy CLI prerequisite is satisfied; doctor read-only acceptance is
  still pending. Ubuntu CI identity remains separate.
- macOS future: Standard isolated Python/Conan boundaries remain extensible;
  Darwin execution stays deferred.

## Completed corrected native prerequisite acceptance — 2026-10-05

Owner approved the bounded read-only doctor correction, reviewed its completed
implementation, and explicitly resumed from the first incomplete acceptance
gate. The correction changes only doctor.py, dev.py, test_doctor.py and
DEVELOPER_BOOTSTRAP.md: doctor inspects isolated project-venv distribution
metadata without launching/importing Conan; explicit setup retains actual CLI
launch/version validation. Prior 38 mocked/unit tests and Python syntax checks
passed and were not repeated this resume. No installation or Task 1–3
implementation was repeated.

Remaining native acceptance command:

    PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 scripts/dev.py doctor --json

Exit 0; 22/22 exact identity/path checks pass on the observed Omarchy/Arch
linux-developer baseline. Host configuration Debug, native build configuration
Release; Conan 2.33.0 at the absolute project-owned bin/conan path. Doctor
reports prerequisites_ready true but native_compatibility and package_readiness
unverified. Windows developer, Ubuntu CI and Windows CI remain independently
pending; Windows candidates unchanged. LLVM backend target stays 23.1.2.

Before/after comparison proves the selected dedicated Conan home and tool venv
unchanged in directory entries, file contents, modes, symlink targets and
modification/change timestamps (access times excluded). Existing repository
files were unchanged by doctor. No Conan CLI command was launched.

Plan 01-01 is complete for the owner-approved available-host scope, with all
missing-environment/native ABI/package acceptance boundaries retained. Created
01-01-SUMMARY.md; updated only STATE.md, ROADMAP.md and this checkpoint in
addition to that summary. Phase 1 remains executing, 1/8 plans complete, zero
phases complete; all eight waves and committed discussion remain unchanged.
REQUIREMENTS.md remains unchanged; no phase-wide requirement is accepted complete.

Stop for interactive owner review of Plan 01-01 closeout and explicit
execution-commit disposition (or explicit deferral) before Wave 2. Next
incomplete plan: 01-02 narrow LLVM 23.1.2 recipe. No Wave 2 work, installation,
Conan package/dependency operation, LLVM/dependency build, repeated unit check,
staging, commit, dedicated phase verification or phase advancement occurred.

Platform impact:
- Windows: Developer and CI evidence remain independently pending; no native
  evidence fabricated or candidate identities rewritten.
- Linux: Omarchy/Arch prerequisite acceptance complete; doctor read-only state
  comparison passed. Ubuntu CI identity and ABI/package acceptance stay separate.
- macOS future: Standard Python/Conan seams retained; Darwin remains deferred.

## Dedicated execution checkpoint commit authorization — 2026-10-05

After reviewing completed Plan 01-01 execution and closeout, the owner explicitly
requested one checkpoint commit: `build: establish Conan-aware developer baseline`.
Include the completed implementation/profiles, reviewed bootstrap documentation,
Plan 01-01's approved Task 3 ownership correction and saved host inventory, plus
ROADMAP.md, STATE.md, this execution record and 01-01-SUMMARY.md. Task 1 is not
committed separately. Only closeout commit-status wording is updated now;
implementation and all acceptance/pending evidence boundaries are preserved.

Exclude Plan 01-03/01-06 edits and phase-wide PLAN-REVIEW, PLANNING-BREAKDOWN and
VALIDATION changes. These remain uncommitted for their own owner disposition.
Generated tool/cache/build state is excluded. Reuse already reviewed 38 mocked
checks and native doctor 22/22 evidence; no tests or doctor invocation repeat.

After the dedicated commit, stop for owner review. Phase 1 remains executing,
1/8 plans complete; Plan 01-02 has not started. Windows developer/CI and Ubuntu
CI evidence remain independently pending, ABI/package readiness unverified,
LLVM target 23.1.2 and project-owned Conan 2.33.0 unchanged. No installation,
Conan package/dependency operation, LLVM/dependency build or phase advancement
is authorized by this commit request.
