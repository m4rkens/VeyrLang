# Feature Research

**Domain:** Veyr M1 compiler walking skeleton
**Researched:** 2026-10-03
**Confidence:** High for scope from canonical documents

## Feature Landscape

### Table stakes for M1

Here, table stakes means required for P01's walking skeleton. A mature language feature catalog is not an M1 checklist.

| Feature | Probe pressure | Complexity | Scope |
|---------|----------------|------------|-------|
| Reproducible developer build | P01 / PLAT_06 | Medium; LLVM package build expensive | Conan, CMake, Ninja, test package, thin orchestration |
| Source IDs, byte spans, retained text | P01 / PLAT_01 | Medium | Stable source lifetime and useful paths |
| Plain-text source diagnostics | P01 / PLAT_03 | Medium | Independent of LLVM/terminal color |
| Milestone-local smoke profile | P01 | Design gate | Explicit provisional spelling, supported forms, failures |
| Tokens and recovered lexing | P01 | Medium | Kind/span only; decimal ASCII shape; EOF; diagnostics |
| Parser and minimal AST | P01 | Low/medium | Fixed entry and single integer-literal return only |
| Semantic smoke validation | P01 | Low | Reviewed profile constraints and range |
| Minimal lowering | P01 | Low | Preserve validated constant and source/module relationships |
| Single root module identity | P01 | Low | No imports/resolver/graph |
| LLVM object generation | P01 | Medium | Native x86_64 target; verified LLVM IR |
| `veyrc` compile/link path | P01 / PLAT_02 | Medium | Explicit paths/process/error handling |
| End-to-end exit-code proof | P01 / PLAT_02 / PLAT_06 | Medium | Positive and negative behavior on both Tier 1 platforms |

### Long-term differentiators, deferred from M1

| Capability | Pressure projects | Prerequisites |
|------------|-------------------|---------------|
| Ownership, inferred borrowing, invalidation diagnostics | Arena allocator, resource wrappers, ownership stress | Useful data/functions, flow representation, reviewed semantics |
| Result/Option, tagged unions, matching | Config parser, todo CLI, package manifest parser | Structured data and calls |
| Modular native libraries and plugins | Staticlib+exe, plugin host, C wrapper | Module graph, ABI and representation decisions |
| Platform and graphics wrappers | Native window, framebuffer, GL clear, Vulkan loader/device/swapchain/triangle | FFI, pointers, handles, cleanup, layout |
| Threads/jobs/networking | File hasher, job system, blocking HTTP server | Ownership transfer, synchronization, buffers |
| Security and graph analysis tooling | Sysmon/MITRE/Atomic/Sigma; AST/CFG/DDG scanners | Text, containers, trees/graphs, diagnostics |
| Hardware/audio applications | Telemetry, calibration, FFB, binary codec, audio buffers | Binary I/O, units, resource/effect decisions |
| Tooling and self-hosting | Formatter/LSP/bindgen, agent IDE, mini-language/compiler subset | Stable compiler library, sufficient language subset |

### Anti-features

| Temptation | Failure | Correct boundary |
|------------|---------|------------------|
| Auto-include all mature compiler features | M1 becomes a language implementation | Include only P01 table stakes |
| Choose familiar function syntax now | Examples become final grammar accidentally | Phase 3 reviewed provisional profile |
| Treat `Veyr.toml` reference as a build checklist | Package/product infrastructure crowds out compiler work | Future probe and owner-approved milestone |
| Solve dependency acquisition in Python | Duplicates Conan ownership | Thin subprocess orchestration |
| Defer Windows until Linux works | Hidden paths/runtime/linking assumptions | Both Tier 1 profiles and evidence from the start |
| Declare old test results validated | Imports forbidden prior-run evidence | All new requirements start Pending |

## Feature Dependencies

```text
Build foundation
  -> source/diagnostics
  -> reviewed Phase 3 smoke profile -> lexer
  -> parser -> AST/semantic smoke -> lowering
  -> single-root context -> LLVM backend -> veyrc -> native execution proof
```

Phase 4 may introduce the minimal parse representation needed by parsing; Phase 5 owns the durable AST and semantic boundary. It must not introduce a second parser merely to follow layer numbering.

## MVP Definition

M1 succeeds when a contributor can build the compiler through the documented Windows/Linux workflows, compile the reviewed provisional smoke source into native code, observe the expected exit code, and receive useful errors for unsupported/malformed input. None of this accepts final Veyr grammar.

The next useful-language probes are argument echo/calculator, then text/config/todo tools. Future ownership, modules, FFI, graphics, security, hardware, tooling, and self-hosting decisions remain driven by their project cards. No future capability becomes active without a separately approved milestone.

## Design probe planning packet

- Pressure: P01; PLAT_01/02/03/06 supply only the relevant infrastructure pressure.
- Minimal capability: one-root fixed-entry integer-return native compilation.
- Semantics: profile-local only; known captured lexer constraints are preserved.
- Open questions: exact spelling/range, source/path policies, layer contracts and link/process contract.
- Non-goals: all future probe implementations and final grammar acceptance.
- Decision-log update: unnecessary for initialization; future final acceptance requires owner decision.

## Sources

- `docs/design/DESIGN_PROBES.md` and `docs/design/tracks/projects/**`.
- `docs/design/milestones/MILESTONE_1.md`, `MILESTONE_STRATEGY.md`, `MILESTONE_MATRIX.md`.
- `docs/design/FEATURE_CATALOG.md`, `MEMORY_MODEL.md`, `TYPE_SYSTEM.md`, `MODULE_SYSTEM.md`, `OPEN_QUESTIONS.md`.
- `gsd/GSD_PROJECT_DESCRIPTION.md`.
