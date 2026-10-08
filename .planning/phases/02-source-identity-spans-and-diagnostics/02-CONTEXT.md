# Phase 2: Source Identity, Spans, and Diagnostics - Context

**Gathered:** 2026-10-08
**Status:** Discussion captured; awaiting owner review and a separately approved discussion commit. Research, planning, and execution have not started.

<domain>
## Phase Boundary

Deliver SRC-01 through SRC-04 and DIAG-01 through DIAG-03 from
`.planning/REQUIREMENTS.md`: compiler clients can retain source text, recover
exact original bytes through checked spans, map byte locations, accumulate
structured diagnostics, and render useful deterministic plain text without
LLVM or driver dependencies. P01 forces this substrate for later compiler
stages; PLAT_01 and PLAT_03 force portable display names, newlines, and output.

Phase 1 is closed at 8/8 with final four-job acceptance from run
`37826767824`; its planning closeout was committed in
`1f7e67d11ea6a68790d29b27bd3e1750a1c5d29f`. Do not reopen or redesign that
foundation unless Phase 2 reveals a concrete regression or missing requirement.
The existing Conan-owned dependency/build/CI contracts remain in force.

Linux remains the primary development host. Windows remains a Tier 1
compatibility/CI target; Windows developer-host inspection is independently
pending, local developer confirmation is deferred, and that gap must not block
Phase 2 planning. CI evidence does not substitute for developer-host evidence.
macOS remains deferred Tier 2 without structural exclusion.

This phase does not define final source encoding, lexer whitespace, language
syntax, modules/imports, Veyr package semantics, or the language memory/error
model. It does not implement a virtual filesystem, file discovery, incremental
compilation, an editor/LSP, or a compiler driver. Phase 3 must separately author,
review, and commit the M1 Experimental Smoke Profile before syntax-dependent
implementation. Discussion does not authorize research, planning, execution,
builds/tests, commits, CI dispatch/retry, or another phase transition.

</domain>

<decisions>
## Implementation Decisions

### Source lifetime and identity

- **D-01:** SourceManager owns immutable source buffers. Registered contents and
  derived text views/span backing remain valid for its entire lifetime. Adding
  sources must not invalidate earlier views. No caller-owned buffer lifetime
  obligation is imposed; this is compiler infrastructure, not Veyr ownership
  semantics.
- **D-02:** Every registration creates a fresh SourceId, even when display name
  and bytes are identical. SourceId identifies the registered immutable
  snapshot, never the filesystem path. Multiple IDs may share a display name;
  older snapshots remain intact. Reparsing/incremental/editor use motivates
  the contract but is not implementation scope here.
- **D-03:** SourceId is meaningful only within its originating SourceManager.
  Consumers resolve it through that manager. Different managers may reuse the
  same numeric values for unrelated snapshots. No cross-manager or cross-run
  identity guarantee is required; numeric coincidence cannot establish origin.
- **D-04:** Registration accepts a caller-supplied display name as diagnostic
  metadata only. Names may be paths or labels such as `<memory>`, `<stdin>`,
  `<repl>`, and `<generated>`. SourceManager performs no filesystem lookup or
  path canonicalization during registration. SourceId is authoritative.

### Span validation and text retrieval

- **D-05:** SourceSpan stores SourceId, begin byte offset, and end byte offset,
  with half-open `[begin, end)` semantics. Begin is inclusive and end exclusive.
  Empty spans use `begin == end`; `[source_size, source_size)` is valid EOF.
  Offsets address original source bytes, not decoded characters/code points.
- **D-06:** Public/runtime operations fail explicitly and deterministically for
  unknown SourceId, `begin > end`, or `end > source_size`. Never clamp offsets,
  fabricate text, silently substitute another source, or rely only on debug
  assertions. Assertions may additionally document internal invariants.
