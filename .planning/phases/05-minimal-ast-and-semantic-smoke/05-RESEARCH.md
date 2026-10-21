# Phase 5: Minimal AST and Semantic Smoke — Research

Research date: 2026-10-21. Scope: accepted M1 only. Baseline: discussion commit
8e7bb1d8ce1f42cedfda06e498887507774635d9. Owner selected research first.
Runtime: inline Codex fallback required by the skill's no-unrequested-subagent
restriction; this is not an independent subagent research or checker run.

## User Constraints

The following locked decisions and implementation discretion are copied verbatim
from the committed CONTEXT. They bind every recommendation below.

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

## Deferred Ideas

No new out-of-scope capability was proposed. General AST hierarchies, broad type
checking, expressions/statements/functions, numeric language semantics, ABI,
module graphs, lowering/LLVM/backend/driver work and final grammar remain outside
this phase. No alternative explicitly rejected above becomes a deferred feature.

Keep existing non-blocking diagnostic severity/renderer hardening and lexer-loop
trace opportunities unchanged and unimplemented. Stage 2 performance remediation
remains separately deferred; abandoned/held CI policy candidates remain excluded.


## Project Constraints (from AGENTS.md)

[VERIFIED: AGENTS.md and docs/development/GSD_WORKFLOW.md] Planning changes only
planning documents; no compiler, build, test, dependency or native verification
execution. Explicit owner authorization gates execution, sensitive commits,
publication, requirements and phase acceptance independently. No automatic commit,
next-phase transition or speculative language decision. Repository memory governs.

[VERIFIED: BUILD_POLICY.md, INCLUDE_POLICY.md, CROSS_PLATFORM_POLICY.md] Reuse
C++23, Conan 2.33.0, LLVM 23.1.2, GoogleTest/GoogleMock 1.18.0, CMake/Ninja and
existing target policies. Owned headers use angle brackets. Windows/Linux remain
Tier 1; macOS future. Preserve locks, prepared dependencies and held CI candidates.
Apply watchdog supervision during future authorized native commands. Retain exact
execution logs and immutable archive mappings; do not rewrite historical receipts.

## Standard Stack

| Need | Existing facility | Provenance |
|---|---|---|
| Common syntax value | ParsedSmokeEntry and SourceSpan | [VERIFIED: include/veyr/parsing/parser.hpp, include/veyr/source/source_span.hpp] |
| Checked immutable text | SourceManager::validate/text, SourceError | [VERIFIED: include/veyr/source/source_manager.hpp] |
| Persistent owning errors | DiagnosticEngine and Diagnostic | [VERIFIED: include/veyr/diagnostics/diagnostic_engine.hpp, diagnostic.hpp] |
| Real integration | lex, parse, PlainTextRenderer | [VERIFIED: existing lexing/parsing/diagnostics headers and parser_integration_test.cpp] |
| Native testing | GoogleTest, PRE_TEST discovery | [VERIFIED: tests/CMakeLists.txt] |

No new dependency, numeric library, parser, framework or driver is needed.

## Architecture Patterns

[VERIFIED: parser.hpp/parser.cpp] ParsedSmokeEntry already has exactly entry_span,
return_span and integer_span, defaulted equality and ordinary copy/move semantics.
Move its unchanged definition to include/veyr/ast/smoke_entry.hpp and include that
header from parser.hpp. Keep namespace veyr, member names, parse signature,
ParseResult alternatives, tight span semantics and all 142 parser tests unchanged.
The AST header includes only source_span.hpp; semantic consumers need no parser,
token or lexer header. Header-only AST adds no compiled AST implementation.

Recommended bounded public semantic contract (planning choices, not existing code):
- include/veyr/semantics/smoke_validator.hpp and src/semantics/smoke_validator.cpp.
- validate_smoke(const SourceManager&, const ParsedSmokeEntry&, DiagnosticEngine&)
  returns SemanticResult with distinct ValidatedSmokeEntry, SemanticRejected,
  SourceError and AstError alternatives.
- SemanticRejectionKind: OutOfRange and BlockedByErrors. AstField: Entry, Return,
  Integer. AstErrorKind: SourceMismatch, EmptySpan, NonNestedSpan,
  NonDecimalLiteral. AstError owns kind, field, offending span and an optional
  absolute byte_offset (only NonDecimalLiteral). Equality supports exact tests.
