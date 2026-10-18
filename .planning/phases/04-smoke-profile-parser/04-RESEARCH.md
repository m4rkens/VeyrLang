# Phase 4: Smoke Profile Parser - Research

**Researched:** 2026-10-18
**Domain:** C++23 compiler-library fixed M1 parser, checked token streams and source-linked diagnostics
**Confidence:** MEDIUM (official-web provider seam; direct repository inspection)

<user_constraints>
## User Constraints (from CONTEXT.md)

The following decision and deferred sections are copied verbatim from the accepted context. Bounded recommendations later in this document are implementation guidance, not additional binding discussion decisions. [VERIFIED: .planning/phases/04-smoke-profile-parser/04-CONTEXT.md]

### Locked Decisions

<!-- DATA_a12e9f6b_START -->
## Implementation Decisions

### Parse result and source relationships

- **D-01:** Return an independent value owning its minimal structural data and
  original SourceSpans. It must remain usable after input token storage is
  destroyed. Retain no borrowed token references or copied source text.
  The originating SourceManager must remain alive for source-text resolution.
- **D-02:** Retain three structural spans: the complete fixed entry, its return
  statement and the integer literal. This supports diagnostics and the later
  AST handoff without introducing a general syntax tree or numeric value.
- **D-03:** Use tight syntax bounds: entry starts at `fn` and ends after `}`;
  return statement starts at `return` and ends after `;`; the literal retains
  its exact token span. Internal whitespace remains within structural spans;
  surrounding whitespace remains available through SourceManager. All offsets
  are original zero-based half-open byte ranges; no normalization occurs.
- **D-04:** The minimal parse result is ordinarily copyable and movable.
  Copying spans creates no source/token ownership dependency. This is C++
  infrastructure policy, not a Veyr language ownership decision.

### Syntax failure and recovery

- **D-05:** For a structurally valid, lexically valid stream, stop at the first
  syntax error and append one precise parser Error. Do not synchronize at later
  delimiters, backtrack to repair the program or continue emitting cascades.
  The lexer's independently accepted recovery behavior remains unchanged.
- **D-06:** A failed parse exposes no partial structural result. Return the
  complete entry/return/literal representation only after recognizing the
  entire required M1 shape and its final EOF; trailing tokens prevent a result.
- **D-07:** Ordinary syntax failure is an explicit checked outcome accompanied
  by its DiagnosticEngine Error, not a C++ syntax exception. Keep infrastructure
  failure distinct. Ordinary allocation-exception behavior is unchanged; no
  new rollback or language exception/OOM policy is introduced.

### Lexical-error handoff and checked inputs

- **D-08:** After structural validation, any Invalid token prevents syntax
  parsing of that stream. Return explicit failure with no parse representation,
  preserve the existing lexical diagnostics and add no parser Errors for that
  stream. This avoids duplicate complaints and misleading recovery cascades.
- **D-09:** An earlier Error already in the engine does not prevent parsing a
  separate well-formed stream without Invalid tokens. That stream may produce
  a complete parse result; earlier diagnostics and Error state remain intact.
  Such a result establishes this input's syntax only, never compilation success.
  Any accumulated Error still prevents a successful compilation handoff.
- **D-10:** The caller invokes `lex()` and supplies completed tokens, the
  originating SourceManager and DiagnosticEngine. Parsing reads the token
  stream synchronously, retains no borrowed token references and does not
  invoke lexing internally. Do not alter existing Token/Lexer or Phase 2 APIs.
- **D-11:** Check token-stream structure before syntax parsing or Invalid-token
  rejection: source identity, checked span bounds, supported token-kind values,
  source-ordered/nonoverlapping nonempty non-EOF spans and exactly one final
  zero-width EOF at source end. All tokens belong to the selected manager-local
  source. Malformed structure returns a distinct checked infrastructure failure,
  not a source-language diagnostic. Preserve original SourceError details where
  applicable; never clamp, fabricate source text or rely only on assertions.

### Parser diagnostic contract

- **D-12:** Syntax Errors identify the expected spelling and found token using
  controlled labels, explicitly naming the `M1 Experimental Smoke Profile`.
  For example: `expected ';', found '}' in M1 Experimental Smoke Profile`.
  Do not interpolate untrusted raw source bytes. Rendering remains a separate
  consumer of existing owning Diagnostic values; explicitly initialize severity.
- **D-13:** An unexpected real token supplies its complete original token span
  as the primary span, including when a required token was missing before it.
  Do not substitute an empty insertion span for that real unexpected token.
- **D-14:** Incomplete input uses the exact EOF primary span
  `[source_size,source_size)` and reports the expected spelling with
  `found end of source`. Empty input uses `[0,0)`. Existing SourceManager
  LF/CRLF/lone-CR/EOF line and byte-column rules apply unchanged.
- **D-15:** An Error expecting `)` or `}` includes exactly one related span
  pointing to its already recognized matching opener, with a controlled label
  such as `opening '{' is here`. Other syntax Errors have no related spans.
  This remains one Error, not a separate Note or additional diagnostic.

### Token/source consistency boundary

- **D-16:** Require the accepted lexer or an equivalent conforming producer.
  Token classification, spelling and lexical grouping/boundaries are producer
  preconditions, not a second lexical-recognition responsibility of the parser.
  Runtime structural checks in D-11 remain required. A caller-created `Fn`
  token spanning the bytes `return` violates the producer contract; checked
  rejection of every such forged combination is not promised. Additional
  kind/spelling checks were not selected. Do not re-lex or duplicate lexer rules.

### Inherited constraints and implementation discretion

All sixteen decisions were explicitly selected; no substantive choice was
delegated through a "you decide" answer. Exact public C++ type/function names,
checked-outcome representation, infrastructure-error taxonomy and deterministic
ordering among multiple structural violations remain research/planning details
within these decisions. Full per-position controlled message/label tables must
be made precise in the owner-reviewed plan; examples here establish the selected
message style, not an unreviewed expanded grammar or rendering format.

Preserve all fourteen Phase 3 decisions and the published profile, including
unsupported numeric/comment/high-byte recovery and the `/*/return` boundary.
SourceManager owns immutable snapshots; IDs are fresh and manager-local.
Coincident numeric IDs from foreign managers cannot be origin-detected by the
existing API. Do not silently promise that structural checks solve that limitation.
Preserve checked original-byte SourceError payloads and exact spans/locations.
Diagnostics own their strings and retain insertion order, duplicates and Error
state. Reacquire borrowed diagnostic views after every emission; store none
across emit or beyond the engine's lifetime. Rendering remains independent.

No new dependency, generator/framework, general parser/AST design, filesystem,
terminal or platform-native behavior is selected. Conan, LLVM, reviewed toolchain,
profile/lock, build, cache and CI policies remain unchanged. Any concrete conflict
with the accepted architecture returns to the owner before dependent execution.
<!-- DATA_a12e9f6b_END -->

### the agent's Discretion

The context places implementation discretion inside its inherited-constraints subsection; the exact relevant paragraph follows. [VERIFIED: 04-CONTEXT.md inherited constraints]

<!-- DATA_f81b26c4_START -->
All sixteen decisions were explicitly selected; no substantive choice was
delegated through a "you decide" answer. Exact public C++ type/function names,
checked-outcome representation, infrastructure-error taxonomy and deterministic
ordering among multiple structural violations remain research/planning details
within these decisions. Full per-position controlled message/label tables must
be made precise in the owner-reviewed plan; examples here establish the selected
message style, not an unreviewed expanded grammar or rendering format.
<!-- DATA_f81b26c4_END -->

### Deferred Ideas (OUT OF SCOPE)

<!-- DATA_c42d9e51_START -->
## Deferred Ideas

No new out-of-scope capability was proposed. General parser recovery, partial
results, exception-based syntax failure, internal re-lexing and kind/spelling
guards were alternatives not selected; they are not authorized future tasks.
Phase 5 owns AST consolidation and semantic decimal/range validation. Broader
syntax, type/ABI design, imports, module graphs, backend/driver and tooling remain
outside this phase. Preserve the non-blocking diagnostic severity-initialization,
combined renderer-clipping, unsupported-severity and optional loop-trace follow-ups
without implementing them during this discussion.
<!-- DATA_c42d9e51_END -->
</user_constraints>

## Summary

The accepted M1 profile requires eleven non-EOF units in one fixed order, followed by EOF. Existing Token, SourceManager, DiagnosticEngine and renderer APIs already provide the needed kinds, original-byte spans, checked source errors and owned diagnostics. No parser implementation exists in the inspected include/src/tests trees. [VERIFIED: docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md sections 3–5; include/veyr/lexing/token.hpp; include/veyr/source/source_manager.hpp; include/veyr/diagnostics/diagnostic_engine.hpp; rg --files include src tests]

**Primary recommendation:** Implement a small fixed-sequence recognizer with three explicit stages: checked whole-stream structural validation, lexical-invalid rejection, then first-mismatch syntax recognition. Publish an independent three-span value only after the closing brace and final EOF match. Keep ordinary syntax rejection, lexical rejection, original SourceError and malformed token-stream errors distinguishable. This is a bounded implementation recommendation derived from D-01–D-16, not a new language decision. [VERIFIED: 04-CONTEXT.md D-01–D-16]