- **D-07:** Validation is structural only: SourceId exists in the resolving
  manager and `begin <= end <= source_size`. Every such byte range is valid,
  including a range splitting CRLF. Retrieval preserves exact original bytes.
  Newline representation never affects span identity/validity; interpretation
  belongs to location mapping and rendering.
- **D-08:** Checked failures distinguish at least unknown source, reversed
  range, and beyond-source range. Retain offending SourceId and relevant
  offsets so callers/tests can inspect the invalid input. These are compiler-
  infrastructure failures, not Veyr source-language diagnostics. Do not merge
  them into one generic category or normalize invalid inputs.

### Display names and source locations

- **D-09:** LF is one line break; CRLF is one line break and advances the line
  number exactly once. Lone CR is an ordinary source byte. Original bytes stay
  unchanged. This governs location mapping/rendering only, not lexer whitespace
  or final encoding semantics.
- **D-10:** SourceLocation reports 1-based lines and 1-based byte columns within
  the original line. A tab is one byte/column; non-ASCII UTF-8 bytes count
  individually. Mapping does not decode Unicode or calculate display width.
  Visual caret alignment, tab expansion, and renderer-specific positioning are
  separate from byte locations; Phase 2 rendering has no Unicode-width dependency.
- **D-11:** Boundary rules are exact: LF and both bytes of CRLF belong to the
  preceding line at their respective byte columns. The byte immediately after
  a break is next line, column 1. Empty-source EOF is `1:1`. EOF after a trailing
  break is next line, column 1; otherwise EOF is final line, one byte column past
  its contents. These rules remain independent of visual rendering.
- **D-12:** Preserve exact caller display spelling: separators, casing, drive
  letters, relative components, spaces, and synthetic labels. Do not normalize
  or canonicalize presentation. Stored metadata remains unchanged. Rendering
  escapes control characters so a display name cannot inject extra lines or
  terminal control sequences. Display spelling never affects SourceId.

### Structured diagnostics and plain rendering

- **D-13:** One structured diagnostic model permits location-free messages and
  an optional primary SourceSpan plus zero or more ordered, labelled related
  spans. Never fabricate a source/dummy span for a location-free message.
  Related spans supplement rather than replace an existing primary. The model
  is independent of LLVM and rendering.
- **D-14:** Support Error, Warning, and Note. Accumulated Error diagnostics make
  `has_errors()` true; any Error prevents successful compilation. Warning/Note
  alone do not affect it. Rendering does not determine severity/error state.
  This is infrastructure only; warning groups, suppression, warnings-as-errors,
  and final language warning policy are outside Phase 2.
- **D-15:** Plain output contains severity/message, primary stored
  `display-name:line:column` when present, a bounded source excerpt around the
  primary location, an ASCII caret/underline for that span, then ordered,
  labelled related locations. Location-free diagnostics render their structured
  message without fabricated source context. Use the stored SourceManager
  snapshot/name, with no filesystem lookup, required color, terminal-specific
  capability, or Unicode-width dependency. Output is deterministic plain text.
- **D-16:** Accumulate in caller insertion/emission order. Do not automatically
  sort by source/location, deduplicate repeated messages, reorder by severity,
  or apply severity filtering. Repeated diagnostics remain observable. Future
  grouping/deduplication is an explicit presentation/query operation, not part
  of the accumulator contract.

### Research and planning boundaries

The owner selected every substantive preference explicitly; none was delegated
through a "you decide" answer. Normal implementation details may be investigated
only after the separate planning authorization and within the contracts above.

Later research/planning must specify and review, without silently expanding scope:

- Stable storage, checked API/result representation, SourceId/offset capacity
  handling, and source lifetime/move restrictions sufficient for D-01 through
  D-08. Never promise detection of cross-manager origin from a manager-local
  numeric ID alone.
- Line indexing and checked location lookup, including empty sources, offsets
  on both CRLF bytes, mixed LF/CRLF, lone CR, and EOF boundary cases.
