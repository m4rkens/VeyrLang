#include <veyr/parsing/parser.hpp>
#include <veyr/lexing/lexer.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace {

using veyr::ParsedSmokeEntry;
using veyr::ParseRejected;
using veyr::ParseRejectionKind;
using veyr::SourceSpan;
using veyr::TokenStreamError;
using veyr::TokenStreamErrorKind;

static_assert(std::is_same_v<decltype(ParsedSmokeEntry::entry_span), SourceSpan>);
static_assert(std::is_same_v<decltype(ParsedSmokeEntry::return_span), SourceSpan>);
static_assert(std::is_same_v<decltype(ParsedSmokeEntry::integer_span), SourceSpan>);
static_assert(std::is_aggregate_v<ParsedSmokeEntry>);
static_assert(std::is_copy_constructible_v<ParsedSmokeEntry>);
static_assert(std::is_copy_assignable_v<ParsedSmokeEntry>);
static_assert(std::is_move_constructible_v<ParsedSmokeEntry>);
static_assert(std::is_move_assignable_v<ParsedSmokeEntry>);
static_assert(std::is_same_v<decltype(ParseRejected::kind), ParseRejectionKind>);
static_assert(std::is_same_v<decltype(TokenStreamError::kind), TokenStreamErrorKind>);
static_assert(std::is_same_v<decltype(TokenStreamError::source_id), veyr::SourceId>);
static_assert(std::is_same_v<decltype(TokenStreamError::token_index),
                             std::optional<std::size_t>>);
static_assert(std::is_same_v<veyr::ParseResult,
                             std::variant<ParsedSmokeEntry, ParseRejected,
                                          veyr::SourceError, TokenStreamError>>);

TEST(SmokeProfileParserValue, ThreeSpansHaveValueSemantics) {
  const ParsedSmokeEntry original{{{7}, 0, 33}, {{7}, 21, 31}, {{7}, 28, 30}};
  const auto [entry, statement, integer] = original;
  EXPECT_EQ(entry, (SourceSpan{{7}, 0, 33}));
  EXPECT_EQ(statement, (SourceSpan{{7}, 21, 31}));
  EXPECT_EQ(integer, (SourceSpan{{7}, 28, 30}));
  ParsedSmokeEntry copy = original;
  EXPECT_EQ(copy, original);
  ++copy.entry_span.begin;
  EXPECT_NE(copy, original);
  copy = original;
  ++copy.return_span.end;
  EXPECT_NE(copy, original);
  copy = original;
  ++copy.integer_span.source_id.value;
  EXPECT_NE(copy, original);
  copy = original;
  ParsedSmokeEntry moved = std::move(copy);
  EXPECT_EQ(moved, original);
  copy = std::move(moved);
  EXPECT_EQ(copy, original);
}