Use the existing C++23/veyrcore/GoogleTest integration and add no packages, generator, general parser machinery, numeric conversion, AST hierarchy, driver behavior or CI changes. Phase 5 owns durable AST consolidation and decimal/range semantics; retain the three source relationships so it can consume this parser rather than introduce a second parser. Research found no technical conflict requiring owner design review before planning. Proposed names, error precedence and complete wording below remain planning details for owner review. [VERIFIED: .planning/ROADMAP.md Phase 4/5; .planning/REQUIREMENTS.md PARSE-01–03, AST-01–02, SEMA-01–02; 04-CONTEXT.md inherited constraints]

## Architectural Responsibility Map

The project is an in-process compiler library, so web/client/server tiers do not apply. Use the following compiler-tier ownership. [VERIFIED: AGENTS.md section 7; .planning/PROJECT.md; 04-CONTEXT.md D-10]

| Capability | Primary Tier | Secondary Tier | Rationale / evidence |
|---|---|---|---|
| Immutable bytes, checked spans and source-end size | Source substrate | Parser consumes | Existing SourceManager API; never duplicate storage or location mapping. [VERIFIED: include/veyr/source/source_manager.hpp] |
| Classification, spelling and lexical grouping | Lexer / conforming producer | Caller invokes lex | Parser trusts classification after structural checks; no parser dependency enters lexer. [VERIFIED: 04-CONTEXT.md D-10/D-16; src/lexing/lexer.cpp] |
| Stream integrity and fixed M1 syntax | Parser | Source validation | Own selected-source agreement, supported enums, ordering and EOF checks, then fixed sequence. [VERIFIED: 04-CONTEXT.md D-05–D-11] |
| Complete three-span result | Parser public value | Phase 5 future consumer | Exactly entry, return and integer spans; no token/source ownership or numeric value. [VERIFIED: 04-CONTEXT.md D-01–D-04; .planning/ROADMAP.md Phase 5] |
| Diagnostic accumulation | DiagnosticEngine | Parser emits | Engine preserves ordered owned data and prior Error state. [VERIFIED: src/diagnostics/diagnostic_engine.cpp] |
| Escaped presentation and line/column mapping | Existing renderer / SourceManager | Caller | Parser emits structured diagnostics only. [VERIFIED: include/veyr/diagnostics/plain_text_renderer.hpp; src/source/source_manager.cpp] |
| Compilation-success decision | Later compiler client / driver | Engine state | A complete syntax result cannot override accumulated Errors; no driver is implemented here. [VERIFIED: 04-CONTEXT.md D-09; .planning/ROADMAP.md Phase 9] |

<phase_requirements>
## Phase Requirements

The requirement descriptions below are verbatim; support is the research recommendation grounded in the accepted context. [VERIFIED: .planning/REQUIREMENTS.md Smoke parser]

<!-- DATA_4b7d291e_START -->
| ID | Description | Research Support |
|---|---|---|
| PARSE-01 | Parser can recognize exactly one profile-defined fixed entry containing one integer-literal return followed by EOF. | Fixed token order and final-EOF check; exact positive/boundary tables and no numeric conversion. |
| PARSE-02 | User can receive source-linked parser diagnostics for malformed, incomplete, trailing, or unsupported input without treating recovery as successful compilation. | Three-stage gating, explicit rejection variants, controlled expected/found table, exact primary and opener spans, lexical-error suppression and persistent Error-state integration. |
| PARSE-03 | Compiler client can consume a minimal source-linked parse representation without lexer dependencies on parser, AST, or semantic components. | Copyable/movable independent three-span value, token-lifetime tests and one-way dependency inspection. |
<!-- DATA_4b7d291e_END -->
</phase_requirements>

## Project Constraints (from AGENTS.md)

These actionable directives constrain the plans; the source for every item in this list is AGENTS.md and its mandatory policy references. [VERIFIED: AGENTS.md sections 1–10; docs/development/GSD_WORKFLOW.md; docs/development/COMMIT_POLICY.md]

- Preserve the accepted names Veyr, veyrc, veyr, veyrcore, veyr-lsp, veyrfmt, veyr-analyze, veyr-doc, veyr-bindgen, veyr-gpu-bindgen, .vyr, Veyr.toml and Veyr.lock. Do not rename without an explicit owner decision.
- Read applicable canonical design, development, tooling and project-system documents; explicit owner decisions and DECISION_LOG win conflicts. Repository files and actual Git state are canonical, not another chat's hidden state.
- Discussion captures decisions only. Planning writes .planning/ or explicitly requested planning documents only; do not create source/tests/CMake/generated implementation or run CMake, Ninja, CTest, compilers, package operations, formatters or verification.
- Begin execution only after the concrete owner command `$gsd-execute-phase 4`. Do not auto-transition, use progress-next, infer lifecycle permission from ambiguous approval or create automatic stage commits.
- Do not stage, commit or push this research. Later reviewed stage commits remain separately authorized, and sensitive implementation/build/test/script/manifest commits require the explicit owner review packet and approval. Inspect actual status/diff before a future authorized commit; avoid broad staging, microscopic commits and vague messages.
- Use local skills when triggered and announce them. Skills/helper roles do not grant execution, verification, retry, cleanup, commit or lifecycle permission. Helper roles announce scope/authority. Subagent use remains separately owner-gated under this repository's planning configuration.
- Rehydrate from canonical files, status and recent commits after actual environment/session/worktree/tool changes, produce the handoff summary and obtain the concrete next command. This researcher is operating inside the explicitly authorized existing planning command.
- Use C++23, CMake/Ninja, Clang, GoogleTest/GoogleMock, Conan 2 and the fixed LLVM 23.1.2 foundation. Preserve managed/system/custom LLVM discovery through `find_package(LLVM CONFIG)`, cache reuse and existing reviewed profiles/locks. Do not recreate package acquisition, solving, storage or SDK normalization.
- Keep development compile databases enabled. All Veyr-owned includes use canonical `<veyr/...>` paths; no quoted or relative owned-header includes.
- Preserve source → diagnostics → lexer → parser → AST → semantics → lowering → LLVM → driver separation. Lexer must never depend on parser/AST/semantics; diagnostics must not depend on LLVM; source/span logic must not depend on the driver.
- Do not silently decide final syntax, ownership/borrowing/lifetimes, ABI, async, OOM, reflection, macros, packages, module graphs or manifest semantics. Candidate examples are not accepted final grammar.
- Apply probe-first design discipline: real project → probe → semantic problem → alternatives → selected semantics → interactions → syntax → valid/invalid examples → diagnostics → lowering → tests → decision record. P01 alone forces this parser.
- Preserve the separately published M1 experimental profile; its fixed single-root entry and integer return do not accept final Veyr grammar/type/ABI. Do not import old lifecycle artifacts, accidental tests or implementation written during planning.
- Windows and Linux remain Tier 1. Include Windows/Linux/macOS-future impact notes for build/test/platform-sensitive planning. Do not introduce hardcoded native suffixes, path separators, case assumptions, terminal or OS behavior without platform gating; macOS remains deferred and must remain feasible.
- During later authorized long commands, announce exact command/cwd/effects, run one command at a time and supervise liveness. About 60 seconds of silence is a soft checkpoint, not failure. Stop on actual errors/conflicts/interruption/hard timeout, never launch duplicate retries or destructive cleanup automatically.

