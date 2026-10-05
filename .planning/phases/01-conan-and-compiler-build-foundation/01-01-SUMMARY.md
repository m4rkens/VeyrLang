---
phase: 01-conan-and-compiler-build-foundation
plan: "01"
subsystem: developer-tooling
tags: [conan, python, profiles, doctor, linux, windows]
requires: []
provides:
  - Exact developer baseline policy and native host/build profile selections
  - Strict read-only prerequisite doctor and absolute project Conan executable selection
  - Project-local Conan 2.33.0 CLI-only bootstrap with safe reuse/failure behavior
  - Available Omarchy/Arch Linux prerequisite acceptance
affects: [01-02, 01-03, 01-06, 01-07, 01-08]
tech-stack:
  added: [Conan 2.33.0 CLI in standard project-owned Python venv]
  patterns: [thin orchestration, isolated distribution metadata, dedicated Conan home]
key-files:
  created:
    - scripts/dev.py
    - scripts/devtools/baseline.json
    - scripts/devtools/doctor.py
    - tests/development/test_doctor.py
    - conan/profiles/linux-x86_64-clang
    - conan/profiles/windows-x86_64-clangcl
    - conan/profiles/debug
    - conan/profiles/release
    - .planning/phases/01-conan-and-compiler-build-foundation/01-01-HOST-INVENTORY.json
  modified:
    - docs/development/DEVELOPER_BOOTSTRAP.md
key-decisions:
  - Omarchy/Arch is the observed Linux developer baseline; Ubuntu CI identity is independent.
  - Windows developer and CI identities remain independently pending; candidates unchanged.
  - Plan 01-01 owns Conan CLI bootstrap; Plan 01-03 owns dependency/package setup.
  - Doctor reads isolated package metadata; explicit setup validates actual CLI launch.
patterns-established:
  - Absolute project Conan invocation with selected dedicated CONAN_HOME
  - Reuse validated CLI venv without pip install or upgrade; broken state fails without automatic repair
  - Normal clean preserves tool environment and package cache
requirements-completed: []
requirements-covered: [BUILD-05, BUILD-06, BUILD-07, BUILD-13]
completed: 2026-10-05
status: complete
acceptance-scope: available Linux developer prerequisites and reviewed fixture-backed implementation
pending-environments: [windows-developer, ubuntu-ci, windows-ci]
native-compatibility: unverified
package-readiness: unverified
---

# Plan 01-01 execution summary

**Exact native selections, read-only doctor and project-local Conan CLI bootstrap now pass prerequisite acceptance on the observed Omarchy/Arch host.**

## Accomplishments

- Tasks 1–3 are complete within the owner-approved available-host scope: explicit profiles/configuration overlays, reviewed Linux baseline and strict selection/identity diagnostics.
- Project-owned Conan 2.33.0 lives under `.veyr-dev/tools/conan-2.33.0`; explicit `setup --tools-only` creates it only when missing, validates isolation/Python/CLI/version and runs pip check. Valid state is reused without installing/upgrading; broken or mismatched state fails with bounded manual cleanup instructions.
- Doctor ignores ambient Conan on PATH, inspects isolated installed-distribution metadata without importing/launching Conan and exposes the absolute executable through BuildSelection. Explicit setup retains actual CLI launch validation; dependency setup is reserved for Plan 01-03.
- Selected host configuration defaults to Debug; native build profile uses Release. Profile inputs cover both Debug/Release and preserve separate Windows runtime settings. Selection fixtures are evidence of behavior, not native builds.

## Acceptance evidence

Prior completed and owner-reviewed checks, reused without rerunning this resume:

- 38 mocked/unit tests passed via `PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 -m unittest discover -s tests/development -p test_doctor.py` (0.085 s). Coverage includes missing bootstrap, reuse without pip, broken/wrong-version environments, unrelated PATH Conan, one-shot failures, tools-only package isolation, read-only metadata/home behavior and setup's real CLI-validation path.
- Python AST syntax checks for `scripts/dev.py`, `scripts/devtools/doctor.py` and `tests/development/test_doctor.py` passed.
- Separately authorized `/usr/bin/python3 scripts/dev.py setup --tools-only` exited 0; exact CLI installation and pip check passed. This was CLI-only pip work inside the project venv, with no system Python modification or Conan dependency/package operation. Installation was not repeated.

