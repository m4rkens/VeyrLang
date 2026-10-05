# Plan 01-03 execution checkpoint

## Authorization and starting state

- Owner invoked `$gsd-execute-phase 1`, then selected option 1, Execute Plan 01-03.
- Start commit: `3f0636bda87da69a8526b0c57bf9844b8448c831`, Plan 01-02 dedicated checkpoint. Worktree was clean.
- Plans 01-01/01-02 remain complete for available Linux scope; progress is 2/8 and zero completed phases. All eight waves and approved discussion/plans are unchanged.
- Execution is inline; no sub-agents requested. Repository no-auto-commit and lifecycle gates override upstream workflow defaults.

## Task 1 progress — 2026-10-05T09:55:36Z

Added root `conanfile.py` with class `VeyrDependencies`:

- Exact `gtest/1.18.0`, with GoogleMock enabled and static libraries by default.
- `llvm_provider` choices managed/system/custom, managed by default. Only managed requires `llvm/23.1.2@veyr/m1`; external modes never add an LLVM acquisition requirement.
- Standard `CMakeToolchain` (Ninja) and `CMakeDeps`; source-root user presets disabled.
- Flat consumer build layout `.` and generators `generators`, relative to the selected Conan output folder. No custom acquisition graph, package storage, SDK normalization or lock format.

Checked relevant generator APIs against installed Conan 2.33.0 source without executing its CLI. Consumer generation and graph resolution are not yet validated by Conan.

### Checks actually run

1. `/usr/bin/python3 -I -B -c 'from pathlib import Path; source = Path("conanfile.py"); compile(source.read_text(encoding="utf-8"), str(source), "exec"); print("conanfile.py: Python syntax passed; recipe not imported or executed")'` — exit 0. Compile-only syntax check; no imported recipe, bytecode or Conan state.
2. Read-only `select_inputs` calls using existing policy for Linux Debug/Release and Windows — exit 0 with the expected caught Windows selection error. Both Linux host configurations use their explicit native profile/config overlays; build profiles use native Release. Windows rejected with `Baseline windows-x86_64-clangcl is not owner-selected; native evidence is pending`. No doctor observations or completed mocked suites repeated, no tool probes or Conan child commands.

## CHECKPOINT — native lock execution scope decision

Historical checkpoint below: owner subsequently selected option 1. Latest continuation record follows at the end of this document.

**Current task:** Task 1, incomplete. Consumer input exists; no locks generated. Task 2 unstarted.

Plan 01-03's Task 1 acceptance requires four real Conan-generated native lock files. Current Windows policy is still a candidate: exact VC tools/CRT/linker identities and native tool paths are missing. The reviewed selection code correctly refuses it. Linux observations cannot supply Windows or CI identities. Generating candidate-only Windows locks on this Linux host would not meet the plan's evidenced-native selection requirement.

Smallest owner decision:

1. Continue Plan 01-03 for available Omarchy/Arch Linux Debug/Release scope now; keep both Windows lock files and native checks pending until independent host inspection. Retain Tier 1 acceptance and all eight waves. Run Linux resolution/lock work one configuration at a time through project-owned Conan 2.33.0 and the existing dedicated home, then continue the shared Task 2 implementation and available Linux checks. Never claim the four-lock acceptance is complete while two Windows locks are missing.
2. Wait for reviewed Windows native inputs before continuing Task 1 lock execution.

No placeholder locks, invented revisions, Windows evidence, changed candidate identities, plan ordering or automatic scope corrections. No Task 1 completion or Plan 01-03 summary claimed.

### Constraints for any owner-approved continuation

- Reuse the existing LLVM recipe/package and dedicated `CONAN_HOME`; do not clean/purge cache or force builds.
- Conan owns real revisions and package mechanics. Lock initialization is explicit; ordinary setup later must not update tracked locks automatically.
- Managed dependency setup uses `--build=missing`; prepared generation uses `--no-remote --build=never` with selected locks. Validate external-provider graph subsets through Conan; stop if that assumption fails.
- Actual Veyr CMake configure is deferred until Plan 01-04 supplies its files. Task 2 fixture tests must not launch unavailable CMake infrastructure.
- Supervise native/package commands serially under the watchdog: stop on first non-zero exit; silence near 60 seconds triggers liveness inspection only, never duplicate retries.
- No commit, phase-wide verification or phase advancement authorized.

