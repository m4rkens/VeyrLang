#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/diagnostics/plain_text_renderer.hpp>
#include <veyr/lexing/lexer.hpp>
#include <veyr/source/source_manager.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
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

class SmokeProfileLexerAllBytes : public testing::TestWithParam<unsigned> {};

TEST_P(SmokeProfileLexerAllBytes, ExactSingleByteTokenDiagnosticAndEof) {
  const auto byte = GetParam();
  const std::string input(1, static_cast<char>(byte));
  // Independent specification partitions, not a second byte-scanner algorithm.
  if (std::string_view{" \t\r\n"}.find(input[0]) != std::string_view::npos) {
    expect_scan(input, {});
  } else if (std::string_view{"0123456789"}.find(input[0]) != std::string_view::npos) {
    expect_scan(input, {{TokenKind::IntegerLiteral, 0, 1}});
  } else {
    const std::array punctuation = {
        TokenKind::LeftParen, TokenKind::RightParen, TokenKind::LeftBrace,
        TokenKind::RightBrace, TokenKind::Semicolon};
    const auto delimiter = std::string_view{"(){};"}.find(input[0]);
    if (delimiter != std::string_view::npos) {
      expect_scan(input, {{punctuation[delimiter], 0, 1}});
    } else {
      const bool word = std::string_view{
          "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"}.find(input[0]) !=
                        std::string_view::npos;
      expect_scan(input, {{TokenKind::Invalid, 0, 1,
                           byte >= 128 ? high_error : word ? word_error : byte_error}});
    }
  }
}

INSTANTIATE_TEST_SUITE_P(SmokeProfileBytes, SmokeProfileLexerAllBytes,
                         testing::Range(0u, 256u));

// Progress/coverage properties complement the separately specified exact oracles.
void expect_bounded_scan(const std::string& bytes, std::size_t token_count,
                         std::size_t error_count) {
  veyr::SourceManager sources;
  const auto registered = sources.register_source("<progress>", bytes);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const auto id = std::get<veyr::SourceId>(registered);
  veyr::DiagnosticEngine diagnostics;
  const auto result = veyr::lex(sources, id, diagnostics);
  ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
  const auto& tokens = std::get<std::vector<veyr::Token>>(result);
  ASSERT_EQ(tokens.size(), token_count);
  ASSERT_FALSE(tokens.empty());
  EXPECT_LE(tokens.size(), bytes.size() + 1);
  const auto errors = diagnostics.diagnostics();
  EXPECT_EQ(errors.size(), error_count);
  EXPECT_LE(errors.size(), bytes.size());
  std::size_t covered = 0;
  std::size_t invalids = 0;
  for (std::size_t index = 0; index < tokens.size(); ++index) {
    SCOPED_TRACE(index);
    const auto& token = tokens[index];
    EXPECT_EQ(token.span.source_id, id);
    ASSERT_GE(token.span.begin, covered);
    ASSERT_LE(token.span.begin, token.span.end);
    ASSERT_LE(token.span.end, bytes.size());
    for (std::size_t gap = covered; gap < token.span.begin; ++gap) {
      EXPECT_NE(std::string_view{" \t\r\n"}.find(bytes[gap]), std::string_view::npos);
    }
    const auto checked = sources.text(token.span);
    ASSERT_TRUE(std::holds_alternative<std::string_view>(checked));
    EXPECT_EQ(std::get<std::string_view>(checked),
              std::string_view{bytes}.substr(token.span.begin, token.span.end - token.span.begin));
    if (index + 1 == tokens.size()) {
      EXPECT_EQ(token, (veyr::Token{TokenKind::EndOfFile, {id, bytes.size(), bytes.size()}}));
    } else {
      EXPECT_NE(token.kind, TokenKind::EndOfFile);
      EXPECT_LT(token.span.begin, token.span.end);
    }
    if (token.kind == TokenKind::Invalid) {
      ASSERT_LT(invalids, errors.size());
      const auto& error = errors[invalids++];
      EXPECT_EQ(error.severity, veyr::DiagnosticSeverity::Error);
      EXPECT_EQ(error.primary, token.span);
      EXPECT_FALSE(error.message.empty());
      EXPECT_TRUE(error.related.empty());
    }
    covered = token.span.end;
  }
  EXPECT_EQ(covered, bytes.size());
  EXPECT_EQ(invalids, error_count);
  EXPECT_EQ(diagnostics.has_errors(), error_count != 0);
  EXPECT_EQ(std::get<std::string_view>(sources.source_text(id)), bytes);
}