First incomplete acceptance check performed this resume:

```text
PYTHONDONTWRITEBYTECODE=1 /usr/bin/python3 scripts/dev.py doctor --json
```

Exit 0; **22/22** checks pass for `linux-developer`, host/target `linux-x86_64-gnu`, profile/build-profile `linux-x86_64-clang`, Debug host and native Release build configuration. Expected and observed identities match:

- Omarchy 4.0.4 / Arch x86_64; Python 3.14.7; Conan 2.33.0; CMake 4.4.3; Ninja 1.13.2; Git 2.55.0.
- Host Clang/Clang++ 22.1.8, target `x86_64-pc-linux-gnu`; GCC/G++ and libstdc++ toolchain 16.2.1; glibc 2.44; GNU Binutils 2.47.
- Reviewed compiler/linker paths, GCC installation, standard-library headers and library identities match. Backend LLVM target remains 23.1.2, distinct from host Clang.
- Conan executable: `/home/m4rkens/Projects/VeyrLang/.veyr-dev/tools/conan-2.33.0/bin/conan`.
- Dedicated Conan home: `/home/m4rkens/.cache/veyr/conan/22e2b2472775`.

Read-only before/after comparison of that home and tool venv found identical directory entries, file contents, modes, symlink targets and modification/change timestamps (access times excluded). All existing repository files also remained unchanged by doctor. Doctor did not launch any Conan CLI command. Reported `prerequisites_ready: true`; `native_compatibility` and `package_readiness` remain `unverified`.

## Owner-approved corrections and issues resolved

- Replaced the proposed Ubuntu developer baseline with observed Omarchy/Arch identities; independent Ubuntu CI evidence remains pending. Windows candidate identities were preserved.
- Assigned CLI bootstrap to Task 3 before Wave 2, leaving Conan dependency/package setup in Plan 01-03 and cache-preserving clean in Plan 01-06. All eight waves retained.
- Initial CLI version probing could initialize/migrate Conan home metadata. The separately approved, implemented and reviewed four-file correction uses isolated distribution metadata in doctor; setup alone retains CLI launch validation. The current native acceptance closes that read-only contract gap.

These are recorded owner-approved corrections, not automatic scope expansion. Completed implementation and mocked checks were not repeated during closeout.

## Task commits

No separate task commits. After completed execution and owner review, the owner explicitly requested one dedicated checkpoint: `build: establish Conan-aware developer baseline`. This summary is included in that checkpoint; its hash is available from Git history. Task 1 is not committed separately. Earlier discussion and planning commits remain separate; future-plan and phase-wide planning corrections are excluded from this execution checkpoint.

## Remaining acceptance boundaries

- Windows developer actual VS/VCToolsVersion/CRT/SDK/linker identities remain pending from its native host. Windows CI and Ubuntu CI identities remain independently pending. No support readiness is inferred from candidates or fixtures.
- Native LLVM exports, ABI/runtime compatibility, Debug/Release builds on each Tier 1 host, offline/cache behavior and hosted CI capacity remain later execution checks.
- BUILD-05/06/07/13 have plan-level contributions, not phase-wide completion. `requirements-completed` is intentionally empty and REQUIREMENTS.md checkboxes unchanged; later plans and independent platform evidence are still required.
- Saved host inventory remains a historical observation; its pre-installation missing-Conan observation is superseded by the explicit installation and acceptance evidence here. The inventory and all existing Task 1 outputs are preserved.

## Next execution gate

The dedicated Plan 01-01 checkpoint commit is explicitly authorized; stop for owner review after committing. Next incomplete plan is **01-02, Wave 2**, the narrow Conan LLVM 23.1.2 recipe. No Wave 2 implementation, dependency/package operation, LLVM build, phase verification or phase advancement has started. Prerequisite acceptance and checkpoint commit authorization grant none of those actions by themselves.

Platform impact:
- Windows: Absolute `Scripts/conan.exe`/isolated Python paths are fixture-covered; native developer/CI identities and runtime acceptance remain pending.
- Linux: Corrected doctor has native read-only prerequisite acceptance on Omarchy/Arch; Ubuntu CI is independent; ABI/package readiness remains unverified.
- macOS future: Standard Python/Conan/path/process boundaries retained; Darwin implementation and evidence deferred.
