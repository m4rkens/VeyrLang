
# Veyr Developer Bootstrap

**Status:** Required starter direction

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

Exact supported versions belong to fresh GSD planning and CI policy.

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

Plan 01-03 retains Conan-owned dependency/package setup. Its full `setup` command
will automatically ensure this CLI through the same bootstrap function before
dependency work; `--tools-only` remains available. Until that later implementation,
plain `setup` fails with tools-only guidance. Ordinary commands require explicit
setup and do not bootstrap tools implicitly; build may configure, test may build,
and all includes setup under the later command-composition plan.

The Linux developer baseline candidate is the observed Omarchy/Arch x86_64 host
with Python 3.14.7. Windows developer evidence, Ubuntu Linux CI identities and
Windows CI identities remain independently pending; these environments cannot
inherit Omarchy observations. Implemented Windows path handling and mocked tests
are not native Windows evidence. macOS remains deferred. A real installation is
an execution action requiring owner authorization; mocked checks do not install
Conan or establish prerequisite acceptance.

## 3. Wrapper responsibilities

Repository-owned Python tooling may expose equivalent developer operations such as:

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

CI should reuse the same tracked Conan profiles/configuration used by developers.

Typical flow:

```text
validate tools
-> conan install / package resolution
-> cmake configure
-> build with Ninja
-> ctest
```

Cache restoration is an optimization. A clean environment must remain buildable.

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
