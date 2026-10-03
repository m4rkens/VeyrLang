# Cross-Platform Policy

**Status:** required development policy  
**Tier 1:** Windows + Linux  
**Tier 2:** macOS future bring-up  
**Tier 3:** WASM/no-std/embedded/freestanding research

Veyr is intended to be a native systems/application language. That means the compiler, tooling, project system, standard-library design, and validation projects must not become accidentally single-platform.

The mandatory early target is **Windows and Linux parity**. macOS support is not required to be implemented in the earliest milestones, but design and implementation decisions must avoid making macOS support structurally difficult later.

## 1. Platform tier policy

### Tier 1 — mandatory from the beginning

Tier 1 platforms are required to stay viable during normal development:

```text
Windows x86_64
Linux x86_64
```

A phase that modifies build logic, paths, process execution, filesystem behavior, dynamic loading, native linking, terminals, threading, networking, windowing, or platform handles must state how it remains compatible with both Windows and Linux.

### Tier 2 — future bring-up, must not be blocked

Tier 2 platforms are not required to pass every early test, but architecture must leave room for them:

```text
macOS aarch64
macOS x86_64
```

Avoid decisions that assume:

- PE/COFF or ELF only;
- `.dll` or `.so` only;
- Win32 or Linux/POSIX handles only;
- Linux-only filesystem conventions;
- Windows-only process or terminal behavior;
- Vulkan/OpenGL surface assumptions that cannot map to macOS/Metal later.

### Tier 3 — research targets

Research targets are intentionally deferred:

```text
WASM
no-std
embedded/HID
freestanding/kernel-like targets
```

Do not optimize the core language around these before the normal desktop systems path exists, but keep low-level design choices explicit enough that these targets remain researchable.

## 2. Mandatory platform-impact note

Every implementation plan that touches platform-sensitive areas must include a short platform-impact note:

```text
Platform impact:
- Windows: ...
- Linux: ...
- macOS future: no blocking assumption / deferred detail ...
```

This is required for phases involving:

- CMake presets/toolchain files;
- process execution and exit codes;
- command-line argument handling;
- filesystem paths and path normalization;
- source-file discovery;
- dynamic library loading;
- native library linking;
- terminal colors/diagnostics;
- threads, atomics, mutexes, condition variables;
- sockets/networking;
- window handles and event loops;
- graphics API surfaces;
- generated bindings;
- package cache layout;
- SDK/dependency bootstrap behavior;
- generated machine-local evidence;
- test runner behavior.

## 3. Build-system constraints

The C++ compiler implementation must not depend on a single shell or operating system.

Required:

- CMake + Ninja as the portable baseline;
- no Bash-only developer workflow without an equivalent Windows entry point;
- no PowerShell-only developer workflow without an equivalent Linux entry point;
- platform-specific wrappers are allowed when they expose the same logical setup/build/test contract and share canonical manifest/policy data;
- no hardcoded absolute user paths;
- no hardcoded path separators;
- no assumptions about case-sensitive or case-insensitive filesystems;
- no generated files outside configured build/cache directories;
- platform-specific build behavior must be explicit and documented.

Preferred:

- CMake functions for build checks where possible;
- Python helper scripts only if they are small, portable, and documented;
- `std::filesystem` for C++ path manipulation;
- CTest labels for platform-specific tests.

## 4. Path and filesystem rules

Veyr tools must treat paths as structured paths, not stringly-typed slash blobs.

Do not assume:

- `/` as the only separator;
- drive-less paths;
- case-sensitive file lookup;
- UTF-8 console behavior on Windows;
- symlink behavior availability;
- executable bit semantics on Windows.

Source identity and diagnostics should preserve the spelling used by the user where useful, while internal path normalization should be deterministic and platform-aware.

## 5. Dynamic library and native-linking rules

Dynamic loading and native linking must be designed around platform differences:

| Platform | Shared library | Static library | Dynamic loading |
|---|---|---|---|
| Windows | `.dll` + import `.lib` | `.lib` | `LoadLibrary` / `GetProcAddress` |
| Linux | `.so` | `.a` | `dlopen` / `dlsym` |
| macOS future | `.dylib` / framework | `.a` | `dlopen` / `dlsym` / framework loading |

The language/project system must not hardcode one convention as universal.

`Veyr.toml` should support platform-specific native libraries, link directories, frameworks, and generated bindings.

## 6. Standard-library/module direction

Platform-sensitive APIs should be isolated behind explicit modules.

Examples:

```text
std.os
std.fs
std.process
std.dylib
std.net
std.thread
std.window
std.gpu.surface
```

Platform modules may have per-platform implementations, but user-facing APIs should expose common concepts only when the behavior is genuinely portable. Do not pretend incompatible OS behavior is identical.

## 7. Test policy

Default tests should be platform-independent. Platform-specific tests must be named and gated clearly.

Examples:

```text
SourceManagerPathTest.PortableRelativePath
DynamicLibraryTest.WindowsLoadsDll
DynamicLibraryTest.LinuxLoadsSo
WindowingTest.RequiresNativeDisplay
```

Early CI direction:

```text
windows-latest
ubuntu-latest
```

Future CI direction:

```text
macos-latest
```

macOS CI is not mandatory in the first clean restart, but decisions should not make it impossible.

## 8. Veyr project-system implications

`Veyr.toml` must eventually support platform-specific target settings:

```toml
[platform.'cfg(os = "windows")'.native]
libs = ["user32", "kernel32"]

[platform.'cfg(os = "linux")'.native]
libs = ["dl", "pthread"]

[platform.'cfg(os = "macos")'.native]
frameworks = ["Cocoa", "Metal", "QuartzCore"]
libs = []
```

Platform conditionals are for build/module selection, not a textual preprocessor. They must not become a way to hide unrelated syntax or semantics.

## 9. Graphics/API implications

The graphics track must treat platform surfaces and GPU backends separately:

```text
windowing
-> native window handle
-> API-specific surface/context
-> backend wrapper
-> cross-API abstraction
```

OpenGL and Vulkan can be the first active GPU probes. DX12 is Windows-specific. Metal is future macOS-specific. A cross-API layer must not be designed before enough backend-specific probes exist.

## 11. Agent rules

Agents must not:

- introduce Linux-only scripts as mandatory project workflow;
- introduce Windows-only scripts as mandatory project workflow;
- hardcode one executable suffix/library suffix convention;
- assume `fork`, POSIX signals, Win32 handles, or `/proc` without platform gating;
- mark cross-platform-sensitive work done without Windows/Linux consideration;
- claim macOS support is implemented unless it is actually built/tested.

Agents must:

- preserve Windows/Linux viability;
- label macOS assumptions as future/deferred;
- add platform-impact notes to plans touching platform-sensitive areas;
- prefer small platform abstraction probes over large universal APIs.



## 10. Canonical host/target identities and capabilities

Cross-platform build/bootstrap code must use the canonical triplet policy in `docs/development/PLATFORM_TRIPLES.md`. Host and target are separate concepts even while native builds dominate early development.

Platform-sensitive implementation should prefer capability-oriented adapters over broad OS conditionals outside platform modules. Typical boundaries include filesystem, process, terminal, dynamic-library, socket, and native-handle behavior. OS-specific checks belong inside those adapters.

Machine-local cache/evidence/build identities must not mix incompatible host/target/ABI/profile state.

## 12. Developer bootstrap parity

Managed SDK/bootstrap work must follow `docs/development/DEVELOPER_BOOTSTRAP.md`. Windows and Linux may use different native archive formats or helper commands, but setup/doctor/configure/build/test semantics, provenance guarantees, cache isolation, and failure behavior must remain equivalent. macOS-specific details may be deferred without introducing a Windows/Linux-only architecture.