## Files changed at this checkpoint

- `conanfile.py` — new Task 1 consumer input.
- `.planning/phases/01-conan-and-compiler-build-foundation/01-03-EXECUTION.md` — this checkpoint.
- `.planning/STATE.md` — active Task 1 checkpoint, unchanged 2/8 progress.
- `.planning/ROADMAP.md` — records started Task 1 and incomplete acceptance, unchanged plan ordering/count.

No new Conan/package operation, dependency download, native build, cache modification, staging or commit. Completed Plan 01-01/01-02 implementation and closeout artifacts remain unchanged. Project-owned Conan stays 2.33.0; LLVM stays 23.1.2. Existing Linux Debug/Release/cached package evidence is preserved, not rerun.

## Platform impact

- Windows: Shared recipe uses standard Conan/CMake APIs. Native profile/lock/runtime validation independently pending; no Windows locks or host identities invented.
- Linux: Omarchy/Arch Debug/Release selections available; native Conan lock/setup work awaits the checkpoint decision. Ubuntu CI remains a distinct pending baseline.
- macOS future: Deferred. No custom platform-specific package infrastructure or SDK normalization introduced.

## Project-system impact packet

- Files affected: root Conan consumer and normal execution checkpoint/state/roadmap artifacts above.
- Manifest/build behavior changed: Compiler implementation dependency inputs only; `Veyr.toml`, `Veyr.lock` and `.veyr` untouched. Conan resolution/generation has not run.
- Targets affected: Future compiler-foundation LLVM and GoogleTest/GoogleMock integration; no CMake/compiler targets created.
- Cross-platform effect: Standard consumer available to both Tier 1 profiles; evidenced locks remain native-selection dependent.
- Compatibility/open questions: Windows native inputs and four-lock acceptance pending; external-provider subset behavior not yet checked.
- Owner review required before commit: yes. No commit requested for this work.

## Owner-approved available-Linux continuation — 2026-10-05T10:18:18Z

Owner selected option 1: continue Linux Debug/Release lock/setup work and shared Task 2 implementation, while deferring both Windows lock outputs/native checks until independent host inspection. This changes execution evidence scheduling only; all eight plans, requirement scope, Tier 1 acceptance and Windows candidate identities remain unchanged. No Windows or CI observation is inferred.

### Task 1 available-Linux results

Commands executed serially through the absolute project-owned Conan 2.33.0 executable with `CONAN_HOME=/home/m4rkens/.cache/veyr/conan/22e2b2472775` and working directory `/home/m4rkens/Projects/VeyrLang`:

1. `conan export conan/recipes/llvm --user=veyr --channel=m1` — exit 0. Unchanged recipe revision `aa4cad25b3148c3bdd22ed96fba04654`; no package build.
2. `conan lock create conanfile.py -pr:h conan/profiles/linux-x86_64-clang -pr:h conan/profiles/debug -pr:b conan/profiles/linux-x86_64-clang -pr:b conan/profiles/release -o:h llvm_provider=managed --lockfile= --lockfile-out=conan/locks/linux-x86_64-clang-Debug.lock` — exit 0. Conan warned about the unscoped root option; subsequent commands use explicit `&:llvm_provider` syntax. Fetched missing GoogleTest/CMake recipes, not package/source builds.
3. Same lock creation with Release host overlay, `-o:h &:llvm_provider=managed`, and Release output lock — exit 0. Same cached LLVM package identified.
4. Four `conan graph info conanfile.py` checks for system/custom × Debug/Release, explicit native host/build overlays, their managed native lock, `--no-remote --build=never --format=json` — exit 0 each. Conan accepted the managed superset locks; each graph omitted LLVM. Missing GoogleTest binary warnings were expected before setup, not proof of prepared package readiness. Ignored raw graphs are `.veyr-dev/logs/01-03-{system,custom}-{Debug,Release}-graph.json`.