TEST(SmokeProfileLexer, LongRunsKeepLexicalShapeAndBoundedProgress) {
  const std::string digits(65536, '9');
  const std::string word(65536, 'x');
  const std::string high(65536, static_cast<char>(0xff));
  const std::array<std::string, 3> comments = {
      "/*" + word + "*/", "/*" + word, "//" + word};
  expect_scan(digits, {{TokenKind::IntegerLiteral, 0, digits.size()}});
  expect_scan(word, {{TokenKind::Invalid, 0, word.size(), word_error}});
  expect_scan(high, {{TokenKind::Invalid, 0, high.size(), high_error}});
  expect_bounded_scan(digits, 2, 0);
  expect_bounded_scan(word, 2, 1);
  expect_bounded_scan(high, 2, 1);
  for (const auto& bytes : comments) {
    expect_scan(bytes, {{TokenKind::Invalid, 0, bytes.size(), comment_error}});
    expect_bounded_scan(bytes, 2, 1);
  }
}

TEST(SmokeProfileLexer, ManyIndependentErrorsPreserveEveryDelimiterAndWhitespaceGap) {
  std::string bytes;
  std::vector<Expected> expected;
  for (std::size_t index = 0; index < 4096; ++index) {
    const auto start = bytes.size();
    bytes += "?; \t";
    expected.push_back({TokenKind::Invalid, start, start + 1, byte_error});
    expected.push_back({TokenKind::Semicolon, start + 1, start + 2});
  }
  expect_scan(bytes, expected);
  expect_bounded_scan(bytes, 8193, 4096);
}

TEST(SmokeProfileLexer, NumericRecoveryRetainsEveryIndependentDelimiter) {
  const std::array delimiters = {TokenKind::LeftParen, TokenKind::RightParen,
      TokenKind::LeftBrace, TokenKind::RightBrace, TokenKind::Semicolon, TokenKind::Arrow};
  const std::array<std::string, 6> spellings = {"(", ")", "{", "}", ";", "->"};
  for (std::size_t index = 0; index < spellings.size(); ++index) {
    const auto width = spellings[index].size();
    const auto bytes = spellings[index] + "1..2" + spellings[index] + "fn";
    expect_scan(bytes, {{delimiters[index], 0, width},
                       {TokenKind::Invalid, width, width + 4, number_error},
                       {delimiters[index], width + 4, width * 2 + 4},
                       {TokenKind::Fn, width * 2 + 4, width * 2 + 6}});
    expect_bounded_scan(bytes, 5, 1);
  }
}

TEST(SmokeProfileLexer, AdversarialMixedPrecedenceHasExactIndependentOracles) {
  expect_scan("--12", {{TokenKind::Invalid, 0, 1, byte_error},
                       {TokenKind::Invalid, 1, 4, number_error}});
  expect_scan("12->int32", {{TokenKind::IntegerLiteral, 0, 2},
                            {TokenKind::Arrow, 2, 4}, {TokenKind::Int32, 4, 9}});
  expect_scan("- > + .5", {{TokenKind::Invalid, 0, 1, byte_error},
                          {TokenKind::Invalid, 2, 3, byte_error},
                          {TokenKind::Invalid, 4, 5, byte_error},
                          {TokenKind::Invalid, 6, 8, number_error}});
  expect_scan("fn?return42;->.5{}", {{TokenKind::Fn, 0, 2},
      {TokenKind::Invalid, 2, 3, byte_error}, {TokenKind::Invalid, 3, 11, word_error},
      {TokenKind::Semicolon, 11, 12}, {TokenKind::Arrow, 12, 14},
      {TokenKind::Invalid, 14, 16, number_error},
      {TokenKind::LeftBrace, 16, 17}, {TokenKind::RightBrace, 17, 18}});
  expect_scan(std::string{"12/*\x80\0\x1B" "1..2*/34", 15},
      {{TokenKind::IntegerLiteral, 0, 2}, {TokenKind::Invalid, 2, 13, comment_error},
       {TokenKind::IntegerLiteral, 13, 15}});
  expect_scan(std::string{"//\xFF\0\x1B" "1e+2\r\nfn", 13},
      {{TokenKind::Invalid, 0, 9, comment_error}, {TokenKind::Fn, 11, 13}});
  expect_scan("/*a/*b*/return/*/fn", {{TokenKind::Invalid, 0, 8, comment_error},
      {TokenKind::Return, 8, 14}, {TokenKind::Invalid, 14, 19, comment_error}});
  expect_scan("fn/*", {{TokenKind::Fn, 0, 2}, {TokenKind::Invalid, 2, 4, comment_error}});
  expect_scan(std::string{"\x80\xFF" "fn12\xFE" "12->return", 17},
      {{TokenKind::Invalid, 0, 2, high_error}, {TokenKind::Invalid, 2, 6, word_error},
       {TokenKind::Invalid, 6, 7, high_error}, {TokenKind::IntegerLiteral, 7, 9},
       {TokenKind::Arrow, 9, 11}, {TokenKind::Return, 11, 17}});
}

