# Phase 2 — Planning Review

**Date:** 2026-10-09
**Verdict:** PASS for owner review / a separately authorized planning-only commit.
**Execution:** Not started; 0/4 Phase 2 plans executed. All seven Phase 2
requirements remain pending. Phase 1 remains closed at 8/8.

This is an inline self-review using the installed planner/checker criteria and
GSD document checks, not an independent agent review or runtime verification.
Repository policy requires explicit delegation authorization; none was requested.
No completed research/decision was rerun. No source/test/CMake/script/workflow,
recipe/profile/baseline/lock or dependency file was changed. No builds/tests,
package operations, CI actions, commit, push or phase advancement occurred.

## Authoritative inputs and scope

- 02-CONTEXT.md and 02-DISCUSSION-LOG.md, committed in
  `910be08de5cacfa4f585e5e548698644760dc6fd`.
- 02-RESEARCH.md, committed in `1fe774595b6efda2295d4b1108c746bcac3c5e99`.
- REQUIREMENTS/ROADMAP Phase 2: SRC-01–SRC-04 and DIAG-01–DIAG-03.
- AGENTS.md, canonical design/development documents and inspected existing core,
  native test, shared-command and accepted Phase 1 target patterns.

Completed discussion/research status sentences describe historical capture;
git history establishes their later commit. Those artifacts stay byte-identical.
Current STATE/ROADMAP now reflect planning completion and owner review pending.
No contradiction/blocker required reopening an owner-selected decision or Phase 1.

## Plan/wave/dependency review

| Wave / plan | Tasks | Implementation files | Produces | Consumes |
|---|---:|---:|---|---|
| 1 / 02-01 | 2 | 7 | Owned snapshot manager, IDs, checked spans/text, source_tests | Accepted core/GoogleTest foundation |
| 2 / 02-02 | 2 | 4 | SourceLocation, immutable line index and checked line lookup | 02-01 SourceManager/error/span contracts |
| 3 / 02-03 | 2 | 6 | Owned diagnostics and insertion-order engine, diagnostic_tests | Earlier SourceSpan/SourceId boundary |
| 4 / 02-04 | 2 + owner checkpoint | 5 | Checked bounded plain renderer and integration evidence | All earlier source/location/diagnostic APIs |

Dependencies are exactly `02-01 → 02-02 → 02-03 → 02-04`, waves 1–4.
No cycle, missing/forward producer or parallel ownership conflict. Shared root/test
CMake and source-manager edits happen sequentially. No extra infrastructure wave,
new external package or framework. Each plan fits the 2–3 task / small-file scope;
the renderer's implementation and boundary/integration coverage are separated.
Normal owner-approved execution/summary/state/requirement closeout artifacts are
workflow outputs, not additional implementation tasks or current acceptance.

## Requirements coverage

| Requirement | Primary plan / tasks | Observable future evidence |
|---|---|---|
| SRC-01 | 02-01 / 1–2 | Caller-independent owned text, growth-stable views, fresh local IDs, retained snapshots |
| SRC-02 | 02-01 / 1–2 | Empty/EOF/CRLF-split structural spans; distinct runtime failure payloads and arithmetic bounds |
| SRC-03 | 02-01 / 1–2 | Original byte substrings including embedded NUL, tabs and high bytes |
| SRC-04 | 02-02 / 1–2 | Exact names; complete LF/CRLF/lone-CR/byte-column/EOF table and bounded oracle |
| DIAG-01 | 02-03 / 1–2 | Owned optional primary / ordered labelled related model without LLVM; 02-04 validates references |
| DIAG-02 | 02-04 / 1–3 | Exact bounded plain text, control-safe presentation, marker/bounds cases and independent Tier 1 execution |
| DIAG-03 | 02-03 / 1–2 | Severity/error state without rendering; insertion order and duplicate preservation |

All seven requirements have exactly one primary plan owner. Later integration
regresses earlier contracts without duplicating implementation ownership.
No requirement is dropped/deferred or prematurely checked off.

