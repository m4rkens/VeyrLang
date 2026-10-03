# Project Research Summary

**Project:** Veyr — M1 Compiler Walking Skeleton
**Domain:** Native compiler and developer build foundation
**Researched:** 2026-10-03
**Confidence:** High for canonical scope; medium for untested platform/recipe compatibility

## Executive Summary

The repository establishes a small first compiler milestone, not a general language launch. P01 forces one root module, fixed entry, single integer-literal return, native executable and observable exit status. Canonical documents already determine the language direction and workflow boundaries; market-style feature expansion would contradict that scope.

Use thin Python orchestration over Conan 2, CMake, Ninja and Clang. Conan owns third-party resolution, source builds, transitive dependencies, package/cache state and native locking. LLVM 23.1.2 is the fixed backend target. Official release availability is confirmed; the checked ConanCenter recipe version list lacks this version, so Phase 1 must recheck and prepare a narrow Conan recipe if still required.

Initialization does not select exact smoke spelling, literal range, host-tool versions, ABI/runtime combinations, or final language semantics. Those belong to authorized phase discussion/planning and review. Every active requirement remains unimplemented and unverified.

## Key Findings

### Recommended Stack

See [STACK.md](STACK.md). C++23, LLVM 23.1.2, Conan 2, CMake/Ninja, Clang, GoogleTest/GoogleMock and thin Python orchestration follow accepted directions. Existing system/custom LLVM modes remain explicit compatibility-checked discovery choices; managed acquisition remains Conan-owned.

### Expected Features

See [FEATURES.md](FEATURES.md). M1 table stakes are source/spans/diagnostics, the reviewed smoke profile, lexer recovery, parser/AST, semantic constant validation, minimal lowering, one-root identity, LLVM emission, driver/native linking, and a portable exit-code proof. Future language and product probes are deferred.

### Architecture Approach

See [ARCHITECTURE.md](ARCHITECTURE.md). Separate compiler layers and keep LLVM out of foundational source/diagnostic/lexer code. `veyrc` orchestrates the pipeline and native link; future `veyr` orchestration and `Veyr.toml` parsing are outside M1. Preserve source/module relationships without implementing a general module system.

### Critical Pitfalls

See [PITFALLS.md](PITFALLS.md): importing old evidence, growing custom provisioning, freezing final syntax accidentally, mistaking recovered tokens for successful compilation, and claiming Windows parity from Linux-only evidence. Quiet LLVM builds require liveness supervision rather than automatic termination.

## Implications for Roadmap

| Phase | Deliverable | Ordering reason |
|-------|-------------|-----------------|
| 1 | Build/developer foundation | Needed for all compiler work; stay bounded to normal Conan integration |
| 2 | Source identity/spans/diagnostics | Location and lifetime substrate for every later layer |
| 3 | Reviewed smoke profile, token model, lexer | Hard syntax gate before any dependent implementation |
| 4 | Profile parser | Consume recovered tokens under explicit error rules |
| 5 | Minimal AST and semantic smoke | Preserve structure and validate return constant/range |
| 6 | Minimal lowering | Explicit validated backend input |
| 7 | Single root-module context | Integrate canonical identity without imports/graph |
| 8 | LLVM backend smoke | Emit native object from validated lowered form |
| 9 | `veyrc` driver smoke | Compile/link using explicit platform process contracts |
| 10 | End-to-end native exit-code proof | Demonstrate full pipeline and negative cases on both Tier 1 platforms |

This is the canonical M1 breakdown, not ten general-language feature slices. The broader milestone strategy remains deferred pressure and sequencing guidance.

### Research Flags

- Phase 1: recipe availability and narrow fallback, tool versions, Conan generators, conditional dependencies, profiles, runtime and Debug/Release combinations, offline/cache reuse.
- Phase 2: source text lifetime, source identity normalization, span bounds and line/column presentation on both OSes.
- Phase 3: exact provisional grammar/tokens, whitespace/recovery details, numeric semantic/exit-code range and examples/diagnostics. Owner review is mandatory.
- Phases 4–7: minimal layer contracts and identity propagation; avoid premature full-language IR/module design.
- Phase 8: pinned LLVM API, target setup, object output and verifier integration.
- Phases 9–10: linker tool/argument/error contracts and observable process status on Windows/Linux.

## Confidence Assessment

| Area | Confidence | Limit |
|------|------------|-------|
| M1 scope and phase gate | High | Canonical instructions; no implementation evidence |
| LLVM 23.1.2 release | High | Official release checked; package build not attempted |
| Conan generator/profile interfaces | High | Official docs checked; exact chosen integration still needs validation |
| Recipe suitability | Medium | Checked version index does not include target; other sources not exhaustively searched |
| Windows/Linux build/native parity | Pending evidence | No configure, package build, compiler, or test run |
| Exact M1 spelling/range | Open | Intentionally awaits Phase 3 review |

### Gaps to Address

Pin tested host tools, dependency revisions/options and platform combinations in Phase 1. Review the actual smoke profile before tests or source encode it. Record separate Tier 1 verification results; do not use this research as execution evidence.

## Platform impact

- Windows: mandatory x86_64 path, SDK/CRT/profile/linker and process contract; implementation pending.
- Linux: mandatory x86_64 path, compiler/ABI/profile/linker and process contract; implementation pending.
- macOS future: defer implementation, profiles and CI; preserve structured target/platform boundaries.

## Sources

- `AGENTS.md`, `gsd/GSD_PROJECT_DESCRIPTION.md`, `gsd/GSD_FRESH_START_CONAN.md`.
- Canonical design/milestone/probe documents and `docs/tooling/CONAN_BUILD_ARCHITECTURE.md`.
- [LLVM 23.1.2 release](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.2).
- [ConanCenter LLVM version index](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/llvm-core/config.yml).
- [Conan CMakeToolchain](https://docs.conan.io/2/reference/tools/cmake/cmaketoolchain.html), [CMakeDeps](https://docs.conan.io/2/reference/tools/cmake/cmakedeps.html), [profiles](https://docs.conan.io/2/reference/config_files/profiles.html), [lockfiles](https://docs.conan.io/2/tutorial/versioning/lockfiles.html).

---
*Research completed: 2026-10-03. Ready for initial roadmap: yes. Execution evidence: none.*
