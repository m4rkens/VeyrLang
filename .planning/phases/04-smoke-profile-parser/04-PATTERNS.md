# Phase 4: Smoke Profile Parser - Pattern Map

**Mapped:** 2026-10-18
**Files analyzed:** 6 proposed new/modified files
**Analogs found:** 6 / 6 by role and data flow; no existing parser algorithm
**Authority:** `04-CONTEXT.md` D-01–D-16 and the accepted M1 profile. Research API names, error ordering and complete label tables are bounded recommendations for the owner-reviewed plan, not additional discussion decisions.

This is a read-only planning map. Only this artifact was written; no source, test, build configuration, dependency, CI or accepted evidence was changed and no build/test command ran. Existing source, diagnostic, token, lexer and renderer APIs remain unchanged.

## File Classification

| New/Modified File | Action | Role | Data Flow | Closest Analog | Match Quality |
|-------------------|--------|------|-----------|----------------|---------------|
| `include/veyr/parsing/parser.hpp` | create | model | transform | `include/veyr/lexing/token.hpp`; checked API companion `include/veyr/lexing/lexer.hpp` | exact value-model match; checked API composition |
| `src/parsing/parser.cpp` | create | service | transform | `src/lexing/lexer.cpp` | exact component/data-flow match; parser algorithm is new |
| `tests/parsing/parser_test.cpp` | create | test | transform | `tests/lexing/lexer_test.cpp`; value/error companion `tests/lexing/token_test.cpp` | exact native contract-test match |
| `tests/parsing/parser_integration_test.cpp` | create | test | transform | `tests/lexing/lexer_test.cpp` integration sections; lifetime companion `tests/lexing/token_test.cpp` | exact pipeline/lifetime-test match |
| `CMakeLists.txt` | modify | config | batch | existing `CMakeLists.txt` veyrcore source list | exact self-analog |
| `tests/CMakeLists.txt` | modify | config | batch | existing `tests/CMakeLists.txt` lexer_tests block | exact self-analog |

Here “exact” means the role/data-flow convention is reusable, not that a parser, its validation order or its diagnostics already exist. The parser consumes an in-memory completed stream synchronously. It performs no file I/O, streaming, process execution or pub/sub. Test registration is build-graph configuration, not authorization to run that graph.

## Pattern Assignments

### `include/veyr/parsing/parser.hpp` (model, transform)

**Analogs:** `include/veyr/lexing/token.hpp` for small independent span values; `include/veyr/lexing/lexer.hpp` for the checked synchronous public API.

**Imports and declaration pattern** — `include/veyr/lexing/lexer.hpp` lines 1–12:

```cpp
#pragma once

#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/lexing/token.hpp>
#include <veyr/source/source_manager.hpp>

#include <variant>
#include <vector>

namespace veyr {

using LexResult = std::variant<std::vector<Token>, SourceError>;
```

Copy the owned-header angle paths, direct standard-library includes and `namespace veyr`. Use the standard headers actually needed by the new declaration (`<span>`, `<variant>`, `<optional>`, `<cstddef>` as appropriate); do not copy `<vector>` merely because the lexer owns a vector. Tokens are a synchronous `std::span<const Token>` input, never retained output storage.

**Independent value pattern** — `include/veyr/lexing/token.hpp` lines 24–31:

```cpp
// Initialize both fields. Resolve original bytes through the originating
// SourceManager, which must outlive that resolution. No lexeme/value is cached.
struct Token {
  TokenKind kind;
  SourceSpan span;

  bool operator==(const Token&) const = default;
};
```

Adapt to exactly three `SourceSpan` fields: complete entry, return statement and integer literal. Preserve ordinary copy/move/value behavior. No borrowed token, source-text copy/view, numeric result, general AST node or compilation-success flag belongs in this value. Source resolution still requires the originating live SourceManager.

**Synchronous checked API and lifetime documentation** — `include/veyr/lexing/lexer.hpp` lines 14–22:

```cpp
// Synchronously scans the accepted M1 Experimental Smoke Profile only. Success
// owns recovered tokens, not a compilation-success indication: clients must also
// inspect diagnostics.has_errors(). Existing diagnostics/order are preserved.
// Unknown source returns the original checked error, with no diagnostic or EOF.
// Source views are call-local; later lexeme resolution needs the originating
// SourceManager to remain alive. No diagnostic view is retained across emit().
// Allocation exceptions retain ordinary C++ semantics, without batch rollback.
[[nodiscard]] LexResult lex(const SourceManager& sources, SourceId source_id,
                            DiagnosticEngine& diagnostics);
```