**Applied skills:** veyr-design-probe-planner for the P01-only boundary, veyr-cross-platform-impact for portable byte/diagnostic and build-test integration, and veyr-gsd-phase-gate for preserving the active planning boundary. All eight local SKILL.md files were inspected for applicable constraints; no native probe ran. [VERIFIED: .codex/skills/*/SKILL.md; research command history]

**Design probe planning packet:** Project pressure P01; minimal task is syntactic recognition of the already reviewed entry; capability forced is checked token-to-three-span transformation; no unresolved semantic choice is introduced; non-goals are general grammar/AST/numeric semantics/driver/backend; M1 fit is Phase 4 only; no DECISION_LOG change is recommended. [VERIFIED: docs/design/DESIGN_PROBES.md P01; docs/design/milestones/MILESTONE_1.md; 04-CONTEXT.md]

## Standard Stack

### Core

Reuse exact accepted versions and APIs, rather than installing or upgrading anything. These are repository dependency requirements, not claims about the latest registry release. [VERIFIED: AGENTS.md section 6; conanfile.py; CMakeLists.txt; tests/CMakeLists.txt]

| Component | Version / contract | Purpose | Evidence |
|---|---|---|---|
| C++ | C++23 | Value types, std::span, std::variant, std::optional | `cxx_std_23` target policy. [VERIFIED: cmake/VeyrTargets.cmake] |
| veyrcore | Existing static target | Own parser implementation and public header | Root source list and public include root. [VERIFIED: CMakeLists.txt] |
| Token / TokenKind | Existing 13-kind kind/span value | Caller-supplied completed stream | No lexeme or numeric value. [VERIFIED: include/veyr/lexing/token.hpp] |
| SourceManager / SourceError | Existing Phase 2 API | Exact checked span/source validation | UnknownSource, ReversedRange, BeyondSource and complete payloads. [VERIFIED: include/veyr/source/source_manager.hpp; src/source/source_manager.cpp] |
| DiagnosticEngine / Diagnostic | Existing Phase 2 API | Owned Error and related opener | Explicit severity initialization; no engine reset. [VERIFIED: include/veyr/diagnostics/diagnostic.hpp; src/diagnostics/diagnostic_engine.cpp] |

### Supporting

| Component | Accepted version | Purpose / when used | Evidence |
|---|---|---|---|
| GoogleTest / GoogleMock | 1.18.0 | Native unit and real-lexer integration tests; existing framework | `gtest/1.18.0` and exact CMake find. [VERIFIED: conanfile.py; tests/CMakeLists.txt] |
| CMake / Ninja | CMake ≥3.31; reviewed Ninja baseline | Add parser source/test target with existing policies and PRE_TEST discovery | [VERIFIED: CMakeLists.txt; tests/CMakeLists.txt; scripts/devtools/baseline.json] |
| Conan / LLVM | Conan 2.33.0; LLVM 23.1.2 | Existing build prerequisites only; parser includes no LLVM headers | [VERIFIED: docs/development/DEVELOPER_BOOTSTRAP.md; conanfile.py; 04-CONTEXT.md inherited constraints] |
| PlainTextRenderer | Existing checked API | Separate integration consumer of actual parser diagnostics | [VERIFIED: include/veyr/diagnostics/plain_text_renderer.hpp] |

**Installation:** None. New package legitimacy/registry/latest-version/publish-date checks are inapplicable because this phase adds no dependency and dependency operations are forbidden in planning. Existing exact dependency declarations and generated GTestConfigVersion.cmake were inspected read-only; no cache refresh or package operation occurred. [VERIFIED: 04-CONTEXT.md inherited constraints; conanfile.py; build/linux-x86_64-gnu/managed/linux-x86_64-clang-linux-developer-3543ddbc0bf7/Debug/generators/GTestConfigVersion.cmake]

## Package Legitimacy Audit

Not applicable: no external package is proposed for installation, replacement or upgrade. Reuse the accepted Conan-owned dependency graph unchanged. Packages removed: none. Packages flagged SUS: none assessed or introduced; this is not a new audit verdict for existing dependencies. [VERIFIED: 04-CONTEXT.md inherited constraints; conanfile.py]

## Architecture Patterns

### System Architecture Diagram

The flow below is a proposed implementation of the binding D-05–D-16 ordering and the existing source/diagnostic boundaries. [VERIFIED: 04-CONTEXT.md D-05–D-16; include/veyr/source/source_manager.hpp; include/veyr/diagnostics/diagnostic_engine.hpp]

~~~mermaid
flowchart TD
  C[Compiler library caller] --> L[Existing lex or conforming producer]
  S[SourceManager immutable snapshot] --> L
  L --> T[Completed kind/span token stream]
  C --> P[Parser call with selected SourceId and DiagnosticEngine]
  T --> P
  S --> V[Checked whole-stream structural validation]
  P --> V
  V -->|SourceError or malformed stream| I[Checked infrastructure outcome; no emission]
  V -->|Structurally valid| X{Any Invalid token?}
  X -->|Yes| LR[Explicit lexical rejection; no parser emission]
  X -->|No| F[Match fixed M1 sequence through final EOF]
  F -->|First mismatch| D[Append one owning syntax Error]
  D --> SR[Explicit syntax rejection; no partial result]
  F -->|Complete| R[Independent entry/return/integer spans]
  D --> E[Existing DiagnosticEngine]
  E --> RT[Separate PlainTextRenderer]
  S --> RT
  R --> H[Later client combines syntax result with persistent Error state]
  E --> H
~~~

### Recommended Project Structure and Component Responsibilities

These are recommended future execution files, not files created by this research. Source/diagnostic/lexer/renderer public APIs remain unchanged. [VERIFIED: 04-CONTEXT.md D-10/inherited constraints; inspected root/tests CMakeLists.txt]

| File | Action | Responsibilities / likely new symbols |
|---|---|---|
| `include/veyr/parsing/parser.hpp` | New | ParsedSmokeEntry, ParseRejectionKind, ParseRejected, TokenStreamErrorKind, TokenStreamError, ParseResult and parse declaration; document producer/lifetime/error ordering |
| `src/parsing/parser.cpp` | New | Known-kind switch, structural validator, fixed expected/found labels, fixed-sequence matcher and diagnostic emission; private helpers only |
| `tests/parsing/parser_test.cpp` | New | Result values, exact syntax fixtures and malformed checked-stream cases |
| `tests/parsing/parser_integration_test.cpp` | New | Actual lex → parse → renderer, accumulated diagnostics, ownership and source-location contracts |
| `CMakeLists.txt` | Bounded edit | Add src/parsing/parser.cpp to existing veyrcore only |
| `tests/CMakeLists.txt` | Bounded edit | Add parser_tests, apply veyr_target_policy, link veyrcore/GTest::gtest_main/GTest::gmock, use PRE_TEST discovery |

Do not add a second compiler library, parser generator, AST arena, virtual file loader, CLI option, new CMake module, workflow, fixture package or manifest change. Prefer in-memory byte strings over new .vyr fixtures for this library slice. [Recommendation derived from VERIFIED: .planning/ROADMAP.md Phase 4/5/9; 04-CONTEXT.md inherited constraints; existing test patterns]

### Pattern 1: Independent value plus explicit checked alternatives

Use a three-field aggregate for the success value. Keep SourceError directly accessible as its original existing value; use a separate token-integrity error for invariants not represented by SourceError. Distinguish lexical rejection from syntax rejection without pretending either is infrastructure failure. [Recommendation derived from VERIFIED: 04-CONTEXT.md D-01–D-11; include/veyr/lexing/lexer.hpp]

Suggested public API for the plan:

~~~cpp
// Proposed Phase 4 API; derived from accepted context, not existing code.
struct ParsedSmokeEntry {
  SourceSpan entry_span;
  SourceSpan return_span;
  SourceSpan integer_span;

  bool operator==(const ParsedSmokeEntry&) const = default;
};

enum class ParseRejectionKind { LexicalErrors, SyntaxError };
struct ParseRejected {
  ParseRejectionKind kind;
  bool operator==(const ParseRejected&) const = default;
};

enum class TokenStreamErrorKind {
  SourceMismatch,
  UnsupportedTokenKind,
  EmptyTokenSpan,
  OutOfOrderOrOverlappingSpan,
  NonFinalEndOfFile,
  InvalidEndOfFileSpan,
  MissingEndOfFile
};

struct TokenStreamError {
  TokenStreamErrorKind kind;
  SourceId source_id;  // selected source
  std::optional<std::size_t> token_index;  // absent only for MissingEndOfFile
  bool operator==(const TokenStreamError&) const = default;
};

using ParseResult =
    std::variant<ParsedSmokeEntry, ParseRejected, SourceError, TokenStreamError>;

[[nodiscard]] ParseResult parse(const SourceManager& sources, SourceId source_id,
                                std::span<const Token> tokens,
                                DiagnosticEngine& diagnostics);
~~~

Use owned-header angle includes plus direct standard includes in the new header. Do not expose a pointer/reference/view into input tokens or source text, a numeric field, AST base class, general statement list or compilation-success flag. An equal comparison of values is useful for independent tests; no special ownership or allocator is needed for the three spans. [Recommendation derived from VERIFIED: 04-CONTEXT.md D-01–D-04/D-06/D-10; AGENTS.md section 6]

std::span is a non-owning contiguous view, so callers must keep the token range valid for the duration of the synchronous call. std::variant and get_if already fit the existing checked-result convention. Use C++23 APIs; newer live-draft additions such as span::at are unnecessary. The proposed input contract checks initialized Token values, not arbitrary dangling memory or uninitialized objects. [CITED: https://raw.githubusercontent.com/cplusplus/draft/n4950/source/containers.tex, views.span; CITED: https://raw.githubusercontent.com/cplusplus/draft/n4950/source/utilities.tex, variant; VERIFIED: include/veyr/lexing/lexer.hpp; include/veyr/source/source_manager.hpp]

### Pattern 2: Deterministic checked-input validation before lexical rejection

Adopt this precise priority in the plan. It is a bounded choice within D-11's delegated implementation details; do not claim it was separately selected during discussion. The algorithm should stop at the first structural error in this ordering, without appending diagnostics. A valid prefix or observed Invalid never bypasses later structural checks. [Recommendation derived from VERIFIED: 04-CONTEXT.md D-08/D-11 and inherited implementation discretion]

| Order | Check | Outcome and payload |
|---|---|---|
| 0 | Call sources.source_text(selected_id) before inspecting tokens | Return its exact SourceError on failure; retain known source_size on success |
| 1 | Visit tokens in increasing index; call sources.validate(token.span) first | Return the exact original SourceError unchanged, including token's ID/offsets/optional size; existing priority is UnknownSource → ReversedRange → BeyondSource |
| 2 | Checked token's source_id equals selected_id | Otherwise TokenStreamError::SourceMismatch at this index; never resolve through another source to continue |
| 3 | TokenKind is one of the thirteen explicit supported enumerators | Otherwise UnsupportedTokenKind at this index; use an explicit switch, not unchecked enum-to-array indexing |
| 4a | For EOF, token index is final | Otherwise NonFinalEndOfFile, including an early/duplicate EOF, even if its shape is also wrong |
| 4b | For final EOF, begin == end == source_size | Otherwise InvalidEndOfFileSpan; beyond-source/reversed shapes were already SourceError at step 1 |
| 4c | For a non-EOF token, begin < end | Otherwise EmptyTokenSpan; SourceManager itself permits empty spans |
| 5 | For every token after the first, begin >= previous.end | Otherwise OutOfOrderOrOverlappingSpan; adjacency and whitespace gaps are allowed |
| 6 | After all tokens pass, final token must exist and be EOF | Otherwise MissingEndOfFile with absent token_index; an empty token vector is malformed infrastructure |
| 7 | Only now inspect the remembered has_invalid flag | Any Invalid returns ParseRejected{LexicalErrors}, with no new diagnostics or success value |
| 8 | Only now recognize syntax | Match the required sequence and emit at most one syntax Error |

Source validation before source agreement is intentional: an unknown token ID preserves UnknownSource rather than becoming SourceMismatch; a known other ID with valid bounds becomes SourceMismatch. With multiple bad tokens, the earlier token wins; within one token the listed checks win. A missing EOF is checked after per-token faults. EOF exclusivity follows from forbidding every nonfinal EOF and requiring a final EOF; no separate duplicate count or manufactured sentinel is necessary. [Recommendation derived from VERIFIED: src/source/source_manager.cpp validate/source_text; 04-CONTEXT.md D-11]

Remember Invalid during validation and continue checking; do not return lexical rejection from inside the loop. Likewise, do not emit syntax errors until validation and Invalid gating finish. Avoid subtracting offsets until checked ordering/bounds make subtraction valid. Use bounded cursor access; reaching actual EOF is a syntax condition, not permission to advance past the span. [Recommendation derived from VERIFIED: 04-CONTEXT.md D-08/D-11; include/veyr/source/source_span.hpp; src/source/source_manager.cpp]

Do not verify keyword text, integer digits, token spelling widths or omitted-gap bytes. Those belong to the conforming producer. Token/source IDs remain manager-local: equal numeric IDs from two managers cannot be rejected reliably, so document and test the limitation rather than promise provenance detection. [VERIFIED: 04-CONTEXT.md D-16/inherited constraints; tests/lexing/token_test.cpp ResolutionRequiresTheOriginatingManager]

### Pattern 3: Fixed sequence, controlled messages, tight result bounds

Match this exact required sequence after gating. The expected column is the complete recommended controlled-label table; append no terminal punctuation. This refines wording within the accepted message style only. [Recommendation derived from VERIFIED: profile section 4/section 5.2; 04-CONTEXT.md D-12–D-15]

| Position | Required TokenKind | Controlled expected label | Related span on mismatch |
|---|---|---|---|
| 0 | Fn | `'fn'` | None |
| 1 | Main | `'main'` | None |
| 2 | LeftParen | `'('` | None |
| 3 | RightParen | `')'` | Exactly matched token 2; `opening '(' is here` |
| 4 | Arrow | `'->'` | None |
| 5 | Int32 | `'int32'` | None |
| 6 | LeftBrace | `'{'` | None |
| 7 | Return | `'return'` | None |
| 8 | IntegerLiteral | `decimal integer literal` | None |
| 9 | Semicolon | `';'` | None |
| 10 | RightBrace | `'}'` | Exactly matched token 6; `opening '{' is here` |
| 11 | EndOfFile | `end of source` | None |

| Actual TokenKind | Controlled found label |
|---|---|
| Fn / Main / Int32 / Return | `'fn'` / `'main'` / `'int32'` / `'return'` respectively |
| LeftParen / RightParen | `'('` / `')'` respectively |
| Arrow | `'->'` |
| LeftBrace / RightBrace | `'{'` / `'}'` respectively |
| Semicolon | `';'` |
| IntegerLiteral | `decimal integer literal` |
| EndOfFile | `end of source` |
| Invalid | No syntax message: lexical gate returns rejection |
| Unsupported enum value | No syntax message: infrastructure gate returns UnsupportedTokenKind |

Form exactly `expected <expected-label>, found <found-label> in M1 Experimental Smoke Profile`. Use only these controlled labels. A real unexpected token contributes its complete span; EOF contributes exactly its validated zero-width source-end span. Only a mismatch at positions 3 or 10 adds the one listed related opener. An earlier successful opener does not add context to a later missing integer or semicolon. [Recommendation derived from VERIFIED: 04-CONTEXT.md D-12–D-15; include/veyr/diagnostics/diagnostic.hpp]

On success, construct exactly entry `{id, fn.begin, right_brace.end}`, return `{id, return.begin, semicolon.end}` and integer `integer_token.span`, after matching EOF. For the no-newline sample `fn main() -> int32 { return 42; }`, the independent expected spans are entry [0,33), return [21,31), integer [28,30). Surrounding whitespace is excluded from entry/return bounds; internal whitespace remains inside them. [VERIFIED: profile section 5.7 sample offsets; 04-CONTEXT.md D-02/D-03; bounds derived directly from those accepted token spans]

Prefer a small expected-token array or straight-line expect operations, with private helpers for validation and diagnostics. There is no expression recursion, precedence table, recovery synchronization or speculative backtracking in this shape. Do not convert decimal text: 0, 255, 256, leading zeros and arbitrarily long decimal tokens all have the same parser acceptance shape. [Recommendation derived from VERIFIED: profile section 3/section 4; 04-CONTEXT.md Phase Boundary/D-05/D-16]

### Anti-Patterns to Avoid

- Early success at `}`: trailing tokens must fail the EOF expectation, with no result. [VERIFIED: D-06/PARSE-01]
- Early return on Invalid: later malformed spans/EOF must be diagnosed as checked infrastructure. [VERIFIED: D-08/D-11]
- Using `diagnostics.has_errors()` as a parser entry gate: prior unrelated Error does not prevent syntax recognition of this stream. [VERIFIED: D-09]
- Inferring lexical cleanliness from the engine or checking diagnostic counts instead of Invalid: lexical rejection is a property of this validated stream. [VERIFIED: D-08/D-09]
- Re-lexing keywords, integer digits or gaps: violates the selected producer boundary and creates a second lexer. [VERIFIED: D-10/D-16]
- Adding numeric conversion, general AST nodes or synthetic recovery: moves later semantics or unselected designs into Phase 4. [VERIFIED: Phase Boundary/D-05–D-07]
- Returning references to tokens, storing source text or borrowing an engine view across emit: violates explicit lifetime contracts. [VERIFIED: D-01/D-10/inherited constraints]

## Don't Hand-Roll

Each recommendation below reuses an already inspected project facility rather than broadening Phase 4. [VERIFIED: headers and source paths listed in each row; 04-CONTEXT.md inherited constraints]

| Problem | Don't Build | Use Instead | Why / evidence |
|---|---|---|---|
| Byte/source bounds and errors | Parser-specific source resolver or clamping | sources.source_text / sources.validate; return original SourceError | Existing exact payloads and validation priority. [VERIFIED: src/source/source_manager.cpp] |
| Source locations and newline handling | Parser line/column cache or normalized text | SourceManager.location | Original LF/CRLF/lone-CR/EOF rules are already implemented. [VERIFIED: src/source/source_manager.cpp; profile section 5.8] |
| Diagnostic rendering | Parser excerpts, escaping or terminal output | Diagnostic + existing PlainTextRenderer | Structured owned data and checked presentation stay separate. [VERIFIED: include/veyr/diagnostics/diagnostic.hpp; include/veyr/diagnostics/plain_text_renderer.hpp] |
| Lexical recognition | Keyword/spelling/number/gap validation | Accepted lexer or conforming producer | Explicit D-16 precondition; structural checks do not imply re-lexing. [VERIFIED: 04-CONTEXT.md D-16] |
| Test discovery | Bespoke enumerator, shell test harness or new framework | Existing GoogleTest and gtest_discover_tests(PRE_TEST) | Already used by native targets and handles parameterized discovery. [VERIFIED: tests/CMakeLists.txt; CITED: https://cmake.org/cmake/help/v3.31/module/GoogleTest.html] |
| Packages/build orchestration | New parser dependency, acquisition script or build wrapper | Existing Conan/CMake/Ninja and scripts/dev.py | No new dependencies selected or needed for this fixed shape. [VERIFIED: 04-CONTEXT.md inherited constraints; conanfile.py; scripts/dev.py] |

**Key insight:** A handwritten fixed-sequence recognizer is the selected narrow implementation, while source validation, lexing, ownership, diagnostics, rendering and build mechanics should reuse their existing owners. [Recommendation derived from VERIFIED: 04-CONTEXT.md; existing public headers]

## Common Pitfalls

### 1. Confusing missing EOF with incomplete source

**Failure:** Treat an empty token vector as an empty source and fabricate EOF. **Cause:** Syntax recognition starts before validating the sentinel contract. **Prevention:** Empty vector and a non-EOF final token return MissingEndOfFile; a valid EOF-only stream reaches the syntax expectation for Fn. **Warning:** A malformed client stream appends a language diagnostic. [VERIFIED: D-11/D-14; source-end EOF in lexer.cpp]

### 2. Accepting a valid prefix or recovering into success

**Failure:** A second return, second entry or trailing token survives because result construction happens at the closing brace. **Prevention:** Require final EOF before constructing the value; mismatch emits exactly one Error and returns syntax rejection. A second return should fail expecting `'}'` and include the body opener; a token after the complete entry fails expecting end of source and has no related opener. [VERIFIED: D-05/D-06/D-15; PARSE-01/PARSE-02]

### 3. Returning lexical rejection too early

**Failure:** An Invalid near the beginning hides a reversed/unknown/misordered span or bad EOF later. **Prevention:** Complete structural validation first; remember Invalid without handling it. Test this with malformed suffixes and with an early syntax mismatch followed by malformed structure. [VERIFIED: D-08/D-11]

### 4. Misusing engine state or borrowed views

**Failure:** An old Error blocks a valid new stream, or a saved diagnostic reference is accessed after emit. **Prevention:** Do not gate parse on has_errors; preserve every prior message/order/duplicate. Copy snapshots needed across calls, and reacquire diagnostics() after emission. Parser success denotes syntax only. [VERIFIED: D-09; include/veyr/diagnostics/diagnostic_engine.hpp; tests/lexing/lexer_test.cpp shared-engine cases]

### 5. Overpromising checked producer conformance

**Failure:** Plans demand rejection of Fn spanning return, whitespace-gap spelling verification or foreign-manager origin detection despite identical IDs. **Prevention:** Document token classification/grouping as producer preconditions and selected-manager use as a caller obligation; structurally validate bounds/IDs/kinds/order/EOF only. [VERIFIED: D-16; tests/lexing/token_test.cpp ResolutionRequiresTheOriginatingManager]

### 6. Diagnostic wording and span drift

**Failure:** Raw lexemes enter messages; missing semicolon highlights an empty insertion span; all errors receive opener context; severity is uninitialized. **Prevention:** Controlled tables, full actual token primary, exact EOF primary, opener only when expecting ')' or '}', and aggregate construction beginning with DiagnosticSeverity::Error. Check fields independently of rendering. [VERIFIED: D-12–D-15; include/veyr/diagnostics/diagnostic.hpp]

### 7. Accidentally implementing semantics or final grammar

**Failure:** Parser rejects 256/huge decimals, converts leading zeros, adds arithmetic/functions or uses final-language test names. **Prevention:** Preserve integer token span verbatim; use SmokeProfileParser/SmokeProfileParserIntegration names; leave conversion/range/AST consolidation to Phase 5. [VERIFIED: profile sections 3–4/section 7; .planning/ROADMAP.md Phase 4/5]

### 8. Treating historical documentation flags as current technical blockers

The profile and several planning artifacts retain immutable draft-era or unstarted prose; inspected later accepted Phase 3 records establish profile publication and foundation acceptance. This task's explicit owner-authorized planning/research scope supersedes historical authorization descriptions without rewriting those accepted files. Do not infer a new CI result or edit acceptance evidence. [VERIFIED: profile introductory status; 03-VERIFICATION.md subsequent acceptance; Git log 9a3c655/15e00af; orchestrator task scope]

## Code Examples

These snippets are proposed applications of existing verified APIs, not implementation created or compiled during research. [VERIFIED: source/diagnostic headers and inspected implementation; research command history]

### Preserve checked source failure before any emission

~~~cpp
// Existing API pattern: src/lexing/lexer.cpp and source_manager.cpp.
const auto input = sources.source_text(source_id);
if (const auto* error = std::get_if<SourceError>(&input)) {
  return *error;  // Preserve exact existing payload.
}
const auto source_size = std::get<std::string_view>(input).size();
for (const auto& token : tokens) {
  const auto checked = sources.validate(token.span);
  if (const auto* error = std::get_if<SourceError>(&checked)) {
    return *error;
  }
  // Continue the ordered identity/kind/shape/order checks specified above.
}
~~~

The snippet shows source validation only; it must not replace the complete checked-input algorithm. get_if permits inspecting the variant alternative without throwing bad_variant_access. [CITED: https://raw.githubusercontent.com/cplusplus/draft/n4950/source/utilities.tex, variant.get; VERIFIED: src/lexing/lexer.cpp]

### Explicit initialized Error with one optional related opener

~~~cpp
// Proposed mismatch handling using existing Diagnostic/RelatedSpan APIs.
// expected/found are selected only from the controlled tables above.
Diagnostic error{DiagnosticSeverity::Error, std::move(message),
                 unexpected.span, {}};
if (matching_opener.has_value()) {  // Only expected ')' or expected '}'.
  error.related.push_back({*matching_opener, std::move(opener_label)});
}
diagnostics.emit(std::move(error));
return ParseRejected{ParseRejectionKind::SyntaxError};
~~~

The existing engine consumes a Diagnostic by value, owns its strings and preserves insertion order; source resolution remains through the caller's live manager. No exception catch, OOM translation or rollback is added. [VERIFIED: include/veyr/diagnostics/diagnostic.hpp; src/diagnostics/diagnostic_engine.cpp; 04-CONTEXT.md D-07]

### Consumer distinguishes syntax from compilation validity

~~~cpp
// Proposed library-client illustration; driver implementation is Phase 9.
const auto result = parse(sources, id, tokens, diagnostics);
const auto* entry = std::get_if<ParsedSmokeEntry>(&result);
const bool eligible_for_later_validation = entry != nullptr && !diagnostics.has_errors();
~~~

This predicate is explanatory test/client logic only. Even a clean complete parse still requires later semantic validation and lowering; do not expose a Phase 4 compiler-success API. [VERIFIED: 04-CONTEXT.md D-09; .planning/ROADMAP.md Phases 5/6/9]

## Validation Architecture

Nyquist validation is explicitly enabled. No parser tests or target currently exist; SourceManager, diagnostic/renderer and lexer suites already provide the reusable native framework. The commands below are **future execution/verification instructions**, not commands run during research. [VERIFIED: .planning/config.json workflow.nyquist_validation; rg --files include src tests; tests/CMakeLists.txt]

### Test Framework

| Property | Value / provenance |
|---|---|
| Framework | GoogleTest/GoogleMock 1.18.0. [VERIFIED: conanfile.py; tests/CMakeLists.txt] |
| Config file | Existing root/tests CMakeLists.txt; proposed parser_tests with PRE_TEST discovery. [VERIFIED: tests/CMakeLists.txt] |
| Quick run command | `ctest --test-dir "<selected-build-dir>" -R SmokeProfileParser --output-on-failure --no-tests=error`, after the authorized incremental build includes new tests |
| Full suite command | `python scripts/dev.py test --config Debug`, separately `python scripts/dev.py test --config Release`; wrapper builds and runs the entire suite |
| Additional phase gate | Existing authorized `python scripts/dev.py verify --config Debug` / Release and direct include policy, with actual evidence captured separately |
| Timing | Focused fixtures are designed as small in-memory cases; under-30-second quick execution is a planning target, not measured evidence or a hard build timeout |

The CTest argument pattern and wrapper options were checked against actual scripts/dev.py. The wrapper has no --filter option; use CTest's -R against the selected build tree rather than inventing a wrapper switch or hardcoding this host's build/cache identity. Windows may use its reviewed base interpreter spelling; selected build paths are structured arguments. No command here grants execution permission. [VERIFIED: scripts/dev.py argument parser/ctest/test/verify methods; docs/development/AGENT_STEP_AND_COMMAND_POLICY.md]

### Phase Requirements → Test Map

Each row is a recommended Wave 0 test obligation, derived from the referenced requirement and decisions; no test is claimed to exist or pass. [VERIFIED: .planning/REQUIREMENTS.md PARSE-01–03; 04-CONTEXT.md D-01–D-16]

| Req ID | Behavior | Test Type | Automated Command after authorized build | File Exists? |
|---|---|---|---|---|
| PARSE-01 | Exact entry, empty parameters, exactly one integer return, EOF | Unit + real lexer | `ctest --test-dir "<selected-build-dir>" -R SmokeProfileParser --output-on-failure --no-tests=error` | No — Wave 0 parser_test.cpp |
| PARSE-01 | Decimal shape independent of conversion/range, whitespace-tight original spans | Unit + integration | Same focused command | No — Wave 0 both parser files |
| PARSE-02 | All twelve expectation positions, incomplete/trailing/malformed supported-token input, first-error only | Parameterized contract cases | Same focused command | No — Wave 0 parser_test.cpp |
| PARSE-02 | Exact controlled messages, full primary/EOF spans, opener context only for expected ')' / '}' | Unit + renderer integration | Same focused command | No — Wave 0 both parser files |
| PARSE-02 | SourceError payloads; structural/Invalid/syntax precedence; no diagnostics on infrastructure/lexical rejection | Adversarial API tests | Same focused command | No — Wave 0 parser_test.cpp |
| PARSE-02 | Prior Error preserved while valid stream parses; Invalid anywhere suppresses new parser Error | Real lexer + shared engine | Same focused command | No — Wave 0 parser_integration_test.cpp |
| PARSE-03 | Independent three-span result, copying/moving and token destruction; fresh source registrations | Unit/lifetime integration | Same focused command | No — Wave 0 both parser files |
| PARSE-03 | Lexer retains one-way dependency; no numeric/AST/LLVM API coupling | Source review + build/include policy | Existing verify wrapper and `python -B scripts/check_includes.py --root .`; inspect actual include/dependency diff | Existing framework; new parser code pending |

### Independent meaningful test strategy

Use the binding profile/context as test oracles. Keep expected strings, byte offsets, outcome alternatives and related-span labels in the tests; never import the implementation's expected-kind or label table to compute the expected result. Parameterized tests expose each fixture with a descriptive ASCII name; traces identify mutations in any small loop. Existing TEST_P/ValuesIn patterns can be reused without new helpers outside the parser tests. [Recommendation derived from VERIFIED: tests/lexing/lexer_test.cpp named recovery/all-byte cases; CITED: https://google.github.io/googletest/advanced.html]

1. **Positive structure:** Real lexer fixtures for 0, 42, 255, 256, 00042 and a long all-digit literal. Require complete result/no new errors; assert exactly three span values and exact text resolution. Include leading/trailing whitespace, TAB/LF/CRLF/lone-CR between tokens and no-whitespace punctuation adjacency. Do not demand parser rejection of lexically valid out-of-range values. [VERIFIED basis: profile sections 3–5; D-01–D-04]
2. **Every syntax position:** Supply lexically valid source that reaches each expectation then substitutes a different supported token. Cover all twelve expectations, including Fn at empty/whitespace-only source and EOF after a complete entry. Use an explicit table of source bytes/expected message/primary/optional opener. Add token-boundary prefixes for every required non-EOF position; each prefix retains lexer-created source-end EOF and fails once at that EOF. [VERIFIED basis: D-05/D-06/D-12–D-15]
3. **Representative non-profile structures:** Nonempty parameters, wrong recognized entry/result token, absent arrow/brace/return/integer/semicolon/closing brace, extra semicolon, second return, second entry, trailing supported token and unexpected closers. Unknown words/operators/comments are lexical rejection, not parser syntax-message fixtures. [VERIFIED basis: profile section 4/section 6; D-08]
4. **Complete diagnostics:** Independently assert severity, full message, primary span, related count, related span and label, no extra Error/Note. Test missing semicolon at real '}' with empty related spans; expecting ')' at a real token and EOF with the '(' related; expecting '}' at a real token and EOF with the '{' related. Test missing integer inside an open brace has no related span. [VERIFIED basis: D-12–D-15]
5. **Infrastructure matrix:** Unknown selected source (zero and unknown nonzero); unknown token source with exact payload; valid different source ID; reversed and beyond-source bounds; unsupported enum values; empty non-EOF; backwards/overlapping tokens; permitted adjacent tokens; no EOF; EOF-only source; nonempty EOF; EOF at a checked in-bounds non-end offset; early/duplicate EOF; real token after EOF. Assert outcome kind/index/payload and zero appended diagnostics. [VERIFIED basis: D-11; SourceManager.validate]
6. **Priority combinations:** Invalid before a later bad span; Invalid before nonfinal/wrong-shape EOF; syntax mismatch before later structural violation; malformed selected source plus empty tokens; same-token bad source/span/kind; nonfinal and malformed EOF together; several malformed tokens to prove earliest-index priority. A structurally valid stream with Invalid only returns lexical rejection, even with no lexical Error preseeded; the engine's history is not a substitute for token validation. [Recommendation derived from VERIFIED: D-08/D-09/D-11]
7. **Real lexical handoff:** Use the existing lexer for Invalid before, inside and after an otherwise complete program, including signed/nondecimal numbers, unsupported comments, high bytes, NUL and the accepted `/*/return` boundary. Copy diagnostics before parse, then prove the exact fields/order/duplicates remain unchanged with no parser additions. A syntax mismatch plus any later Invalid still emits no parser Error. [VERIFIED basis: D-08; profile section 5; existing lexer tests]
8. **Persistent engine and ownership:** Preseed initialized Note/Warning/Error, parse a valid independent stream, preserve all prior fields and has_errors state; parse bad syntax twice and prove exactly two ordered appended duplicates. Destroy/replace token storage before accessing a saved parse result; copy/move the value; add registrations with the same display name, then resolve its three original spans. Keep the manager alive, and do not dereference dangling storage as a test. [VERIFIED basis: D-01/D-04/D-09; diagnostic/source API lifetimes]
9. **Location and renderer integration:** Real parser Error → real renderer for missing ')' / '}' and EOF after LF, CRLF, lone CR and trailing whitespace; assert original byte locations with one related opener. Use hostile display-name bytes while keeping source syntax lexically valid; check escaped owned rendering without engine-state changes. Copy diagnostics across later emits instead of retaining borrowed views. [VERIFIED basis: D-12–D-15; existing renderer/lexer integration patterns]
10. **No recovery as success:** Assert all syntax/lexical/infrastructure outcomes hold no ParsedSmokeEntry alternative. Independently verify valid syntax plus prior Error yields a value but fails the illustrative later-handoff predicate; no semantic/backend result is claimed. [VERIFIED basis: D-06/D-08/D-09; PARSE-02]

Keep parser algorithm inspection separate from test oracle inspection. Large suites of random bytes alone cannot establish exact expected/found spans or selected precedence. Do not add fuzzing packages, death tests, OOM injection, a property-testing framework or brittle wall-clock assertions. A bounded long token stream with a malformed final token can exercise full-stream checking without defining a new performance requirement. [Recommendation derived from VERIFIED: D-07/D-11; no-dependency/first-error scope; existing exact-oracle lexer tests]

### Sampling Rate

- **Per coherent implementation task:** Authorized incremental build and the focused SmokeProfileParser selection; reacquire passing names/counts and make zero selected tests an error.
- **Per wave:** Existing full Debug/Release suite, include-policy and relevant execution checks after actual code changes.
- **Phase gate:** Fresh authorized verify-work and independently scoped Ubuntu/Windows Debug/Release hosted evidence. Hosted success does not certify Windows developer-host readiness; macOS is deferred.
- **Commit boundary:** Passing checks do not grant a commit; sensitive-file review and explicit owner approval remain separate.

These are future planning recommendations under existing workflow/platform constraints, not actions completed by this research. [VERIFIED: AGENTS.md sections 2/5; docs/development/GSD_WORKFLOW.md; accepted Phase 3 evidence/platform boundaries]

### Wave 0 Gaps

- [ ] Add `include/veyr/parsing/parser.hpp` and agree the bounded checked-outcome/error table in the owner-reviewed plan.
- [ ] Add `tests/parsing/parser_test.cpp` and `tests/parsing/parser_integration_test.cpp` with the independent fixture categories above.
- [ ] Register `parser_tests` using existing veyr_target_policy, Conan-owned GTest targets and PRE_TEST discovery.
- [ ] Add `src/parsing/parser.cpp` to veyrcore during authorized execution.
- [ ] Record the actual selected Debug/Release build directories before invoking focused CTest; do not copy another configuration's path.
- [ ] Confirm actual native test execution and platform results after implementation; research performs no runtime verification.

No framework installation, new dependency, shared tests/conftest-style layer, parser generator or CI change is required. These gaps are implementation-plan tasks only. [VERIFIED: existing tests/CMakeLists.txt/conanfile.py; 04-CONTEXT.md inherited constraints]

## Decision Coverage

Every binding decision maps to a concrete proposed mechanism and independent validation obligation. The following table is a recommendation derived from the verbatim 04-CONTEXT.md decisions, not a replacement decision record. [VERIFIED: 04-CONTEXT.md D-01–D-16]

| Decision | Mechanism / file | Independent validation |
|---|---|---|
| D-01 | Value-only ParsedSmokeEntry in parser.hpp | Destroy token storage; resolve original spans while manager lives |
| D-02 | Exactly entry/return/integer span members | Three-member aggregate/value and exact fixture spans |
| D-03 | Bounds from first/last syntax tokens | Leading/trailing/internal whitespace and original byte oracles |
| D-04 | Ordinary value copy/move | Type traits and copy/move behavior |
| D-05 | First mismatch returns immediately | One appended Error despite multiple syntax faults |
| D-06 | Value constructed only after EOF | Every failure lacks ParsedSmokeEntry; trailing-token/second-entry cases |
| D-07 | Explicit rejection vs infrastructure alternatives | No ordinary syntax throw path; checked variant assertions; no OOM policy test |
| D-08 | Complete validation then Invalid gate | Invalid anywhere adds zero parser diagnostics, including mixed malformed structure |
| D-09 | No engine-error entry gate | Valid new stream parses with old Error; handoff remains blocked |
| D-10 | Caller-supplied span; no lex call in parser | Real lex/parse integration and dependency/include inspection |
| D-11 | Ordered validation/error taxonomy | Each failure plus multi-fault priority and exact SourceError payloads |
| D-12 | Controlled labels; initialized severity | Exact full message/Error assertions and hostile-name renderer test |
| D-13 | Unexpected actual token.span primary | Missing/extra-token cases highlight full actual span |
| D-14 | Validated exact EOF primary | Empty/prefix/newline/trailing-whitespace EOF locations |
| D-15 | One related recognized opener only | Expected ')' / '}' at real token and EOF; all other related sets empty |
| D-16 | Producer conformance documented; no spelling validation | Review no source rescanning; manager-coincidence limitation documented |

## State of the Art

This phase follows the project's accepted current architecture, not an ecosystem migration. No tool/library upgrade is recommended. [VERIFIED: 04-CONTEXT.md; existing dependency declarations]

| Existing boundary | Phase 4 addition | Impact / evidence |
|---|---|---|
| Lexer classifies/recover-scans units without parsing order | Fixed sequence recognizer consumes caller's completed stream | Preserve one-way dependency. [VERIFIED: src/lexing/lexer.cpp; D-10/D-16] |
| Existing SourceError and variant-based checked APIs | New stream-integrity and explicit rejection alternatives | Preserve existing API/payloads. [VERIFIED: source_manager.hpp; lexer.hpp; D-07/D-11] |
| Existing owned structured diagnostics and separate renderer | Parser adds one controlled syntax Error when permitted | Reuse current diagnostic contract. [VERIFIED: diagnostic.hpp; D-12–D-15] |
| No public AST or semantic validator yet | Minimal three-span parse value | Phase 5 consolidates shared AST/semantic boundary; no duplicate parser. [VERIFIED: rg --files include src; ROADMAP Phase 5] |

**Outdated references to avoid:** unversioned ASVS chapter numbers and current live-draft C++ APIs newer than C++23. Use versioned ASVS 5.0.0 references for the advisory security mapping below and N4950 for C++23-era library shape. This is documentation precision, not a project security-standard adoption or a compiler upgrade. [CITED: https://owasp.org/projects/asvs; https://github.com/OWASP/ASVS/tree/v5.0.0/5.0/en; https://raw.githubusercontent.com/cplusplus/draft/n4950/source/containers.tex]

## Security Domain

Security enforcement is not explicitly disabled in .planning/config.json, so this section is included. ASVS is a web-application verification standard; the following mapping borrows only applicable parser/diagnostic principles and makes no ASVS compliance or certification claim. No authentication/session/network/storage subsystem is introduced. [VERIFIED: .planning/config.json; 04-CONTEXT.md Phase Boundary/inherited constraints; CITED: https://owasp.org/projects/asvs]

### Applicable ASVS Categories

Use versioned 5.0.0 categories, not the template's legacy V2-auth/V3-session/V4-access/V5-validation/V6-crypto numbering. The official 5.0.0 chapter inventory places these concerns as below. Applicability is a scope inference from the actual in-process C++ parser. [CITED: https://github.com/OWASP/ASVS/tree/v5.0.0/5.0/en; VERIFIED: existing public APIs; 04-CONTEXT.md]

| ASVS 5.0.0 category | Applies | Standard control / phase-specific application |
|---|---|---|
| V1 Encoding and Sanitization, especially memory/string/unmanaged code section 1.4 | Yes, analogous memory boundary | Bounded token access, checked spans, no dangling token references, controlled message strings; existing renderer escapes at presentation |
| V2 Validation and Business Logic section 2.1/section 2.2 | Yes, analogous input contract | Document and enforce allowed kind/identity/bounds/order/EOF structure before syntax or Invalid handling |
| V6 Authentication | No | No principal, credential or login boundary |
| V7 Session Management | No | No session state |
| V8 Authorization | No | No resource authorization system |
| V11 Cryptography | No | No crypto operation or key storage; add no crypto dependency |
| V15 Secure Coding and Architecture | Relevant scope principle | Keep parser bounded and dependencies/ownership explicit; no extra runtime subsystem |
| V16 Security Logging and Error Handling | Relevant error/presentation principle | Distinct checked failures; no partial-success path or raw-source message interpolation |

The table's control recommendations are derived from D-01/D-05–D-16 and the existing checked source/renderer APIs; official chapter claims are cited above. No web schema library, logging retention rule, catch-all exception policy, resource quota or security framework is implied. Ordinary allocation exceptions remain unchanged. [VERIFIED: 04-CONTEXT.md D-07/inherited constraints; CITED: official ASVS v5.0.0 V1/V2/V15/V16 source chapters listed in Sources]

### Known Threat Patterns for This C++ Parser

| Pattern | STRIDE | Standard mitigation / test |
|---|---|---|
| Malformed source spans or unexpected enum used as array index | Tampering / Denial of service | Existing checked source validation; explicit known-kind switch before label lookup; malformed-stream matrix |
| EOF/cursor overrun | Denial of service | Required unique final EOF, bounded sequence access, no advancing beyond actual sentinel; EOF/prefix tests |
| Dangling token or diagnostic references | Tampering / Denial of service | Success owns spans; synchronous token borrowing; no engine view retained across emit; lifetime tests |
| Raw-byte message/terminal injection | Spoofing | Controlled expected/found/opener labels; renderer owns escaping; hostile display-name integration |
| Invalid stream or prior Error accepted as compilation success | Tampering | Explicit rejection; no partial result; caller still inspects persistent Error state; handoff tests |
| Expensive recovery/backtracking or numeric conversion | Denial of service | One structural pass and bounded fixed-sequence matching; no recovery or integer conversion; no new size/OOM policy |

These threats/mitigations are engineering inferences from the inspected C++/source/diagnostic boundary and binding decisions; they are not observed vulnerabilities in existing code. [VERIFIED basis: source_manager.cpp; diagnostic_engine.hpp; token.hpp; 04-CONTEXT.md D-01–D-16; CITED: ASVS v5.0.0 V1.4/V2/V15/V16]

## Environment Availability

This is a read-only planning audit. command -v checks and file inspection establish presence only; versions/readiness were not revalidated by executing native tools or doctor. Historical acceptance is identified separately and is not evidence that a future parser build already succeeds. [VERIFIED: research command history; .planning/STATE.md accepted evidence]

| Dependency | Required By | Available | Version / evidence | Fallback |
|---|---|---|---|---|
| Python | Existing developer wrapper | Present at /usr/bin/python3 | Path checked only; reviewed version belongs to existing baseline | Use existing reviewed host interpreter; no install proposed |
| Clang | Later C++23 compilation | Present at /usr/bin/clang++ | Path checked only; existing target policy requires reviewed Clang | Stop for concrete baseline mismatch during authorized execution |
| CMake / Ninja / CTest | Later build and native tests | Present at /usr/bin/cmake, /usr/bin/ninja, /usr/bin/ctest | No --version/build invocation; existing minimum/policy remains | Existing wrapper/profile, no alternate build system |
| Conan/GTest/LLVM prepared foundation | Existing full build | Existing generated GTest config and Debug/Release lexer/source/diagnostic binaries observed | GTestConfigVersion says 1.18.0; conanfile requires LLVM 23.1.2; no package-readiness operation run | Reuse accepted prepared workflow; no automatic setup/download |
| parser_tests | New phase validation | Absent | No parser source/test target yet | Wave 0 adds it during execution |
| Context7/Jina MCP; ctx7 CLI | Research docs fetch | Not available in this tool set / PATH | Research-plan selected Jina; fallback used built-in web access to primary docs | Official C++ draft sources, GoogleTest/CMake docs and OWASP source chapters |

All presence claims above are directly from command -v, read-only generated-version content and rg --files inspection of existing build/source trees. [VERIFIED: research command history; build/.../Debug/generators/GTestConfigVersion.cmake; conanfile.py; tests/CMakeLists.txt]

**Missing dependencies with no fallback:** None identified for planning; no fresh native-readiness claim is made. A future genuine tool/baseline error must stop at the existing owner gate rather than trigger installation/retry here. [VERIFIED: inspected prerequisites/paths; docs/development/AGENT_STEP_AND_COMMAND_POLICY.md]

**Missing dependencies with fallback:** Documentation-provider tools were unavailable; built-in web fetched primary documentation. No source/package dependency is missing as a new Phase 4 requirement. [VERIFIED: tool catalog/PATH inspection; web fetches; 04-CONTEXT.md no-dependency constraint]

Platform impact:
- Windows: Same initialized-token/original-byte/checked-error contracts and PRE_TEST native discovery under existing reviewed CRT/profile policies. Ubuntu/Windows hosted Debug/Release evidence must be fresh for the new code. Developer-host inspection stays independently pending/non-blocking.
- Linux: Primary Omarchy developer host; Ubuntu hosted evidence remains independent. Preserve CRLF/lone-CR bytes even on Linux; no filesystem lookup, locale, terminal or POSIX behavior enters parser.
- macOS future: Deferred; std value/span/variant and existing source/diagnostic boundaries add no new OS dependency. Existing build gate still limits current native builds to Windows/Linux; Phase 4 does not claim Darwin support or change that gate.

This note follows the accepted platform scope and actual existing CMake policy; it adds no CI/platform work. [VERIFIED: 04-CONTEXT.md platform impact; AGENTS.md section 5; CMakeLists.txt host gate; .codex/skills/veyr-cross-platform-impact/SKILL.md]

## Assumptions Log

| # | Claim | Section | Risk if Wrong |
|---|---|---|---|
| — | No training-only factual claim is used as a locked decision. API names, taxonomy priority, exact controlled table and test structure are explicitly labelled recommendations within accepted discretion. | Throughout | Owner-reviewed planning must adopt/finalize these bounded details before execution |

All empirical claims are directly verified from inspected repository/tools or cited primary documentation. Proposed checks have not been run against new code; no timing, runtime acceptance, new package verdict, CI outcome, security-compliance level or new language semantic is assumed. [VERIFIED: research activity scope; 04-CONTEXT.md inherited discretion]

## Open Questions (RESOLVED)

1. **RESOLVED: Exact API names/error layout/wording adoption.** The bounded implementation details are adopted in `04-01-PLAN.md`, sections "Artifacts this phase produces", "Exact checked-input and failure contract" and "Exact recognition, message and span tables", and Tasks 1–2. `04-02-PLAN.md` Tasks 1–2 supply the independent structural, message and span oracles. This resolves the planning question without reopening a language-design decision. [VERIFIED: 04-01-PLAN.md; 04-02-PLAN.md; 04-CONTEXT.md inherited implementation discretion]
2. **RESOLVED: Fresh execution/platform evidence obligations assigned; results remain pending.** `04-03-PLAN.md` Tasks 1–2 and its exact future commands assign independent local Debug/Release integration and final-code evidence. Its owner stops separately gate publication, fresh Ubuntu/Windows four-job hosted evidence, canonical verification and final owner acceptance. Native timing/results cannot be established in planning and are not claimed here. Windows developer-host inspection remains independently pending/non-blocking; macOS remains deferred. [VERIFIED: 04-03-PLAN.md; AGENTS.md section 2; 04-CONTEXT.md platform scope]
3. **RESOLVED: Historical bookkeeping reserved for parent planning closeout.** The parent planning command may reconcile current bookkeeping only within its approved closeout scope using supported helpers; this revision changes only these research dispositions. The published profile and accepted historical evidence remain unchanged. Pending documentation CI supplies no newer result, and no native/hosted success, commit or lifecycle transition is inferred. [VERIFIED: orchestrator revision scope; 04-PLANNING-BREAKDOWN.md remaining gates; 03-VERIFICATION.md]

**Concrete technical blocker:** None found. [VERIFIED: inspected complete public API/implementation boundaries against 04-CONTEXT.md D-01–D-16]

## Sources

### Direct repository evidence

- AGENTS.md and its required design/development/project/tooling documents: fixed scope, naming, stage/commit/build/include/platform policy. [VERIFIED: read-only file inspection]
- .planning/PROJECT.md, STATE.md, ROADMAP.md, REQUIREMENTS.md and config.json: active requirements, Phase 4/5 split, Nyquist and preserved historical acceptance. [VERIFIED: read-only file inspection]
- 04-CONTEXT.md and 04-DISCUSSION-LOG.md: all sixteen explicit choices, inherited contracts, delegated implementation details and rejected alternatives. [VERIFIED: read-only file inspection]
- docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md sections 1–8: exact experimental shape, lexical boundaries, independent sample offsets and semantic separation; publication established by accepted Phase 3 records. [VERIFIED: actual profile plus 03-VERIFICATION.md; no reliance on draft status alone]
- Phase 2/3 context, 03-03-SUMMARY.md and 03-VERIFICATION.md: existing inherited source/diagnostic/lexical contracts and evidence limitations. [VERIFIED: read-only file inspection]
- include/veyr/{source,diagnostics,lexing}/ public headers; src/source/source_manager.cpp; src/diagnostics/diagnostic_engine.cpp; src/lexing/lexer.cpp: actual API/data/lifetime/failure behavior. [VERIFIED: read-only code inspection]
- tests/lexing/token_test.cpp, lexer_test.cpp; root/tests CMakeLists.txt; cmake/VeyrTargets.cmake; conanfile.py; scripts/dev.py; scripts/devtools/baseline.json: actual native framework, integration, dependency versions and command arguments. [VERIFIED: read-only file inspection]

### Primary official documentation (provider confidence MEDIUM)

- [C++23 N4950 containers source](https://raw.githubusercontent.com/cplusplus/draft/n4950/source/containers.tex), views.span: synchronous non-owning view and bounded-access contract. [CITED: committee draft repository, tag n4950]
- [C++23 N4950 utilities source](https://raw.githubusercontent.com/cplusplus/draft/n4950/source/utilities.tex), variant: checked alternatives and get_if behavior. [CITED: committee draft repository, tag n4950]
- [GoogleTest advanced guide](https://google.github.io/googletest/advanced.html): named parameterized tests and diagnostic traces. [CITED: official GoogleTest documentation, inspected 2026-10-18]
- [CMake 3.31 GoogleTest module](https://cmake.org/cmake/help/v3.31/module/GoogleTest.html): parameterized enumeration and PRE_TEST discovery; version selected to match project minimum instead of depending on newer 4.x behavior. [CITED: official CMake documentation]
- [OWASP ASVS project](https://owasp.org/projects/asvs) and [versioned 5.0.0 chapter inventory](https://github.com/OWASP/ASVS/tree/v5.0.0/5.0/en): versioned advisory scope/category mapping. [CITED: official OWASP sources]
- [ASVS V1](https://raw.githubusercontent.com/OWASP/ASVS/v5.0.0/5.0/en/0x10-V1-Encoding-and-Sanitization.md), [V2](https://raw.githubusercontent.com/OWASP/ASVS/v5.0.0/5.0/en/0x11-V2-Validation-and-Business-Logic.md), [V15](https://raw.githubusercontent.com/OWASP/ASVS/v5.0.0/5.0/en/0x24-V15-Secure-Coding-and-Architecture.md), [V16](https://raw.githubusercontent.com/OWASP/ASVS/v5.0.0/5.0/en/0x25-V16-Security-Logging-and-Error-Handling.md): memory/input/architecture/error-handling principles, no compliance claim. [CITED: official OWASP versioned source chapters]

### Research procedure and confidence

Installed GSD 1.6.1 researcher/reference instructions and init.phase-op 4 were inspected. The research-plan seam selected Jina for five known-URL scrape questions; unavailable Jina/Context7/ctx7 caused fallback to built-in web access against primary official sources. Five digests were stored through the research-store seam under an isolated /tmp working directory so no additional repository artifact or user-home cache write was needed. classify-confidence with provider websearch --verified returned MEDIUM; an initial webfetch provider query returned LOW because that identifier is not a recognized authority in the installed classifier, and was not used to upgrade claims. [VERIFIED: actual helper outputs; /home/m4rkens/.codex/gsd-core/bin/lib/research-provider.cjs; research command history]

No knowledge graph exists at .planning/graphs, so no graph relationship or freshness claim was used. No external package was selected or installed, no native tool/build/test was executed, and no source/accepted document was changed by this researcher. The environment audit is presence-only; no runtime-state inventory applies because this is a new compiler slice, not a rename/refactor/migration. [VERIFIED: rg file inventory; research task/write scope and command history]

No secondary/community/training-only source underpins a recommendation. Date-sensitive release-version assertions were avoided except the directly inspected accepted project pins and explicitly versioned official documentation. [VERIFIED: source list and dependency declarations above]

## Metadata

**Confidence:** MEDIUM overall, matching the actual verified-primary-web provider seam; repository contracts/API facts are directly inspected, while proposed implementation has not been compiled or executed. [VERIFIED: classify-confidence output; read-only research scope]

| Area | Level | Reason |
|---|---|---|
| Standard stack | MEDIUM | Exact project pins/API declarations inspected; no new package or native readiness claim |
| Architecture | MEDIUM | All sixteen binding decisions have a concrete compatible mechanism; proposed API/priority details remain owner-reviewed planning choices |
| Pitfalls / validation | MEDIUM | Independent contract cases derive from actual source/diagnostic/lexer behavior and explicit decisions; new parser tests have not run |

**Research date:** 2026-10-18. **Review by:** 2026-11-17, or immediately if accepted profile, source/diagnostic/token API or Phase 4 decisions change. The review date is a planning recommendation, not a guarantee of external-tool currency. [VERIFIED: current environment date; accepted artifact dependencies; recommendation]

**Research-only result:** one canonical 04-RESEARCH.md; no staging, commit, push, implementation, package operation, build/test, CI mutation or automatic lifecycle transition. Owner authorization of planning remains separate from all later execution/verification/publication gates. [VERIFIED: task scope; AGENTS.md sections 2.2–2.8]
