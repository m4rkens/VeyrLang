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

// 04-02 oracles come from the approved plan's literal fixtures, not parser tables.
std::vector<veyr::Diagnostic> owned_diagnostics(const veyr::DiagnosticEngine& engine) {
  const auto view = engine.diagnostics();
  return {view.begin(), view.end()};
}

void expect_diagnostic_history(const veyr::DiagnosticEngine& engine,
                               const std::vector<veyr::Diagnostic>& before) {
  const auto current = engine.diagnostics();  // Reacquired after any emission.
  ASSERT_GE(current.size(), before.size());
  for (std::size_t i = 0; i < before.size(); ++i) {
    SCOPED_TRACE(i);
    EXPECT_EQ(current[i].severity, before[i].severity);
    EXPECT_EQ(current[i].message, before[i].message);
    EXPECT_EQ(current[i].primary, before[i].primary);
    ASSERT_EQ(current[i].related.size(), before[i].related.size());
    for (std::size_t j = 0; j < before[i].related.size(); ++j) {
      SCOPED_TRACE(j);
      EXPECT_EQ(current[i].related[j].span, before[i].related[j].span);
      EXPECT_EQ(current[i].related[j].label, before[i].related[j].label);
    }
  }
}

void seed_diagnostic_history(veyr::DiagnosticEngine& engine, veyr::SourceId id) {
  engine.emit({veyr::DiagnosticSeverity::Warning, "prior warning", std::nullopt, {}});
  engine.emit({veyr::DiagnosticSeverity::Note, "prior note", SourceSpan{id, 0, 0},
               {{{id, 0, 0}, "first context"}, {{id, 0, 0}, "second context"}}});
  engine.emit({veyr::DiagnosticSeverity::Error, "prior Error", SourceSpan{id, 0, 0}, {}});
}

class SmokeProfileParserStructure : public SmokeProfileParser {
 protected:
  void SetUp() override {
    ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
    ASSERT_EQ(tokens.size(), 12u);
  }

  void expect_checked(const veyr::ParseResult& expected,
                      std::optional<veyr::SourceId> selected = std::nullopt) {
    seed_diagnostic_history(diagnostics, id);
    const auto history = owned_diagnostics(diagnostics);
    const auto actual = veyr::parse(sources, selected.value_or(id), tokens, diagnostics);
    EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(actual));
    EXPECT_EQ(actual, expected);  // Equality checks all kind/ID/range/size/index fields.
    EXPECT_EQ(diagnostics.diagnostics().size(), history.size());
    expect_diagnostic_history(diagnostics, history);
    EXPECT_TRUE(diagnostics.has_errors());
  }

  void expect_stream(TokenStreamErrorKind kind, std::optional<std::size_t> index) {
    expect_checked(TokenStreamError{kind, id, index});
  }

  veyr::SourceId other_source() {
    const auto added = sources.register_source("other.vyr", "fn main() -> int32 { return 42; }");
    EXPECT_TRUE(std::holds_alternative<veyr::SourceId>(added));
    return std::get<veyr::SourceId>(added);
  }
};