Adapt this documentation to the parser's distinct semantics: successful syntax returns only a complete three-span value; syntax rejection and lexical rejection are explicit checked alternatives; original SourceError and token-stream infrastructure failures remain distinguishable. A previous engine Error must not gate a well-formed current stream. Success never clears that Error or establishes successful compilation.

**Checked-error value pattern** — `include/veyr/source/source_manager.hpp` lines 16–26:

```cpp
enum class SourceErrorKind { UnknownSource, ReversedRange, BeyondSource };

struct SourceError {
  SourceErrorKind kind;
  SourceId source_id;
  std::size_t begin;
  std::size_t end;
  std::optional<std::size_t> source_size;

  bool operator==(const SourceError&) const = default;
};
```

Reuse this exact existing type as an alternative; do not edit, flatten or reconstruct its payload. Follow its enum-plus-value form for parser-specific structural errors.

**Research-proposed names for planning:** `ParsedSmokeEntry`, `ParseRejectionKind`, `ParseRejected`, `TokenStreamErrorKind`, `TokenStreamError`, `ParseResult` and `parse(...)`. The proposed result is `std::variant<ParsedSmokeEntry, ParseRejected, SourceError, TokenStreamError>`; parser rejection distinguishes `LexicalErrors` from `SyntaxError`. The structural error carries selected `SourceId` and optional token index, absent for missing EOF. Finalize these names/layouts in the plan without expanding D-01–D-16.

**Auth/guard:** no authentication or authorization boundary exists. The applicable guard is checked selected-source/token-stream validation. Document conforming producer, initialized tokens, live call-duration input storage and manager-local identity as preconditions. Numeric SourceIds do not certify foreign-manager provenance.

**Testing assignment:** value shape, copy/move, exact variant alternatives and SourceError payloads in `parser_test.cpp`; token-storage destruction and later original-span resolution in `parser_integration_test.cpp`.

---

### `src/parsing/parser.cpp` (service, transform)

**Analog:** `src/lexing/lexer.cpp` for component structure, original-source access, checked propagation and owned diagnostic emission. `src/source/source_manager.cpp` and the renderer provide supporting validation boundaries, not replacement implementations.

**Imports/private-helper pattern** — `src/lexing/lexer.cpp` lines 1–10:

```cpp
#include <veyr/lexing/lexer.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace veyr {
namespace {
```

Include the parser's own canonical header first; put fixed labels, known-kind checking, stream validation and recognition helpers in the anonymous namespace. Add only directly used standard headers. Do not copy lexical byte classifiers or call `lex()` from the parser implementation.

**Source-error propagation pattern** — `src/lexing/lexer.cpp` lines 65–71:

```cpp
LexResult lex(const SourceManager& sources, SourceId source_id,
              DiagnosticEngine& diagnostics) {
  const auto input = sources.source_text(source_id);
  if (const auto* error = std::get_if<SourceError>(&input)) {
    return *error;
  }
  const auto text = std::get<std::string_view>(input);
```

Parser first resolves the selected source and retains only its size for structural checks. Return the exact original checked SourceError before any diagnostic emission. For every token, call existing `sources.validate(token.span)` and propagate its error unchanged before parser-specific identity/kind/shape/order checks. Do not use source bytes to certify token spelling or omitted whitespace.

**Validation-before-success analog** — `src/diagnostics/plain_text_renderer.cpp` lines 129–142:

```cpp
  // Validate all references before constructing any successful output. Preserve
  // the exact original range/error and primary-first, related-insertion order.
  if (diagnostic.primary) {
    const auto checked = sources.validate(*diagnostic.primary);
    if (const auto* error = std::get_if<SourceError>(&checked)) {
      return RenderError{*error, std::nullopt};
    }
  }
  for (std::size_t index = 0; index < diagnostic.related.size(); ++index) {
    const auto checked = sources.validate(diagnostic.related[index].span);
    if (const auto* error = std::get_if<SourceError>(&checked)) {
      return RenderError{*error, index};
    }
  }
```

Copy the separation of complete structural checking from successful output construction. Parser-specific sequencing has no existing full analog: validate the entire stream, then reject Invalid, then recognize syntax. Remember Invalid while validating; a later structural fault must still win over it. Do not return lexical rejection inside the structural pass or construct a parse value at `}` before EOF.

