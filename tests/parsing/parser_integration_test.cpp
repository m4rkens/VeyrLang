#include <veyr/diagnostics/plain_text_renderer.hpp>
#include <veyr/lexing/lexer.hpp>
#include <veyr/parsing/parser.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace {

using namespace veyr;

// Compare owning fields, never engine views retained across an emit().
std::vector<Diagnostic> copied_diagnostics(const DiagnosticEngine& engine) {
  const auto view = engine.diagnostics();
  return {view.begin(), view.end()};
}

void expect_diagnostic(const Diagnostic& actual, const Diagnostic& expected) {
  EXPECT_EQ(actual.severity, expected.severity);
  EXPECT_EQ(actual.message, expected.message);
  EXPECT_EQ(actual.primary, expected.primary);
  ASSERT_EQ(actual.related.size(), expected.related.size());
  for (std::size_t index = 0; index < expected.related.size(); ++index) {
    SCOPED_TRACE(index);
    EXPECT_EQ(actual.related[index].span, expected.related[index].span);
    EXPECT_EQ(actual.related[index].label, expected.related[index].label);
  }
}

void expect_history(const DiagnosticEngine& engine,
                    const std::vector<Diagnostic>& expected) {
  const auto actual = copied_diagnostics(engine);
  ASSERT_EQ(actual.size(), expected.size());
  for (std::size_t index = 0; index < expected.size(); ++index) {
    SCOPED_TRACE(index);
    ASSERT_NO_FATAL_FAILURE(expect_diagnostic(actual[index], expected[index]));
  }
}

void expect_rejection(const ParseResult& result, ParseRejectionKind kind) {
  ASSERT_TRUE(std::holds_alternative<ParseRejected>(result));
  EXPECT_EQ(std::get<ParseRejected>(result).kind, kind);
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result));
}

class SmokeProfileParserIntegration : public testing::Test {
 protected:
  void load(std::string bytes, std::string name = "<integration>") {
    const auto registered = sources.register_source(std::move(name), std::move(bytes));
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    id = std::get<SourceId>(registered);
    auto scanned = lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    tokens = std::get<std::vector<Token>>(std::move(scanned));
  }

  SourceManager sources;
  SourceId id{};
  DiagnosticEngine diagnostics;
  std::vector<Token> tokens;
};