TEST(SmokeProfileLexer, TokenAndDiagnosticLocationsPreserveOriginalNewlineBytes) {
  veyr::SourceManager sources;
  const std::string bytes = "//x\r\nreturn\r42\n";
  const auto registered = sources.register_source("<locations>", bytes);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(registered));
  const auto id = std::get<veyr::SourceId>(registered);
  veyr::DiagnosticEngine diagnostics;
  const auto result = veyr::lex(sources, id, diagnostics);
  ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
  const std::vector<veyr::Token> expected = {{TokenKind::Invalid, {id, 0, 3}},
      {TokenKind::Return, {id, 5, 11}}, {TokenKind::IntegerLiteral, {id, 12, 14}},
      {TokenKind::EndOfFile, {id, 15, 15}}};
  EXPECT_EQ(std::get<std::vector<veyr::Token>>(result), expected);
  ASSERT_EQ(diagnostics.diagnostics().size(), 1u);
  EXPECT_EQ(diagnostics.diagnostics()[0].primary, expected[0].span);
  EXPECT_EQ(std::get<std::string_view>(sources.text(expected[0].span)), "//x");
  const std::array<veyr::SourceLocation, 9> locations = {{
      {id, 0, 1, 1}, {id, 2, 1, 3}, {id, 3, 1, 4}, {id, 4, 1, 5},
      {id, 5, 2, 1}, {id, 11, 2, 7}, {id, 12, 2, 8}, {id, 14, 2, 10}, {id, 15, 3, 1}}};
  for (const auto& expected_location : locations) {
    const auto actual = sources.location(id, expected_location.offset);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceLocation>(actual));
    EXPECT_EQ(std::get<veyr::SourceLocation>(actual), expected_location);
  }
  EXPECT_EQ(std::get<std::string_view>(sources.source_text(id)), bytes);
}

TEST(SmokeProfileLexer, EmptyAndTrailingBreakEofLocationsRemainByteBased) {
  struct Ending { std::string bytes; std::size_t line; std::size_t column; };
  const std::array<Ending, 8> cases = {{{"", 1, 1}, {"\n", 2, 1}, {"\r\n", 2, 1},
      {"\r", 1, 2}, {"fn\n", 2, 1}, {"fn\r", 1, 4},
      {"//x\r", 1, 5}, {"//x\r\n", 2, 1}}};
  for (const auto& item : cases) {
    veyr::SourceManager sources;
    const auto added = sources.register_source("<eof>", item.bytes);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(added));
    const auto id = std::get<veyr::SourceId>(added);
    veyr::DiagnosticEngine diagnostics;
    const auto result = veyr::lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
    const auto& tokens = std::get<std::vector<veyr::Token>>(result);
    ASSERT_FALSE(tokens.empty());
    EXPECT_EQ(tokens.back(), (veyr::Token{TokenKind::EndOfFile,
                                        {id, item.bytes.size(), item.bytes.size()}}));
    const auto location = sources.location(id, tokens.back().span.begin);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceLocation>(location));
    EXPECT_EQ(std::get<veyr::SourceLocation>(location),
              (veyr::SourceLocation{id, item.bytes.size(), item.line, item.column}));
  }
}