**Required new structural checks, grounded in D-11:** selected-source lookup; checked spans; source agreement; explicit supported-kind switch over all thirteen enumerators; nonempty non-EOF spans; source order/nonoverlap; exactly one final zero-width EOF at source end. Allow adjacency and whitespace gaps. Never use unchecked enum-to-array indexing, manufacture EOF, clamp bounds or rely solely on assertions. Research's proposed deterministic precedence is recorded under Shared Patterns below.

**Explicit Error emission pattern** — `src/lexing/lexer.cpp` lines 160–167:

```cpp
    const SourceSpan span{source_id, begin, cursor};
    tokens.push_back({kind, span});
    if (kind == TokenKind::Invalid) {
      diagnostics.emit({DiagnosticSeverity::Error, std::string{message}, span, {}});
    }
  }
  tokens.push_back({TokenKind::EndOfFile, {source_id, text.size(), text.size()}});
  return tokens;
```

Copy the explicit `DiagnosticSeverity::Error`, owned message and original primary span. Do not copy lexer recovery or EOF production. The parser appends at most one syntax Error after gating, returns explicit syntax rejection immediately and exposes no partial structure. For expected `)` or `}`, supply exactly one RelatedSpan for the recognized matching opener; otherwise related spans are empty. Existing `RelatedSpan` owns its label (`include/veyr/diagnostics/diagnostic.hpp` lines 13–16), so a locally constructed Diagnostic may safely be moved to `emit()`.

**Core recognizer:** fixed sequence `Fn, Main, LeftParen, RightParen, Arrow, Int32, LeftBrace, Return, IntegerLiteral, Semicolon, RightBrace, EndOfFile`. Use bounded access to the actual validated sentinel. After complete recognition construct entry `[fn.begin,right_brace.end)`, return `[return.begin,semicolon.end)`, and literal as its original token span, all with the selected source identity. No integer conversion/range test, recovery, AST hierarchy, recursion or general grammar framework.

**Error handling:** checked variants for infrastructure and ordinary rejection; no syntax exception, catch-all translation, allocation rollback or new OOM policy. Rendering, source locations and escaping remain with their existing consumers. Infrastructure/lexical rejection emits nothing and preserves prior diagnostics; earlier Error does not gate valid syntax.

**Testing assignment:** independent per-position syntax/EOF fixtures, exact diagnostic fields, malformed structures, multifault precedence and no-success-on-failure assertions in `parser_test.cpp`; real lexer/renderer/handoff cases in `parser_integration_test.cpp`.

---

### `tests/parsing/parser_test.cpp` (test, transform)

**Analogs:** `tests/lexing/lexer_test.cpp` for native exact-oracle tables and checked calls; `tests/lexing/token_test.cpp` for independent value and infrastructure-error assertions.

**Imports/fixture style** — `tests/lexing/lexer_test.cpp` lines 1–6 and 21–26 use canonical project includes, `<gtest/gtest.h>`, an anonymous namespace, and a compile-time exact variant-shape check. Copy that structure with `<veyr/parsing/parser.hpp>` and direct headers actually used. Keep unit tests focused on public parser contracts; use initialized caller token streams for structural adversarial cases. No new helper package/framework is needed.

**Checked success and independent original-span assertions** — `tests/lexing/lexer_test.cpp` lines 44–60:

```cpp
void expect_scan(const std::string& bytes, const std::vector<Expected>& expected) {
  veyr::SourceManager sources;
  const auto registered = sources.register_source("<smoke-profile>", bytes);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const auto id = std::get<veyr::SourceId>(registered);
  veyr::DiagnosticEngine diagnostics;
  const auto result = veyr::lex(sources, id, diagnostics);
  ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
  const auto& tokens = std::get<std::vector<veyr::Token>>(result);
  ASSERT_EQ(tokens.size(), expected.size() + 1);
  const auto errors = diagnostics.diagnostics();
  std::size_t error_index = 0;
  for (std::size_t index = 0; index < expected.size(); ++index) {
    SCOPED_TRACE(testing::Message() << "token=" << index);
    const auto& item = expected[index];
    const veyr::Token wanted{item.kind, {id, item.begin, item.end}};
    EXPECT_EQ(tokens[index], wanted);
```

