# Phase 1: Conan and Compiler Build Foundation - Context

**Gathered:** 2026-10-03
**Status:** Discussion captured; awaiting owner review and separately approved discussion commit. Planning has not started.

<domain>
## Phase Boundary

Deliver the BUILD-01 through BUILD-14 foundation from `.planning/REQUIREMENTS.md`: a contributor can build and test the C++23 compiler foundation through equivalent Windows/Linux workflows using Conan-owned dependencies. P01 compiler smoke infrastructure and PLAT_06 platform/CI pressure justify this work.

The stack remains thin Python orchestration -> Conan 2 -> CMake -> Ninja -> Clang. Conan owns third-party recipes, dependency resolution, acquisition, source builds, package identities/revisions, storage/cache, and dependency locking. LLVM 23.1.2 is the initial backend target; prefer a suitable exact recipe, otherwise one narrow project-owned Conan recipe.

This phase does not implement language syntax, lexer/parser/AST/semantics, native smoke compilation, the user-facing `veyr` project/package system, or a custom provisioning subsystem. Windows x86_64 and Linux x86_64 are Tier 1; macOS remains deferred Tier 2. No earlier Veyr run's implementation, lifecycle artifacts, or evidence may be imported.

</domain>

<decisions>
## Implementation Decisions

### Developer commands
- **D-01:** Provide one Python entry point with the same command interface on Windows/Linux. Separate shell/PowerShell convenience launchers are not required. `python scripts/dev.py setup` was an illustrative command; the exact script path was not separately selected.
- **D-02:** Normal output includes step headers, the selected profile/configuration, the invoked command, and live Conan/CMake/Ninja/CTest output. Long quiet operations remain supervised; silence alone is not failure.
- **D-03:** Allow limited preparation chaining: `build` may configure and `test` may build. These commands must not implicitly perform dependency setup, acquisition, or missing-package source builds. Missing prepared dependencies produce actionable setup instructions.
- **D-04:** Routine commands select a reviewed native Windows/Linux profile and the development configuration, print their selections, and allow explicit overrides to supported profiles/configurations. Do not adopt `conan profile detect` output as canonical policy.

### Debug/Release support
- **D-05:** Support Debug/Release Veyr builds while using optimized LLVM packages where runtime/ABI compatibility is verified. This is a preference, not evidence that every Debug/Release dependency combination is valid. Research must establish each supported combination; never silently substitute an incompatible runtime or fall back to another provider. A dedicated Debug LLVM debugging configuration is not required in Phase 1.
- **D-06:** Debug is the default Veyr development configuration; Release remains explicitly selectable.
- **D-07:** Start with one reviewed tool/runtime baseline per Tier 1 platform. Document exact versions and validate Debug/Release against each baseline. Exact Python/Conan/CMake/Ninja/Clang/Git and platform SDK/runtime choices remain research/planning work.
- **D-08:** Normal commands stop when the environment falls outside the reviewed baseline. Report the mismatch and expected configuration with actionable instructions; do not automatically accept an unsupported environment or introduce experimental opt-in as the normal workflow.

### Network and cache behavior
- **D-09:** Explicit `setup` permits Conan downloads and missing-package source builds. Show what Conan will do. A long first LLVM build requires no extra per-package confirmation once the setup operation is authorized. This does not authorize running setup during GSD discussion/planning or bypass tool-level execution permissions.
- **D-10:** `all` explicitly includes setup -> configure -> build -> test -> verify, and documents possible dependency downloads/source builds. Dependency acquisition stays in the visible setup step. This is a developer build operation, not permission to advance GSD lifecycle stages. Avoid duplicate work when implementing the composed operations.
- **D-11:** Use a dedicated Conan home per project by default, outside tracked source and fully Conan-owned. Do not default to the standard shared user Conan home, mirror packages into a Veyr store, or define a Veyr package identity/solver/cache format. Exact location and project/worktree identity rules remain research/planning details. Veyr wrapper receipts and Conan package state retain separate ownership.
- **D-12:** Ordinary `clean` removes only the selected profile/configuration's build output. Preserve other configurations and Conan's cache. Package/cache deletion must be a separate explicit destructive action; no automatic cache cleanup, duplicate retries, or repair loops.