TEST_F(SmokeProfileParserStructure, UnknownSelectedZero) {
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::UnknownSource, {0}, 0, 0, std::nullopt}, veyr::SourceId{0});
}
TEST_F(SmokeProfileParserStructure, UnknownSelectedNonzero) {
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::UnknownSource, {9999}, 0, 0, std::nullopt}, veyr::SourceId{9999});
}
TEST_F(SmokeProfileParserStructure, UnknownTokenSource) {
  tokens[8].span.source_id = {9999};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::UnknownSource, {9999}, 28, 30, std::nullopt});
}
TEST_F(SmokeProfileParserStructure, KnownOtherSource) {
  tokens[8].span.source_id = other_source();
  expect_stream(TokenStreamErrorKind::SourceMismatch, 8);
}
TEST_F(SmokeProfileParserStructure, ReversedToken) {
  tokens[8].span = {id, 30, 28};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::ReversedRange, id, 30, 28, 33});
}
TEST_F(SmokeProfileParserStructure, BeyondToken) {
  tokens[8].span = {id, 28, 34};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::BeyondSource, id, 28, 34, 33});
}
TEST_F(SmokeProfileParserStructure, UnsupportedKind) {
  tokens[8].kind = static_cast<veyr::TokenKind>(999);
  expect_stream(TokenStreamErrorKind::UnsupportedTokenKind, 8);
}
TEST_F(SmokeProfileParserStructure, EmptyNonEof) {
  tokens[8].span = {id, 28, 28};
  expect_stream(TokenStreamErrorKind::EmptyTokenSpan, 8);
}
TEST_F(SmokeProfileParserStructure, BackwardsSpan) {
  tokens[8].span = {id, 18, 19};
  expect_stream(TokenStreamErrorKind::OutOfOrderOrOverlappingSpan, 8);
}
TEST_F(SmokeProfileParserStructure, OverlappingSpan) {
  tokens[8].span = {id, 26, 30};
  expect_stream(TokenStreamErrorKind::OutOfOrderOrOverlappingSpan, 8);
}
TEST_F(SmokeProfileParserStructure, EmptyStorage) {
  tokens.clear();
  expect_stream(TokenStreamErrorKind::MissingEndOfFile, std::nullopt);
}
TEST_F(SmokeProfileParserStructure, MissingFinalEof) {
  tokens.pop_back();
  expect_stream(TokenStreamErrorKind::MissingEndOfFile, std::nullopt);
}
TEST_F(SmokeProfileParserStructure, EarlyEof) {
  tokens[1].kind = veyr::TokenKind::EndOfFile;
  expect_stream(TokenStreamErrorKind::NonFinalEndOfFile, 1);
}
TEST_F(SmokeProfileParserStructure, DuplicateEof) {
  tokens.push_back({veyr::TokenKind::EndOfFile, {id, 33, 33}});
  expect_stream(TokenStreamErrorKind::NonFinalEndOfFile, 11);
}
TEST_F(SmokeProfileParserStructure, TokenAfterEof) {
  tokens.push_back({veyr::TokenKind::Semicolon, {id, 30, 31}});
  expect_stream(TokenStreamErrorKind::NonFinalEndOfFile, 11);
}
TEST_F(SmokeProfileParserStructure, NonemptyFinalEof) {
  tokens[11].span = {id, 32, 33};
  expect_stream(TokenStreamErrorKind::InvalidEndOfFileSpan, 11);
}
TEST_F(SmokeProfileParserStructure, EofBeforeSourceEnd) {
  tokens[11].span = {id, 32, 32};
  expect_stream(TokenStreamErrorKind::InvalidEndOfFileSpan, 11);
}
TEST_F(SmokeProfileParserStructure, ReversedEof) {
  tokens[11].span = {id, 33, 32};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::ReversedRange, id, 33, 32, 33});
}
TEST_F(SmokeProfileParserStructure, BeyondEof) {
  tokens[11].span = {id, 34, 34};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::BeyondSource, id, 34, 34, 33});
}
TEST_F(SmokeProfileParserStructure, SelectedLookupBeforeEmptyStorage) {
  tokens.clear();
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::UnknownSource, {9999}, 0, 0, std::nullopt}, veyr::SourceId{9999});
}
TEST_F(SmokeProfileParserStructure, SelectedLookupBeforeBadToken) {
  tokens[0] = {static_cast<veyr::TokenKind>(999), {{8888}, 100, 90}};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::UnknownSource, {0}, 0, 0, std::nullopt}, veyr::SourceId{0});
}
TEST_F(SmokeProfileParserStructure, UnknownTokenBeforeReversedAndUnsupported) {
  tokens[8] = {static_cast<veyr::TokenKind>(999), {{9999}, 34, 28}};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::UnknownSource, {9999}, 34, 28, std::nullopt});
}
TEST_F(SmokeProfileParserStructure, OtherSourceRangeBeforeSourceMismatch) {
  const auto other = other_source();
  tokens[8].span = {other, 30, 28};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::ReversedRange, other, 30, 28, 33});
}
TEST_F(SmokeProfileParserStructure, SourceMismatchBeforeUnsupportedKind) {
  tokens[8] = {static_cast<veyr::TokenKind>(999), {other_source(), 28, 30}};
  expect_stream(TokenStreamErrorKind::SourceMismatch, 8);
}
TEST_F(SmokeProfileParserStructure, ReversedBeforeBeyondSource) {
  tokens[8].span = {id, 35, 34};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::ReversedRange, id, 35, 34, 33});
}
TEST_F(SmokeProfileParserStructure, UnsupportedBeforeEmptySpan) {
  tokens[8] = {static_cast<veyr::TokenKind>(999), {id, 28, 28}};
  expect_stream(TokenStreamErrorKind::UnsupportedTokenKind, 8);
}
TEST_F(SmokeProfileParserStructure, NonfinalEofBeforeWrongShape) {
  tokens[8] = {veyr::TokenKind::EndOfFile, {id, 28, 30}};
  expect_stream(TokenStreamErrorKind::NonFinalEndOfFile, 8);
}
TEST_F(SmokeProfileParserStructure, EofSourceErrorBeforeFinality) {
  tokens[8] = {veyr::TokenKind::EndOfFile, {id, 28, 34}};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::BeyondSource, id, 28, 34, 33});
}
TEST_F(SmokeProfileParserStructure, EofShapeBeforeOverlap) {
  tokens[11].span = {id, 30, 30};
  expect_stream(TokenStreamErrorKind::InvalidEndOfFileSpan, 11);
}
TEST_F(SmokeProfileParserStructure, EarliestIndexBeforeLaterHigherPriority) {
  tokens[1].kind = static_cast<veyr::TokenKind>(999);
  tokens[8].span = {id, 30, 28};
  expect_stream(TokenStreamErrorKind::UnsupportedTokenKind, 1);
}
TEST_F(SmokeProfileParserStructure, ExistingTokenFaultBeforeMissingEof) {
  tokens[8].span = {id, 28, 28};
  tokens.pop_back();
  expect_stream(TokenStreamErrorKind::EmptyTokenSpan, 8);
}
TEST_F(SmokeProfileParserStructure, LaterReversedSpanBeforeLexicalRejection) {
  ASSERT_NO_FATAL_FAILURE(load("x main() -> int32 { return 42; }"));
  ASSERT_EQ(tokens[0].kind, veyr::TokenKind::Invalid);
  tokens[8].span = {id, 29, 27};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::ReversedRange, id, 29, 27, 32});
}
TEST_F(SmokeProfileParserStructure, LaterBeyondSpanBeforeLexicalRejection) {
  ASSERT_NO_FATAL_FAILURE(load("x main() -> int32 { return 42; }"));
  ASSERT_EQ(tokens[0].kind, veyr::TokenKind::Invalid);
  tokens[8].span = {id, 27, 33};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::BeyondSource, id, 27, 33, 32});
}
TEST_F(SmokeProfileParserStructure, LaterNonfinalEofBeforeLexicalRejection) {
  ASSERT_NO_FATAL_FAILURE(load("x main() -> int32 { return 42; }"));
  tokens.push_back({veyr::TokenKind::EndOfFile, {id, 32, 32}});
  expect_stream(TokenStreamErrorKind::NonFinalEndOfFile, 11);
}
TEST_F(SmokeProfileParserStructure, LaterEofShapeBeforeLexicalRejection) {
  ASSERT_NO_FATAL_FAILURE(load("x main() -> int32 { return 42; }"));
  tokens[11].span = {id, 31, 31};
  expect_stream(TokenStreamErrorKind::InvalidEndOfFileSpan, 11);
}
TEST_F(SmokeProfileParserStructure, LaterStructureBeforeFirstSyntaxMismatch) {
  ASSERT_NO_FATAL_FAILURE(load("return main() -> int32 { return 42; }"));
  ASSERT_FALSE(diagnostics.has_errors());
  tokens[8].span = {id, 32, 38};
  expect_checked(veyr::SourceError{veyr::SourceErrorKind::BeyondSource, id, 32, 38, 37});
}