## Decision translation coverage

| Decision | Executable plan/task coverage |
|---|---|
| D-01 owned lifetime / growth-stable views | 02-01/1–2; regression after index addition 02-02/2 |
| D-02 fresh snapshot ID for every registration | 02-01/1–2, duplicate name/bytes tests |
| D-03 manager-local origin only | 02-01/1–2; diagnostic caller/renderer contract 02-03/1 and 02-04/1 |
| D-04 exact caller metadata / no filesystem | 02-01/1; 02-02/2 name cases |
| D-05 byte half-open / empty / EOF | 02-01/1–2; renderer boundary cases 02-04/2 |
| D-06 explicit fail-closed public checks | 02-01/1–2; 02-02/1; 02-04/1–2 |
| D-07 structural-only validity / original bytes / split CRLF | 02-01/2; 02-02/2; 02-04/2 |
| D-08 distinct inspectable infrastructure errors | 02-01/1–2; checked renderer error/index 02-04/1–2 |
| D-09 LF/CRLF once, lone CR ordinary | 02-02/1–2; escaped excerpt cases 02-04/2 |
| D-10 one-based byte location, no Unicode width | 02-02/1–2; separate rendered-cell mapping 02-04/1–2 |
| D-11 exact newline and EOF boundary ownership | 02-02/1–2; zero/EOF/newline markers 02-04/2 |
| D-12 preserve caller spelling / escape controls | 02-01/1, 02-02/2; explicit safe byte presentation 02-04/1–2 |
| D-13 optional primary / ordered labelled related / no dummy | 02-03/1–2; location-free and related-only layout 02-04/1–2 |
| D-14 Error/Warning/Note and rendering-independent has_errors | 02-03/1–2; render-failure integration 02-04/2 |
| D-15 bounded plain excerpts/ASCII markers/related locations | 02-04/1–3, exact output and clipping/escaping regressions |
| D-16 insertion order / duplicates / no automatic sorting | 02-03/1–2; ordered rendering integration 02-04/2 |

GSD decision coverage reports **16/16**, no uncovered decision. Post-planning
gap analysis reports **23/23** requirement/decision items covered. Keyword coverage
alone is insufficient; this table links each contract to concrete task behavior.

## Checker dimensions

| Dimension | Result / rationale |
|---|---|
| Requirement coverage | PASS; seven frontmatter IDs with direct task criteria |
| Task completeness | PASS; files, read_first, concrete actions, automated gates, acceptance criteria and done conditions; explicit final checkpoint |
| Dependency correctness | PASS; four sequential producer/consumer waves, no missing API |
| Key-link wiring | PASS; core source lists, two existing-policy native test targets, renderer/source/model links explicitly assigned |
| Scope sanity | PASS; no filesystem/lexer/driver/LLVM/reporting/package framework work; 2–3 tasks and 4–7 implementation files per plan |
| Goal-derived must-haves | PASS; retained/retrieved text and observable diagnostics, not only artifact existence |
| Context/scope/tier compliance | PASS; sixteen settled contracts and Tier 1 compatibility preserved; no scope moved to later phases |
| Nyquist/feedback coverage | PASS as a design contract; VALIDATION exists, every implementation task has native automated feedback and same-task test creation ownership; runtime results/latency still unmeasured |
| Cross-plan data contracts | PASS; owned immutable originals remain raw; only renderer output escapes; source locations remain byte-based |
| AGENTS/lifecycle compliance | PASS; planning-only files; no automatic commit, execution, publication or phase movement; watchdog and sensitive-review gates retained for future execution |
| Research resolution | PASS; all five bounded open implementation topics explicitly proposed below; no std::expected/toolchain upgrade |
| Pattern compliance | PASS; namespace/include/core/GTest/PRE_TEST and accepted shared-command reuse; no invented analog implementation |
| Verify format / numeric authority | PASS; selected build directory/configuration explicit, include checker has --root; actual future test counts required; renderer limits are proposals, not measured/accepted prior facts |

