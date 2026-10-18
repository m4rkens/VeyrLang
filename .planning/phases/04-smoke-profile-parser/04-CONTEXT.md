# Phase 4: Smoke Profile Parser - Context

**Gathered:** 2026-10-18
**Status:** Discussion captured; awaiting owner review and a separately approved discussion commit. Research, planning and execution have not started.

<domain>
## Phase Boundary

Deliver PARSE-01, PARSE-02 and PARSE-03: compiler clients can parse the fixed
experimental M1 entry/return shape through EOF, obtain a minimal source-linked
representation and receive precise syntax diagnostics under an explicit failure
contract. P01's compiler walking skeleton forces this parser portion only.

The binding profile remains `fn main() -> int32 { return <decimal>; }`, with
exact lexical units and whitespace supplied by the accepted Phase 3 lexer.
The literal's decimal conversion and 0–255 semantic range belong to later
semantic work. Phase 5 consolidates the AST boundary without a second parser;
this phase does not introduce a general grammar, expression/statement hierarchy,
semantic analysis, lowering, LLVM/backend work or a driver/source loader.
All M1 syntax, signed-32 representation and eventual result restrictions remain
experimental, not final Veyr grammar, general type semantics or stable ABI.

Phase 1 remains 8/8, Phase 2 4/4 and Phase 3 3/3 complete and owner-accepted.
The Phase 3 closeout was published in 15e00afec923ffd1d38ee9c12832fa7cb2e2f040.
Its implementation acceptance is supported by final four-job run 39107885454
at be84bb9702afa75ad07803a678e7fd5a4e6358d7 and canonical verification passed.
The subsequent documentation-only run 39128864315 was last observed queued;
this discussion does not claim a newer conclusion. The owner explicitly permits
local discussion despite that pending documentation CI.

Linux remains the primary developer host; Omarchy and Ubuntu CI are separate
environments. Windows remains Tier 1 compatibility/hosted CI; developer-host
inspection is independently pending/non-blocking. macOS remains deferred.
Discussion authorizes only its normal documentation and checkpoints, not
research, plans, implementation, builds/tests, CI mutations, commits or pushes.
</domain>

<decisions>
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
</decisions>

<canonical_refs>
## Canonical References

Downstream agents must read these before research, planning or implementation.
Explicit owner decisions and the decision log take precedence. No separate
Phase 4 SPEC, external technical specification or matching pending todo exists.

### Scope and accepted profile

- `AGENTS.md` — architecture, fixed names and lifecycle/commit boundaries.
- `.planning/PROJECT.md` — validated foundation and remaining M1 work.
- `.planning/REQUIREMENTS.md` — PARSE-01–03; AST/semantic/backend/driver exclusions.
- `.planning/ROADMAP.md` — Phase 4 goal, four success criteria and Phase 5 boundary.
- `.planning/phases/02-source-identity-spans-and-diagnostics/02-CONTEXT.md` — all sixteen source/diagnostic contracts.
- `.planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-CONTEXT.md` — fourteen accepted M1 decisions.
- `.planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-03-SUMMARY.md` — accepted adversarial/integration coverage and platform scope.
- `.planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-VERIFICATION.md` — actual canonical Phase 3 acceptance evidence and limitations.
- `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md` — binding experimental lexical/program contract, published in 52f598aaf031e549ed3bbe5c598bc43f5f7be6bb. Immutable draft-era status prose is historical; later acceptance/publication records establish the satisfied gates.
- `docs/design/milestones/MILESTONE_1.md` — P01 walking skeleton and ten-phase decomposition.
- `docs/design/milestones/MILESTONE_STRATEGY.md` — probe-driven scope; broader strategy labels do not replace the active roadmap.
- `docs/design/DESIGN_PROBES.md` — P01 pressure only; no general-language parser feature expansion.
- `docs/design/DECISION_LOG.md` — final-language design and existing implementation-policy authority.
- `docs/design/VISION.md` — compiler separation and diagnostic direction.
- `docs/design/TYPE_SYSTEM.md` — later semantic direction, not parser numeric conversion.
- `docs/design/MEMORY_MODEL.md` — language ownership remains separate from C++ parse-result ownership.
- `docs/design/MODULE_SYSTEM.md` — one source snapshot is not a full module/package model.

### Existing APIs and integration