TEST_F(SmokeProfileParserIntegration, OwnedThreeSpanResultOutlivesTokens) {
  std::optional<ParsedSmokeEntry> saved;
  {
    std::string caller_name = "same.vyr";
    std::string caller_bytes = "\tfn main() -> int32 {\r\nreturn 00042;\r\n}\n";
    ASSERT_EQ(caller_bytes.size(), 40u);
    const auto registered = sources.register_source(caller_name, caller_bytes);
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    id = std::get<SourceId>(registered);
    DiagnosticEngine local_engine;
    auto scanned = lex(sources, id, local_engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    auto local_tokens = std::get<std::vector<Token>>(std::move(scanned));
    const auto result = parse(sources, id, local_tokens, local_engine);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
    saved = std::get<ParsedSmokeEntry>(result);
    EXPECT_FALSE(local_engine.has_errors());
    EXPECT_TRUE(local_engine.diagnostics().empty());
    caller_name.assign(100, 'x');
    caller_bytes.assign(100, 'x');
  }  // Caller strings, tokens, LexResult, ParseResult and engine have all died.
  const ParsedSmokeEntry expected{{id, 1, 39}, {id, 23, 36}, {id, 30, 35}};
  ASSERT_TRUE(saved.has_value());
  EXPECT_EQ(*saved, expected);
  auto copy = *saved;
  ParsedSmokeEntry moved = std::move(copy);
  copy = moved;
  moved = std::move(copy);
  saved.reset();
  for (std::size_t index = 0; index < 3000; ++index) {
    SCOPED_TRACE(index);
    const auto later = sources.register_source(index % 2 == 0 ? "same.vyr" : "other.vyr",
                                               "different bytes " + std::to_string(index));
    ASSERT_TRUE(std::holds_alternative<SourceId>(later));
    EXPECT_NE(std::get<SourceId>(later), id);
  }
  EXPECT_EQ(moved, expected);
  EXPECT_EQ(std::get<std::string_view>(sources.text(moved.entry_span)),
            "fn main() -> int32 {\r\nreturn 00042;\r\n}");
  EXPECT_EQ(std::get<std::string_view>(sources.text(moved.return_span)), "return 00042;");
  EXPECT_EQ(std::get<std::string_view>(sources.text(moved.integer_span)), "00042");
  EXPECT_EQ(std::get<std::string_view>(sources.display_name(id)), "same.vyr");
  EXPECT_EQ(std::get<std::string_view>(sources.source_text(id)),
            "\tfn main() -> int32 {\r\nreturn 00042;\r\n}\n");
}

struct DecimalCase { const char* name; std::string digits; };
class SmokeProfileParserIntegrationDecimal
    : public SmokeProfileParserIntegration, public testing::WithParamInterface<DecimalCase> {};

TEST_P(SmokeProfileParserIntegrationDecimal, DecimalShapeDoesNotPerformSemantics) {
  const auto& item = GetParam();
  SCOPED_TRACE(item.name);
  SCOPED_TRACE(testing::Message() << "decimal length=" << item.digits.size());
  // Offsets follow these literal fixture prefixes/suffixes, not producer output.
  ASSERT_NO_FATAL_FAILURE(load("fn main()->int32{return " + item.digits + ";}\r\n"));
  const auto result = parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
  const auto end = 24 + item.digits.size();
  EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
            (ParsedSmokeEntry{{id, 0, end + 2}, {id, 17, end + 1}, {id, 24, end}}));
  EXPECT_EQ(std::get<std::string_view>(sources.text(std::get<ParsedSmokeEntry>(result).integer_span)),
            item.digits);
  EXPECT_TRUE(diagnostics.diagnostics().empty());
  EXPECT_FALSE(diagnostics.has_errors());
}

INSTANTIATE_TEST_SUITE_P(SmokeProfileParserIntegrationCases, SmokeProfileParserIntegrationDecimal,
    testing::Values(DecimalCase{"Zero", "0"}, DecimalCase{"FortyTwo", "42"},
                    DecimalCase{"ByteMaximum", "255"}, DecimalCase{"BeyondByteRange", "256"},
                    DecimalCase{"LeadingZeros", "00042"},
                    DecimalCase{"SixteenThousandDigits", std::string(16384, '9')}),
    [](const testing::TestParamInfo<DecimalCase>& info) { return info.param.name; });

TEST_F(SmokeProfileParserIntegration, WhitespaceAndPunctuationKeepTightSyntaxBounds) {
  ASSERT_NO_FATAL_FAILURE(load(" \tfn\tmain( )\r->\nint32{ return\t42 ;}\r\n"));
  const auto result = parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
  EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
            (ParsedSmokeEntry{{id, 2, 35}, {id, 23, 34}, {id, 30, 32}}));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

struct LexicalCase {
  const char* name;
  std::string bytes;
  std::vector<std::pair<std::size_t, std::size_t>> invalid;
  const char* message;
};
class SmokeProfileParserIntegrationLexical
    : public SmokeProfileParserIntegration, public testing::WithParamInterface<LexicalCase> {};