### Windows/Linux evidence and CI
- **D-13:** Phase 1 Tier 1 acceptance requires passing CI on both Windows and Linux. Local checks supplement CI; Linux-only execution cannot establish Windows support. All acceptance evidence remains pending until authorized execution/verification.
- **D-14:** Use GitHub Actions workflows in this repository. CI uses the same reviewed profiles/configuration and Python developer commands as local development.
- **D-15:** Trigger CI on pull requests, pushes to `main`, and manual runs.
- **D-16:** Routine CI covers Windows x86_64 and Linux x86_64, each with Debug and Release Veyr configurations: four combinations. Keep bootstrap validation and configure/build/test validation logically distinct as required by CI policy. Restore Conan-owned caches only as an optimization; a deliberate clean source-build path must remain possible.

### Research and planning discretion

The owner did not delegate any listed preference via a "you decide" answer. Research/planning may select implementation details within these boundaries: exact script location, flags/preset names, version pins, Conan recipe/options/lock integration, cache location, and portable orchestration structure. Those selections belong in the separately authorized research/plan, not executable files produced during discussion.

Required research gates:
- Recheck exact LLVM 23.1.2 recipe suitability and native CMake exports/targets; prefer the existing recipe only if suitable, otherwise the narrow Conan recipe path. Do not substitute an LLVM version silently.
- Select exact dependency versions/revisions and only necessary LLVM options/components/backends. Include zlib/zstd/LibXml2 only when selected LLVM options require them; GoogleTest/GoogleMock remains Conan-owned.
- Establish runtime/ABI-compatible Debug/Release combinations for Windows clang-cl and Linux Clang. If optimized LLVM cannot support a requested combination, record the conflict for owner review rather than treating this discussion as permission to bypass compatibility checks.
- Define strict baseline diagnostics and reproducible tool/SDK selection for local hosts and CI, without creating host-tool/package provisioning infrastructure.
- Resolve dedicated Conan-home location and cache identity using Conan-native mechanisms. Keep package state untracked and separate from optional wrapper receipts and future language-level `Veyr.lock`.
- Define prepared-dependency, acquisition-disabled configure/build behavior. Preserve the explicit managed/system/custom LLVM discovery boundary; managed means Conan, and external modes never acquire dependencies secretly.
- Check GitHub Actions runner resources/hard limits and cache support against expensive LLVM source builds. A quiet-output threshold is not a hard timeout. If CI capacity cannot satisfy the selected matrix/source-build path, report the concrete constraint for owner review; do not invent custom SDK acquisition or narrow Tier 1 acceptance silently.
- Define meaningful compiler-foundation tests, compile database export, and the owned-header include guardrail/build target/CTest entry. Do not create lexer or smoke-syntax tests in Phase 1.

</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read these before planning or implementing. Explicit owner decisions and `docs/design/DECISION_LOG.md` take precedence. No separate phase SPEC.md or prior-phase context exists.

### Project and scope
- `AGENTS.md` — required reading, names, architecture boundaries, explicit stage/commit gates.
- `.planning/PROJECT.md` — fresh lifecycle constraints and project mission.
- `.planning/REQUIREMENTS.md` — BUILD-01 through BUILD-14 and traceability.
- `.planning/ROADMAP.md` — Phase 1 goal, boundary, acceptance criteria, and later-phase exclusions.
- `gsd/GSD_PROJECT_DESCRIPTION.md` — canonical fresh project description.
- `gsd/GSD_FRESH_START_CONAN.md` — no imported lifecycle/provisioning implementation or evidence.
- `docs/design/DECISION_LOG.md` — D-064/D-065 Conan/LLVM policy and D-066 quiet-build supervision; earlier SDK policy is superseded.
- `docs/design/DESIGN_PROBES.md` — P01 pressure; no syntax acceptance through build foundation.
- `docs/design/milestones/MILESTONE_1.md` — walking skeleton boundaries and ten-phase decomposition.
- `docs/design/milestones/MILESTONE_STRATEGY.md` — probe-driven scope.
- `docs/design/tracks/projects/platform/PLAT_06_CI_PLATFORM_MATRIX.md` — Windows/Linux validation pressure.