- Bounded excerpt limits, multiline/zero-width/EOF marker layout, deterministic
  tab/control/non-ASCII byte presentation, and control escaping. No final encoding
  or Unicode-width policy may be inferred from a renderer implementation.
- Renderer handling of invalid referenced IDs/spans and output failures within
  the explicit checked/fail-closed contract, without fabricated source context.
- Ownership of diagnostic messages/labels, collection access, and lightweight
  source/diagnostic target/test integration without LLVM or driver coupling.

These are remaining implementation details for the later reviewed plan, not
new owner-selected defaults, executable changes, or new language capabilities.

</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read these before planning/implementation. Explicit owner
decisions and `docs/design/DECISION_LOG.md` take precedence. No Phase 2 SPEC,
research, or plans exist; no new external specification was introduced.

### Scope, accepted foundation, and probe pressure

- `AGENTS.md` — required reading, names, layer boundaries, and lifecycle gates.
- `.planning/PROJECT.md` — fresh M1 mission/constraints; initialization statements
  are historical and do not supersede subsequent accepted evidence.
- `.planning/REQUIREMENTS.md` — SRC-01 through SRC-04 and DIAG-01 through DIAG-03;
  BUILD-01 through BUILD-14 accepted, other phase requirements pending.
- `.planning/ROADMAP.md` — Phase 2 boundary and later-phase exclusions.
- `.planning/phases/01-conan-and-compiler-build-foundation/01-CONTEXT.md` — prior
  command/dependency decisions; acceptance subsequently established in closeout.
- `.planning/phases/01-conan-and-compiler-build-foundation/01-08-SUMMARY.md` —
  four-job foundation acceptance run 37826767824 and independent host evidence.
- `gsd/GSD_PROJECT_DESCRIPTION.md` — canonical M1 source/span and smoke boundaries.
- `docs/design/DECISION_LOG.md` — diagnostic design requirement and existing
  Conan/toolchain/M1 profile decisions; examples do not accept final grammar.
- `docs/design/VISION.md` — diagnostics and compiler architecture direction.
- `docs/design/FEATURE_CATALOG.md` — source spans/provenance direction, not permission
  to implement later language features.
- `docs/design/MEMORY_MODEL.md` — future provenance diagnostics; Phase 2 buffer
  ownership does not implement Veyr's memory model.
- `docs/design/TYPE_SYSTEM.md` — future diagnostic requirements; no type system here.
- `docs/design/MODULE_SYSTEM.md` — SourceId is not final module/package identity.
- `docs/design/OPEN_QUESTIONS.md` — unresolved encoding/string/tooling questions;
  accepted naming decisions in AGENTS/decision log take precedence over stale entries.
- `docs/design/DESIGN_PROBES.md` — P01 pressure and later tooling probes.
- `docs/design/milestones/MILESTONE_1.md` — source/diagnostic phase and smoke limits.
- `docs/design/milestones/MILESTONE_STRATEGY.md` — probe-driven scope.
- `docs/design/tracks/projects/platform/PLAT_01_PATHS_FILESYSTEM_CASES.md` —
  display paths, source identity, and no general virtual filesystem.
- `docs/design/tracks/projects/platform/PLAT_03_TERMINAL_DIAGNOSTICS.md` —
  portable deterministic plain text without final Unicode/theme/localization policy.

### Reuse, platform policy, and lifecycle constraints

- `CMakeLists.txt` — existing target-oriented `veyrcore`/`veyrc` and test integration.
- `include/veyr/build_info.hpp` — canonical public include root/namespace pattern.
- `src/core/build_info.cpp` — existing small core implementation boundary.
- `src/driver/main.cpp` — current version-only driver, not a Phase 2 source loader.
- `tests/infrastructure/build_info_test.cpp` — existing GoogleTest/GoogleMock pattern.
- `docs/development/BUILD_POLICY.md` — C++23 and Conan-owned dependencies.
- `docs/development/DEVELOPER_BOOTSTRAP.md` — shared commands; historical pending
  checkpoints are superseded by the accepted Phase 1 planning closeout where applicable.