TEST_P(SmokeProfileParserIntegrationLexical, InvalidStreamPreservesEveryLexicalDiagnostic) {
  const auto& item = GetParam();
  SCOPED_TRACE(item.name);
  diagnostics.emit({DiagnosticSeverity::Note, "prior note", std::nullopt, {}});
  diagnostics.emit({DiagnosticSeverity::Warning, "prior warning", std::nullopt, {}});
  diagnostics.emit({DiagnosticSeverity::Error, "prior error", std::nullopt, {}});
  ASSERT_NO_FATAL_FAILURE(load(item.bytes));
  const auto before = copied_diagnostics(diagnostics);
  ASSERT_EQ(before.size(), 3u + item.invalid.size());
  std::vector<SourceSpan> invalid_spans;
  for (const auto& token : tokens) {
    if (token.kind == TokenKind::Invalid) invalid_spans.push_back(token.span);
  }
  ASSERT_EQ(invalid_spans.size(), item.invalid.size());
  for (std::size_t index = 0; index < item.invalid.size(); ++index) {
    SCOPED_TRACE(index);
    const SourceSpan expected{id, item.invalid[index].first, item.invalid[index].second};
    EXPECT_EQ(invalid_spans[index], expected);
    expect_diagnostic(before[3 + index], {DiagnosticSeverity::Error, item.message, expected, {}});
  }
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens.back().kind, TokenKind::EndOfFile);
  EXPECT_EQ(tokens.back().span, (SourceSpan{id, item.bytes.size(), item.bytes.size()}));
  if (std::string_view{item.name} == "OverlappingBlockDelimiter") {
    ASSERT_EQ(tokens.size(), 2u);
    EXPECT_EQ(tokens[0].kind, TokenKind::Invalid);  // No Return token was fabricated.
  }
  const auto result = parse(sources, id, tokens, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejection(result, ParseRejectionKind::LexicalErrors));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, before));
  EXPECT_TRUE(diagnostics.has_errors());
}

INSTANTIATE_TEST_SUITE_P(SmokeProfileParserIntegrationCases, SmokeProfileParserIntegrationLexical,
    testing::Values(
      LexicalCase{"BeforeEntry", "@ fn main() -> int32 { return 42; }", {{0, 1}},
                  "unsupported byte in M1 Experimental Smoke Profile"},
      LexicalCase{"PositiveSign", "fn main() -> int32 { return +12; }", {{28, 31}},
                  "unsupported numeric spelling in M1 Experimental Smoke Profile"},
      LexicalCase{"NegativeSign", "fn main() -> int32 { return -12; }", {{28, 31}},
                  "unsupported numeric spelling in M1 Experimental Smoke Profile"},
      LexicalCase{"NonDecimal", "fn main() -> int32 { return 0x10; }", {{28, 32}},
                  "unsupported numeric spelling in M1 Experimental Smoke Profile"},
      LexicalCase{"BetweenIntegers", "fn main() -> int32 { return 12/*x*/34; }", {{30, 35}},
                  "unsupported comment-shaped input in M1 Experimental Smoke Profile"},
      LexicalCase{"AfterEntry", "fn main() -> int32 { return 42; } //x", {{34, 37}},
                  "unsupported comment-shaped input in M1 Experimental Smoke Profile"},
      LexicalCase{"HighBytes", std::string{"\x80\xFF"} + " fn main() -> int32 { return 42; }",
                  {{0, 2}}, "unsupported non-ASCII byte run in M1 Experimental Smoke Profile"},
      LexicalCase{"EmbeddedNul", std::string{"fn\0 main() -> int32 { return 42; }", 34}, {{2, 3}},
                  "unsupported byte in M1 Experimental Smoke Profile"},
      LexicalCase{"OverlappingBlockDelimiter", "/*/return", {{0, 9}},
                  "unsupported comment-shaped input in M1 Experimental Smoke Profile"},
      LexicalCase{"EarlierSyntaxMismatch", "return @", {{7, 8}},
                  "unsupported byte in M1 Experimental Smoke Profile"},
      LexicalCase{"OrderedDuplicateErrors", "@@", {{0, 1}, {1, 2}},
                  "unsupported byte in M1 Experimental Smoke Profile"}),
    [](const testing::TestParamInfo<LexicalCase>& info) { return info.param.name; });

