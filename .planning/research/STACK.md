# Stack Research

**Domain:** Native compiler walking skeleton
**Researched:** 2026-10-03
**Method:** Inline canonical-document synthesis plus read-only official-source research
**Confidence:** High for architecture and interfaces; medium for untested recipe/toolchain compatibility

## Recommended Stack

| Technology | Version/direction | Purpose | Basis |
|------------|-------------------|---------|-------|
| C++ | C++23 | Compiler implementation | Canonical implementation baseline |
| LLVM | 23.1.2 | IR verification and native object emission | Fixed owner target; official release confirmed |
| Conan | 2.x; pin a tested exact release in Phase 1 | Third-party resolution, recipes, source builds, package/cache identity | D-064 and supported Conan integration |
| CMake + Ninja | Pin supported versions in Phase 1 | Veyr build graph and executor | Canonical baseline; no custom graph |
| Clang | clang/clang++ on Linux; clang-cl on Windows | Host compiler family | Independent reviewed platform profiles |
| Python + Git | Pin/document supported versions in Phase 1 | Thin developer orchestration and source control | Canonical developer prerequisites |

LLVM 23.1.2 exists upstream, including source releases. This confirms the requested backend target, not Conan recipe or local compiler compatibility. [Official LLVM release](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.2).

The checked ConanCenter `llvm-core` version index lists 19.1.7, 13.0.0, 12.0.0, and 11.1.0, not 23.1.2. Recheck the selected recipe source during Phase 1; if a suitable exact recipe is still absent, use the canonically permitted narrow project-owned recipe. Do not downgrade LLVM silently. [Recipe version index](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/llvm-core/config.yml).

### Supporting libraries

| Library | Initial treatment | Selection gate |
|---------|-------------------|----------------|
| GoogleTest/GoogleMock | Conan package; gtest 1.18.0 is a researched candidate | Pin exact version/options after compiler/runtime review |
| zlib | Include only when selected LLVM options require it | Exact version/revision belongs to reviewed Conan configuration |
| zstd | Include only when selected LLVM options require it | Same Conan-owned transitive graph |
| LibXml2 | Include only when selected LLVM build requires it | Do not enable features merely to match a broad SDK |

The checked GoogleTest recipe index includes 1.18.0; this is availability research, not an accepted dependency lock or tested compatibility result. [GoogleTest recipe index](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/gtest/config.yml).

## CMake integration

Start from `CMakeToolchain` plus `CMakeDeps`, using normal package discovery and imported targets. Conan's documentation describes the toolchain generator and its pairing with CMakeDeps. [CMakeToolchain](https://docs.conan.io/2/reference/tools/cmake/cmaketoolchain.html), [CMakeDeps](https://docs.conan.io/2/reference/tools/cmake/cmakedeps.html).

Retain `find_package(LLVM CONFIG)` at Veyr's CMake boundary. Inspect the selected recipe's config files, component targets, variables, and native link requirements before writing the plan. The checked existing recipe customizes LLVM's package configuration; copying upstream `LLVMConfig.cmake` assumptions into consumer code is not justified without that inspection. [Existing recipe](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/llvm-core/all/conanfile.py).

Conan also documents `CMakeConfigDeps`. Its existence is a Phase 1 compatibility question, not permission to replace the starter integration silently. Evaluate it only if the tested Conan version/recipe needs it. [CMakeConfigDeps](https://docs.conan.io/2/reference/tools/cmake/cmakeconfigdeps.html).

## Profiles, locks, and reuse

Use explicit reviewed host/build profiles for `linux-x86_64-clang` and `windows-x86_64-clangcl`. Conan profiles support platform settings; detected profiles are guesses rather than stable project policy. [Profile documentation](https://docs.conan.io/2/reference/config_files/profiles.html).

Use exact dependency references/options and Conan-native recipe revisions/locks where useful. Locks constrain dependency references; they do not replace compiler/runtime/profile compatibility or a successful native link. [Conan lockfiles](https://docs.conan.io/2/tutorial/versioning/lockfiles.html).

Permit deliberate source builds during setup when a compatible binary is absent. Repeated workflows reuse Conan-owned package state. Expose network/source-build permission explicitly, including a prepared-cache offline mode; ordinary CMake configuration must not hide downloads. Conan install already performs graph resolution, binary selection, source builds, and generator output. [Conan install](https://docs.conan.io/2/reference/commands/install.html).

Development Ninja presets must export `compile_commands.json`; CMake documents support for that export with Ninja. [Compile database documentation](https://cmake.org/cmake/help/latest/variable/CMAKE_EXPORT_COMPILE_COMMANDS.html).

## Alternatives considered

| Choice | Alternative | Treatment |
|--------|-------------|-----------|
| Conan-managed LLVM | Already installed system/custom LLVM | Explicit external discovery modes remain possible with version/ABI/component checks |
| Narrow Conan LLVM recipe when needed | Custom SDK acquisition/provisioning framework | Excluded by accepted architecture |
| Thin Python orchestration | Mandatory Bash/PowerShell implementation | Excluded as the only developer path; optional launchers must share behavior |
| Milestone-required LLVM components | Full toolchain/LLDB/JIT/LTO/every backend | Defer unnecessary components and optional dependency closure |

## Version compatibility and remaining research

Do not treat backend version as a requirement that the host Clang version must equal 23.1.2. Phase 1 must select and document a tested C++23 host compiler and platform SDK/runtime combination, exact Conan/CMake/Ninja/Python versions, LLVM recipe revision/options, and permitted Debug/Release combinations. Windows CRT compatibility can constrain mixed build types; Linux standard-library ABI must also be explicit. No tool installation, package resolution, or build was run during research.

## Platform impact

- Windows: clang-cl, SDK/CRT/linker details and Conan runtime settings require dedicated review; validate imported targets on PE/COFF without assuming Linux flags.
- Linux: clang/clang++, reviewed GNU or explicitly selected libc++ ABI, native links and compatible Conan cache reuse.
- macOS future: defer Apple compiler/SDK profiles and Mach-O linking; do not encode a two-platform-only package or target abstraction.

## Canonical sources

- `docs/design/DECISION_LOG.md`, D-064/D-065.
- `docs/tooling/CONAN_BUILD_ARCHITECTURE.md`.
- `docs/development/BUILD_POLICY.md`, `DEVELOPER_BOOTSTRAP.md`, `CROSS_PLATFORM_POLICY.md`.
- `gsd/GSD_FRESH_START_CONAN.md`.

All web checks are point-in-time observations, not durable package availability guarantees.