### Build, platform, and examples
- `docs/tooling/CONAN_BUILD_ARCHITECTURE.md` — dependency ownership and thin-wrapper boundary.
- `docs/development/BUILD_POLICY.md` — compiler/tool/dependency baseline, CMake integration, ABI compatibility, generated state.
- `docs/development/DEVELOPER_BOOTSTRAP.md` — explicit Conan setup, prerequisites, developer operations, cache ownership.
- `docs/development/CROSS_PLATFORM_POLICY.md` — Windows/Linux parity and future macOS feasibility.
- `docs/development/PLATFORM_TRIPLES.md` — distinct host/target/toolchain identities.
- `docs/development/CI_MATRIX_POLICY.md` — shared configuration and logically separate CI validation stages.
- `docs/development/INCLUDE_POLICY.md` — canonical `<veyr/...>` includes and include checks.
- `docs/tooling/TOOLCHAIN.md` — tool names and responsibilities.
- `docs/project-system/VEYR_TOML.md` — future language project system is separate from compiler Conan dependencies.
- `Veyr.toml` — design sketch, not executable compiler build input.
- `templates/development/dependency-manifest.example.json` — documentation-only Conan-oriented example; not a custom solver input or executable schema.
- `templates/development/environment.lock.example.json` — documentation-only machine-local tool/Conan-input receipt; not a package graph/lock or future `Veyr.lock`.

### Workflow and supervision
- `docs/development/GSD_WORKFLOW.md` — discussion/planning/execution/verification boundaries.
- `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md` — exact commands, soft liveness checks, no duplicate execution.
- `docs/development/SESSION_HANDOFF_POLICY.md` — repository-based continuity.
- `docs/development/COMMIT_POLICY.md` — explicit separate lifecycle-stage commits.
- `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md` — sensitive-file review before implementation commits.
- `docs/development/SKILL_USAGE_GUIDE.md` — local skill triggers and corrected quiet-build example.
- `.codex/skills/veyr-developer-bootstrap/SKILL.md` — Conan ownership checks.
- `.codex/skills/veyr-cmake-watchdog/SKILL.md` — healthy quiet builds continue; confirmed stalls stop once and report.
- `.codex/skills/veyr-gsd-phase-gate/SKILL.md` — no automatic transition or stage commit.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- Canonical build/platform/workflow policies and corrected skills/templates already exist in starter commit `74ccfbac3de54e81749acafd8b4043d7323d46e2`.
- Fresh project/research/requirements/roadmap artifacts exist in lifecycle commit `b1f0bf585bd57b3e020b5103d71ec3458e2822e8`; they establish direction, not implemented behavior or Tier 1 evidence.

### Established Patterns
- No `include/`, `src/`, `tests/`, `cmake/`, `conan/`, `scripts/`, CMake targets/presets, CI workflows, or prior phase implementation were present at discussion start.
- Example manifests describe future product behavior; development JSON examples are documentation-only and must not become a parallel package subsystem.
- Source/diagnostic/compiler boundaries remain independent of LLVM and the build driver as mandated by `AGENTS.md`.

### Integration Points
- Future Phase 1 plans may introduce reviewed Conan inputs, target-oriented CMake/Ninja foundation, the Python entry point, include/infrastructure checks, development presets, and GitHub Actions configuration within BUILD-01 through BUILD-14.
- Native profiles, wrappers, and CI must share configuration; Conan's package identities/cache/locks supply dependency mechanics.

</code_context>

<specifics>
## Specific Ideas

User selected all four discussion areas and answered sixteen concrete choices. The dedicated per-project Conan home and limited build/test chaining are explicit owner selections. Full `all` orchestration is authorized only when that developer operation is actually invoked during permitted execution; it never chains GSD stages.

Platform impact:
- Windows: reviewed x86_64 clang-cl/SDK/CRT/linker baseline; Debug/Release compatibility must be demonstrated by CI using the same Python interface.
- Linux: reviewed x86_64 Clang/standard-library ABI baseline; equivalent Debug/Release CI and prerequisite diagnostics.
- macOS future: profiles/runners/native checks deferred; retain host/target distinctions, structured process/path handling, and standard Conan/CMake boundaries.

</specifics>

<deferred>
## Deferred Ideas

None introduced as new capabilities. A dedicated Debug LLVM configuration, broader toolchain-version support, convenience launchers, and experimental unsupported-environment opt-in were alternatives not selected for Phase 1; they are not authorized future tasks.

The canonical exclusions remain: macOS implementation, cross-compilation bring-up, full language/project/package tooling, general provisioning/provenance systems, and later M1 compiler stages.

</deferred>

---

*Phase: 01-conan-and-compiler-build-foundation*
*Context gathered: 2026-10-03*
*Discussion artifacts are uncommitted. No plan, implementation, build/test, verification, or next-phase advancement occurred.*