Adapt the invocation and success alternative, not the lexical oracle. Assert all three independently specified result spans. For `fn main() -> int32 { return 42; }`, expected entry is `[0,33)`, return `[21,31)`, integer `[28,30)` (profile §5.7; lexer fixture lines 85–92). Add surrounding/internal whitespace and decimal-shape variants including `256`, leading zeros and long digits without semantic rejection.

**Full diagnostic-field pattern** — `tests/lexing/lexer_test.cpp` lines 66–73:

```cpp
    if (item.kind == TokenKind::Invalid) {
      ASSERT_LT(error_index, errors.size());
      const auto& error = errors[error_index++];
      EXPECT_EQ(error.severity, veyr::DiagnosticSeverity::Error);
      EXPECT_EQ(error.message, item.message);
      EXPECT_EQ(error.primary, wanted.span);
      EXPECT_TRUE(error.related.empty());
    }
```

For parser rejection assert the exact checked rejection alternative, diagnostic delta of one, explicit Error severity, complete message, primary and related count/span/label. Independent test rows must hold literal expected strings and byte offsets. Do not import parser expectation/label tables or derive the expected result from the implementation. Expected `)`/`}` alone adds its one recognized opener; missing integer or semicolon does not inherit an opener. Include first-mismatch-only and trailing-token failures, and assert no ParsedSmokeEntry alternative for every failure.

**Parameterized discovery pattern** — `tests/lexing/lexer_test.cpp` lines 251–256 and 303–304:

```cpp
class SmokeProfileLexerRecovery : public testing::TestWithParam<RecoveryCase> {};

TEST_P(SmokeProfileLexerRecovery, ReviewedOriginalByteSpansMessagesAndEof) {
  SCOPED_TRACE(testing::PrintToString(GetParam().bytes));
  expect_scan(GetParam().bytes, GetParam().tokens);
}
```

```cpp
INSTANTIATE_TEST_SUITE_P(SmokeProfileExamples, SmokeProfileLexerRecovery,
                         testing::ValuesIn(profile_rows));
```

Use `SmokeProfileParser...` suite names and descriptive deterministic ASCII case names. Existing `PrintTo` at lexer-test lines 242–249 avoids raw object-address labels; adapt a controlled case-name printer or explicit naming callback. Each of the twelve syntax positions should have independently observable cases, including real-token/EOF close-delimiter failures, empty source and tokens after the complete entry. Small mutation loops need SCOPED_TRACE; broad random bytes do not replace exact cases.

**Exact infrastructure payload pattern** — `tests/lexing/token_test.cpp` lines 107–117:

```cpp
  const std::array<veyr::SourceError, 3> errors = {{
      {veyr::SourceErrorKind::UnknownSource, {0}, 9, 1, std::nullopt},
      {veyr::SourceErrorKind::ReversedRange, id, 2, 1, 3},
      {veyr::SourceErrorKind::BeyondSource, id, 1, 4, 3}}};
  for (const auto& expected : errors) {
    const Token token{TokenKind::Invalid,
                      {expected.source_id, expected.begin, expected.end}};
    const auto text = sources.text(token.span);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(text));
    EXPECT_EQ(std::get<veyr::SourceError>(text), expected);
  }
```

Apply to parser SourceError alternatives, preserving every kind/ID/range/optional-size field; assert no new diagnostics. Separately table parser-specific infrastructure kind/index/selected-source payloads: known foreign source, unsupported enum, empty/reversed/out-of-bounds/overlapping/backward spans, missing/nonfinal/duplicate/wrong-shaped EOF. Empty vector is infrastructure failure; EOF-only empty source is valid structure followed by syntax rejection. Add explicit combined-fault fixtures proving structural checking wins over Invalid or earlier syntax mismatch, and the plan's selected within-token/earliest-token priority. Do not require spelling checks for forged Fn-over-return tokens or foreign-manager detection when numeric IDs coincide.

**Value testing** — `tests/lexing/token_test.cpp` lines 23–39 demonstrate aggregate/type checks and independent copies. Adapt ordinary copy/move/equality tests to the three-span result; avoid turning infrastructure ownership tests into language ownership rules.

---

### `tests/parsing/parser_integration_test.cpp` (test, transform)

**Analogs:** real-client sections of `tests/lexing/lexer_test.cpp` and scoped storage in `tests/lexing/token_test.cpp`. Preserve the existing framework and in-memory source registration; no `.vyr` fixture loader or driver work.