TEST_F(SmokeProfileParserIntegration, EarlierErrorsDoNotGateValidSyntax) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  const std::array severities = {DiagnosticSeverity::Note, DiagnosticSeverity::Warning,
                                  DiagnosticSeverity::Error};
  for (std::size_t index = 0; index < severities.size(); ++index) {
    SCOPED_TRACE(index);
    diagnostics.emit({severities[index], "prior " + std::to_string(index),
                      SourceSpan{id, index, index + 1},
                      {{SourceSpan{id, 28, 30}, "owned context " + std::to_string(index)}}});
    const auto before = copied_diagnostics(diagnostics);
    const auto scanned = lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    const auto result = parse(sources, id, std::get<std::vector<Token>>(scanned), diagnostics);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
    EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
              (ParsedSmokeEntry{{id, 0, 33}, {id, 21, 31}, {id, 28, 30}}));
    ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, before));
    EXPECT_EQ(diagnostics.has_errors(), index == 2);
    // Test-local consumer predicate, not a new compilation-success API.
    const bool eligible_handoff = std::holds_alternative<ParsedSmokeEntry>(result) &&
                                   !diagnostics.has_errors();
    EXPECT_EQ(eligible_handoff, index != 2);
  }
}

TEST_F(SmokeProfileParserIntegration, RepeatedSyntaxErrorsPreserveOrderedDuplicates) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42 }"));
  diagnostics.emit({DiagnosticSeverity::Note, "earlier", SourceSpan{id, 0, 2}, {}});
  auto expected = copied_diagnostics(diagnostics);
  for (std::size_t call = 0; call < 2; ++call) {
    SCOPED_TRACE(call);
    const auto result = parse(sources, id, tokens, diagnostics);
    ASSERT_NO_FATAL_FAILURE(expect_rejection(result, ParseRejectionKind::SyntaxError));
    expected.push_back({DiagnosticSeverity::Error,
        "expected ';', found '}' in M1 Experimental Smoke Profile", SourceSpan{id, 31, 32}, {}});
    ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, expected));
    EXPECT_TRUE(diagnostics.has_errors());
  }
}

struct EofCase {
  const char* name;
  const char* bytes;
  std::size_t eof, line, column;
  const char* message;
  std::optional<std::pair<std::size_t, std::size_t>> opener;
  const char* label;
};
class SmokeProfileParserIntegrationEof
    : public SmokeProfileParserIntegration, public testing::WithParamInterface<EofCase> {};

TEST_P(SmokeProfileParserIntegrationEof, OriginalNewlineEofLocations) {
  const auto& item = GetParam();
  SCOPED_TRACE(item.name);
  ASSERT_NO_FATAL_FAILURE(load(item.bytes));
  ASSERT_FALSE(diagnostics.has_errors());
  ASSERT_FALSE(tokens.empty());
  EXPECT_EQ(tokens.back().kind, TokenKind::EndOfFile);
  EXPECT_EQ(tokens.back().span, (SourceSpan{id, item.eof, item.eof}));
  const auto result = parse(sources, id, tokens, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejection(result, ParseRejectionKind::SyntaxError));
  Diagnostic expected{DiagnosticSeverity::Error, item.message, SourceSpan{id, item.eof, item.eof}, {}};
  if (item.opener) expected.related.push_back({{id, item.opener->first, item.opener->second}, item.label});
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {expected}));
  const auto location = sources.location(id, item.eof);
  ASSERT_TRUE(std::holds_alternative<SourceLocation>(location));
  EXPECT_EQ(std::get<SourceLocation>(location), (SourceLocation{id, item.eof, item.line, item.column}));
  if (item.opener) {
    const auto opening_location = sources.location(id, item.opener->first);
    ASSERT_TRUE(std::holds_alternative<SourceLocation>(opening_location));
    EXPECT_EQ(std::get<SourceLocation>(opening_location),
              (SourceLocation{id, item.opener->first, 1, item.opener->first + 1}));
  }
  EXPECT_TRUE(diagnostics.has_errors());
}

