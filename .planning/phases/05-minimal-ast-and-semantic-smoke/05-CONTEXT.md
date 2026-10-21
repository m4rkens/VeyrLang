# Phase 5: Minimal AST and Semantic Smoke - Context

**Gathered:** 2026-10-21
**Status:** Discussion captured; awaiting owner review and a separately approved discussion commit. Research, planning and execution have not started.

<domain>
## Phase Boundary

Deliver AST-01, AST-02, SEMA-01 and SEMA-02: one source-linked minimal AST shared
across the accepted parser and semantic clients, decimal value validation for the
experimental M1 result range, useful semantic diagnostics and an explicit validity
handoff before lowering. P01 requires a validated constant return, not a general
AST, grammar or type system.

The binding profile remains `fn main() -> int32 { return <decimal>; }` through
EOF. Signed 32-bit result semantics, the inclusive 0–255 restriction, unsigned
ASCII decimal spelling and decimal interpretation of leading zeros are inherited
from the published profile; this discussion does not choose them again. Numeric
conversion/range checks belong here, while LLVM representation and native process
behavior belong to later phases. No final integer, arithmetic, ABI or ownership
policy is accepted by this implementation slice.

Phase 4 is complete and owner-accepted at 3/3; its closeout is published in
012bebd6cd40388bfb075016f9ebdaa5e404e95a. Actual canonical verification passed
four roadmap criteria, eighteen original truths, twenty deduplicated truths,
twelve artifact entries, eleven directly inspected links, sixteen decisions and
three PARSE requirements, with four behavioral checks executed. Implementation
08e50520afdab9e46da39182a8dcc4b56be66801 passed all four Ubuntu/Windows jobs
in run 39580960005. Subsequent documentation CI is distinct; this discussion
does not claim its completion or success.

Phases 1–4 remain accepted; eighteen written plans are complete, four of ten
phases accepted. All four Phase 5 requirements remain Pending; global counts
remain 30 accepted/19 pending. Discussion authorizes its normal records and
checkpoint only, not research, executable plans, builds, implementation, commits
or publication. Held CI candidates and deferred Stage 2 performance work remain
isolated and do not block this compiler discussion.
</domain>

<decisions>
## Implementation Decisions

### Shared AST handoff

- **D-01:** Reuse the existing owned three-span `ParsedSmokeEntry` value as the
  common minimal M1 AST boundary. Preserve the accepted parser API and avoid a
  second structural representation or conversion to a duplicate AST. The entry,
  return-statement and integer-literal spans retain exact original syntax bounds.
- **D-02:** Make the shared AST independently consumable from parser/token
  interfaces, depending only on source-span types. The parser continues exposing
  and returning the same value. This is organization of the common boundary,
  not a change to syntax, spans, ownership or the lexer/Phase 2 public APIs.
- **D-03:** Semantic validation accepts ordinary public AST values, including
  caller-constructed values, and checks the documented source/span and literal
  invariants before producing validated output. Do not require an opaque
  parser-issued AST or reparse the program. D-09–D-12 define the checked boundary.
- **D-04:** Successful semantic output owns its required data by value and remains
  usable after destruction of the input AST. No borrowed AST/token references or
  copied source text are retained. Resolving spans still requires the originating
  SourceManager to remain alive. D-16 fixes the complete retained payload.

### Semantic diagnostics

- **D-05:** An out-of-range decimal value produces a primary span covering the
  entire original integer literal, including leading zeros or arbitrarily long
  digit sequences. Do not narrow it to an overflow digit or widen it to the whole
  return statement. Existing renderer bounds apply to presentation, not span data.
- **D-06:** Use one fixed range message for every out-of-range decimal value:
  `return value must be between 0 and 255 in M1 Experimental Smoke Profile`.
  Do not copy literal digits or a truncated digit preview into the message.
  The source excerpt supplies the offending text. This is the milestone-specific
  range diagnostic, not a general Veyr integer-overflow rule.
- **D-07:** Emit one explicitly initialized Error for that range failure, with the
  full literal primary span and an explicitly empty related-span list. Do not add
  a return-statement context span or extra diagnostic. Rendering stays separate.
- **D-08:** An earlier Error does not suppress checking the current input or
  reporting its new range Error. Preserve all earlier diagnostic fields, insertion
  order, duplicates and persistent Error state. Do not clear, reorder or deduplicate
  the engine. The successful-output gate is separately defined by D-13.

