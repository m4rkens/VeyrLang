# Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer - Context

**Gathered:** 2026-10-15
**Status:** Discussion captured; awaiting owner review and a separately approved discussion commit. Research, planning and execution have not started.

<domain>
## Phase Boundary

Deliver LEX-01 through LEX-06: an owner-reviewed milestone-local smoke profile,
minimal kind/span tokens and recovered lexing with precise lexical diagnostics.
P01 in docs/design/DESIGN_PROBES.md forces one root source/module, one fixed
entry and one integer-literal return toward an eventual native exit-code check.
PLAT_01/PLAT_03 constrain exact source identity, byte positions and portable
diagnostics. This phase implements no parser, AST, semantic analysis, lowering,
backend or compiler-driver source-loading path.

Phase 1 remains closed at 8/8 and Phase 2 at 4/4. Their final accepted runs are
37826767824 and 38722486719 respectively. Phase 2 closeout was published in
fc208b4b200076fc210c75535cbe9e30f64d3c26. Preserve its immutable source and
structured diagnostic contracts; do not reopen accepted architecture without a
concrete regression or missing requirement.

The selected spelling and result contract below are M1-only, non-final choices.
Phase 3 must separately produce docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md
from the existing template. The owner must review it and explicitly authorize
its dedicated artifact commit before syntax-dependent implementation/tests.
This discussion records its decisions; it does not create that profile or grant
research, planning, implementation, commit, publication or CI authorization.

Linux is the primary developer host; Ubuntu CI is independent. Windows remains
Tier 1 compatibility/CI, with developer-host inspection independently pending
and non-blocking. macOS is deferred. Historical CI does not validate new lexer
code that has not been implemented.
</domain>

<decisions>
## Implementation Decisions

### Smoke result contract

- **D-01:** M1 accepts return values 0–255. The small range supports identical
  expected exit-code checks on Windows/Linux. Decimal token recognition remains
  lexical; range validation belongs to the later semantic phase. This accepts
  no final Veyr integer-range or general arithmetic policy.
- **D-02:** The M1 entry result has signed 32-bit integer semantics, with its
  accepted values restricted to 0–255. Later lowering/backend work has a concrete
  result width; no stable ABI or final type syntax is accepted here.

### Provisional spelling

- **D-03:** Use `fn main` as the M1-only introducer and fixed entry name.
  General functions remain outside M1; this is not final function syntax.
- **D-04:** Spell the selected result type `int32`, following the existing
  type-system document's naming direction for this experimental profile.
- **D-05:** The signature is `fn main() -> int32`: fixed entry, empty parameter
  list, explicit result annotation. M1 accepts only this entry signature.
- **D-06:** The body uses braces, `return`, one integer literal and a mandatory
  semicolon, as illustrated by `{ return 42; }`. The illustration does not fix
  the returned value to 42; D-01 defines the accepted semantic range.

### Lexical boundaries

- **D-07:** Skip only space (0x20), TAB (0x09), LF (0x0A) and CR (0x0D).
  Preserve source bytes. Phase 2 location mapping still treats lone CR as an
  ordinary byte; the lexer may skip it without changing that mapping contract.
- **D-08:** Match `fn`, `main`, `int32` and `return` exactly in lowercase.
  No ASCII case-insensitive matching is selected.
- **D-09:** Match complete ASCII words. Scan an ASCII letter/underscore-led run
  containing subsequent ASCII letters, digits or underscores, then recognize
  only the four fixed spellings. `return42` and `fnmain` are unsupported M1
  words. This scan class groups input; it does not add general identifiers.
- **D-10:** `->` is one contiguous two-byte token. Whitespace may surround it;
  `- >` is unsupported. This defines M1 signature punctuation only.

### Recovery and diagnostics

- **D-11:** Retain offending runs as explicit Invalid tokens, in addition to
  Error diagnostics. Preserve valid recovered tokens and original token order;
  the stream ends with explicit EOF. This was the owner's selection of option 2,
  rather than the suggested valid-token-only recovery. Token values still contain
  only TokenKind and SourceSpan. Accumulated Errors prevent successful compilation.
- **D-12:** Consume a whole unsupported numeric spelling as one offending run,
  emitting one Invalid token and one Error. Preserve surrounding program
  delimiters instead of returning apparently valid digit fragments from the
  rejected spelling. Discussed examples: `-12`, `0x10`, `12_3`, `12u`, `1.5`.
  Signs, prefixes, separators, suffixes and non-decimal forms remain unsupported.
- **D-13:** Group consecutive non-ASCII bytes (adjacent bytes 0x80–0xFF) as one
  Invalid token and one Error. Preserve exact original-byte spans. Perform no
  Unicode decoding; final source-encoding policy remains open.