struct InvalidPlacement { const char* name; const char* source; };
class SmokeProfileParserInvalidPlacement : public SmokeProfileParser,
                                          public testing::WithParamInterface<InvalidPlacement> {};
TEST_P(SmokeProfileParserInvalidPlacement, RejectsWithAndWithoutExistingErrors) {
  ASSERT_NO_FATAL_FAILURE(load(GetParam().source));
  ASSERT_TRUE(diagnostics.has_errors());
  const auto history = owned_diagnostics(diagnostics);
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  EXPECT_EQ(result, (veyr::ParseResult{ParseRejected{ParseRejectionKind::LexicalErrors}}));
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result));
  EXPECT_EQ(diagnostics.diagnostics().size(), history.size());
  expect_diagnostic_history(diagnostics, history);
  EXPECT_TRUE(diagnostics.has_errors());
  veyr::DiagnosticEngine empty_engine;
  const auto isolated = veyr::parse(sources, id, tokens, empty_engine);
  EXPECT_EQ(isolated, (veyr::ParseResult{ParseRejected{ParseRejectionKind::LexicalErrors}}));
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(isolated));
  EXPECT_TRUE(empty_engine.diagnostics().empty());
  EXPECT_FALSE(empty_engine.has_errors());
}
INSTANTIATE_TEST_SUITE_P(SmokeProfileParserStructure, SmokeProfileParserInvalidPlacement,
    testing::Values(InvalidPlacement{"Beginning", "x fn main() -> int32 { return 42; }"},
                    InvalidPlacement{"Within", "fn main() -> int32 { return -12; }"},
                    InvalidPlacement{"AfterEntry", "fn main() -> int32 { return 42; } ?"}),
    [](const testing::TestParamInfo<InvalidPlacement>& info) { return info.param.name; });