### Malformed AST inputs

- **D-09:** Invalid relationships in a caller-constructed AST return a distinct
  checked AST infrastructure error with inspectable details, not an added
  source-language diagnostic. SourceManager failures preserve their original
  checked SourceError kind and payload. Never clamp, fabricate source context or
  rely only on assertions to establish input validity.
- **D-10:** Validate all three SourceManager spans first, in entry → return →
  literal order, returning the first SourceError. Only after those checks pass,
  check AST relationships, then literal content and range. Stop at the first
  failure and expose no successful validated value. An earlier AST relationship
  issue must not mask a later span's SourceError.
- **D-11:** Literal bytes must be one or more ASCII decimal digits. A malformed
  caller-created literal, such as `42x`, `-1` or an empty range, is a checked AST
  infrastructure failure with no added language diagnostic. It is not an
  out-of-range decimal value and must not receive D-06's range Error.
- **D-12:** All three spans have the same SourceId and are nonempty. The return
  statement lies strictly inside the entry, and the literal strictly inside the
  return statement: entry.begin < return.begin < literal.begin < literal.end <
  return.end < entry.end. The entry need not cover the whole source; surrounding
  whitespace remains allowed. These structural checks do not certify keyword or
  punctuation spelling by reparsing.

### Validated output and Error state

- **D-13:** Successful output ready for lowering requires both valid current input
  and no accumulated DiagnosticEngine Errors. Finish the current input checks;
  if input is valid but an earlier Error remains, return an explicit blocked-by-
  errors outcome, without a duplicate diagnostic or successful validated value.
  Notes and Warnings alone remain non-blocking. Infrastructure and current range
  failures retain their own outcomes rather than being hidden by an early check
  of prior Error state.
- **D-14:** Only successful validation may construct the new validated-output
  type. Clients can inspect its constant and spans but cannot directly construct
  or mutate arbitrary data into a validated value. This restriction does not
  make the shared syntax AST opaque or alter the parser's ordinary value result.
- **D-15:** Validated output is ordinarily copyable and movable. Assignment may
  replace it with another already-validated value. Its owned constant/span data
  has no input-AST lifetime dependence. This is C++ infrastructure, not Veyr
  ownership semantics, a single-use capability or a stable ABI commitment.
- **D-16:** Retain the complete shared three-span AST plus the validated constant,
  all by value. Do not reduce successful output to only a constant/literal span.
  Preserve entry, return and literal context for later diagnostics and lowering,
  without tokens or copied source text; text resolution uses the original manager.

### Inherited constraints and implementation discretion

All sixteen substantive choices were explicitly selected by the owner. No listed
choice was delegated through a "you decide" answer. Research/planning may choose
exact shared-header/type names, checked-outcome representation, AST error taxonomy
and inspectable fields, deterministic tie-breaks inside the relationship stage,
decimal conversion algorithm and narrow test/build integration within these rules.
Those details must be made precise in the later reviewed plan, not implemented
during discussion.

Keep the accepted parser entry point, result alternatives and syntax-only behavior:
the parser still accepts decimal token shapes such as `256` or a very long numeral
without semantic conversion. No second parser, internal re-lexing or independent
certification of fixed signature/punctuation is added. Parsed/conforming-producer
syntax remains a precondition beyond the explicit AST/source/literal checks; those
checks do not prove that arbitrary caller-supplied surrounding bytes form a program.

Consequences of D-10/D-11: malformed literal content is an infrastructure failure,
even if a digit prefix would already be out of range. Complete literal conformance
precedes a range verdict. Decimal leading zeros do not imply octal or make a small
value invalid merely because the token is long. No general integer conversion,
arithmetic policy, new dependency or unbounded numeric framework is authorized.

SourceManager owns immutable original bytes; IDs are fresh and manager-local.
Different managers can reuse numeric IDs, so existing APIs cannot certify foreign-
manager provenance from numeric coincidence. Text/name views depend on the live
originating manager; copying AST spans does not copy or own that manager.
SourceError payloads and LF/CRLF/lone-CR/EOF location rules remain unchanged.

Diagnostic values own their strings. Reacquire borrowed collection views after
emission; retain none across emit or engine destruction. A validated value is not
proof that a shared engine can never gain later Errors: subsequent compilation
handoffs must still honor the inherited rule that any accumulated Error prevents
successful compilation. D-13 establishes the gate at this validation boundary;
it does not authorize later stages to ignore new errors.