Cold/package work is not a new Phase 2 validation prerequisite. Reuse accepted
prepared dependencies and existing CI snapshot behavior; if preparation is not
available, stop for owner review before package work. A cache hit is never proof
of source/diagnostic/native acceptance. Existing healthy quiet-command liveness
policy remains intact.

## Explicit implementation proposals requiring owner plan review

These fill research's bounded open details without changing D-01–D-16:

1. Strong uint64 manager-local ID, size_t byte offsets and zero sentinel; explicit
   capacity headroom and checked source-error precedence/payloads. Local variant
   aliases accommodate the reviewed Ubuntu Clang 18/GCC-13 pair without expected,
   a generic Result library or macro/toolchain changes.
2. Noncopyable/nonmovable manager with completed heap-stable immutable records;
   private numeric capacity checker with a narrow friend test accessor, not a
   production synthetic-capacity mode. No reset/erase, caller lifetime dependency,
   filesystem identity or cross-manager detection promise.
3. Immutable LF-derived line index, checked point/containing-line access, complete
   original line bytes including terminators; no public mutable source record.
4. Owned diagnostic values/engine, read-only collection view until next emit;
   typed checked renderer failure for primary or a specific related index. Returned
   text owns its bytes; actual I/O/delivery belongs to the future caller, with no
   new output framework or Veyr OOM/error semantics.
5. Exact renderer header/related layout; three source lines / eighty original
   bytes per line / bounded escaped cells and markers. Explicit ASCII byte
   escaping and byte-boundary cell mapping keep control injection and tab/high-byte
   positioning out of SourceLocation. Stored display metadata remains unchanged;
   escaping is a representation, not path normalization or final source encoding.
   Related-only diagnostics retain related labels without fabricating a primary.

No numeric renderer limit was owner-selected during discussion/research; these
numbers are visibly proposed by this planning packet. No new language decision
or DECISION_LOG edit is required.

## Checks performed and evidence limits

- GSD `verify plan-structure`: all four plans valid, zero errors/warnings; task
  counts 2/2/2/3. Required frontmatter, tasks and must-have links present.
- GSD `check decision-coverage-plan`: all 16 decisions covered.
- GSD `check gap-analysis.plan-post`: all 23 items covered, no blocking gap.
- Document-only frontmatter/task/dependency/requirement/scope/whitespace review.
- GSD drift precheck: skipped because no codebase STRUCTURE.md exists; no claim
  of an independent codebase-drift audit. Schema/database hook not applicable.
- GSD warns that custom `veyr_policy` config is unknown to its schema. Repository
  owner/lifecycle rules were enforced directly; config was not changed.

During inline review, corrected the future include-check command to require
`--root .`, made capacity-test access concrete and added final checkpoint review/
resume instructions. These were planning refinements, not reopened discussion
choices or implementation fixes. No outstanding blocking inconsistency found.
Runtime compatibility, marker behavior and test counts remain future execution
claims until actual commands run; planning check success does not prove them.

## Owner boundary

Ready for owner review and, separately, a planning-only commit of exactly:

- .planning/ROADMAP.md
- .planning/STATE.md
- this phase's 02-01-PLAN.md through 02-04-PLAN.md
- this phase's 02-PATTERNS.md, 02-VALIDATION.md and 02-PLAN-REVIEW.md

Suggested commit message: `docs: plan Phase 2 source and diagnostics foundation`.
No commit is authorized by `$gsd-plan-phase 2`. After that reviewed boundary, a
separate explicit `$gsd-execute-phase 2` is required. Do not start Phase 3.

Platform impact:
- Windows: Tier 1 native CI evidence is required for new code; developer-host evidence remains independently pending and non-blocking. No accepted DevShell/CRT/compiler/linker policy change.
- Linux: Primary Omarchy development and separate Ubuntu CI Debug/Release validation. No compiler/library/profile change.
- macOS future: Deferred; APIs/presentation remain standard-C++ and platform-independent without asserting Darwin validation.