TEST(SmokeProfileParserValue, CheckedAlternativesRemainDistinct) {
  const ParsedSmokeEntry value{{{3}, 0, 33}, {{3}, 21, 31}, {{3}, 28, 30}};
  const veyr::ParseResult success = value;
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(success));
  EXPECT_EQ(std::get<ParsedSmokeEntry>(success), value);
  const auto rejections = {ParseRejectionKind::LexicalErrors,
                           ParseRejectionKind::SyntaxError};
  for (const auto kind : rejections) {
    SCOPED_TRACE(static_cast<int>(kind));
    const veyr::ParseResult result = ParseRejected{kind};
    ASSERT_TRUE(std::holds_alternative<ParseRejected>(result));
    EXPECT_EQ(std::get<ParseRejected>(result), (ParseRejected{kind}));
    EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result));
  }
  EXPECT_NE(ParseRejected{ParseRejectionKind::LexicalErrors},
            ParseRejected{ParseRejectionKind::SyntaxError});
  const std::array<veyr::SourceError, 3> errors = {{
      {veyr::SourceErrorKind::UnknownSource, {0}, 9, 1, std::nullopt},
      {veyr::SourceErrorKind::ReversedRange, {3}, 29, 28, 33},
      {veyr::SourceErrorKind::BeyondSource, {3}, 28, 34, 33}}};
  for (const auto& error : errors) {
    SCOPED_TRACE(static_cast<int>(error.kind));
    const veyr::ParseResult result = error;
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
    EXPECT_EQ(std::get<veyr::SourceError>(result), error);
  }
  const std::array kinds = {
      TokenStreamErrorKind::SourceMismatch,
      TokenStreamErrorKind::UnsupportedTokenKind,
      TokenStreamErrorKind::EmptyTokenSpan,
      TokenStreamErrorKind::OutOfOrderOrOverlappingSpan,
      TokenStreamErrorKind::NonFinalEndOfFile,
      TokenStreamErrorKind::InvalidEndOfFileSpan,
      TokenStreamErrorKind::MissingEndOfFile};
  for (std::size_t index = 0; index < kinds.size(); ++index) {
    SCOPED_TRACE(index);
    const std::optional<std::size_t> token_index =
        kinds[index] == TokenStreamErrorKind::MissingEndOfFile
            ? std::nullopt : std::optional<std::size_t>{8};
    const TokenStreamError error{kinds[index], {3}, token_index};
    const auto [kind, source_id, position] = error;
    EXPECT_EQ(kind, kinds[index]);
    EXPECT_EQ(source_id, (veyr::SourceId{3}));
    EXPECT_EQ(position, token_index);
    const veyr::ParseResult result = error;
    ASSERT_TRUE(std::holds_alternative<TokenStreamError>(result));
    EXPECT_EQ(std::get<TokenStreamError>(result), error);
    auto changed = error;
    ++changed.source_id.value;
    EXPECT_NE(changed, error);
    changed = error;
    changed.token_index = 9;
    EXPECT_NE(changed, error);
    for (std::size_t other = index + 1; other < kinds.size(); ++other) {
      SCOPED_TRACE(other);
      EXPECT_NE(kinds[index], kinds[other]);
    }
  }
}

TEST(SmokeProfileParserValue, CopiesResolveOriginalSnapshot) {
  veyr::SourceManager sources;
  std::string caller = "fn main() -> int32 { return 42; }";
  std::string name = "snapshot.vyr";
  const auto registered = sources.register_source(name, caller);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const auto id = std::get<veyr::SourceId>(registered);
  const ParsedSmokeEntry original{{id, 0, 33}, {id, 21, 31}, {id, 28, 30}};
  auto copy = original;
  const auto moved = std::move(copy);
  caller.assign(4096, '?');
  name.assign(4096, '!');
  for (int index = 0; index < 100; ++index) {
    SCOPED_TRACE(index);
    const auto newer = sources.register_source("snapshot.vyr", "other bytes");
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(newer));
    EXPECT_NE(std::get<veyr::SourceId>(newer), id);
  }
  const std::array spans = {moved.entry_span, moved.return_span, moved.integer_span};
  const std::array<std::string_view, 3> expected = {
      "fn main() -> int32 { return 42; }", "return 42;", "42"};
  for (std::size_t index = 0; index < spans.size(); ++index) {
    SCOPED_TRACE(index);
    const auto text = sources.text(spans[index]);
    ASSERT_TRUE(std::holds_alternative<std::string_view>(text));
    EXPECT_EQ(std::get<std::string_view>(text), expected[index]);
  }
  EXPECT_EQ(moved, original);
}

TEST(SmokeProfileParserValue, ManagerSelectionIsCallerResponsibility) {
  veyr::SourceManager first;
  veyr::SourceManager second;
  const auto one = first.register_source("same.vyr", "42");
  const auto two = second.register_source("same.vyr", "99");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(one));
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(two));
  const auto id = std::get<veyr::SourceId>(one);
  ASSERT_EQ(id, std::get<veyr::SourceId>(two));
  const ParsedSmokeEntry value{{id, 0, 2}, {id, 0, 2}, {id, 0, 2}};
  const auto original = first.text(value.integer_span);
  const auto coincident = second.text(value.integer_span);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(original));
  ASSERT_TRUE(std::holds_alternative<std::string_view>(coincident));
  EXPECT_EQ(std::get<std::string_view>(original), "42");
  EXPECT_EQ(std::get<std::string_view>(coincident), "99");
  // Numeric IDs alone cannot detect a caller's choice of a foreign manager.
}

class SmokeProfileParser : public testing::Test {
 protected:
  void load(std::string bytes) {
    const auto registered = sources.register_source("<m1-parser>", std::move(bytes));
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
    id = std::get<veyr::SourceId>(registered);
    auto scanned = veyr::lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(scanned));
    tokens = std::get<std::vector<veyr::Token>>(std::move(scanned));
  }

  veyr::SourceManager sources;
  veyr::DiagnosticEngine diagnostics;
  veyr::SourceId id;
  std::vector<veyr::Token> tokens;
};