INSTANTIATE_TEST_SUITE_P(SmokeProfileParserIntegrationCases, SmokeProfileParserIntegrationEof,
    testing::Values(
      EofCase{"BodyLF", "fn main() -> int32 {\n return 42;\n", 33, 3, 1,
        "expected '}', found end of source in M1 Experimental Smoke Profile", {{19, 20}}, "opening '{' is here"},
      EofCase{"BodyCRLF", "fn main() -> int32 {\r\n return 42;\r\n", 35, 3, 1,
        "expected '}', found end of source in M1 Experimental Smoke Profile", {{19, 20}}, "opening '{' is here"},
      EofCase{"BodyLoneCR", "fn main() -> int32 {\r return 42;\r", 33, 1, 34,
        "expected '}', found end of source in M1 Experimental Smoke Profile", {{19, 20}}, "opening '{' is here"},
      EofCase{"ParenLF", "fn main(\n", 9, 2, 1,
        "expected ')', found end of source in M1 Experimental Smoke Profile", {{7, 8}}, "opening '(' is here"},
      EofCase{"ParenCRLF", "fn main(\r\n", 10, 2, 1,
        "expected ')', found end of source in M1 Experimental Smoke Profile", {{7, 8}}, "opening '(' is here"},
      EofCase{"ParenLoneCR", "fn main(\r", 9, 1, 10,
        "expected ')', found end of source in M1 Experimental Smoke Profile", {{7, 8}}, "opening '(' is here"},
      EofCase{"Empty", "", 0, 1, 1,
        "expected 'fn', found end of source in M1 Experimental Smoke Profile", std::nullopt, ""},
      EofCase{"WhitespaceOnly", "\t \r\n", 4, 2, 1,
        "expected 'fn', found end of source in M1 Experimental Smoke Profile", std::nullopt, ""}),
    [](const testing::TestParamInfo<EofCase>& info) { return info.param.name; });