TEST(SmokeProfileLexer, FreshScansProduceDeterministicTokensAndOwnedDiagnostics) {
  veyr::SourceManager sources;
  const auto added = sources.register_source("<deterministic>", "?fn\x80; +");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(added));
  const auto id = std::get<veyr::SourceId>(added);
  std::vector<veyr::Token> saved_tokens;
  std::vector<veyr::Diagnostic> saved_errors;
  for (int pass = 0; pass < 2; ++pass) {
    veyr::DiagnosticEngine diagnostics;
    const auto result = veyr::lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
    const auto& tokens = std::get<std::vector<veyr::Token>>(result);
    const auto errors = diagnostics.diagnostics();
    if (pass == 0) {
      saved_tokens = tokens;
      saved_errors.assign(errors.begin(), errors.end());
    } else {
      EXPECT_EQ(tokens, saved_tokens);
      ASSERT_EQ(errors.size(), saved_errors.size());
      for (std::size_t index = 0; index < errors.size(); ++index) {
        EXPECT_EQ(errors[index].severity, saved_errors[index].severity);
        EXPECT_EQ(errors[index].message, saved_errors[index].message);
        EXPECT_EQ(errors[index].primary, saved_errors[index].primary);
        EXPECT_TRUE(errors[index].related.empty());
      }
    }
  }
  ASSERT_EQ(saved_errors.size(), 3u);
  EXPECT_EQ(saved_errors[0].message, byte_error);
  EXPECT_EQ(saved_errors[1].message, high_error);
  EXPECT_EQ(saved_errors[2].message, byte_error);
}

TEST(SmokeProfileLexer, SharedEnginePreservesEveryPriorSeverityAndRejectsSuccessHandoff) {
  for (const auto severity : {veyr::DiagnosticSeverity::Warning, veyr::DiagnosticSeverity::Note,
                              veyr::DiagnosticSeverity::Error}) {
    veyr::SourceManager sources;
    const auto clean = sources.register_source("<clean>", "fn");
    const auto bad = sources.register_source("<bad>", "?");
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(clean));
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(bad));
    const auto clean_id = std::get<veyr::SourceId>(clean);
    const auto bad_id = std::get<veyr::SourceId>(bad);
    veyr::DiagnosticEngine diagnostics;
    diagnostics.emit({severity, "prior", veyr::SourceSpan{clean_id, 0, 1},
                       {{{bad_id, 0, 1}, "context"}}});
    const auto clean_result = veyr::lex(sources, clean_id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(clean_result));
    EXPECT_EQ(diagnostics.has_errors(), severity == veyr::DiagnosticSeverity::Error);
    EXPECT_EQ(std::holds_alternative<std::vector<veyr::Token>>(clean_result) &&
                  !diagnostics.has_errors(), severity != veyr::DiagnosticSeverity::Error);
    for (int pass = 0; pass < 2; ++pass) {
      const auto recovered = veyr::lex(sources, bad_id, diagnostics);
      ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(recovered));
      EXPECT_FALSE(std::holds_alternative<std::vector<veyr::Token>>(recovered) &&
                   !diagnostics.has_errors());
    }
    const auto values = diagnostics.diagnostics();
    ASSERT_EQ(values.size(), 3u);
    EXPECT_EQ(values[0].severity, severity);
    EXPECT_EQ(values[0].message, "prior");
    EXPECT_EQ(values[0].primary, (veyr::SourceSpan{clean_id, 0, 1}));
    ASSERT_EQ(values[0].related.size(), 1u);
    EXPECT_EQ(values[0].related[0].span, (veyr::SourceSpan{bad_id, 0, 1}));
    EXPECT_EQ(values[0].related[0].label, "context");
    for (std::size_t index = 1; index < values.size(); ++index) {
      EXPECT_EQ(values[index].severity, veyr::DiagnosticSeverity::Error);
      EXPECT_EQ(values[index].message, byte_error);
      EXPECT_EQ(values[index].primary, (veyr::SourceSpan{bad_id, 0, 1}));
      EXPECT_TRUE(values[index].related.empty());
    }
  }
}