TEST_F(SmokeProfileParser, FixedEntryHasTightSpans) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  ASSERT_FALSE(diagnostics.has_errors());
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
  EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
            (ParsedSmokeEntry{{id, 0, 33}, {id, 21, 31}, {id, 28, 30}}));
  EXPECT_TRUE(diagnostics.diagnostics().empty());
  EXPECT_FALSE(diagnostics.has_errors());

  ASSERT_NO_FATAL_FAILURE(load(" \tfn main() -> int32 { return 42; }\r\n"));
  const auto surrounded = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(surrounded));
  EXPECT_EQ(std::get<ParsedSmokeEntry>(surrounded),
            (ParsedSmokeEntry{{id, 2, 35}, {id, 23, 33}, {id, 30, 32}}));
  EXPECT_TRUE(diagnostics.diagnostics().empty());
  EXPECT_FALSE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParser, DecimalTokensDeferConversionAndRange) {
  const std::array decimals = {std::string{"0"}, std::string{"255"},
                               std::string{"256"}, std::string{"00042"},
                               std::string(4096, '9')};
  for (const auto& decimal : decimals) {
    SCOPED_TRACE(testing::Message() << "decimal bytes=" << decimal.size()
                                  << " prefix=" << decimal.substr(0, 5));
    ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return " + decimal + "; }"));
    ASSERT_FALSE(diagnostics.has_errors());
    const auto result = veyr::parse(sources, id, tokens, diagnostics);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
    EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
              (ParsedSmokeEntry{{id, 0, 31 + decimal.size()},
                                {id, 21, 29 + decimal.size()},
                                {id, 28, 28 + decimal.size()}}));
    const auto text = sources.text(std::get<ParsedSmokeEntry>(result).integer_span);
    ASSERT_TRUE(std::holds_alternative<std::string_view>(text));
    EXPECT_EQ(std::get<std::string_view>(text), decimal);
    EXPECT_TRUE(diagnostics.diagnostics().empty());
    EXPECT_FALSE(diagnostics.has_errors());
  }
}