- **D-14:** Group comment-shaped input for rejection. `//...` ends before
  LF/CRLF or at EOF; `/*...*/` ends at the first `*/`. An unterminated block runs
  to EOF. Emit one Invalid token and one Error per run. Comments remain
  unsupported and are never silently accepted as trivia. This is recovery
  grouping, not a general comment feature.

### Inherited constraints and implementation discretion

The owner selected every substantive preference explicitly. No answer delegated
a listed preference to the agent. Research/planning may choose implementation
details within the selected contracts; they must not silently change the choices.

Already locked by the roadmap/template and prior phase, not newly reopened:

- Integer token shape is one or more unsigned ASCII decimal digits. Token values
  store only kind/span; lexemes are recovered through SourceManager. No eager
  semantic value/range check or copied lexeme is added to the token model.
- Every recovered stream, including empty/error-only input, ends with an explicit
  zero-width source-end EOF span. Recovery consumes the smallest clearly related
  offending run and makes progress; surrounding independent input remains available.
- SourceManager owns immutable snapshots; IDs are fresh and manager-local. Use
  the originating manager. Views survive registration, not manager destruction;
  coincident foreign-manager numeric IDs are not automatically origin-detected.
- Original half-open byte spans, checked distinct SourceError payloads and
  LF/CRLF/lone-CR/EOF location rules remain unchanged. Invalid infrastructure
  inputs are checked failures, not fabricated source-language diagnostics.
- Reuse owned Diagnostic values and the ordered DiagnosticEngine. Initialize
  severity explicitly. Engine views must be reacquired after emission. Rendering
  remains a separate consumer, with no lexer dependency on LLVM or driver state.
- Diagnostic wording identifies M1 Experimental Smoke Profile limitations rather
  than claiming final Veyr syntax, integer-literal or source-encoding rejection.

The later reviewed profile/research/plan must make deterministic recovery
boundaries and precedence precise for combinations beyond the discussed examples,
while honoring D-09 through D-14, minimum offending-run recovery and delimiter
preservation. Exact API/result shapes, enum labels, files/targets, implementation
algorithms and bounded progress checks are research/planning details. Any concrete
contradiction must return to owner review before dependent implementation.

No final warning/suppression policy, lexer framework, new dependency, filesystem
loader, encoding layer, module graph or language package subsystem is authorized.
Conan 2.33.0, LLVM 23.1.2 and reviewed build/profile/lock/CI policy remain fixed.
</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read these before research, planning or implementation.
Explicit owner decisions and docs/design/DECISION_LOG.md take precedence.
No external technical specification or Phase 3 SPEC was introduced.

### Phase scope and profile authority

- `AGENTS.md` — lifecycle gates, compiler boundaries and mandatory profile commit.
- `.planning/PROJECT.md` — M1 mission and validated foundation; its closeout text
  is the historical pre-publication snapshot, superseded by the recorded commit.
- `.planning/REQUIREMENTS.md` — LEX-01 through LEX-06; later phases own parser,
  semantic checking, lowering, backend, driver and end-to-end implementation.
- `.planning/ROADMAP.md` — Phase 3 success criteria, scope and profile gate.
- `.planning/phases/02-source-identity-spans-and-diagnostics/02-CONTEXT.md` — all
  sixteen accepted source/diagnostic contracts.
- `.planning/phases/02-source-identity-spans-and-diagnostics/02-04-SUMMARY.md` —
  accepted source-to-renderer foundation and evidence limitations.
- `.planning/phases/02-source-identity-spans-and-diagnostics/02-VERIFICATION.md` —
  canonical goal verification and preserved follow-ups/platform distinctions.
- `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE_TEMPLATE.md` — source
  template; the filled profile does not yet exist and requires separate review.
- `docs/design/milestones/MILESTONE_1.md` — walking-skeleton shape/non-goals.
- `docs/design/milestones/MILESTONE_STRATEGY.md` — probe-driven scope; strategy
  numbers do not supersede the active ten-phase GSD roadmap.
- `docs/design/DESIGN_PROBES.md` — P01 forces this smallest native smoke path.

### Design and platform constraints

- `docs/design/DECISION_LOG.md` — milestone-local profile and single-root rules;
  examples do not accept final grammar.
- `docs/design/VISION.md` — compiler pipeline and diagnostics direction.
- `docs/design/FEATURE_CATALOG.md` — long-term directions, not M1 scope expansion.
- `docs/design/TYPE_SYSTEM.md` — integer/type naming direction; general types and
  contextual typing are outside this phase's implementation.
- `docs/design/MEMORY_MODEL.md` — future language ownership is distinct from
  compiler buffer/value ownership.