Profile environment selected from current reviewed policy: `VEYR_PROFILE_OS=Linux`, `VEYR_C_COMPILER=/usr/bin/clang`, `VEYR_CPP_COMPILER=/usr/bin/clang++`, `VEYR_LINKER=/usr/bin/ld`, `VEYR_GCC_INSTALL_DIR=/usr/lib/gcc/x86_64-pc-linux-gnu/16`.

Conan-native lock entries (both configuration files intentionally share recipe revisions; profiles retain separate package/config identities):

- `gtest/1.18.0#e2507fc4efe2ec9e2db9e5c91eefab9f`.
- `llvm/23.1.2@veyr/m1#aa4cad25b3148c3bdd22ed96fba04654`.
- GoogleTest's Conan-owned build requirement `cmake/4.4.3#8b46ce9dbf1937af1c05f19579c1f105`, resolved from its upstream range and captured in the native locks. No parallel tool/package acquisition mechanism added.

Both lock creates reported LLVM package `a214d896e569f9b075f68e0b581ce595d09d80df#cd9a88c23979fe65c10c97b796943a64` from Cache. The original LLVM package manifest SHA256 checked before operations was `2eba146976c0f4460a0fb60fdf7363eb9c91a00b9a9926b47b893f0dd6cb297e`. No source/build/package action performed yet in Plan 01-03. Available-Linux Task 1 lock/subset work is complete; four-lock all-platform acceptance remains incomplete.

### Task 2 implementation and mocked checks

- Added `scripts/devtools/conan_ops.py`: reuses BuildSelection, validates external dedicated home/lock/profile/output inputs, uses absolute Conan with selected environment, and one exclusive marker beside the home to reject overlapping wrapper operations. Marker is wrapper state, not Conan package storage. No stale-marker repair, retry, custom graph, package identity or cache cleanup.
- Explicit setup invokes the existing CLI bootstrap/prerequisite callback under that guard, exports the narrow managed recipe, consumes an existing lock unchanged, installs with `--build=missing`, then runs `conan test` cache-only with `--build=never --no-remote` and matching profiles/runtime. `--init-lock` explicitly initializes only a missing native lock using Conan; existing locks are never rewritten automatically.
- Prepare validates an existing home/lock and runs only `conan install --no-remote --build=never`. Configure validates prerequisites without bootstrap; Plan 01-04 CMake files must exist before actual preparation/configure. It passes selected build/toolchain/Python paths in child-only environment and explicit provider cache inputs.
- `setup --tools-only` retains its early return; its existing bootstrap now shares the exclusive-home guard. It never reaches package operations. Doctor remains read-only and unchanged.
- Added mocked `test_conan_ops.py`; added ignore rules for generated root user presets and Python bytecode only. Conan inputs/locks and `.planning` stay trackable.
- Bounded consistency fix outside the plan's original `files_modified` list: changed only `test_plain_setup_is_deferred_until_dependency_plan_without_bootstrap` in `tests/development/test_doctor.py` to assert the now-approved dependency dispatch. The old assertion required Plan 01-03 to remain unimplemented, contradicting this task. All doctor/bootstrap production implementation is preserved.

Checks:

- New mocked suite initially 21/22 passed: the wrong-lock test fixture lacked the alternate lock and therefore correctly got a missing-lock error first. Created that fixture file; no production change or native retry. Rerun: 22/22 passed.
- Existing bootstrap/doctor suite rerun because CLI dispatch/guard and one stale test changed: 38/38 passed. These are mocked/unit checks, not repeated native acceptance or installation.
- Compile-only syntax checks passed for root consumer, CLI, Conan ops and both affected test files. `git diff --check` passed.

### First incomplete execution step

Task 2 native acceptance: explicit Linux Debug setup, then independent Release setup and prepared no-remote/no-build generation checks. No native consumer project configure until Plan 01-04. Stop on first non-zero native/package command. Existing CLI environment must be validated/reused, never reinstalled; existing LLVM package must be reused unless Conan genuinely identifies it as missing/incompatible. No cache purge or forced rebuild.

Current progress remains 2/8; no Plan 01-03 summary/closeout, Plan 01-04 start, commit, separate phase verification or phase advancement. Windows lock outputs/developer evidence, Windows CI and Ubuntu CI remain independently pending; macOS deferred.

