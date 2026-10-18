#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/lexing/lexer.hpp>
#include <veyr/source/source_manager.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace {

using veyr::TokenKind;

static_assert(std::is_same_v<veyr::LexResult,
                             std::variant<std::vector<veyr::Token>, veyr::SourceError>>);

constexpr std::string_view word_error = "unsupported word in M1 Experimental Smoke Profile";
constexpr std::string_view number_error =
    "unsupported numeric spelling in M1 Experimental Smoke Profile";
constexpr std::string_view high_error =
    "unsupported non-ASCII byte run in M1 Experimental Smoke Profile";
constexpr std::string_view comment_error =
    "unsupported comment-shaped input in M1 Experimental Smoke Profile";
constexpr std::string_view byte_error = "unsupported byte in M1 Experimental Smoke Profile";

struct Expected {
  TokenKind kind;
  std::size_t begin;
  std::size_t end;
  std::string_view message = {};
};

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
    const auto text = sources.text(tokens[index].span);
    ASSERT_TRUE(std::holds_alternative<std::string_view>(text));
    EXPECT_EQ(std::get<std::string_view>(text),
              std::string_view{bytes}.substr(item.begin, item.end - item.begin));
    EXPECT_LT(tokens[index].span.begin, tokens[index].span.end);
    if (item.kind == TokenKind::Invalid) {
      ASSERT_LT(error_index, errors.size());
      const auto& error = errors[error_index++];
      EXPECT_EQ(error.severity, veyr::DiagnosticSeverity::Error);
      EXPECT_EQ(error.message, item.message);
      EXPECT_EQ(error.primary, wanted.span);
      EXPECT_TRUE(error.related.empty());
    }
  }
  const veyr::Token eof{TokenKind::EndOfFile, {id, bytes.size(), bytes.size()}};
  EXPECT_EQ(tokens.back(), eof);
  EXPECT_EQ(std::count_if(tokens.begin(), tokens.end(), [](const auto& token) {
              return token.kind == TokenKind::EndOfFile;
            }), 1);
  EXPECT_EQ(error_index, errors.size());
  EXPECT_EQ(diagnostics.has_errors(), error_index != 0);
  EXPECT_EQ(std::get<std::string_view>(sources.source_text(id)), bytes);
}

TEST(SmokeProfileLexer, FixedSampleHasExactOriginalTokenSpans) {
  expect_scan("fn main() -> int32 { return 42; }",
              {{TokenKind::Fn, 0, 2}, {TokenKind::Main, 3, 7},
               {TokenKind::LeftParen, 7, 8}, {TokenKind::RightParen, 8, 9},
               {TokenKind::Arrow, 10, 12}, {TokenKind::Int32, 13, 18},
               {TokenKind::LeftBrace, 19, 20}, {TokenKind::Return, 21, 27},
               {TokenKind::IntegerLiteral, 28, 30}, {TokenKind::Semicolon, 30, 31},
               {TokenKind::RightBrace, 32, 33}});
}

TEST(SmokeProfileLexer, RecognizesUnitsWithoutParsingProgramStructure) {
  expect_scan("return fn int32 main;{}()->",
              {{TokenKind::Return, 0, 6}, {TokenKind::Fn, 7, 9},
               {TokenKind::Int32, 10, 15}, {TokenKind::Main, 16, 20},
               {TokenKind::Semicolon, 20, 21}, {TokenKind::LeftBrace, 21, 22},
               {TokenKind::RightBrace, 22, 23}, {TokenKind::LeftParen, 23, 24},
               {TokenKind::RightParen, 24, 25}, {TokenKind::Arrow, 25, 27}});
}

TEST(SmokeProfileLexer, KeywordsAreExactCaseSensitiveWholeWords) {
  for (const auto& bytes : {"Fn", "MAIN", "Int32", "Return", "_", "_return",
                            "return42", "fnmain", "int32_", "main0"}) {
    SCOPED_TRACE(bytes);
    expect_scan(bytes, {{TokenKind::Invalid, 0, std::string_view{bytes}.size(), word_error}});
  }
}