**Actual pipeline:** register immutable bytes → actual `lex()` → require token-vector alternative → actual `parse()` → inspect success/rejection and owning Diagnostic fields → actual PlainTextRenderer when presentation is under test. Only tests/callers invoke lex; the parser implementation does not. Use lexically valid wrong syntax for syntax-message fixtures; unsupported words/numbers/comments/high bytes are lexical handoff cases.

**Prior severity and compilation-handoff pattern** — `tests/lexing/lexer_test.cpp` lines 559–566:

```cpp
    veyr::DiagnosticEngine diagnostics;
    diagnostics.emit({severity, "prior", veyr::SourceSpan{clean_id, 0, 1},
                       {{{bad_id, 0, 1}, "context"}}});
    const auto clean_result = veyr::lex(sources, clean_id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(clean_result));
    EXPECT_EQ(diagnostics.has_errors(), severity == veyr::DiagnosticSeverity::Error);
    EXPECT_EQ(std::holds_alternative<std::vector<veyr::Token>>(clean_result) &&
                  !diagnostics.has_errors(), severity != veyr::DiagnosticSeverity::Error);
```

Adapt to a complete fixed-entry parse and all prior severities. A previous Error still permits a ParsedSmokeEntry for a clean separate stream; it makes the illustrative later-handoff predicate false. Preserve previous fields, order, related labels, duplicates and Error state. Parse syntax failure twice to prove two ordered appended duplicates, never recovery-as-success.

**Owned diagnostic snapshot pattern** — `tests/lexing/lexer_test.cpp` lines 619–629:

```cpp
  std::vector<veyr::Diagnostic> copies;
  {
    const auto view = diagnostics.diagnostics();
    copies.assign(view.begin(), view.end());
  }
  ASSERT_EQ(copies.size(), 3u);
  std::vector<std::string> output;
  for (std::size_t index = 0; index < copies.size(); ++index) {
    const auto rendered = veyr::PlainTextRenderer{}.render(copies[index], sources);
    ASSERT_TRUE(std::holds_alternative<std::string>(rendered));
    output.push_back(std::get<std::string>(rendered));
```

Copy diagnostics before a parse that might emit; compare complete fields afterward using a freshly acquired engine view. Do not retain a span/reference across `emit()`. For Invalid before/inside/after an otherwise valid program, prove explicit lexical rejection, no parse value and exact unchanged lexical diagnostics; include numeric/comment/high-byte/NUL and accepted `/*/return` behavior. A later Invalid suppresses even an earlier potential syntax complaint after structural validation succeeds.

**Hostile presentation pattern** — `tests/lexing/lexer_test.cpp` lines 631–637:

```cpp
    EXPECT_EQ(output.back(), "bad\\n\\x1B.vyr:1:" + std::to_string(index + 1) +
        ": error: " + std::string{message} + "\n1 | \\x1B\\x00\\xFF\n  | " +
        std::string(index * 4, ' ') + "^~~~\n");
    EXPECT_EQ(std::count(output.back().begin(), output.back().end(), '\n'), 3);
    for (const unsigned char byte : output.back()) {
      EXPECT_TRUE(byte == '\n' || (byte >= 0x20 && byte <= 0x7e));
    }
```

Use hostile display-name bytes with lexically valid malformed profile syntax so the emitted Error is the real parser Error. Independently assert escaped owned rendered output, original locations, opener-related presentation and preserved engine state. Source text containing unsupported hostile bytes belongs to lexical rejection, not a syntax-message fixture.

**Token-storage lifetime pattern** — `tests/lexing/token_test.cpp` lines 134–149:

```cpp
TEST(SmokeProfileToken, ReturnedTokensOutliveCallerStorageAndFurtherRegistrations) {
  veyr::SourceManager sources;
  std::vector<Token> tokens;
  {
    std::string caller_bytes = "fn main() -> int32 { return 42; }";
    std::string caller_name = "caller.vyr";
    const auto registered = sources.register_source(caller_name, caller_bytes);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
    caller_bytes.assign(4096, '?');
    caller_name.assign(4096, '!');
    veyr::DiagnosticEngine diagnostics;
    auto result = veyr::lex(sources, std::get<veyr::SourceId>(registered), diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(result));
    EXPECT_FALSE(diagnostics.has_errors());
    tokens = std::get<std::vector<Token>>(std::move(result));
  }  // Caller storage, result wrapper and DiagnosticEngine are gone; manager lives.
```

