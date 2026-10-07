
# Veyr Developer Bootstrap

**Status:** Implemented developer-command contract; native acceptance currently limited to Omarchy/Arch Linux. Independent Windows and CI evidence remains pending.

## 1. Goal

Developer bootstrap should make the normal Veyr workflow predictable on Windows and Linux without
turning the repository into a package manager.

The desired stack is:

```text
thin Python wrapper
    -> Conan 2
    -> CMake
    -> Ninja
    -> Clang
```

## 2. Host prerequisites

The initial developer contract should stay small:

- Python;
- Conan 2.33.0 in the project-owned tool environment described below;
- CMake;
- Ninja;
- Clang;
- Git;
- platform SDK/runtime components genuinely required by the selected toolchain.

Exact prerequisite policy is tracked in `scripts/devtools/baseline.json`. Selected
prerequisites are not a general ABI/package-readiness claim.

| Environment | Exact tools / native identity | Evidence |
|---|---|---|
| Omarchy/Arch x86_64 developer | Python 3.14.7; Conan 2.33.0; CMake 4.4.3; Ninja 1.13.2; Clang 22.1.8; Git 2.55.0; GCC/libstdc++ 16.2.1; glibc 2.44; Binutils 2.47; Omarchy 4.0.4 | Read-only doctor 22/22 and separate native Debug/Release foundation acceptance recorded |
| Windows x86_64 developer candidate | Python 3.13.15; Conan 2.33.0; CMake 3.31.8; Ninja 1.13.2; Clang 20.1.8; Git 2.55.0.windows.5; VS candidate 17.14.37710.0 and SDK candidate 10.0.26100.0 | Actual VC tools/CRT/linker/paths and native inspection pending; candidate unchanged |
| Ubuntu 24.04 x86_64 CI | Reviewed candidate: Python 3.12.3; CMake 3.31.6; Ninja 1.13.2; Clang 18.1.3; explicit GCC-13/libstdc++11; glibc 2.39; Binutils 2.42 | Run 37643695998 observations passed; CLI/bootstrap/native/package acceptance unverified; execution blocked |
| Windows 2022 x86_64 CI | Reviewed candidate: Python 3.12.10; CMake 3.31.6; Ninja 1.13.2; standalone Clang 20.1.8; VS 17.14.37710.0; VC 14.44.35207; CRT 14.44.35220.0; SDK 10.0.26100.0; link.exe 14.44.35229.0 | Run 37643695998 observations passed; Windows developer evidence, locks and CI acceptance independently pending; execution blocked |

Git versions in the table are recorded observations, not exact prerequisites or
ABI/package pins. Git must remain available for repository queries; actual
version/path are reported by doctor. No existing minimum Git version requirement
is defined. Hosted Git drift alone does not invalidate a compiler/runtime baseline.

The Linux profile receives the reviewed Clang major from the wrapper (Omarchy 22,
Ubuntu CI 18), with explicit per-environment GCC/compiler/linker paths. Windows CI
selects `C:\Program Files\LLVM\bin\clang-cl.exe` for both C and C++ after
DevShell provides VC/SDK/linker inputs. Bundled LLVM 19.1.5 is not selected by PATH.
The workflow execution flag remains false; reviewing candidates does not enable
CI tools-only bootstrap, package setup or native work. See [CI policy](CI_MATRIX_POLICY.md).

LLVM backend 23.1.2 is distinct from the host Clang version. Linux's observed
static optimized LLVM Support compatibility covers this selected host and the
tested surfaces only. Windows Debug/Release CRT/iterator compatibility must be
demonstrated independently.

### Project-owned Conan CLI bootstrap

Plan 01-01 Task 3 owns the CLI prerequisite. From the repository root, using the
reviewed base Python interpreter, run:

```text
python scripts/dev.py setup --tools-only
```

This command creates `.veyr-dev/tools/conan-2.33.0` only when missing. It uses
standard `venv`, then that venv's Python with `-I -m pip --isolated
--require-virtualenv --disable-pip-version-check install --index-url
https://pypi.org/simple --no-cache-dir --only-binary=:all: conan==2.33.0`.
`PIP_CONFIG_FILE` is set to the platform null device. System Python packages,
ambient global pip state and unrelated system repositories are untouched. Python
runtime dependencies of the Conan CLI are installed only inside this venv; no
separate resolver, registry, lock format or provenance subsystem is introduced.
If a required compatible wheel is unavailable, installation fails and stops;
there is no automatic source-build fallback or retry.

Setup validates the isolated venv, its reviewed Python identity and exact Conan
package metadata, then launches the absolute Conan executable with `--version`
and runs isolated `pip check`. This actual CLI launch is setup's responsibility:
Conan may initialize or migrate metadata in the selected dedicated `CONAN_HOME`
even for `--version`. A valid existing environment is reused:
no `pip install`, upgrade or venv recreation. A broken, non-isolated or mismatched
environment fails with expected/actual details. The owner may explicitly remove
**only** `.veyr-dev/tools/conan-2.33.0` and rerun tools-only setup; the wrapper never
deletes or repairs it automatically. Failed installation leaves any partial
environment for inspection. A changed base Python or relocated checkout may
require this same explicit recreation path.