TEST(SmokeProfileLexer, DecimalShapeHasNoSemanticRangeOrConversion) {
  for (const auto& bytes : {std::string{"0"}, std::string{"255"}, std::string{"256"},
                            std::string{"00042"}, std::string(16384, '9')}) {
    SCOPED_TRACE(testing::Message() << "length=" << bytes.size());
    expect_scan(bytes, {{TokenKind::IntegerLiteral, 0, bytes.size()}});
  }
}

TEST(SmokeProfileLexer, WhitespacePreservesLfCrlfLoneCrAndEofOffsets) {
  expect_scan("\tfn\r\nreturn\r42\n",
              {{TokenKind::Fn, 1, 3}, {TokenKind::Return, 5, 11},
               {TokenKind::IntegerLiteral, 12, 14}});
  expect_scan("", {});
  expect_scan(" \t\r\n\r", {});
}

TEST(SmokeProfileLexer, UnsupportedNumbersPreserveSurroundingDelimiters) {
  for (const auto& bytes : {"+12", "-12", "0x10", "12_3", "12u", "1.5", ".5",
                            "1..2", "1e+2", "1E-2", "1p+2", "+.5", "-.5"}) {
    SCOPED_TRACE(bytes);
    const std::size_t length = std::string_view{bytes}.size();
    expect_scan("(" + std::string{bytes} + ");return",
                {{TokenKind::LeftParen, 0, 1},
                 {TokenKind::Invalid, 1, length + 1, number_error},
                 {TokenKind::RightParen, length + 1, length + 2},
                 {TokenKind::Semicolon, length + 2, length + 3},
                 {TokenKind::Return, length + 3, length + 9}});
  }
}

TEST(SmokeProfileLexer, NumericSignsAndArrowHaveDistinctBoundaries) {
  expect_scan("12-34->fn", {{TokenKind::IntegerLiteral, 0, 2},
                            {TokenKind::Invalid, 2, 5, number_error},
                            {TokenKind::Arrow, 5, 7}, {TokenKind::Fn, 7, 9}});
  expect_scan("1e+", {{TokenKind::Invalid, 0, 2, number_error},
                      {TokenKind::Invalid, 2, 3, byte_error}});
  expect_scan("1e->", {{TokenKind::Invalid, 0, 2, number_error},
                       {TokenKind::Arrow, 2, 4}});
}

TEST(SmokeProfileLexer, IncompleteSingleByteLookaheadTailsMakeProgress) {
  for (const auto& bytes : {"/", "+", "-", "."}) {
    SCOPED_TRACE(bytes);
    expect_scan(bytes, {{TokenKind::Invalid, 0, 1, byte_error}});
  }
  expect_scan("/*", {{TokenKind::Invalid, 0, 2, comment_error}});
  expect_scan("//", {{TokenKind::Invalid, 0, 2, comment_error}});
  expect_scan("/**/", {{TokenKind::Invalid, 0, 4, comment_error}});
}

TEST(SmokeProfileLexer, CommentContentsProduceNoNestedDiagnostics) {
  const std::string bytes{"/*\0\x1B\xFF\r\n12_3*/fn", 15};
  expect_scan(bytes, {{TokenKind::Invalid, 0, 13, comment_error},
                      {TokenKind::Fn, 13, 15}});
}

TEST(SmokeProfileLexer, UnknownSourceReturnsExactErrorWithoutDiagnosticOrTokens) {
  veyr::SourceManager sources;
  veyr::DiagnosticEngine diagnostics;
  for (const veyr::SourceId id : {veyr::SourceId{0}, veyr::SourceId{1},
                                 veyr::SourceId{std::numeric_limits<std::uint64_t>::max()}}) {
    const auto checked = sources.source_text(id);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(checked));
    const auto result = veyr::lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
    EXPECT_EQ(std::get<veyr::SourceError>(result), std::get<veyr::SourceError>(checked));
    EXPECT_TRUE(diagnostics.diagnostics().empty());
    EXPECT_FALSE(diagnostics.has_errors());
  }
}