Adapt the scope to save only the parsed value, then destroy the token vector/result wrapper before resolving all three spans. Copy/move the saved value, register further same-name snapshots and require fresh IDs/original resolved bytes. Keep SourceManager alive; never test by dereferencing dead token/source storage. `token_test.cpp` lines 159–174 provides the repeated-registration/resolution assertions. Lines 120–132 explicitly document coincident IDs across different managers; preserve that limitation.

**Location/EOF patterns:** lexer-test lines 465–489 use explicit `SourceLocation` rows for original LF/CRLF/lone-CR bytes. Lines 492–513 distinguish empty source, trailing LF/CRLF and lone CR at EOF. Adapt to parser Errors with real primary/related spans. Expected EOF is exactly `[source_size,source_size)`, including trailing whitespace, and never an insertion fabricated before a real unexpected token.

---

### `CMakeLists.txt` (config, batch)

**Analog:** existing root `CMakeLists.txt` lines 43–51:

```cmake
add_library(veyrcore STATIC
  src/core/build_info.cpp
  src/diagnostics/diagnostic_engine.cpp
  src/diagnostics/plain_text_renderer.cpp
  src/lexing/lexer.cpp
  src/source/source_manager.cpp)
veyr_target_policy(veyrcore)
target_include_directories(veyrcore PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_definitions(veyrcore PRIVATE VEYR_BUILD_VERSION="${PROJECT_VERSION}")
```

During authorized execution add only `src/parsing/parser.cpp` to this existing source list, following its component ordering. Reuse veyrcore, target policy and public include root. No second compiler library, new LLVM coupling, package lookup or generated native-build layer. Preserve the existing native Windows/Linux/Clang/build-selection gates (lines 3–28), exported compilation database (line 16), and include check (lines 57–61).

**Validation:** planning may inspect the source list but must not configure/build it. Later incremental compilation and existing include-policy checks provide concrete evidence; no success is claimed here.

---

### `tests/CMakeLists.txt` (config, batch)

**Analog:** existing `tests/CMakeLists.txt` lines 26–31:

```cmake
add_executable(lexer_tests
  lexing/token_test.cpp
  lexing/lexer_test.cpp)
veyr_target_policy(lexer_tests)
target_link_libraries(lexer_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(lexer_tests DISCOVERY_MODE PRE_TEST)
```

Add the analogous `parser_tests` executable containing `parsing/parser_test.cpp` and `parsing/parser_integration_test.cpp`. Apply `veyr_target_policy`, link exactly the existing native test dependencies shown above and keep `DISCOVERY_MODE PRE_TEST`. Existing lines 1–7 already find exact GoogleTest/GoogleMock 1.18.0 and include GoogleTest; no new find/package command or framework is needed. Preserve the existing include-policy test at lines 194–196. No CI, native profile, lock or dependency edit follows from adding this target.

**Evidence pattern:** actual discovered names/counts and targeted nonzero results must be collected later; source registration alone proves neither discovery nor passing tests. Suites should be selected by `SmokeProfileParser` naming in the existing CTest workflow. Future focused command pattern: `ctest --test-dir "<selected-build-dir>" -R SmokeProfileParser --output-on-failure --no-tests=error` after an authorized build. Full Debug/Release wrapper checks and hosted evidence remain separate execution/verification actions. Do not invent a `scripts/dev.py --filter` option, hardcode a machine build path or run any command from this map.

## Shared Patterns

### Original source identity, validation and unchanged error payloads

**Source:** `include/veyr/source/source_span.hpp` lines 9–15, `src/source/source_manager.cpp` lines 74–88.
**Apply to:** parser API, structural validation and both test files.

```cpp
// Original byte offsets, half-open [begin, end); empty spans include EOF.
struct SourceSpan {
  SourceId source_id;
  std::size_t begin = 0;
  std::size_t end = 0;

  bool operator==(const SourceSpan&) const = default;
};
```

```cpp
ValidationResult SourceManager::validate(SourceSpan span) const {
  const auto* source = lookup(span.source_id);
  if (!source) {
    return SourceError{SourceErrorKind::UnknownSource, span.source_id,
                       span.begin, span.end, std::nullopt};
  }
  if (span.begin > span.end) {
    return SourceError{SourceErrorKind::ReversedRange, span.source_id,
                       span.begin, span.end, source->bytes.size()};
  }
  if (span.end > source->bytes.size()) {
    return SourceError{SourceErrorKind::BeyondSource, span.source_id,
                       span.begin, span.end, source->bytes.size()};
  }
  return std::monostate{};
}
```

