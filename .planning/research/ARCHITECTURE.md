# Architecture Research

**Domain:** Native compiler walking skeleton
**Researched:** 2026-10-03
**Confidence:** High for canonical boundaries; medium for implementation details not yet designed

## System Overview

```text
Developer workflow: thin Python -> Conan 2 -> CMake -> Ninja -> Clang

Compiler data flow:
source management -> diagnostics + token/lexer -> parser -> minimal AST
 -> semantic smoke -> minimal lowering -> single-root context
 -> LLVM backend -> native object

veyrc orchestrates source-to-object, native linking, and user diagnostics.
The future veyr CLI sits above veyrc and is outside M1.
```

The root-module context is conceptually shared by compilation layers; Phase 7 integrates its durable identity rather than requiring syntax for a module declaration.

## Component Responsibilities

| Component | Owns | Must not depend on |
|-----------|------|--------------------|
| Source management | Source identity, retained text, byte spans, location lookup | Driver/build state, LLVM |
| Diagnostics | Severity/message/location and portable renderer | LLVM or mandatory terminal escape sequences |
| Token/lexer | Kind/span stream, explicit EOF, lexical recovery | Parser, AST, semantics |
| Parser | Profile-local structure and syntax diagnostics | LLVM |
| AST | Minimal source-linked fixed-entry/return representation | LLVM types |
| Semantic smoke | Literal/range and profile-shape validation | Package resolution, full ownership analyzer |
| Lowering | Validated constant-return form and explicit backend handoff | Speculative whole-language IR/effect system |
| Root context | One canonical module/source association | Imports or dependency resolver |
| LLVM backend | IR construction/verification, native object output | Manifest/registry orchestration |
| Driver | Phase orchestration, paths, artifact/link process, reporting | New core language semantics |

## Recommended Project Structure

These are candidate implementation locations to refine in authorized phase planning; no directories/files are created by initialization.

```text
include/veyr/        canonical owned header root
src/                separated compiler layers and driver
tests/              layer checks and native integration harness
cmake/              Veyr target/build policy
conan/              reviewed profiles; narrow LLVM recipe only if needed
scripts/            thin portable developer orchestration
.planning/          fresh requirements, research, roadmap and later stage records
```

CMake owns compiler targets. Conan owns the third-party graph and package state. Generated build/generator/cache output stays untracked. Future `Veyr.toml` targets and lockfiles do not feed the C++ implementation build.

## Architectural Patterns

1. **Span-based source relationships:** Preserve identity and locations through tokens, parse/AST, diagnostics, and lowering. SourceManager owns text so lexemes need not be duplicated into tokens. Buffer lifetime and span validation must be settled in Phase 2.
2. **Explicit recovered results:** Lexing returns recovered tokens plus diagnostics; downstream phases must define when errors prevent native output. A recovered token stream is not successful compilation.
3. **Minimal milestone contracts:** A fixed-entry constant-return representation is sufficient for M1; general HIR/MIR layering remains OQ-028 until richer probes require it.
4. **Backend isolation:** LLVM types remain inside the backend and deliberate integration boundary, preserving diagnostic/source testability without LLVM coupling.
5. **Narrow platform adapters:** Paths, native linking, process status and terminal behavior have explicit platform contracts. Do not build a universal OS framework to test one executable.

## Integration Points

- Conan-generated toolchain/configuration → target-oriented CMake. Validate the exact recipe's exported LLVM components/configuration before implementing consumer assumptions.
- Lowered smoke representation → LLVM native target setup, IR verifier, object emitter. Native linking belongs to explicit driver orchestration, not the source language.
- Driver → linker process: executable and argument vector, useful captured failure diagnostics, platform-specific artifact conventions. Choose the actual command contract during Phase 9 planning.
- Compiler core → tests: small positive/negative fixtures scoped to the M1 profile; Phase 10 runs resulting executables and compares observable exit status on each Tier 1 platform.

## Build Order Implications

Preserve the canonical ten phases. Keep source/diagnostics foundational; Phase 3's profile is a hard prerequisite to syntax-dependent work. Phase 4 can return a minimal parse structure; Phase 5 consolidates AST and semantic smoke. Phase 6 lowering stays small. Phase 7 integrates root identity. Phase 8 accepts lowered input; Phase 9 wires compilation/linking; Phase 10 proves the integrated path.

## Scaling Considerations

The immediate bottleneck is first LLVM package build cost. Reuse Conan-compatible packages and restrict optional components. Future source/module scale and incremental builds are deferred; M1 must not freeze module artifact formats or package cache semantics.

## Platform impact

- Windows: source display paths, clang-cl/runtime/native-link setup, process argument quoting, `.exe`/COFF behavior and error reporting require explicit contracts.
- Linux: clang/ABI configuration, source path casing, ELF/native-link behavior and child exit status require explicit contracts.
- macOS future: isolate target format and linker/platform policies so Mach-O, Apple SDKs and process conventions can be added later; no support claimed.

## Sources

- Canonical architecture in `AGENTS.md`, `docs/design/VISION.md`, and `docs/design/MODULE_SYSTEM.md`.
- Scope/order in `docs/design/milestones/MILESTONE_1.md`.
- Build ownership in `docs/tooling/CONAN_BUILD_ARCHITECTURE.md`.
- Platform boundaries in `docs/development/CROSS_PLATFORM_POLICY.md`.
- [LLVM CMake documentation](https://llvm.org/docs/CMake.html): component and target build configuration, to recheck against the pinned release during Phase 1.