TEST(SmokeProfileLexer, UnknownSourceLeavesExistingDiagnosticsUntouched) {
  veyr::SourceManager sources;
  veyr::DiagnosticEngine diagnostics;
  diagnostics.emit({veyr::DiagnosticSeverity::Error, "prior", std::nullopt, {}});
  const auto result = veyr::lex(sources, {0}, diagnostics);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
  ASSERT_EQ(diagnostics.diagnostics().size(), 1u);
  EXPECT_EQ(diagnostics.diagnostics()[0].message, "prior");
  EXPECT_EQ(diagnostics.diagnostics()[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST(SmokeProfileLexer, CleanScanCannotErasePriorErrorState) {
  veyr::SourceManager sources;
  const auto registered = sources.register_source("<clean>", "fn");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  veyr::DiagnosticEngine diagnostics;
  diagnostics.emit({veyr::DiagnosticSeverity::Error, "prior", std::nullopt, {}});
  const auto result = veyr::lex(sources, std::get<veyr::SourceId>(registered), diagnostics);
  ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
  ASSERT_EQ(std::get<std::vector<veyr::Token>>(result).size(), 2u);
  EXPECT_EQ(diagnostics.diagnostics().size(), 1u);
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST(SmokeProfileLexer, RecoveryAppendsInOrderAndRepeatedScansKeepDuplicates) {
  veyr::SourceManager sources;
  const auto registered = sources.register_source("<recover>", "?fn+");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const auto id = std::get<veyr::SourceId>(registered);
  veyr::DiagnosticEngine diagnostics;
  diagnostics.emit({veyr::DiagnosticSeverity::Note, "prior", std::nullopt, {}});
  for (int scan = 0; scan < 2; ++scan) {
    const auto result = veyr::lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
    const std::vector<veyr::Token> expected = {
        {TokenKind::Invalid, {id, 0, 1}}, {TokenKind::Fn, {id, 1, 3}},
        {TokenKind::Invalid, {id, 3, 4}}, {TokenKind::EndOfFile, {id, 4, 4}}};
    EXPECT_EQ(std::get<std::vector<veyr::Token>>(result), expected);
  }
  const auto values = diagnostics.diagnostics();
  ASSERT_EQ(values.size(), 5u);
  EXPECT_EQ(values[0].message, "prior");
  for (std::size_t index = 1; index < values.size(); ++index) {
    EXPECT_EQ(values[index].severity, veyr::DiagnosticSeverity::Error);
    EXPECT_EQ(values[index].message, byte_error);
    const veyr::SourceSpan expected{id, index % 2 == 1 ? 0u : 3u,
                                   index % 2 == 1 ? 1u : 4u};
    EXPECT_EQ(values[index].primary, expected);
    EXPECT_TRUE(values[index].related.empty());
  }
  EXPECT_TRUE(diagnostics.has_errors());
}

struct RecoveryCase {
  std::string bytes;
  std::vector<Expected> tokens;
};

// Keep CTest's parameter labels deterministic; never print object storage/addresses.
void PrintTo(const RecoveryCase& value, std::ostream* output) {
  constexpr std::string_view digits = "0123456789ABCDEF";
  *output << "bytes";
  for (const unsigned char byte : value.bytes) {
    *output << '_' << digits[byte >> 4] << digits[byte & 0x0F];
  }
}

class SmokeProfileLexerRecovery : public testing::TestWithParam<RecoveryCase> {};

TEST_P(SmokeProfileLexerRecovery, ReviewedOriginalByteSpansMessagesAndEof) {
  SCOPED_TRACE(testing::PrintToString(GetParam().bytes));
  expect_scan(GetParam().bytes, GetParam().tokens);
}

const std::vector<RecoveryCase> profile_rows = {
    {"+12", {{TokenKind::Invalid, 0, 3, number_error}}},
    {"1..2", {{TokenKind::Invalid, 0, 4, number_error}}},
    {"-12;", {{TokenKind::Invalid, 0, 3, number_error}, {TokenKind::Semicolon, 3, 4}}},
    {"0x10", {{TokenKind::Invalid, 0, 4, number_error}}},
    {"12_3", {{TokenKind::Invalid, 0, 4, number_error}}},
    {"12u", {{TokenKind::Invalid, 0, 3, number_error}}},
    {"1.5", {{TokenKind::Invalid, 0, 3, number_error}}},
    {".5", {{TokenKind::Invalid, 0, 2, number_error}}},
    {"1e+2", {{TokenKind::Invalid, 0, 4, number_error}}},
    {"12+34", {{TokenKind::IntegerLiteral, 0, 2}, {TokenKind::Invalid, 2, 5, number_error}}},
    {"+ 12", {{TokenKind::Invalid, 0, 1, byte_error}, {TokenKind::IntegerLiteral, 2, 4}}},
    {"12//x\nreturn", {{TokenKind::IntegerLiteral, 0, 2},
                       {TokenKind::Invalid, 2, 5, comment_error}, {TokenKind::Return, 6, 12}}},
    {"12/*x*/34", {{TokenKind::IntegerLiteral, 0, 2},
                   {TokenKind::Invalid, 2, 7, comment_error}, {TokenKind::IntegerLiteral, 7, 9}}},
    {"//\xC3\xA9\n", {{TokenKind::Invalid, 0, 4, comment_error}}},
    {"/*\xC3\xA9*/", {{TokenKind::Invalid, 0, 6, comment_error}}},
    {"\xC3\xA9" "fn", {{TokenKind::Invalid, 0, 2, high_error}, {TokenKind::Fn, 2, 4}}},
    {"fn\xC3\xA9return", {{TokenKind::Fn, 0, 2}, {TokenKind::Invalid, 2, 4, high_error},
                          {TokenKind::Return, 4, 10}}},
    {"12\xC3\xA9_", {{TokenKind::IntegerLiteral, 0, 2},
                     {TokenKind::Invalid, 2, 4, high_error}, {TokenKind::Invalid, 4, 5, word_error}}},
    {std::string{"\x80" "a\xFF", 3}, {{TokenKind::Invalid, 0, 1, high_error},
                                     {TokenKind::Invalid, 1, 2, word_error},
                                     {TokenKind::Invalid, 2, 3, high_error}}},
    {"return42", {{TokenKind::Invalid, 0, 8, word_error}}},
    {"fnmain", {{TokenKind::Invalid, 0, 6, word_error}}},
    {"- >", {{TokenKind::Invalid, 0, 1, byte_error}, {TokenKind::Invalid, 2, 3, byte_error}}},
    {"12->", {{TokenKind::IntegerLiteral, 0, 2}, {TokenKind::Arrow, 2, 4}}},
    {"//x\ry\n", {{TokenKind::Invalid, 0, 5, comment_error}}},
    {"//x\r\n", {{TokenKind::Invalid, 0, 3, comment_error}}},
    {"//x", {{TokenKind::Invalid, 0, 3, comment_error}}},
    {"/*x", {{TokenKind::Invalid, 0, 3, comment_error}}},
    {"/*/return", {{TokenKind::Invalid, 0, 9, comment_error}}},
    {"/*a/*b*/return", {{TokenKind::Invalid, 0, 8, comment_error}, {TokenKind::Return, 8, 14}}},
    {std::string{"\0return", 7}, {{TokenKind::Invalid, 0, 1, byte_error},
                                {TokenKind::Return, 1, 7}}},
    {"\x0B\x0C", {{TokenKind::Invalid, 0, 1, byte_error},
                  {TokenKind::Invalid, 1, 2, byte_error}}},
    {" \t\r\n", {}},
    {"", {}},
    {"256", {{TokenKind::IntegerLiteral, 0, 3}}},
    {"00042", {{TokenKind::IntegerLiteral, 0, 5}}}};

INSTANTIATE_TEST_SUITE_P(SmokeProfileExamples, SmokeProfileLexerRecovery,
                         testing::ValuesIn(profile_rows));

}  // namespace