TEST_F(SmokeProfileParserStructure, BoundedLongStreamChecksFinalFaultBeforeSyntax) {
  ASSERT_NO_FATAL_FAILURE(load(std::string(4096, ';')));
  ASSERT_EQ(tokens.size(), 4097u);
  tokens.back().span = {id, 4095, 4095};
  expect_stream(TokenStreamErrorKind::InvalidEndOfFileSpan, 4096);
}
TEST_F(SmokeProfileParserStructure, PunctuationAdjacencySucceeds) {
  ASSERT_NO_FATAL_FAILURE(load("fn main()->int32{return 42;}"));
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
  EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
            (ParsedSmokeEntry{{id, 0, 28}, {id, 17, 27}, {id, 24, 26}}));
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}
TEST_F(SmokeProfileParserStructure, WhitespaceGapsPreserveOriginalSpans) {
  ASSERT_NO_FATAL_FAILURE(load(" \tfn main() -> int32 { return 42; }\r\n"));
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
  EXPECT_EQ(std::get<ParsedSmokeEntry>(result),
            (ParsedSmokeEntry{{id, 2, 35}, {id, 23, 33}, {id, 30, 32}}));
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}
TEST_F(SmokeProfileParserStructure, EmptySourceHasActualEofAndFnSyntaxError) {
  ASSERT_NO_FATAL_FAILURE(load(""));
  ASSERT_EQ(tokens.size(), 1u);
  EXPECT_EQ(tokens[0], (veyr::Token{veyr::TokenKind::EndOfFile, {id, 0, 0}}));
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  EXPECT_EQ(result, (veyr::ParseResult{ParseRejected{ParseRejectionKind::SyntaxError}}));
  const auto errors = diagnostics.diagnostics();
  ASSERT_EQ(errors.size(), 1u);
  EXPECT_EQ(errors[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(errors[0].message, "expected 'fn', found end of source in M1 Experimental Smoke Profile");
  EXPECT_EQ(errors[0].primary, (SourceSpan{id, 0, 0}));
  EXPECT_TRUE(errors[0].related.empty());
}
TEST_F(SmokeProfileParserStructure, WhitespaceOnlyUsesOriginalSourceEnd) {
  ASSERT_NO_FATAL_FAILURE(load(" \t\r\n\n\r"));
  ASSERT_EQ(tokens.size(), 1u);
  EXPECT_EQ(tokens[0], (veyr::Token{veyr::TokenKind::EndOfFile, {id, 6, 6}}));
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  EXPECT_EQ(result, (veyr::ParseResult{ParseRejected{ParseRejectionKind::SyntaxError}}));
  const auto errors = diagnostics.diagnostics();
  ASSERT_EQ(errors.size(), 1u);
  EXPECT_EQ(errors[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(errors[0].message, "expected 'fn', found end of source in M1 Experimental Smoke Profile");
  EXPECT_EQ(errors[0].primary, (SourceSpan{id, 6, 6}));
  EXPECT_TRUE(errors[0].related.empty());
}

struct ParserSyntaxCase {
  const char* name;
  const char* source;
  const char* message;
  std::size_t begin;
  std::size_t end;
  const char* related_label = nullptr;
  std::size_t related_begin = 0;
  std::size_t related_end = 0;
};

class ParserDiagnosticConformance : public SmokeProfileParser {
 protected:
  void check_syntax(const ParserSyntaxCase& row) {
    ASSERT_NO_FATAL_FAILURE(load(row.source));
    ASSERT_FALSE(diagnostics.has_errors());
    seed_diagnostic_history(diagnostics, id);
    const auto history = owned_diagnostics(diagnostics);
    const auto result = veyr::parse(sources, id, tokens, diagnostics);
    EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result));
    ASSERT_TRUE(std::holds_alternative<ParseRejected>(result));
    EXPECT_EQ(std::get<ParseRejected>(result), (ParseRejected{ParseRejectionKind::SyntaxError}));
    expect_diagnostic_history(diagnostics, history);
    const auto current = diagnostics.diagnostics();
    ASSERT_EQ(current.size(), history.size() + 1);
    const auto& error = current.back();
    EXPECT_EQ(error.severity, veyr::DiagnosticSeverity::Error);
    EXPECT_EQ(error.message, row.message);
    EXPECT_EQ(error.primary, (SourceSpan{id, row.begin, row.end}));
    if (row.related_label == nullptr) {
      EXPECT_TRUE(error.related.empty());
    } else {
      ASSERT_EQ(error.related.size(), 1u);
      EXPECT_EQ(error.related[0].span, (SourceSpan{id, row.related_begin, row.related_end}));
      EXPECT_EQ(error.related[0].label, row.related_label);
    }
    EXPECT_TRUE(diagnostics.has_errors());
  }
};

class SmokeProfileParserSyntax : public ParserDiagnosticConformance,
    public testing::WithParamInterface<ParserSyntaxCase> {};
TEST_P(SmokeProfileParserSyntax, ExactFirstErrorAndPreservedHistory) {
  ASSERT_NO_FATAL_FAILURE(check_syntax(GetParam()));
}
INSTANTIATE_TEST_SUITE_P(SmokeProfileParserSyntax, SmokeProfileParserSyntax, testing::Values(
    ParserSyntaxCase{"ExpectedFn", "return", "expected 'fn', found 'return' in M1 Experimental Smoke Profile", 0, 6},
    ParserSyntaxCase{"ExpectedMain", "fn return", "expected 'main', found 'return' in M1 Experimental Smoke Profile", 3, 9},
    ParserSyntaxCase{"ExpectedLeftParen", "fn main;", "expected '(', found ';' in M1 Experimental Smoke Profile", 7, 8},
    ParserSyntaxCase{"ExpectedRightParen", "fn main(1)", "expected ')', found decimal integer literal in M1 Experimental Smoke Profile", 8, 9, "opening '(' is here", 7, 8},
    ParserSyntaxCase{"ExpectedArrow", "fn main() int32", "expected '->', found 'int32' in M1 Experimental Smoke Profile", 10, 15},
    ParserSyntaxCase{"ExpectedInt32", "fn main() -> return", "expected 'int32', found 'return' in M1 Experimental Smoke Profile", 13, 19},
    ParserSyntaxCase{"ExpectedLeftBrace", "fn main() -> int32 return", "expected '{', found 'return' in M1 Experimental Smoke Profile", 19, 25},
    ParserSyntaxCase{"ExpectedReturn", "fn main() -> int32 { ;", "expected 'return', found ';' in M1 Experimental Smoke Profile", 21, 22},
    ParserSyntaxCase{"ExpectedInteger", "fn main() -> int32 { return ;", "expected decimal integer literal, found ';' in M1 Experimental Smoke Profile", 28, 29},
    ParserSyntaxCase{"ExpectedSemicolon", "fn main() -> int32 { return 42 }", "expected ';', found '}' in M1 Experimental Smoke Profile", 31, 32},
    ParserSyntaxCase{"ExpectedRightBrace", "fn main() -> int32 { return 42; return 1; }", "expected '}', found 'return' in M1 Experimental Smoke Profile", 32, 38, "opening '{' is here", 19, 20},
    ParserSyntaxCase{"ExpectedEof", "fn main() -> int32 { return 42; } ;", "expected end of source, found ';' in M1 Experimental Smoke Profile", 34, 35}), [](const testing::TestParamInfo<ParserSyntaxCase>& info) { return info.param.name; });

class SmokeProfileParserPrefixes : public ParserDiagnosticConformance,
    public testing::WithParamInterface<ParserSyntaxCase> {};
TEST_P(SmokeProfileParserPrefixes, ExactFirstErrorAndPreservedHistory) {
  ASSERT_NO_FATAL_FAILURE(check_syntax(GetParam()));
}
INSTANTIATE_TEST_SUITE_P(SmokeProfileParserPrefixes, SmokeProfileParserPrefixes, testing::Values(
    ParserSyntaxCase{"Empty", "", "expected 'fn', found end of source in M1 Experimental Smoke Profile", 0, 0},
    ParserSyntaxCase{"AfterFn", "fn", "expected 'main', found end of source in M1 Experimental Smoke Profile", 2, 2},
    ParserSyntaxCase{"AfterMain", "fn main", "expected '(', found end of source in M1 Experimental Smoke Profile", 7, 7},
    ParserSyntaxCase{"AfterLeftParen", "fn main(", "expected ')', found end of source in M1 Experimental Smoke Profile", 8, 8, "opening '(' is here", 7, 8},
    ParserSyntaxCase{"AfterRightParen", "fn main()", "expected '->', found end of source in M1 Experimental Smoke Profile", 9, 9},
    ParserSyntaxCase{"AfterArrow", "fn main() ->", "expected 'int32', found end of source in M1 Experimental Smoke Profile", 12, 12},
    ParserSyntaxCase{"AfterInt32", "fn main() -> int32", "expected '{', found end of source in M1 Experimental Smoke Profile", 18, 18},
    ParserSyntaxCase{"AfterLeftBrace", "fn main() -> int32 {", "expected 'return', found end of source in M1 Experimental Smoke Profile", 20, 20},
    ParserSyntaxCase{"AfterReturn", "fn main() -> int32 { return", "expected decimal integer literal, found end of source in M1 Experimental Smoke Profile", 27, 27},
    ParserSyntaxCase{"AfterInteger", "fn main() -> int32 { return 42", "expected ';', found end of source in M1 Experimental Smoke Profile", 30, 30},
    ParserSyntaxCase{"AfterSemicolon", "fn main() -> int32 { return 42;", "expected '}', found end of source in M1 Experimental Smoke Profile", 31, 31, "opening '{' is here", 19, 20}), [](const testing::TestParamInfo<ParserSyntaxCase>& info) { return info.param.name; });

class SmokeProfileParserFoundLabels : public ParserDiagnosticConformance,
    public testing::WithParamInterface<ParserSyntaxCase> {};
TEST_P(SmokeProfileParserFoundLabels, ExactFirstErrorAndPreservedHistory) {
  ASSERT_NO_FATAL_FAILURE(check_syntax(GetParam()));
}
INSTANTIATE_TEST_SUITE_P(SmokeProfileParserFoundLabels, SmokeProfileParserFoundLabels, testing::Values(
    ParserSyntaxCase{"Main", "main", "expected 'fn', found 'main' in M1 Experimental Smoke Profile", 0, 4},
    ParserSyntaxCase{"Int32", "int32", "expected 'fn', found 'int32' in M1 Experimental Smoke Profile", 0, 5},
    ParserSyntaxCase{"LeftParen", "(", "expected 'fn', found '(' in M1 Experimental Smoke Profile", 0, 1},
    ParserSyntaxCase{"RightParen", ")", "expected 'fn', found ')' in M1 Experimental Smoke Profile", 0, 1},
    ParserSyntaxCase{"Arrow", "->", "expected 'fn', found '->' in M1 Experimental Smoke Profile", 0, 2},
    ParserSyntaxCase{"LeftBrace", "{", "expected 'fn', found '{' in M1 Experimental Smoke Profile", 0, 1},
    ParserSyntaxCase{"RightBrace", "}", "expected 'fn', found '}' in M1 Experimental Smoke Profile", 0, 1},
    ParserSyntaxCase{"Semicolon", ";", "expected 'fn', found ';' in M1 Experimental Smoke Profile", 0, 1},
    ParserSyntaxCase{"Integer", "42", "expected 'fn', found decimal integer literal in M1 Experimental Smoke Profile", 0, 2},
    ParserSyntaxCase{"TrailingFn", "fn main() -> int32 { return 42; } fn", "expected end of source, found 'fn' in M1 Experimental Smoke Profile", 34, 36}), [](const testing::TestParamInfo<ParserSyntaxCase>& info) { return info.param.name; });

class SmokeProfileParserForms : public ParserDiagnosticConformance,
    public testing::WithParamInterface<ParserSyntaxCase> {};
TEST_P(SmokeProfileParserForms, ExactFirstErrorAndPreservedHistory) {
  ASSERT_NO_FATAL_FAILURE(check_syntax(GetParam()));
}
INSTANTIATE_TEST_SUITE_P(SmokeProfileParserForms, SmokeProfileParserForms, testing::Values(
    ParserSyntaxCase{"WrongRecognizedEntry", "fn return() -> int32 { return 42; }", "expected 'main', found 'return' in M1 Experimental Smoke Profile", 3, 9},
    ParserSyntaxCase{"WrongRecognizedType", "fn main() -> main { return 42; }", "expected 'int32', found 'main' in M1 Experimental Smoke Profile", 13, 17},
    ParserSyntaxCase{"MissingReturn", "fn main() -> int32 { 42; }", "expected 'return', found decimal integer literal in M1 Experimental Smoke Profile", 21, 23},
    ParserSyntaxCase{"ExtraSemicolonBeforeReturn", "fn main() -> int32 { ; return 42; }", "expected 'return', found ';' in M1 Experimental Smoke Profile", 21, 22},
    ParserSyntaxCase{"ExtraSemicolonAfterReturn", "fn main() -> int32 { return 42;; }", "expected '}', found ';' in M1 Experimental Smoke Profile", 31, 32, "opening '{' is here", 19, 20},
    ParserSyntaxCase{"TwoEntries", "fn main() -> int32 { return 42; } fn main() -> int32 { return 1; }", "expected end of source, found 'fn' in M1 Experimental Smoke Profile", 34, 36},
    ParserSyntaxCase{"UnexpectedParenthesis", "fn main) -> int32 { return 42; }", "expected '(', found ')' in M1 Experimental Smoke Profile", 7, 8},
    ParserSyntaxCase{"UnexpectedBrace", "fn main() -> int32 }", "expected '{', found '}' in M1 Experimental Smoke Profile", 19, 20},
    ParserSyntaxCase{"MultipleFaultsStopAtFirst", "fn return; } main", "expected 'main', found 'return' in M1 Experimental Smoke Profile", 3, 9}), [](const testing::TestParamInfo<ParserSyntaxCase>& info) { return info.param.name; });

struct ParserLexicalForm { const char* name; std::string source; };
class SmokeProfileParserLexicalForms : public SmokeProfileParser,
    public testing::WithParamInterface<ParserLexicalForm> {};
TEST_P(SmokeProfileParserLexicalForms, NoDuplicateParserErrorsOrPartialSuccess) {
  ASSERT_NO_FATAL_FAILURE(load(GetParam().source));
  ASSERT_TRUE(diagnostics.has_errors());
  const auto history = owned_diagnostics(diagnostics);
  ASSERT_FALSE(history.empty());
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  EXPECT_EQ(result, (veyr::ParseResult{ParseRejected{ParseRejectionKind::LexicalErrors}}));
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result));
  EXPECT_EQ(diagnostics.diagnostics().size(), history.size());
  expect_diagnostic_history(diagnostics, history);
  EXPECT_TRUE(diagnostics.has_errors());
}
INSTANTIATE_TEST_SUITE_P(SmokeProfileParserForms, SmokeProfileParserLexicalForms,
    testing::Values(
        ParserLexicalForm{"UnknownEntry", "fn other() -> int32 { return 42; }"},
        ParserLexicalForm{"Operator", "fn main() -> int32 { return 12+34; }"},
        ParserLexicalForm{"BlockComment", "fn main() -> int32 { /*x*/ return 42; }"},
        ParserLexicalForm{"OverlappingComment", "/*/return"},
        ParserLexicalForm{"PositiveSign", "fn main() -> int32 { return +12; }"},
        ParserLexicalForm{"NegativeSign", "fn main() -> int32 { return -12; }"},
        ParserLexicalForm{"NonAscii", std::string("fn main() -> int32 { return ") + static_cast<char>(0x80) + "; }"}),
    [](const testing::TestParamInfo<ParserLexicalForm>& info) { return info.param.name; });