- `include/veyr/lexing/token.hpp` — thirteen kinds, kind/span values and source lifetime.
- `include/veyr/lexing/lexer.hpp` — checked synchronous lexing, recovered output and Error-state contract.
- `include/veyr/source/source_manager.hpp` — checked immutable source/text/location resolution.
- `include/veyr/source/source_span.hpp` — original half-open byte bounds.
- `include/veyr/diagnostics/diagnostic.hpp` — owning Error/related-span data; explicit severity initialization.
- `include/veyr/diagnostics/diagnostic_engine.hpp` — ordered accumulation and borrowed-view invalidation.
- `include/veyr/diagnostics/plain_text_renderer.hpp` — separate checked escaped presentation.
- `tests/lexing/token_test.cpp` — value, span and lifetime patterns.
- `tests/lexing/lexer_test.cpp` — real lexer, hostile-input and diagnostic/renderer integration patterns.
- `CMakeLists.txt` — existing veyrcore target and common build policies.
- `tests/CMakeLists.txt` — GoogleTest/PRE_TEST discovery and include-policy integration.

### Workflow and platform policy

- `docs/development/GSD_WORKFLOW.md` — discussion review/commit before separate planning authorization.
- `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md` — exact commands and no discussion-time builds/tests.
- `docs/development/SESSION_HANDOFF_POLICY.md` — repository-based continuity.
- `docs/development/COMMIT_POLICY.md` — explicit separate stage commits.
- `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md` — later sensitive-file review.
- `docs/development/GIT_WORKFLOW.md` — reviewed stage publication.
- `docs/development/BUILD_POLICY.md` — established C++23/Conan/CMake policies.
- `docs/development/DEVELOPER_BOOTSTRAP.md` — prepared-dependency commands and independent host evidence.
- `docs/development/INCLUDE_POLICY.md` — canonical angle includes.
- `docs/development/CROSS_PLATFORM_POLICY.md` — Linux/Windows Tier 1 and deferred macOS.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- Token/lex provide the complete accepted kind/span stream and original-source EOF.
- SourceManager resolves checked immutable spans and exact byte locations.
- DiagnosticEngine owns ordered messages and preserves prior Error state;
  PlainTextRenderer supplies separate checked, escaped presentation.
- Existing native component tests, hostile-input assertions and target/discovery
  conventions support later parser tests without a new framework or dependency.

### Established Patterns and Integration Points

- No parser or AST implementation, Phase 4 plans/research or codebase maps existed
  at discussion start. The lightweight scout used existing public APIs and paths.
- The dependency direction remains source → diagnostics → lexer → parser; the
  lexer never gains a parser/AST/semantic dependency. Tokens retain no lexemes.
- Future parser code can integrate with existing veyrcore/native test targets
  only after reviewed planning and explicit execution authorization.
- The public result carries three independent span values for later consumers;
  it does not require token-vector lifetime or promise SourceManager ownership.
</code_context>

<specifics>
## Specific Ideas

For the experimental sample `fn main() -> int32 { return 42; }`, the result
retains the tight entry, return-statement and literal spans. A missing semicolon
before `}` reports the expected/found message at the real `}` token span.
If a closing brace is expected at EOF, the primary span is source-end EOF and
the related span identifies the recognized `{`. No recovery fabricates a result.

The lexical-classification example `Fn` spanning `return` distinguishes producer
conformance from checked structural validity; it is not a new lexer feature.
Examples describe experimental M1 contracts, never final Veyr grammar.
</specifics>

<deferred>
## Deferred Ideas

No new out-of-scope capability was proposed. General parser recovery, partial
results, exception-based syntax failure, internal re-lexing and kind/spelling
guards were alternatives not selected; they are not authorized future tasks.
Phase 5 owns AST consolidation and semantic decimal/range validation. Broader
syntax, type/ABI design, imports, module graphs, backend/driver and tooling remain
outside this phase. Preserve the non-blocking diagnostic severity-initialization,
combined renderer-clipping, unsupported-severity and optional loop-trace follow-ups
without implementing them during this discussion.
</deferred>

Platform impact:

- Windows: Same source-byte, token, checked-failure and parser diagnostic contracts;
  hosted compatibility remains Tier 1, developer-host inspection independently pending.
- Linux: Primary Omarchy developer host and independent Ubuntu CI; original newline
  bytes and source locations remain unchanged. No native command ran in discussion.
- macOS future: Deferred; no filesystem, terminal, LLVM or OS API dependency added.

---
*Phase: 04-smoke-profile-parser*
*Context gathered: 2026-10-18; sixteen substantive choices explicitly selected.*
*Discussion only; no research, plan, implementation, build/test, CI mutation,
commit, push or automatic lifecycle transition is authorized by this document.*