Additional Task 2 files changed: `scripts/dev.py`, `scripts/devtools/conan_ops.py`, `tests/development/test_conan_ops.py`, the one stale assertion in `tests/development/test_doctor.py`, and `.gitignore`. Normal STATE/ROADMAP/checkpoint records updated; approved discussion/plan documents and prior implementation artifacts remain unchanged.

## Available-Linux native acceptance and closeout — 2026-10-05T10:28:28Z

The preceding saved checkpoint is historical; subsequent authorized native checks all exited 0. No non-zero native/package command, automatic retry, process conflict, cache purge or forced build occurred. Commands ran serially, streamed/logged output, and completed without a 60-second output-idle checkpoint. Temporary `/tmp` logging/check drivers were execution helpers only; no bootstrap/package framework or additional tracked tooling was introduced.

### Explicit setup commands and outputs

Working directory `/home/m4rkens/Projects/VeyrLang`:

```text
/usr/bin/python3 -B scripts/dev.py setup --profile linux-x86_64-clang --config Debug --environment linux-developer --conan-home /home/m4rkens/.cache/veyr/conan/22e2b2472775
/usr/bin/python3 -B scripts/dev.py setup --profile linux-x86_64-clang --config Release --environment linux-developer --conan-home /home/m4rkens/.cache/veyr/conan/22e2b2472775
```

- Debug start `2026-10-05T10:22:30.909317+00:00`, exit 0, 19.569s. Raw ignored log `.veyr-dev/logs/01-03-setup-Debug.log`.
- Release start `2026-10-05T10:23:40.095557+00:00`, exit 0, 20.924s. Raw ignored log `.veyr-dev/logs/01-03-setup-Release.log`.
- Existing project-owned Conan 2.33.0 environment validated/reused both times. Only CLI launch validation and isolated `pip check`, with `No broken requirements found`; no venv creation, pip install, upgrade, global pip/system Python modification or CLI reinstall.
- Current host prerequisites passed the full setup's read-only checks. Doctor production code unchanged, metadata-only Conan inspection retained.
- Actual child `conan install` used absolute executable, selected dedicated home, explicit native host/config and native Release build overlays, `&:llvm_provider=managed`, selected `--lockfile`, `--build=missing`, and selected `--output-folder`. It did not update native lock files.
- Conan built missing GoogleTest Debug/Release packages (static GoogleTest + GoogleMock) with Clang 22.1.8/C++23/libstdc++11. Debug ID/revision `77ee4a6c9da2ff6b1df8eac330ceb33aad0d1d69#3604fb7abde2c4493bc7897a104b3995`; Release `4c47d11dc631ceaef479f42595e1124ab519c480#f244293f6c361ee557e76d8fed0392f4`. GoogleTest's upstream recipe downloaded Conan-owned CMake 4.4.3 binary for Debug and reused it for Release. Its internal source build generator was Unix Makefiles; no future cold-host Make prerequisite review inferred. Consumer generators/LLVM probes use Ninja.
- Original LLVM package was `Cache`/`Already installed!` in each install and test graph, never source/package rebuilt. Same exact RREV/package ID/PREV as Plan 01-02.
- Each setup launched exactly one `conan test` for the existing LLVM test_package, explicit same host/build overlays and lock, `--no-remote --build=never`. LLVM native config/compile/link/run passed in Debug and Release: configure 0.5s each, four Ninja steps each, `LLVM 23.1.2: Support allocation/destruction passed`, native exit 0. Conan's standard test command recreated only ignored test_package consumer build directories; no package cache removal/rebuild.

### Cache-only prepared-generation checks

Used existing `select_inputs`/`make_selection` and implemented `prepare_dependencies` for Debug, then Release, through a temporary read-only input/check driver. The actual child command for each selection was:

```text
<absolute project-owned conan> install /home/m4rkens/Projects/VeyrLang/conanfile.py -pr:h <absolute linux profile> -pr:h <absolute debug-or-release overlay> -pr:b <absolute linux profile> -pr:b <absolute release overlay> -o:h &:llvm_provider=managed --lockfile=<selected native lock> --no-remote --build=never --output-folder=<selected build directory>
```