TEST(SmokeProfileLexer, UnknownSourcePreservesMixedDiagnosticsOnNonemptyManager) {
  veyr::SourceManager sources;
  const auto added = sources.register_source("<known>", "fn");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(added));
  veyr::DiagnosticEngine diagnostics;
  diagnostics.emit({veyr::DiagnosticSeverity::Warning, "warning", std::nullopt, {}});
  diagnostics.emit({veyr::DiagnosticSeverity::Error, "error", std::nullopt, {}});
  const veyr::SourceId unknown{std::numeric_limits<std::uint64_t>::max()};
  const auto checked = sources.source_text(unknown);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(checked));
  const auto result = veyr::lex(sources, unknown, diagnostics);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
  EXPECT_EQ(std::get<veyr::SourceError>(result), std::get<veyr::SourceError>(checked));
  const auto values = diagnostics.diagnostics();
  ASSERT_EQ(values.size(), 2u);
  EXPECT_EQ(values[0].severity, veyr::DiagnosticSeverity::Warning);
  EXPECT_EQ(values[0].message, "warning");
  EXPECT_EQ(values[1].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(values[1].message, "error");
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST(SmokeProfileLexer, RealLexicalErrorsRenderHostileBytesIntoStableOwnedAscii) {
  veyr::SourceManager sources;
  const auto added = sources.register_source("bad\n\x1B.vyr", std::string{"\x1B\0\xFF", 3});
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(added));
  veyr::DiagnosticEngine diagnostics;
  const auto result = veyr::lex(sources, std::get<veyr::SourceId>(added), diagnostics);
  ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
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
    const auto message = index == 2 ? high_error : byte_error;
    EXPECT_EQ(output.back(), "bad\\n\\x1B.vyr:1:" + std::to_string(index + 1) +
        ": error: " + std::string{message} + "\n1 | \\x1B\\x00\\xFF\n  | " +
        std::string(index * 4, ' ') + "^~~~\n");
    EXPECT_EQ(std::count(output.back().begin(), output.back().end(), '\n'), 3);
    for (const unsigned char byte : output.back()) {
      EXPECT_TRUE(byte == '\n' || (byte >= 0x20 && byte <= 0x7e));
    }
  }
  diagnostics.emit({veyr::DiagnosticSeverity::Note, "later", std::nullopt, {}});
  for (int index = 0; index < 1000; ++index) {
    ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(sources.register_source("bad", "changed")));
  }
  for (std::size_t index = 0; index < copies.size(); ++index) {
    const auto again = veyr::PlainTextRenderer{}.render(copies[index], sources);
    ASSERT_TRUE(std::holds_alternative<std::string>(again));
    EXPECT_EQ(std::get<std::string>(again), output[index]);
  }
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST(SmokeProfileLexer, RenderingCheckedFailureCannotClearAccumulatedLexicalError) {
  veyr::SourceManager sources;
  const auto added = sources.register_source("<bad>", "?");
  ASSERT_TRUE(std::holds_alternative<veyr::SourceId>(added));
  veyr::DiagnosticEngine diagnostics;
  const auto result = veyr::lex(sources, std::get<veyr::SourceId>(added), diagnostics);
  ASSERT_TRUE(std::holds_alternative<std::vector<veyr::Token>>(result));
  const veyr::SourceSpan invalid{{0}, 7, 2};
  const auto original_error = sources.validate(invalid);
  ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(original_error));
  const veyr::Diagnostic separate{veyr::DiagnosticSeverity::Error, "invalid reference", invalid, {}};
  const auto rendered = veyr::PlainTextRenderer{}.render(separate, sources);
  ASSERT_TRUE(std::holds_alternative<veyr::RenderError>(rendered));
  EXPECT_EQ(std::get<veyr::RenderError>(rendered).source_error,
            std::get<veyr::SourceError>(original_error));
  EXPECT_EQ(std::get<veyr::RenderError>(rendered).related_index, std::nullopt);
  const auto retained = diagnostics.diagnostics();
  ASSERT_EQ(retained.size(), 1u);
  EXPECT_EQ(retained[0].severity, veyr::DiagnosticSeverity::Error);
  EXPECT_EQ(retained[0].message, byte_error);
  EXPECT_EQ(retained[0].primary, (veyr::SourceSpan{std::get<veyr::SourceId>(added), 0, 1}));
  EXPECT_TRUE(diagnostics.has_errors());
}

}  // namespace
