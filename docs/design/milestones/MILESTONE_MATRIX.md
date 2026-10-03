# Milestone Feature Pressure Matrix

**Status:** planning aid, not a contract.

This matrix explains why features exist. It should be updated when a project proves that a different feature ordering is needed.

| Feature family | First serious pressure | Example projects |
|---|---|---|
| Source spans | M1-M2 | compiler diagnostics, user parsers |
| Cross-platform path handling | M0-M12 | source manager, diagnostics, project cache, module roots |
| Windows/Linux CI gate | M0-M7, recurring | compiler smoke, driver tests, platform-labeled tests |
| Platform-native library suffixes | M21-M23 | plugins, dynamic library loader, GPU loaders |
| macOS future readiness | M49-M50 | `.dylib`, frameworks, Metal, toolchain discovery |
| Platform modules | M51 | `std.os`, `std.fs`, `std.process`, `std.dylib`, `std.net` |
| Lexical recovery | M3 | M1 lexer, IDE diagnostics |
| Minimal AST | M4 | smoke parser, mini language later |
| Result/Option | M13-M14 | config parser, file IO, Vulkan wrappers |
| Pattern matching | M14 | parser ASTs, rule engines |
| Strings/views/slices | M11-M12 | grep/wc, HTTP, logs, bindgen |
| Byte buffers | M12/M25 | framebuffer, binary protocols, shaders |
| Drop/defer | M16 | resource wrappers, Vulkan handles |
| Unsafe/raw pointers | M17 | allocators, C/Vulkan/OpenGL APIs |
| Modules | M18 | libraries, plugin host, stdlib split |
| Project manifest | M20 | `veyr build`, workspaces, libraries |
| C ABI | M21 | C wrappers, plugins, GPU bindings |
| Dynamic loading | M22 | OpenGL/Vulkan loaders, plugins |
| Native window handles | M24 | Vulkan surface, OpenGL context |
| Function pointers | M21/M26 | callbacks, GL/Vulkan loaders |
| Threads/atomics | M32 | file hasher, HTTP, job system |
| Graph data structures | M37-M38 | AST/CFG/DDG export, vulnerability scanner |
| Maps/sets | M15/M35 | todo store, rule engine, logs |
| Newtypes/units | M41 | simracing calibration, binary protocols |
| Formatter | M39 | consistent syntax stabilization |
| LSP diagnostics | M39 | editor feedback, recovered parse trees |
| Bindgen | M40 | C APIs, GPU APIs, native system libraries |
| Package lockfile | M45 | repeatable builds, dependency graph |
| No-std/freestanding | M46-M48 | embedded, WASM, kernel research |

## Use rule

When adding a feature proposal, include a sentence like:

```text
This is required by PROJECT_X because without FEATURE_Y the project cannot express Z safely/predictably.
```

If that sentence is weak, the feature should stay in `OPEN_QUESTIONS.md` or a design probe.