- `docs/development/CROSS_PLATFORM_POLICY.md` — Tier 1 Windows/Linux and future macOS.
- `docs/development/INCLUDE_POLICY.md` — `<veyr/...>` includes.
- `docs/tooling/TOOLCHAIN.md` — tooling names and diagnostic/core separation.
- `docs/project-system/VEYR_TOML.md` — future project system, outside this phase.
- `docs/development/GSD_WORKFLOW.md` — discussion/review/commit before planning.
- `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md` — exact lifecycle commands.
- `docs/development/SESSION_HANDOFF_POLICY.md` — repository-based continuity.
- `docs/development/COMMIT_POLICY.md` — separate, explicitly approved stage commits.
- `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md` — later implementation review.
- `docs/development/GIT_WORKFLOW.md` — stage history discipline.
- `docs/development/SKILL_USAGE_GUIDE.md` — local skill triggers and boundaries.

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- The accepted C++23/Conan/CMake/Ninja/GoogleTest foundation, shared developer
  commands, and independently reviewed Windows/Linux Debug/Release CI already exist.
- `veyrcore` is a small static library with canonical `include/veyr/` headers;
  `veyrc` currently exposes version output only.
- Infrastructure tests provide GoogleTest/GoogleMock and portable output-test
  examples. The existing include guardrail and compile database verification remain.

### Established Patterns

- No source manager, source-span/location types, or diagnostic model/rendering
  implementation exists at discussion start. There is no existing Phase 2 SPEC,
  research, plan, discussion context, codebase map, or matching pending todo.
- Public owned headers use angle-bracket includes; target policies/Debug/Release
  runtime selection are already reviewed. New source/diagnostic components must
  remain independent of LLVM, parser/AST/semantics, and driver/build state.

### Integration Points

- Later plans can add bounded source/diagnostic code and tests through existing
  core/test targets. Exact target/file breakdown is planning work, not a new
  architecture/framework selected during discussion.
- Later lexer/token/AST/diagnostic consumers use retained source snapshots and
  spans rather than owning duplicate lexemes. No syntax-dependent tests begin here.
- Existing CI supplies Windows compatibility evidence; Windows developer-host
  confirmation remains separately deferred and does not block this planning stage.

</code_context>

<specifics>
## Specific Ideas

Examples are infrastructure illustrations only: repeated registrations can
share `<memory>` while receiving distinct IDs; `[size, size)` is EOF; CRLF
bytes remain separately addressable despite representing one line break.
No new `.vyr` spelling or final grammar was accepted.

Platform impact:
- Windows: Preserve caller Windows paths and CRLF bytes; plain rendering requires
  no particular console. Existing Tier 1 CI is the compatibility target; independent
  developer-host confirmation stays pending.
- Linux: Primary development on the existing Omarchy host; LF and the same
  byte-based contracts apply, preserving path spelling without case assumptions.
- macOS future: Source/diagnostic contracts avoid mandatory OS lookup and terminal
  behavior; Darwin implementation/validation remain deferred.

</specifics>

<deferred>
## Deferred Ideas

No new phase scope was added. Potential future incremental/reparse/editor
use, `<repl>`/`<generated>` labels, and explicit UI grouping/deduplication explain
contracts but do not authorize those capabilities.

Remain outside Phase 2: final source encoding/Unicode width, lexer whitespace
and grammar, warning policy/groups/suppression/warnings-as-errors, colored/JSON/
SARIF/LSP output, virtual filesystem/file discovery, persistent source identity,
full modules/packages, and macOS bring-up. Phase 3 profile work and all later
compiler phases require their own exact owner commands.

</deferred>

---

*Phase: 02-source-identity-spans-and-diagnostics*
*Context gathered: 2026-10-08*
*Discussion artifacts are uncommitted; no research, phase plan, implementation,
build/test, CI action, commit, push, or further lifecycle transition occurred.*
