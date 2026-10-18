#include <veyr/lexing/token.hpp>
#include <veyr/source/source_manager.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

namespace {

using veyr::SourceSpan;
using veyr::Token;
using veyr::TokenKind;

static_assert(std::is_same_v<decltype(Token::kind), TokenKind>);
static_assert(std::is_same_v<decltype(Token::span), SourceSpan>);
static_assert(std::is_aggregate_v<Token>);
static_assert(std::is_trivially_copyable_v<Token>);

TEST(SmokeProfileToken, TwoMemberAggregateHasValueSemantics) {
  const Token original{TokenKind::Return, {{7}, 2, 8}};
  const auto [kind, span] = original;
  EXPECT_EQ(kind, TokenKind::Return);
  EXPECT_EQ(span, original.span);
  Token copy = original;
  EXPECT_EQ(copy, original);
  copy.kind = TokenKind::Invalid;
  EXPECT_NE(copy, original);
  copy = original;
  ++copy.span.begin;
  EXPECT_NE(copy, original);
}

TEST(SmokeProfileToken, ThirteenProfileKindsAreDistinct) {
  const std::array kinds = {
      TokenKind::Fn, TokenKind::Main, TokenKind::Int32, TokenKind::Return,
      TokenKind::LeftParen, TokenKind::RightParen, TokenKind::Arrow,
      TokenKind::LeftBrace, TokenKind::RightBrace, TokenKind::Semicolon,
      TokenKind::IntegerLiteral, TokenKind::Invalid, TokenKind::EndOfFile};
  for (std::size_t first = 0; first < kinds.size(); ++first) {
    for (std::size_t second = first + 1; second < kinds.size(); ++second) {
      EXPECT_NE(kinds[first], kinds[second]);
    }
  }
}

TEST(SmokeProfileToken, LexemesResolveExactOriginalBytesIncludingNulAndHighBytes) {
  veyr::SourceManager sources;
  const std::string bytes{"A\0\t\r\n\xC3\xA9", 7};
  const auto registered = sources.register_source("<token>", bytes);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const auto id = std::get<veyr::SourceId>(registered);
  for (std::size_t begin = 0; begin <= bytes.size(); ++begin) {
    for (std::size_t end = begin; end <= bytes.size(); ++end) {
      const Token token{TokenKind::Invalid, {id, begin, end}};
      const auto text = sources.text(token.span);
      ASSERT_TRUE(std::holds_alternative<std::string_view>(text));
      EXPECT_EQ(std::get<std::string_view>(text),
                std::string_view{bytes}.substr(begin, end - begin));
    }
  }
}

TEST(SmokeProfileToken, EmptyAndSourceEndSpansResolveEmptyText) {
  veyr::SourceManager sources;
  for (const auto& bytes : {std::string{}, std::string{"fn\r\n"}}) {
    const auto registered = sources.register_source("<eof>", bytes);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
    const Token eof{TokenKind::EndOfFile,
                    {std::get<veyr::SourceId>(registered), bytes.size(), bytes.size()}};
    const auto text = sources.text(eof.span);
    ASSERT_TRUE(std::holds_alternative<std::string_view>(text));
    EXPECT_TRUE(std::get<std::string_view>(text).empty());
  }
}

TEST(SmokeProfileToken, OriginalSnapshotAndViewsSurviveFurtherRegistrations) {
  veyr::SourceManager sources;
  const auto registered = sources.register_source("same.vyr", "return");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const Token token{TokenKind::Return, {std::get<veyr::SourceId>(registered), 0, 6}};
  const auto result = sources.text(token.span);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(result));
  const auto saved = std::get<std::string_view>(result);
  for (int index = 0; index < 3000; ++index) {
    const auto newer = sources.register_source("same.vyr", "return");
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(newer));
    EXPECT_NE(std::get<veyr::SourceId>(newer), token.span.source_id);
  }
  EXPECT_EQ(saved, "return");
  EXPECT_EQ(saved.data(), std::get<std::string_view>(sources.text(token.span)).data());
}

TEST(SmokeProfileToken, InvalidSpansPreserveCheckedSourceErrors) {
  veyr::SourceManager sources;
  const auto registered = sources.register_source("<bounds>", "abc");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const auto id = std::get<veyr::SourceId>(registered);
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
}

TEST(SmokeProfileToken, ResolutionRequiresTheOriginatingManager) {
  veyr::SourceManager first;
  veyr::SourceManager second;
  const auto one = first.register_source("same.vyr", "fn");
  const auto two = second.register_source("same.vyr", "42");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(one));
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(two));
  ASSERT_EQ(std::get<veyr::SourceId>(one), std::get<veyr::SourceId>(two));
  const Token token{TokenKind::Fn, {std::get<veyr::SourceId>(one), 0, 2}};
  EXPECT_EQ(std::get<std::string_view>(first.text(token.span)), "fn");
  // Numeric SourceIds cannot detect a caller choosing an unrelated manager.
  EXPECT_EQ(std::get<std::string_view>(second.text(token.span)), "42");
}

}  // namespace