The absolute executable is exposed as `BuildSelection.conan_executable`:

- Linux: `<checkout>/.veyr-dev/tools/conan-2.33.0/bin/conan`.
- Windows: `<checkout>/.veyr-dev/tools/conan-2.33.0/Scripts/conan.exe`.

Every Conan invocation uses that absolute executable and the selected dedicated
external `CONAN_HOME`. No shell activation or persistent PATH change is required;
an unrelated Conan on PATH is ignored. `doctor` inspects only this project-owned
installation. It checks the expected executable exists and validates venv
structure, isolation and Python identity, then reads the exact installed Conan
version and distribution location using `importlib.metadata` through that venv's
Python with `-I -B`. It never executes the Conan executable or imports the Conan
CLI/API. Its metadata probe does not receive `CONAN_HOME`; doctor neither needs
writable Conan home state nor creates, modifies, migrates or initializes it.
Doctor never creates or repairs the tool environment. CLI launch validation
remains exclusively in explicit setup, including validated reuse.

`--tools-only` stops before Conan dependency resolution, package installation,
recipe export, lock generation, source acquisition or builds. Success means the
CLI is installed and internally consistent, not that all host prerequisites,
native ABI compatibility or LLVM/package readiness are accepted.

Plan 01-03 owns the implemented Conan dependency/package setup. Full `setup`
ensures this CLI through the same bootstrap function before dependency work;
`--tools-only` remains available. Ordinary commands require explicit setup and
do not bootstrap tools implicitly. Build may configure, test may build, and all
includes setup. Equivalent work is reused within one composed invocation.

The Linux developer baseline is the observed Omarchy/Arch x86_64 host
with Python 3.14.7. Windows developer evidence remains independently pending.
Ubuntu/Windows CI candidate identities are independently reviewed from run
37643695998; CI prerequisite/native/package acceptance and execution remain gated.
These environments cannot inherit Omarchy observations. Implemented Windows path handling and mocked tests
are not native Windows evidence. macOS remains deferred. A real installation is
an execution action requiring owner authorization; mocked checks do not install
Conan or establish prerequisite acceptance.

## 3. Wrapper responsibilities

`scripts/dev.py` exposes these operations on native Windows/Linux x86_64:

```text
setup
doctor
configure
build
test
verify
all
clean
```

From the checkout root with the reviewed base Python (on this Linux host,
`/usr/bin/python3`), Debug is the default and Release is explicit:

```text
python scripts/dev.py setup --tools-only
python scripts/dev.py doctor --json
python scripts/dev.py setup --config Debug
python scripts/dev.py configure --config Debug
python scripts/dev.py build --config Debug
python scripts/dev.py test --config Debug
python scripts/dev.py verify --config Debug
python scripts/dev.py all --config Release
python scripts/dev.py clean --config Debug
```

Choose `--profile linux-x86_64-clang` or `--profile windows-x86_64-clangcl`,
and the independent `--environment linux-developer`, `windows-developer`,
`ubuntu-ci` or `windows-ci`. Missing/unreviewed identity fails before package
work; these flags do not bypass baseline review. Both host and build use the
selected native profile; host adds `conan/profiles/debug` or `release`, build
always adds `release`. `--build-profile` currently accepts only that same
native profile. Cross compilation is deferred.

Explicit `setup`/`all` may download and source-build missing dependencies through
Conan (`--build=missing`). Configure prepares only from the tracked lock/cache
(`--no-remote --build=never`); build may configure and test may build without
setup. Verify requires prepared native outputs and checks the compile database,
include-policy registration and CTest. Run `all` once for the composed path;
do not follow it with duplicate build/test/verify commands. Clean removes only
the selected output beneath `build/` and preserves sibling selections,
`.veyr-dev/tools/conan-2.33.0` and the entire Conan package home.

The CMake presets are `linux-x86_64-clang-Debug`,
`linux-x86_64-clang-Release`, `windows-x86_64-clangcl-Debug` and
`windows-x86_64-clangcl-Release`, selected by the wrapper with its structured
environment inputs; ordinary developer use goes through `scripts/dev.py`.
`compile_commands.json` is generated in the printed selected build directory.

The wrapper may:

- validate host tools;
- bootstrap the exact Conan CLI through explicit setup using standard venv/pip;
- select tracked Conan/CMake profiles;
- set the intended Conan home/configuration;
- invoke Conan;
- invoke CMake/Ninja/CTest;
- provide concise human-readable diagnostics;
- optionally provide stable machine-readable `doctor --json` output when useful.

The wrapper must not:

- solve third-party dependency graphs itself;
- maintain its own third-party source registry;
- implement a binary package cache;
- normalize arbitrary LLVM SDK archives;
- duplicate Conan lock/package identity semantics.

## 4. Conan configuration

Use explicit project-reviewed Windows/Linux profiles.

Do not accept `conan profile detect` output as permanent canonical configuration without review.

Use Conan-native package IDs/revisions and lockfiles where useful. Keep compiler implementation
dependency locking separate from the future language-level `Veyr.lock`.