- ValidatedSmokeEntry owns a private ParsedSmokeEntry and std::int32_t constant.
  ast() returns ParsedSmokeEntry by value; return_value() returns std::int32_t.
  Only the friend validate_smoke function can call its private payload constructor;
  no public default/payload construction, setters, writable reference or mutable
  AST view. Public ordinary copy/move construction and assignment remain allowed.
  Put a non-success alternative first in SemanticResult so default construction
  cannot fabricate validation. No generic factory/helper abstraction is required.

[CITED: https://eel.is/c++draft/class.friend] C++ friend access supports the narrow
construction boundary. [CITED: https://eel.is/c++draft/variant.ctor] Variant default
construction initializes its first alternative; tests must guard against accidental
default success. These are language-reference checks, not compiled API evidence;
exact declarations must compile during 05-01. No current draft addition is required.
Construct the private success object inside the friend, then move it into the
variant; do not ask std::variant in-place machinery to access a private constructor.

## Architectural Responsibility Map

| Component | Responsibility | Excluded work |
|---|---|---|
| Lexer | Original bytes → recovered tokens and lexical Errors | Numeric conversion/range, AST |
| Parser | Fixed complete syntax/EOF → owned three-span value | Semantic success gate, conversion |
| Shared AST | Same public three-span source-linked value | Tokens, source ownership, LLVM/general nodes |
| Semantic validator | Checked spans/relationships/digits/range, Error gate, owning validated value | Reparse surrounding syntax, driver/lowering |
| Diagnostics/renderer | Owning ordered values, persistent Errors, independent escaped display | Numeric validation or source mutation |

## Exact Validation and Error Priority

Planning contract within D-09–D-12 (deterministic tie-breaks, not new semantics):
1. SourceManager::validate entry, return, integer in that order. Stop at first
   exact original SourceError. Complete this pass before any relationship check.
2. Compare return SourceId then integer SourceId with entry SourceId; report first
   SourceMismatch and the offending field/span.
3. Check empty entry, return, integer in that order; report EmptySpan.
4. Check entry.begin < return.begin and return.end < entry.end (Return), then
   return.begin < integer.begin and integer.end < return.end (Integer), with
   NonNestedSpan. Valid SourceManager spans plus nonempty checks establish the
   remaining ordering; no unchecked subtraction or offset arithmetic is needed.
5. Resolve the already validated integer span through SourceManager::text,
   preserving any checked SourceError. Inspect every original byte as unsigned
   ASCII. First nondigit is NonDecimalLiteral with its absolute original offset.
6. Only after complete digit conformance may an out-of-range verdict emit the
   one exact Error and return OutOfRange. Otherwise prior engine Errors yield
   BlockedByErrors without emission; only the remaining branch constructs success.

Public AST relationships do not prove full source syntax, manager provenance or
absence of later engine Errors. Preserve the conforming-producer and originating
manager preconditions; never relabel a checked caller-contract failure as a user
range diagnostic. Invalid source programs already receive lexer/parser diagnostics.

## Decimal Conversion Without Host Overflow

Derived bounded algorithm: hold an unsigned accumulator only in [0,255] and a
latched range flag. For each verified digit d, if not already flagged, compare
accumulator against 25 and d against 5 when accumulator is 25; flag before any
operation exceeding 255. Otherwise accumulate 10*value+d. Continue checking all
remaining bytes after range is known. Emit nothing until all bytes are digits.
This supports arbitrarily long leading zeros, detects a late nondigit after an
out-of-range prefix, and uses O(n) byte checks/O(1) numeric auxiliary storage.
Return the in-range value as std::int32_t; no bigint, stoll, atoi, floating point,
locale, recursion, digit-count shortcut or fixed-width overflow is needed.
This is a mathematical bound, not an executed benchmark or implementation.

## Don't Hand-Roll

Reuse checked source lookup and rendering; do not duplicate source storage,
foreign-manager detection, token classification, parser matching, package handling
or terminal escaping. Use existing variant/value tests, diagnostic-field comparisons
and native target policy. No AST hierarchy, visitor, arena or semantic type graph.

## Common Pitfalls

- Checking has_errors before source/literal validation suppresses required failures.
- Stopping at numeric overflow before inspecting a trailing nondigit returns the
  wrong checked alternative; all literal bytes must first conform.
- Copying only a literal span drops D-16 context; const getters must not return a
  mutable alias. Public aggregate/default construction must not forge success.
- Empty EOF spans are source-valid but AST-invalid; compare complete error payloads.
- Identical numeric IDs from different managers cannot prove origin. Do not test
  destroyed manager access or fabricate foreign-manager detection guarantees.
- Invalid AST relationships are checked infrastructure errors; malformed source
  syntax remains the existing parser's source-linked diagnostic responsibility.
- GoogleTest parameterized names may have a prefix before the suite; use an
  unanchored semantic suite regex and reconcile actual names, not missing labels.

## Code Examples / Existing Analogs

[VERIFIED: parser.cpp] Checked source calls inspect std::get_if<SourceError> and
return the original payload. [VERIFIED: parser_integration_test.cpp] Helpers copy
Diagnostic fields before a later emit and compare severity/message/primary/related
values after reacquiring views. Reuse these patterns, not implementation tables.
[CITED: https://google.github.io/googletest/advanced.html] Parameterized fixtures
support named rows; [CITED: https://cmake.org/cmake/help/latest/module/GoogleTest.html]
gtest_discover_tests enumerates the compiled executable. Preserve PRE_TEST and
prove the expected row identities executed in both configurations.

## Validation Architecture

05-01: shared AST isolation, complete validator/API, build integration and baseline
unit tests. 05-02: independent forged-AST, precedence, decimal and diagnostic
matrices; stop for owner review before any production correction. 05-03: real
lexer/parser/semantics/renderer and lifetime integration plus independent final
Debug/Release evidence. Each plan has two automated tasks and one blocking owner
checkpoint. Sequential dependencies arise from tests consuming the delivered API
and from review gates, not arbitrary numbering. No new scaffolding wave.

Use exact future commands in 05-VALIDATION.md; each task gets focused native
feedback, each plan full local test/strict/include checks, and phase acceptance
requires fresh hosted Ubuntu/Windows Debug/Release at the published implementation
commit plus canonical goal verification and explicit owner acceptance. Existing
561 tests/142 parser cases and 16 C++23 entries are accepted historical baselines,
not future execution results. Retain actual identities, counts and immutable hashes.

## Security and Resource Review

Input is caller-controlled spans and immutable bytes. Checked source validation
precedes arithmetic/text access; explicit ASCII comparisons avoid signed-char and
locale hazards. Constant bounded numeric state avoids host overflow and giant
numeric allocation. A range message has fixed length and uses existing bounded
rendering. Private success construction and Error gating protect the handoff.
Ordinary C++ allocation exceptions remain inherited, with no rollback/OOM promise.
ASVS web/auth/storage categories are inapplicable to this in-process C++ library;
there is no new service, credential, persistence layer or package to audit.

## Runtime State Inventory (shared-header refactor)

1. Persisted data: no new file/archive/schema; spans remain process-local values.
2. In-memory data: unchanged ParsedSmokeEntry name/fields and copy/move behavior.
3. Configuration: same targets/policies; only native source registration changes.
4. External consumers: parser.hpp still exposes the same type and parse API.
5. Historical evidence: retain original header/test/CMake bytes and hashes before
   authorized edits; archive mappings preserve accepted Phase 4 and discussion.

## Open Questions (RESOLVED)

No unresolved owner language choice. Exact bounded API/error/priority choices
above are proposed planning details for owner review. No runtime result, dependency
installation, source change or performance guarantee is claimed by this research.

## Sources and Confidence

Repository files cited above were read directly; prescribed API changes and the
algorithm are explicit proposals, not already implemented facts. Official primary
C++/GoogleTest/CMake documentation was inspected read-only; no new package/API
version is selected. The documentation lookup seam suggested Context7, unavailable
in this session, so primary web pages supplied the fallback. The generic
classify-confidence seam labels webfetch LOW; this is retained as a tooling limit,
not promoted into runtime proof. All uncertain C++ declaration details remain
subject to actual Debug/Release compilation in 05-01. No external assumption
overrides the accepted context or local pinned build policies.

Platform impact:
- Windows: identical byte/error/value contracts; preserve clang-cl, CRT, PRE_TEST
  and four-job acceptance. No process exit-code truncation assumption.
- Linux: identical host-independent checks; Omarchy local and Ubuntu hosted are
  distinct evidence. No POSIX-only implementation.
- macOS future: deferred, with no platform API or ABI dependency introduced.