- `docs/design/MODULE_SYSTEM.md` — SourceId is not a full module/package model.
- `docs/design/OPEN_QUESTIONS.md` — unresolved encoding and broader language policy.
- `docs/design/tracks/projects/platform/PLAT_01_PATHS_FILESYSTEM_CASES.md` — source
  identity/display names without introducing general filesystem semantics.
- `docs/design/tracks/projects/platform/PLAT_03_TERMINAL_DIAGNOSTICS.md` — portable
  plain output without final Unicode/theme/console policy.

### Reuse and workflow

- `include/veyr/source/source_manager.hpp` — checked immutable source APIs.
- `include/veyr/source/source_span.hpp` — original half-open byte spans.
- `include/veyr/diagnostics/diagnostic.hpp` — owning structured values; explicit severity.
- `include/veyr/diagnostics/diagnostic_engine.hpp` — emission order and view lifetime.
- `include/veyr/diagnostics/plain_text_renderer.hpp` — separate checked presentation.
- `CMakeLists.txt` — existing veyrcore target and common build policy.
- `tests/CMakeLists.txt` — native test wiring and PRE_TEST discovery.
- `tests/source/source_manager_test.cpp` — byte-boundary, checked failure and lifetime analogs.
- `tests/diagnostics/diagnostic_test.cpp` — ownership/order/error-state analogs.
- `docs/development/GSD_WORKFLOW.md` — discussion/review/commit before later stages.
- `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md` — exact commands and no automatic progression.
- `docs/development/BUILD_POLICY.md` — C++23/Conan foundation.
- `docs/development/DEVELOPER_BOOTSTRAP.md` — existing prepared dependency commands.
- `docs/development/INCLUDE_POLICY.md` — canonical <veyr/...> owned includes.
- `docs/development/CROSS_PLATFORM_POLICY.md` — mandatory Windows/Linux and deferred macOS.
- `docs/development/COMMIT_POLICY.md` — explicit stage commit approval.
- `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md` — later sensitive-file review.
- `docs/development/SESSION_HANDOFF_POLICY.md` — repository-based context recovery.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- SourceManager supplies immutable text, checked span retrieval and exact byte
  locations; lexer input can retain one manager-local snapshot identity.
- SourceSpan and SourceId provide lightweight values for tokens; no duplicate
  lexeme storage or filesystem identity mechanism is needed.
- Diagnostic/DiagnosticEngine supply owned Error messages, ordered accumulation
  and rendering-independent has_errors. Existing rendering can present lexer
  diagnostics without becoming a lexer dependency.
- Native source/diagnostic tests and CMake integration provide established
  checked-error, ownership, byte-fixture and PRE_TEST discovery patterns.

### Established Patterns and Integration Points

- No token model, lexer or filled M1 profile exists at discussion capture.
- Source → diagnostics → lexer remains the dependency direction; lexer cannot
  depend on parser, AST, semantics, backend or driver components.
- Existing veyrcore and test target conventions may be extended after explicit
  execution authorization. No CMake/source/test changes occur during discussion.
- Reuse prepared dependencies and the reviewed Omarchy/Ubuntu/Windows toolchains;
  no build/cache/bootstrap changes are required by these discussion decisions.
</code_context>

<specifics>
## Specific Ideas

Selected provisional example, recording discussion only:

```text
fn main() -> int32 {
    return 42;
}
```

The signed 32-bit result contract accepts 0 through 255. A decimal spelling such
as `256` still has valid integer token shape; its range rejection belongs to
later semantic work. Unsupported numeric examples above require Invalid tokens
and lexical Errors. Future profile/test naming must clearly identify the
experimental M1 profile rather than final Veyr grammar.
</specifics>

<deferred>
## Deferred Ideas

No new capability was proposed outside the phase scope. Final grammar, general
identifiers/functions/expressions, comments, Unicode/encoding, modules/packages
and later compiler stages retain their existing deferrals.

Preserve the three non-blocking Phase 2 follow-ups separately: explicit severity
initialization enforcement, combined left-clipping/mixed-escape/empty-span
coverage, and unsupported initialized-severity documentation/tests. They are not
implicitly authorized as Phase 3 tasks. Current diagnostic callers still initialize
severity explicitly.
</deferred>

Platform impact:
- Windows: Same M1 byte/token/recovery contract and eventual 0–255 exit-code checks;
  hosted compatibility remains Tier 1, developer-host inspection independently pending.
- Linux: Primary Omarchy development; independent Ubuntu CI. Lexer rules use
  explicit ASCII bytes and preserve source-location mapping.
- macOS future: Deferred; this profile introduces no required POSIX/Win32 or terminal API.

---
*Phase: 03-m1-experimental-smoke-profile-tokens-and-lexer*
*Context gathered: 2026-10-15; all fourteen substantive decisions explicitly selected.*
