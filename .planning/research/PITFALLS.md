# Pitfalls Research

**Domain:** Fresh Veyr M1 lifecycle
**Researched:** 2026-10-03
**Confidence:** High for documented risks; integration findings require later execution evidence

## Critical Pitfalls

| Pitfall | What goes wrong / warning signs | Prevention | Phase |
|---------|---------------------------------|------------|-------|
| Importing old evidence | Requirements start checked; copied build trees or old phase plans appear | Start every requirement Pending; use only canonical design input | Initialization/all |
| Planning creates implementation | Source/tests/CMake/scripts appear during discuss/plan; RED tests run | Restrict planning to authorized documents; record checks as future work | All |
| Auto-chain or automatic commit | Workflow defaults advance stages or commit without review | Interactive lifecycle, auto flags false, exact-command gate and separate commit approval | All |
| Python becomes a package manager | Wrapper stores transitive graphs, archives, SDK transactions, custom locks | Delegate all third-party mechanics to Conan; keep only orchestration | 1 |
| Wrong LLVM assumed available | Recipe versions differ from 23.1.2; backend silently downgraded | Recheck suitable recipe; use narrow reviewed recipe if absent | 1 |
| LLVM discovery assumptions | Consumer expects upstream config/targets that selected recipe alters | Inspect exact recipe exports; validate config discovery and native link | 1, 8 |
| Accidental optional dependency expansion | Full LLVM defaults pull unnecessary projects/backends/libraries | Enable only M1-required target/components; document conditional dependencies | 1 |
| Quiet build treated as failed | Healthy LLVM build killed/retried after one minute | Liveness supervision; no duplicate retries or automatic cleanup | 1 and later execution |
| Debug/Release/runtime incompatibility | Windows CRT or standard-library mismatch; cache state mixed | Explicit supported combinations and reviewed host/build profiles | 1 |
| Provisional examples become grammar | Test names imply final Veyr function/return syntax | Profile-local naming and diagnostics; Phase 3 review/commit prerequisite | 3–10 |
| Literal shape confused with semantics | Lexer converts/range-checks literals using a guessed final type | Lexer recognizes captured decimal shape; reviewed semantic range checked later | 3, 5 |
| Recovery hides compilation failure | Lexer reports errors but driver emits an executable anyway | Define error boundaries; negative end-to-end checks prevent successful artifacts | 3, 4, 9, 10 |
| Layer coupling | Source/diagnostic code includes LLVM; lexer includes AST/parser | Small explicit contracts and narrow backend boundary | 2–8 |
| Fake cross-platform success | Only Linux output used as evidence for Windows | Separate Tier 1 build/native-run evidence; no macOS support claim | 1, 9, 10 |
| POSIX status mistaken for portable integer | Arbitrary integer returns compare differently across OSes | Review M1 semantic range and shared smoke cases before tests depend on them | 3, 5, 10 |
| Manifest becomes current build authority | C++ wrapper reads future Veyr dependency/profile fields | Conan/CMake implementation build; `Veyr.toml` remains product design | 1, 7, 9 |

## Integration Gotchas

Conan's checked LLVM recipe removes/replaces portions of upstream configuration and provides its own component metadata. Generated config behavior must be tested rather than assumed. The same recipe also has platform/configuration restrictions; it is research input, not a ready-to-use 23.1.2 recipe. [Checked recipe source](https://raw.githubusercontent.com/conan-io/conan-center-index/master/recipes/llvm-core/all/conanfile.py).

Conan locks capture dependency versions and recipe revisions, not all ABI/profile facts. Keep explicit runtime/compiler/options review alongside locking. [Conan lockfiles](https://docs.conan.io/2/tutorial/versioning/lockfiles.html).

## Technical Debt Patterns

| Shortcut | Cost | Acceptable scope |
|----------|------|------------------|
| Fixed-entry smoke representation | Cannot express useful general programs | M1 only, with explicit migration boundary |
| Minimal root context | Cannot resolve imports | M1 only |
| Native-only package build | Future cross-compilation needs more research | M1; preserve separate host/target concepts |
| Hardcoded user cache/compiler paths | Breaks other machines and cache compatibility | Never |
| One-off SDK downloader/normalizer | Permanent maintenance outside Conan | Never under current architecture |

## Security and UX

Use ordinary recipe/source integrity mechanisms and reviewed versions/options. Invoke native tools with structured arguments; avoid shell interpolation of source/output paths. Report missing prerequisites, dependency misses, unsupported profile input and linker failures clearly. Do not expand M1 into a generic supply-chain/provenance platform or global OS abstraction.

## Looks Done But Isn't

- Build foundation requires test-package and LLVM discovery/link checks, not just successful configure.
- Lexer success requires EOF/span/recovery behavior, not just one token snapshot.
- Parser/semantic success requires rejected malformed/unsupported source and range checks, not only one positive program.
- Backend success requires verified IR and native object output, not printed LLVM text.
- Driver success requires native linking and useful failure behavior, not object output only.
- Milestone success requires separately recorded Windows/Linux native execution evidence and preserved provisional-language boundaries.

These are future authorized verification checks, not claims or tests run during initialization.

## Recovery Strategies

If lifecycle mixing occurs, stop and report the contaminated files; preserve owner work and seek a scoped recovery decision. If a command stalls, report command/cwd/output and liveness findings; do not duplicate or clean automatically. If a semantic question blocks a phase, record it and request owner design review rather than selecting final semantics silently.

## Sources

- `AGENTS.md` and required GSD/command/commit/handoff policies.
- `gsd/GSD_FRESH_START_CONAN.md`.
- `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE_TEMPLATE.md`.
- `docs/development/BUILD_POLICY.md`, `CROSS_PLATFORM_POLICY.md`, `PLATFORM_TRIPLES.md`.