Ordinary C++ allocation/exception behavior remains infrastructure policy inherited
from Phase 4. No new rollback guarantee, language exception model or OOM policy
is selected by the checked semantic-input contracts.

Preserve Conan 2.33.0, LLVM 23.1.2, accepted locks/profiles/build/include/CI policies
and component separation. Do not change toolchain, package/cache mechanics or
held infrastructure candidates as part of this phase's design.
</decisions>

<canonical_refs>
## Canonical References

Downstream agents MUST read these before research, planning or implementation.
There is no separate Phase 5 SPEC, external technical specification, matching
pending todo or packaged spike/sketch finding. Explicit owner decisions and the
canonical decision log take precedence over illustrative examples.

### Scope, requirements and accepted decisions

- `AGENTS.md` — lifecycle, architecture, fixed names and implementation guardrails.
- `.planning/PROJECT.md` — accepted capabilities and remaining M1 boundaries.
- `.planning/REQUIREMENTS.md` — AST-01/02 and SEMA-01/02; later AST/IR/backend/driver scope.
- `.planning/ROADMAP.md` — Phase 5 goal/four success criteria and Phase 6 boundary.
- `.planning/phases/02-source-identity-spans-and-diagnostics/02-CONTEXT.md` — immutable sources, original-byte spans, checked failures and owning diagnostics.
- `.planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-CONTEXT.md` — accepted range, decimal and lexical handoff decisions.
- `.planning/phases/04-smoke-profile-parser/04-CONTEXT.md` — all sixteen accepted parser contracts.
- `.planning/phases/04-smoke-profile-parser/04-03-SUMMARY.md` — accepted parser integration and lifetime evidence.
- `.planning/phases/04-smoke-profile-parser/04-VERIFICATION.md` — actual final parser goal verification and owner acceptance.
- `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md` — binding experimental M1 shape, inclusive 0–255 range, signed 32-bit result and decimal interpretation. Historical draft-era wording does not reopen its satisfied publication/acceptance gates.
- `docs/design/milestones/MILESTONE_1.md` — walking-skeleton boundaries.
- `docs/design/milestones/MILESTONE_STRATEGY.md` — project/probe pressure, not an alternate active phase numbering.
- `docs/design/DESIGN_PROBES.md` — P01 only; no general language features.
- `docs/design/DECISION_LOG.md` — accepted language directions and policy authority.
- `docs/design/VISION.md` — compiler separation and diagnostic direction.
- `docs/design/TYPE_SYSTEM.md` — future type semantics, not a general type-system implementation here.
- `docs/design/MEMORY_MODEL.md` — distinguish language ownership from C++ AST/value ownership.
- `docs/design/MODULE_SYSTEM.md` — future module semantics, not a new Phase 5 root/module design.

### Existing component APIs and test patterns

- `include/veyr/parsing/parser.hpp` — three-span value, source-compatible parser API and explicit checked alternatives.
- `src/parsing/parser.cpp` — existing complete recognizer and validation order; preserve behavior.
- `include/veyr/source/source_span.hpp` — original half-open ranges.
- `include/veyr/source/source_manager.hpp` — checked source/text resolution and lifetime.
- `include/veyr/diagnostics/diagnostic.hpp` — owning primary/related fields and explicit severity.
- `include/veyr/diagnostics/diagnostic_engine.hpp` — ordered accumulation, persistent Errors and view invalidation.
- `include/veyr/diagnostics/plain_text_renderer.hpp` — independent checked escaped presentation.
- `include/veyr/lexing/token.hpp` — existing token/source boundary; no semantic value field.
- `include/veyr/lexing/lexer.hpp` — synchronous recovered scan and Error-state contract.
- `tests/parsing/parser_test.cpp` — value/error-payload/precedence and independent literal expectations.
- `tests/parsing/parser_integration_test.cpp` — source/token destruction, 3,000 registrations, long literals, prior Errors and independent renderer integration.
- `CMakeLists.txt` — existing veyrcore integration, not a new framework.
- `tests/CMakeLists.txt` — existing GoogleTest/PRE_TEST and target policies.

### Workflow and implementation policy