struct ParserDecimalForm { const char* name; std::string digits; };
class SmokeProfileParserDecimalForms : public SmokeProfileParser,
    public testing::WithParamInterface<ParserDecimalForm> {};
TEST_P(SmokeProfileParserDecimalForms, CompleteEntryOwnsThreeSpansWithoutNumericConversion) {
  const auto& digits = GetParam().digits;
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return " + digits + "; }"));
  ASSERT_FALSE(diagnostics.has_errors());
  seed_diagnostic_history(diagnostics, id);
  const auto history = owned_diagnostics(diagnostics);
  const auto result = veyr::parse(sources, id, tokens, diagnostics);
  ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(result));
  const ParsedSmokeEntry expected{{id, 0, 31 + digits.size()},
                                  {id, 21, 29 + digits.size()},
                                  {id, 28, 28 + digits.size()}};
  const auto original = std::get<ParsedSmokeEntry>(result);
  auto copy = original;
  auto moved = std::move(copy);
  EXPECT_EQ(original, expected);
  EXPECT_EQ(moved, expected);
  copy = moved;
  EXPECT_EQ(copy, expected);
  ++copy.entry_span.begin;
  EXPECT_NE(copy, original);
  EXPECT_EQ(original, expected);
  const auto literal = sources.text(original.integer_span);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(literal));
  EXPECT_EQ(std::get<std::string_view>(literal), digits);
  EXPECT_EQ(diagnostics.diagnostics().size(), history.size());
  expect_diagnostic_history(diagnostics, history);
  EXPECT_TRUE(diagnostics.has_errors());
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(result) && !diagnostics.has_errors());
}
INSTANTIATE_TEST_SUITE_P(SmokeProfileParserForms, SmokeProfileParserDecimalForms,
    testing::Values(ParserDecimalForm{"CompletePrefix", "42"},
                    ParserDecimalForm{"BeyondFutureByteRange", "256"},
                    ParserDecimalForm{"LeadingZeros", "00042"},
                    ParserDecimalForm{"NoMachineIntegerConversion", std::string(512, '9')}),
    [](const testing::TestParamInfo<ParserDecimalForm>& info) { return info.param.name; });

}  // namespace