- Debug start `2026-10-05T10:25:26.825104+00:00`, exit 0, 0.474s; `.veyr-dev/logs/01-03-prepare-Debug.log`.
- Release start `2026-10-05T10:26:46.106595+00:00`, exit 0, 0.461s; `.veyr-dev/logs/01-03-prepare-Release.log`.
- Both reported existing LLVM and matching GoogleTest binaries from Cache/Already installed. No remote connection, recipe/source download or package build. Repeated compatible install/generation reused Conan identities.
- Standard toolchain, GTest config and Ninja preset output verified at `build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-5221f1471fc7/{Debug,Release}/generators`; no source-root `CMakeUserPresets.json`.
- Missing-cache no-acquisition/no-retry behavior is mocked-tested; no destructive actual missing-cache test or purge performed.
- No Veyr compiler project CMake command attempted. Configure dispatch and child-only path/provider inputs are fixture-tested, gated on Plan 01-04 files.

### Read-only post-command checks and preserved state

- Original LLVM manifest SHA256 unchanged: `2eba146976c0f4460a0fb60fdf7363eb9c91a00b9a9926b47b893f0dd6cb297e`.
- Both generated native lock contents/timestamps/revisions remain exactly the initial lock output; SHA256 each `b76a5f857ba40da4a59d2bdcd8690fba6d12d15d53bd3b0dc6aceb252585f30b`. These are recipe locks, not duplicated binary/runtime identities.
- Exclusive operation marker was released; Windows locks remain absent/pending.
- Existing Linux profile quote warnings persist; no profiles or prior LLVM implementation modified. Native paths-with-spaces and broader ABI surfaces unproved; Windows fixtures remain synthetic.
- Committed discussion, all eight PLAN files, doctor/baseline/profiles, and Plan 01-01/01-02 implementation/closeout artifacts remain unchanged.

### Closeout and owner-review boundary

Plan 01-03 Tasks 1–2 are complete for the owner-approved available Linux developer scope. Wrote normal `01-03-SUMMARY.md` and updated STATE/ROADMAP to 3/8, zero completed phases. Four-lock/Tier 1 acceptance remains incomplete: two Windows locks, native Windows developer evidence and independent Windows/Ubuntu CI observations are pending; macOS deferred. All requirements remain phase-wide pending.

Exact changed files (12):

- `.gitignore`
- `.planning/ROADMAP.md`
- `.planning/STATE.md`
- `.planning/phases/01-conan-and-compiler-build-foundation/01-03-EXECUTION.md`
- `.planning/phases/01-conan-and-compiler-build-foundation/01-03-SUMMARY.md`
- `conanfile.py`
- `conan/locks/linux-x86_64-clang-Debug.lock`
- `conan/locks/linux-x86_64-clang-Release.lock`
- `scripts/dev.py`
- `scripts/devtools/conan_ops.py`
- `tests/development/test_conan_ops.py`
- `tests/development/test_doctor.py` — only stale setup assertion.

No staging/commit, Plan 01-04 or later plan execution, separate phase verification or phase advancement. Recommended dedicated reviewed checkpoint message: `build: wire Conan dependency setup and cache-only preparation`. Stop for owner review; no automatic stage commit or next plan.

## Owner-authorized dedicated checkpoint — 2026-10-05T10:42:40Z

The owner reviewed the completed available-Linux execution/closeout and explicitly requested one commit with message `build: wire Conan dependency setup and cache-only preparation`. Include exactly the 12 files listed in the closeout above; exclude generated state, logs and unrelated future-plan files. This record and summary are included in that checkpoint; resolve the commit hash from Git history. Earlier no-commit statements describe their historical execution gates.

Only commit-status wording in ROADMAP, STATE, this execution record and SUMMARY was aligned with the explicit instruction. Reviewed implementation/configuration remains unchanged. No tests, package operations, LLVM/dependency rebuilds or Plan 01-04 execution occur in this commit step. Preserve 3/8 complete, zero completed phases, successful Linux Debug/Release setup and cache-only preparation, existing LLVM reuse and Conan-built GoogleTest. Windows locks/native evidence and Windows/Ubuntu CI remain independently pending; macOS deferred. Stop for owner review after commit; no phase advancement.