- `docs/development/GSD_WORKFLOW.md` — review and dedicated discussion commit before separately authorized planning.
- `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md` — no discussion-time build/test or automatic transition.
- `docs/development/SESSION_HANDOFF_POLICY.md` — repository-based continuity.
- `docs/development/COMMIT_POLICY.md` — explicit lifecycle commit authorization.
- `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md` — later sensitive-file review.
- `docs/development/GIT_WORKFLOW.md` — reviewed stage publication.
- `docs/development/BUILD_POLICY.md` — C++23/Conan/CMake policy.
- `docs/development/DEVELOPER_BOOTSTRAP.md` — prepared dependencies and independent platform evidence.
- `docs/development/INCLUDE_POLICY.md` — canonical angle-bracket includes.
- `docs/development/CROSS_PLATFORM_POLICY.md` — Windows/Linux Tier 1 and future macOS.
</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets

- ParsedSmokeEntry already owns exactly the three required SourceSpans and has
  ordinary value semantics. It is currently defined in parser.hpp, which also
  imports lexer/token and source/diagnostic interfaces. An independent common
  value boundary must preserve existing parser-facing names and behavior.
- SourceManager provides immutable storage and checked text/span resolution;
  no new source loader, token borrow or source-text copy is needed.
- DiagnosticEngine and PlainTextRenderer already provide owning ordered errors,
  persistent Error state and separate bounded/escaped output.

### Established Patterns and Integration Points

- Public infrastructure uses explicit checked alternatives; source failures
  retain exact payloads. Syntax rejection is separate from infrastructure error.
- The parser remains responsible for complete fixed syntax/EOF recognition;
  semantics consumes the common AST and reads decimal bytes through the source
  manager. Semantic clients need not depend on parser/token interfaces.
- Later lowering consumes only the new successful validated-output boundary,
  retaining the constant and original source relationships. No lowering or LLVM
  implementation occurs in Phase 5.
- No separate AST or semantic implementation exists at discussion time. Existing
  native target/discovery, checked-error and integration patterns are reusable
  only after reviewed plans and explicit execution authorization.
</code_context>

<specifics>
## Specific Ideas

- `0`, `42` and `255` are within the inherited range; `00042` means decimal 42.
- `256` and arbitrarily long out-of-range decimal literals get the same fixed
  message, one full-literal Error and no related spans. Do not first require a
  successful host-integer conversion to classify an out-of-range decimal value.
- Caller-created `42x`, `-1` or empty literal ranges produce checked AST failure,
  not a numeric-range Error. A later invalid SourceManager span outranks an
  earlier relationship fault because the source-validation pass comes first.
- Valid current input plus prior Error produces the explicit blocked outcome;
  invalid current range still appends its own Error. Notes/Warnings alone do not
  prevent success. No failure exposes a successful partial validated value.
- Successful output survives input-AST destruction and retains all three spans;
  resolution still requires the live original manager. Subsequent source
  registrations must preserve inherited snapshot/view behavior.

These are discussion examples and future verification expectations, not newly
executed tests, `.vyr` fixtures or claims of Phase 5 implementation.
</specifics>

<deferred>
## Deferred Ideas

No new out-of-scope capability was proposed. General AST hierarchies, broad type
checking, expressions/statements/functions, numeric language semantics, ABI,
module graphs, lowering/LLVM/backend/driver work and final grammar remain outside
this phase. No alternative explicitly rejected above becomes a deferred feature.

Keep existing non-blocking diagnostic severity/renderer hardening and lexer-loop
trace opportunities unchanged and unimplemented. Stage 2 performance remediation
remains separately deferred; abandoned/held CI policy candidates remain excluded.
</deferred>

Platform impact:
- Windows: Same decimal range and checked/source/diagnostic contracts; preserve Tier 1 local/hosted verification gates. No process exit/ABI implementation here; developer-host inspection remains independently pending/non-blocking.
- Linux: Same original-byte and semantic contracts; Omarchy development and Ubuntu hosted evidence remain distinct. No POSIX exit-status truncation assumption enters validation.
- macOS future: Deferred; keep the common AST and semantic boundary host-independent. No Darwin execution claimed.

---
*Phase: 05-minimal-ast-and-semantic-smoke*
*Context gathered: 2026-10-21; sixteen substantive choices explicitly selected.*
*Discussion only. No research, plan, implementation, build/test, commit, push or automatic lifecycle transition is authorized by this document.*
