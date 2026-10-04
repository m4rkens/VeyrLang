# Phase 1 — Pattern Map

**Captured:** 2026-10-04
**Status:** Complete for the fresh starter; no implementation analogs exist
**Method:** Inline pattern-mapper role, repository inspection only

## Existing Implementation Patterns

No include/, src/, tests/, cmake/, conan/, scripts/, CMake targets/presets or CI workflows exist in this fresh baseline. There are no code excerpts, existing test harnesses or prior implementations to reuse. Prior Veyr runs are excluded.

Canonical policy documents provide contracts; JSON development templates and Veyr.toml remain examples/design sketches. Do not infer executable schemas or package mechanisms from them.

## Proposed File-to-Contract Map

| New responsibility | Closest canonical contract | Pattern required |
|---|---|---|
| Conan profiles/recipe/consumer/locks | docs/tooling/CONAN_BUILD_ARCHITECTURE.md; docs/development/BUILD_POLICY.md | Conan-native mechanics; exact refs/options and runtime inputs |
| Host identity/doctor/setup | docs/development/DEVELOPER_BOOTSTRAP.md; committed 01-CONTEXT.md D-01–D-11 | One thin Python interface; explicit acquisition step; strict actionable mismatches |
| CMake foundation/presets/providers | AGENTS.md; BUILD_POLICY.md | Target-oriented C++23, Ninja compile database, exact native LLVM CONFIG discovery |
| Infrastructure test harness | REQUIREMENTS.md BUILD-10; AGENTS.md | Conan GoogleTest/GoogleMock; foundation-only assertions |
| Owned-header checker | docs/development/INCLUDE_POLICY.md | Angle owned includes; reject quoted/relative owned includes; build and CTest reachability |
| Command supervision/clean | AGENT_STEP_AND_COMMAND_POLICY.md; committed D-02/D-12 | Credible liveness after quiet checkpoint; no duplicate retries; scoped clean |
| CI matrix/cold path | CI_MATRIX_POLICY.md; PLAT_06; committed D-13–D-16 | Same native profiles/entry point, four combinations, cache optimization only |
| Wrapper receipts vs package state | docs/project-system/VEYR_TOML.md; Conan architecture | Tool/input identity only, no graph duplication or future Veyr.lock semantics |

## Integration and Conflict Notes

Use the selected scripts/dev.py entry point consistently in later plans. Shared Python/CMake files have sequential ownership in 01-PLANNING-BREAKDOWN.md. Existing design/docs stay canonical; change a completed artifact only to resolve a documented inconsistency. No generated package/cache/build state is tracked.

Platform impact:
- Windows: Runtime/toolchain-specific profiles and structured path/process handling.
- Linux: Explicit compiler/standard-library ABI selection and equivalent wrapper semantics.
- macOS future: No implementation now; platform-gated profiles and standard build boundaries remain extensible.