TEST_F(SmokeProfileParserIntegration, ParserErrorsRenderWithEscapedOwningMetadata) {
  struct RenderCase { const char* name; const char* bytes; Diagnostic expected; const char* rendered; };
  const std::string hostile_name{"bad\x1B\0\xFF.vyr", 10};
  const std::array cases = {
    RenderCase{"Paren", "fn main(\n",
      {DiagnosticSeverity::Error, "expected ')', found end of source in M1 Experimental Smoke Profile",
       SourceSpan{{0}, 9, 9}, {{{{0}, 7, 8}, "opening '(' is here"}}},
      "bad\\x1B\\x00\\xFF.vyr:2:1: error: expected ')', found end of source in M1 Experimental Smoke Profile\n"
      "2 | \n  | ^\nnote: opening '(' is here (bad\\x1B\\x00\\xFF.vyr:1:8)\n"},
    RenderCase{"Body", "fn main() -> int32 {\r\n return 42;\r\n",
      {DiagnosticSeverity::Error, "expected '}', found end of source in M1 Experimental Smoke Profile",
       SourceSpan{{0}, 35, 35}, {{{{0}, 19, 20}, "opening '{' is here"}}},
      "bad\\x1B\\x00\\xFF.vyr:3:1: error: expected '}', found end of source in M1 Experimental Smoke Profile\n"
      "3 | \n  | ^\nnote: opening '{' is here (bad\\x1B\\x00\\xFF.vyr:1:20)\n"},
    RenderCase{"Semicolon", "fn main() -> int32 { return 42 }",
      {DiagnosticSeverity::Error, "expected ';', found '}' in M1 Experimental Smoke Profile",
       SourceSpan{{0}, 31, 32}, {}},
      "bad\\x1B\\x00\\xFF.vyr:1:32: error: expected ';', found '}' in M1 Experimental Smoke Profile\n"
      "1 | fn main() -> int32 { return 42 }\n  |                                ^\n"}
  };
  for (const auto& item : cases) {
    SCOPED_TRACE(item.name);
    std::string rendered;
    std::vector<Diagnostic> saved;
    SourceId selected{};
    {
      DiagnosticEngine local;
      const auto registered = sources.register_source(hostile_name, item.bytes);
      ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
      selected = std::get<SourceId>(registered);
      auto scanned = lex(sources, selected, local);
      ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
      ASSERT_TRUE(local.diagnostics().empty());
      const auto result = parse(sources, selected, std::get<std::vector<Token>>(scanned), local);
      ASSERT_NO_FATAL_FAILURE(expect_rejection(result, ParseRejectionKind::SyntaxError));
      saved = copied_diagnostics(local);
      ASSERT_EQ(saved.size(), 1u);
      auto expected = item.expected;
      expected.primary->source_id = selected;
      for (auto& related : expected.related) related.span.source_id = selected;
      ASSERT_NO_FATAL_FAILURE(expect_diagnostic(saved[0], expected));
      const auto output = PlainTextRenderer{}.render(saved[0], sources);
      ASSERT_TRUE(std::holds_alternative<std::string>(output));
      rendered = std::get<std::string>(output);
      EXPECT_EQ(rendered, item.rendered);
      ASSERT_NO_FATAL_FAILURE(expect_history(local, saved));
      EXPECT_TRUE(local.has_errors());
    }
    // The independent owning Diagnostic still renders after engine/token death.
    const auto repeated = PlainTextRenderer{}.render(saved[0], sources);
    ASSERT_TRUE(std::holds_alternative<std::string>(repeated));
    EXPECT_EQ(std::get<std::string>(repeated), item.rendered);
    saved.clear();
    EXPECT_EQ(rendered, item.rendered);
    for (const unsigned char byte : rendered) {
      EXPECT_TRUE(byte == '\n' || (byte >= 0x20 && byte <= 0x7e));
    }
  }
}

TEST_F(SmokeProfileParserIntegration, UnknownSourcePropagatesWithoutDiagnosticsOrValue) {
  const SourceId unknown{999999};
  diagnostics.emit({DiagnosticSeverity::Warning, "prior", std::nullopt, {}});
  const auto before = copied_diagnostics(diagnostics);
  const SourceError expected{SourceErrorKind::UnknownSource, unknown, 0, 0, std::nullopt};
  const auto scanned = lex(sources, unknown, diagnostics);
  ASSERT_TRUE(std::holds_alternative<SourceError>(scanned));
  EXPECT_EQ(std::get<SourceError>(scanned), expected);
  const auto result = parse(sources, unknown, std::span<const Token>{}, diagnostics);
  ASSERT_TRUE(std::holds_alternative<SourceError>(result));
  EXPECT_EQ(std::get<SourceError>(result), expected);
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, before));
  EXPECT_FALSE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParserIntegration, CheckedCorruptionOutranksRealLexicalRejection) {
  ASSERT_NO_FATAL_FAILURE(load("return @"));
  const auto before = copied_diagnostics(diagnostics);
  ASSERT_EQ(before.size(), 1u);
  ASSERT_EQ(tokens.size(), 3u);
  tokens.back().span.end = 9;
  const auto result = parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<SourceError>(result));
  EXPECT_EQ(std::get<SourceError>(result), (SourceError{SourceErrorKind::BeyondSource, id, 8, 9, 8}));
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, before));
  tokens.pop_back();
  const auto missing_eof = parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<TokenStreamError>(missing_eof));
  EXPECT_EQ(std::get<TokenStreamError>(missing_eof),
            (TokenStreamError{TokenStreamErrorKind::MissingEndOfFile, id, std::nullopt}));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, before));
  EXPECT_TRUE(diagnostics.has_errors());
}

}  // namespace