Tracked `conan/locks/<profile>-Debug.lock` and `-Release.lock` fix recipe
resolution, not native ABI or binary-package compatibility. Both Linux locks
exist; Windows locks are independently pending. Existing locks are never
updated automatically. `setup --init-lock` creates a missing lock only during
an explicitly approved initialization step, followed by owner review; CI does
not initialize or modify missing locks automatically.

The normal workflow should make network/build behavior visible to the developer. It is acceptable
for a missing binary to trigger a deliberate source build when the selected workflow permits it.

## 5. LLVM

Initial LLVM target: 23.1.2.

Consume LLVM through Conan.

If a suitable exact recipe does not exist in the selected recipe source, use a narrow
project-owned LLVM recipe.

A fresh LLVM build may be expensive and quiet for long periods. This is expected. Do not work
around that cost by growing a bespoke SDK acquisition/provisioning subsystem.

Reuse compatible Conan packages/cache in subsequent local and CI runs.

`--llvm-provider managed` is the default Conan package path. Explicit `system`
or `custom` requires `--llvm-dir <directory-containing-LLVMConfig.cmake>` and
the same exact LLVM 23.1.2 configuration/runtime/target/header guards. There is
no fallback, archive normalization or implicit external-provider acquisition.
GoogleTest/GoogleMock remains Conan-owned in each provider mode.

## 6. Local state

Generated developer state remains untracked.

The project-owned CLI environment is `.veyr-dev/tools/conan-2.33.0`. Conan's
dedicated external package cache/home remains Conan-owned and must not be mirrored
into a second Veyr dependency store. Neither location is tracked. This tool
environment is separate from Conan's C/C++ package storage and future `Veyr.lock`.

Normal `clean` removes only the selected ordinary build output. It must preserve
the Conan tool environment and Conan package home/cache, including other build
configurations. Destructive tool-environment removal or package/cache purging
must be explicit and separate.

The dedicated external home defaults to `$XDG_CACHE_HOME/veyr/conan/<project-key>`
(or `~/.cache/veyr/conan/<project-key>`) on Linux and
`%LOCALAPPDATA%/Veyr/conan/<project-key>` on Windows. The key derives from the
resolved Git common directory; worktrees share the project's Conan home.
`--conan-home <absolute-external-directory>` overrides it. Shared default
`~/.conan2` and homes inside the checkout are rejected. Mutating local operations
are guarded against concurrent use; CI uses separate job-owned homes.

## 7. Cross-platform behavior

Windows and Linux expose equivalent high-level operations while using independent profiles and
platform-specific toolchain details.

Do not hardcode POSIX paths into Windows logic or Windows SDK assumptions into Linux logic.

macOS remains future Tier 2 and should not be structurally blocked.

Platform impact:
- Windows: Standard venv `Scripts/python.exe` and `Scripts/conan.exe`, structured
  process arguments and independent reviewed host/CI identities; native evidence pending.
- Linux: Standard venv `bin/python` and `bin/conan`, Omarchy developer candidate
  separate from Ubuntu CI; no system-package or persistent PATH changes.
- macOS future: Standard venv/Conan boundaries remain usable; no Darwin support
  or evidence is claimed at this checkpoint.

## 8. CI

The local draft `.github/workflows/build-foundation.yml` covers Ubuntu 24.04 and
Windows 2022 × Debug/Release, with one active matrix job at a time. It uses
shared commands/dependency policy with independently reviewed CI expectations:

```text
observe installed runner identity
-> require independent CI policy review
-> select exact reviewed base Python / isolated CMake and Ninja
-> setup --tools-only
-> read-only doctor and require tracked lock
-> optional Conan cache restore
-> explicit setup
-> configure -> build -> test -> verify
-> foundation prepared and offline task checks
```

The initial draft stops at pending CI identity before tool/package setup. Raw
runner tool/VS observations are inputs for review, not accepted CRT/SDK/ABI
evidence. Host Clang/GCC or VS/SDK must match the independently reviewed
installed native environment; mismatches stop rather than provisioning or
accepting drift. If actual tools require another Conan profile or changes to
the fixed compiler-major policy, obtain a bounded ownership correction first.

Only Conan's job-owned home is cached with full input/runner identity, and
`cold_build: true` skips restore/save for a fresh source-build path. No CLI venv,
build directory or downloaded backend SDK is cached/published. See
[CI_MATRIX_POLICY.md](CI_MATRIX_POLICY.md) for action pins, run authorization,
resource limits and required evidence. A locally checked YAML file is not a
passing CI run.

## 9. Long-running operations

LLVM/package builds can legitimately run for a long time with little output.

The ~60-second threshold is a liveness-inspection point only. Do not automatically terminate a
healthy build merely because it is quiet.

Stop on real failure, owner interruption, invariant violation, process conflict, or a deliberate
hard timeout.

## 10. Relationship to project build/package tooling

Conan is only for building the Veyr implementation and its native third-party dependencies.

The future user-facing `veyr` package/build system and `Veyr.toml`/`Veyr.lock` ecosystem remain
separate product features.