Call existing validation; do not reproduce this implementation. It permits empty spans generally, so the parser separately rejects empty non-EOF token spans. Its UnknownSource → ReversedRange → BeyondSource priority/payload must survive unchanged. SourceId is manager-local (`source_id.hpp` lines 7–11); selecting the originating manager remains a caller obligation.

**Recommended parser precedence for the plan** (research Architecture Pattern 2; bounded discretion under D-11):

1. Resolve selected source before inspecting token storage.
2. Visit increasing token indices; validate each span first and preserve any SourceError.
3. Then source agreement, supported kind, and EOF/non-EOF shape.
4. EOF must be final before checking its exact source-end zero width; non-EOF must be nonempty.
5. Then require current begin ≥ previous end.
6. After all per-token checks, require a final EOF, including for empty token vectors.
7. Only then reject remembered Invalid, without diagnostic emission.
8. Only then match syntax, returning its first Error rejection or the complete result.

Earlier token index wins; within one index the listed order wins. Missing EOF has no token index. The plan should finalize this deterministic taxonomy/priority and independently test combined faults. It must not claim SourceManager validation alone establishes the full D-11 contract.

### Owning diagnostics, explicit severity and separate presentation

**Source:** `include/veyr/diagnostics/diagnostic.hpp` lines 13–24.
**Apply to:** parser mismatch emission and all diagnostic tests.

```cpp
struct RelatedSpan {
  SourceSpan span;
  std::string label;
};

// Owns message/label bytes. Spans refer to the originating SourceManager;
// checked source resolution and rendering remain separate operations.
struct Diagnostic {
  DiagnosticSeverity severity;
  std::string message;
  std::optional<SourceSpan> primary;
  std::vector<RelatedSpan> related;
};
```

Explicitly initialize `DiagnosticSeverity::Error`. No new diagnostic type, renderer, raw-source interpolation, terminal output, escaping routine or logger is required. Related opener context is a field in this one Error, not a separate emitted Note. The existing renderer prints related context independently (`plain_text_renderer.cpp` lines 160–168); parser code does not format it.

**Complete proposed wording table** (research Architecture Pattern 3; finalize in owner-reviewed plan):

| Expected position | Required kind | Expected controlled label | Related context on mismatch |
|---|---|---|---|
| 0 | Fn | `'fn'` | none |
| 1 | Main | `'main'` | none |
| 2 | LeftParen | `'('` | none |
| 3 | RightParen | `')'` | exactly matched `(` token; `opening '(' is here` |
| 4 | Arrow | `'->'` | none |
| 5 | Int32 | `'int32'` | none |
| 6 | LeftBrace | `'{'` | none |
| 7 | Return | `'return'` | none |
| 8 | IntegerLiteral | `decimal integer literal` | none |
| 9 | Semicolon | `';'` | none |
| 10 | RightBrace | `'}'` | exactly matched `{` token; `opening '{' is here` |
| 11 | EndOfFile | `end of source` | none |

Found labels for supported real kinds are the corresponding quoted fixed spelling above; IntegerLiteral is `decimal integer literal`, EOF is `end of source`. Invalid and unsupported enums never reach message construction. Compose exactly `expected <expected-label>, found <found-label> in M1 Experimental Smoke Profile` without terminal punctuation. Do not echo lexemes or display names. D-13 supplies the complete unexpected token span; D-14 supplies the exact validated EOF span. Missing semicolon before `}` therefore reports `expected ';', found '}' in M1 Experimental Smoke Profile` at that real `}` span with no related context.

### Ordered accumulation, persistent Error state and view invalidation

**Source:** `include/veyr/diagnostics/diagnostic_engine.hpp` lines 19–23 and `src/diagnostics/diagnostic_engine.cpp` lines 8–20.
**Apply to:** parser/caller integration and both test files.

```cpp
  void emit(Diagnostic diagnostic);
  // Preserves insertion order and duplicates. Reacquire after every emit;
  // views/references are valid only until the next emit or engine destruction.
  [[nodiscard]] std::span<const Diagnostic> diagnostics() const;
  [[nodiscard]] bool has_errors() const;
```

```cpp
void DiagnosticEngine::emit(Diagnostic diagnostic) {
  diagnostics_.push_back(std::move(diagnostic));
}
```

No parser engine reset, deduplication or prior-Error entry gate. `has_errors()` scans for Error severity; Warning/Note alone do not set it. Capture owning diagnostic copies before later emissions and reacquire the engine view afterward. Successful syntax and eligibility for compilation are separate facts; Phase 4 adds no driver/backend success API.