TEST_F(SmokeProfileParser, MissingSemicolonReportsUnexpectedBrace) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42 }"));
  ASSERT_FALSE(diagnostics.has_errors());
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParseRejected>(result));
  EXPECT_EQ(std::get<ParseRejected>(result),
            (ParseRejected{ParseRejectionKind::SyntaxError}));
  const auto errors = diagnostics.diagnostics();
  ASSERT_EQ(errors.size(), 1u);
  EXPECT_EQ(errors[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(errors[0].message,
            "expected ';', found '}' in M1 Experimental Smoke Profile");
  EXPECT_EQ(errors[0].primary, (SourceSpan{id, 31, 32}));
  EXPECT_TRUE(errors[0].related.empty());
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParser, MissingBodyCloserReportsEofAndOpener) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42;"));
  ASSERT_FALSE(diagnostics.has_errors());
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParseRejected>(result));
  EXPECT_EQ(std::get<ParseRejected>(result),
            (ParseRejected{ParseRejectionKind::SyntaxError}));
  const auto errors = diagnostics.diagnostics();
  ASSERT_EQ(errors.size(), 1u);
  EXPECT_EQ(errors[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(errors[0].message,
            "expected '}', found end of source in M1 Experimental Smoke Profile");
  EXPECT_EQ(errors[0].primary, (SourceSpan{id, 31, 31}));
  ASSERT_EQ(errors[0].related.size(), 1u);
  EXPECT_EQ(errors[0].related[0].span, (SourceSpan{id, 19, 20}));
  EXPECT_EQ(errors[0].related[0].label, "opening '{' is here");
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParser, TrailingTokenCannotProduceValue) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; } fn"));
  ASSERT_FALSE(diagnostics.has_errors());
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParseRejected>(result));
  EXPECT_EQ(std::get<ParseRejected>(result),
            (ParseRejected{ParseRejectionKind::SyntaxError}));
  const auto errors = diagnostics.diagnostics();
  ASSERT_EQ(errors.size(), 1u);
  EXPECT_EQ(errors[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(errors[0].message,
            "expected end of source, found 'fn' in M1 Experimental Smoke Profile");
  EXPECT_EQ(errors[0].primary, (SourceSpan{id, 34, 36}));
  EXPECT_TRUE(errors[0].related.empty());
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParser, UnknownSelectedSourcePreservesOriginalError) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  const veyr::SourceId unknown{99};
  const auto original = sources.source_text(unknown);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(original));
  const auto result = veyr::parse(sources, unknown, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
  const veyr::SourceError expected{
      veyr::SourceErrorKind::UnknownSource, unknown, 0, 0, std::nullopt};
  EXPECT_EQ(std::get<veyr::SourceError>(result), expected);
  EXPECT_EQ(std::get<veyr::SourceError>(result),
            std::get<veyr::SourceError>(original));
  EXPECT_TRUE(diagnostics.diagnostics().empty());
  EXPECT_FALSE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParser, MalformedTokenStreamEmitsNothing) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  ASSERT_EQ(tokens.size(), 12u);
  tokens[8].span = {id, 30, 28};
  const auto original = sources.validate(tokens[8].span);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(original));
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
  const veyr::SourceError expected{
      veyr::SourceErrorKind::ReversedRange, id, 30, 28, 33};
  EXPECT_EQ(std::get<veyr::SourceError>(result), expected);
  EXPECT_EQ(std::get<veyr::SourceError>(result),
            std::get<veyr::SourceError>(original));
  EXPECT_TRUE(diagnostics.diagnostics().empty());
  EXPECT_FALSE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParser, InvalidTokenSuppressesParserDiagnostic) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return -12; }"));
  ASSERT_TRUE(diagnostics.has_errors());
  ASSERT_EQ(diagnostics.diagnostics().size(), 1u);
  const veyr::Diagnostic original = diagnostics.diagnostics()[0];
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParseRejected>(result));
  EXPECT_EQ(std::get<ParseRejected>(result),
            (ParseRejected{ParseRejectionKind::LexicalErrors}));
  const auto errors = diagnostics.diagnostics();
  ASSERT_EQ(errors.size(), 1u);
  EXPECT_EQ(errors[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(errors[0].message,
            "unsupported numeric spelling in M1 Experimental Smoke Profile");
  EXPECT_EQ(errors[0].primary, (SourceSpan{id, 28, 31}));
  EXPECT_TRUE(errors[0].related.empty());
  EXPECT_EQ(errors[0].severity, original.severity);
  EXPECT_EQ(errors[0].message, original.message);
  EXPECT_EQ(errors[0].primary, original.primary);
  EXPECT_EQ(errors[0].related.size(), original.related.size());
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileParser, EarlierErrorDoesNotGateIndependentValidSyntax) {
  const auto earlier = sources.register_source("earlier.vyr", "bad");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(earlier));
  const auto old_id = std::get<veyr::SourceId>(earlier);
  diagnostics.emit({veyr::DiagnosticSeverity::Error, "earlier Error",
                    SourceSpan{old_id, 0, 3},
                    {{{old_id, 0, 1}, "earlier context"}}});
  const veyr::Diagnostic original = diagnostics.diagnostics()[0];
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  EXPECT_NE(id, old_id);
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
  EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
            (ParsedSmokeEntry{{id, 0, 33}, {id, 21, 31}, {id, 28, 30}}));
  const auto retained = diagnostics.diagnostics();
  ASSERT_EQ(retained.size(), 1u);
  EXPECT_EQ(retained[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(retained[0].message, "earlier Error");
  EXPECT_EQ(retained[0].primary, (SourceSpan{old_id, 0, 3}));
  ASSERT_EQ(retained[0].related.size(), 1u);
  EXPECT_EQ(retained[0].related[0].span, (SourceSpan{old_id, 0, 1}));
  EXPECT_EQ(retained[0].related[0].label, "earlier context");
  EXPECT_EQ(retained[0].severity, original.severity);
  EXPECT_EQ(retained[0].message, original.message);
  EXPECT_EQ(retained[0].primary, original.primary);
  EXPECT_EQ(retained[0].related[0].span, original.related[0].span);
  EXPECT_EQ(retained[0].related[0].label, original.related[0].label);
  EXPECT_TRUE(diagnostics.has_errors());
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result) &&
               !diagnostics.has_errors());
}

}  // namespace