### Layering, imports and platform boundaries

**Sources:** canonical headers and source imports above; root `CMakeLists.txt` lines 22–28 and 43–50; `tests/CMakeLists.txt` lines 26–31; `docs/development/INCLUDE_POLICY.md`; `docs/development/CROSS_PLATFORM_POLICY.md`.
**Apply to:** all six files.

Preserve source → diagnostics → lexer → parser separation. No lexer/source/diagnostic/renderer edit or new dependency is implied. Parser code uses token/span/value contracts without LLVM, filesystem, terminal, native linking or numeric semantics. All owned headers use `<veyr/...>`; direct standard includes stay explicit. Tests use in-memory original bytes and existing CMake/GoogleTest policies.

Platform impact:
- Windows: Preserve the same original-byte/checked-error contracts, existing target/runtime policy and native PRE_TEST discovery. Future fresh Windows hosted Debug/Release evidence is distinct from independently pending developer-host inspection. No executable suffix, shell convention or newline translation enters the parser.
- Linux: Omarchy developer-host and Ubuntu hosted evidence remain independent. Test LF, CRLF and lone CR as original bytes; no POSIX behavior, filesystem loader, locale classifier or machine-specific build path is introduced.
- macOS future: Deferred. Standard span/value/variant boundaries add no structural OS dependency. Preserve the existing native Windows/Linux build gate; this map does not enable or claim macOS builds.

**Design probe planning packet:** P01 compiler walking skeleton forces only fixed token-to-three-span recognition. The published experimental shape is the semantic boundary; final grammar, AST consolidation, decimal/range validation and lowering belong outside Phase 4. No DECISION_LOG change or new syntax decision is needed for this map.

## No Analog Found

All six files have close role/data-flow analogs. The following responsibilities have no existing complete algorithm/contract implementation and must be specified from `04-CONTEXT.md` and `04-RESEARCH.md`, using the supporting excerpts above:

| File / responsibility | Role | Data Flow | Reason |
|---|---|---|---|
| `src/parsing/parser.cpp`: fixed M1 sequence recognizer | service | transform | No parser exists. Lexer classification/recovery must not become parser behavior. |
| `src/parsing/parser.cpp`: whole-stream structure before Invalid and syntax | service | transform | Existing source validation covers bounds/IDs only; renderer validates references but has no token-order/EOF/Invalid contract. |
| `include/veyr/parsing/parser.hpp`: four-way parse outcome and three-span success | model | transform | Existing variant/value patterns are reusable; exact new alternatives and names are Phase 4 planning details. |
| `tests/parsing/parser_test.cpp`: full expected/found/openers and structural precedence oracles | test | transform | Existing assertion/table structure exists; parser-specific expected data must be independently authored from the accepted contract. |

These are new bounded responsibilities within matched files, not additional files or authorization for general parser/AST work. Authentication, network logging, database transactions, file loaders, package acquisition and native runtime subsystems have no applicable pattern requirement in this phase.

## Metadata

**Analog search scope:** all paths listed by `rg --files include src tests`; targeted complete reads of existing public source/diagnostic/token/lexer/renderer headers and implementations, token/lexer native tests and root/test CMake lists. No parser/AST path was present. Search stopped after the strong token/lexer API, implementation, native-test and configuration matches.

**Files scanned:** 17 existing code/config files read in full: 9 public headers, 4 component implementations, 2 lexer/token test files and 2 CMake lists. Exact inventory: lexer.hpp, token.hpp, source_id.hpp, source_span.hpp, source_location.hpp, source_manager.hpp, diagnostic.hpp, diagnostic_engine.hpp, plain_text_renderer.hpp; lexer.cpp, source_manager.cpp, diagnostic_engine.cpp, plain_text_renderer.cpp; lexer_test.cpp, token_test.cpp; root/tests CMakeLists.txt.

**Pattern extraction date:** 2026-10-18
**Planning inputs:** 04-CONTEXT.md (all sixteen decisions); 04-RESEARCH.md; accepted M1 profile; AGENTS.md and applicable design/development/include/build/platform/role/local-skill policies.
**Verification status:** source inspection only. No new implementation, test discovery, timing, runtime acceptance, platform readiness or CI result is asserted. Existing accepted Phase 3 evidence is preserved and does not certify the future parser.
