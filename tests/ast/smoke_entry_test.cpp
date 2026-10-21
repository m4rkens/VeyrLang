#include <veyr/ast/smoke_entry.hpp>
#include <veyr/source/source_manager.hpp>

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>

namespace {

using namespace veyr;

static_assert(std::is_same_v<decltype(ParsedSmokeEntry::entry_span), SourceSpan>);
static_assert(std::is_same_v<decltype(ParsedSmokeEntry::return_span), SourceSpan>);
static_assert(std::is_same_v<decltype(ParsedSmokeEntry::integer_span), SourceSpan>);
static_assert(std::is_aggregate_v<ParsedSmokeEntry>);

TEST(SmokeProfileAst, ThreeSpansHaveValueSemantics) {
  const ParsedSmokeEntry original{{{7}, 0, 33}, {{7}, 21, 31}, {{7}, 28, 30}};
  const auto [entry, statement, integer] = original;
  EXPECT_EQ(entry, original.entry_span);
  EXPECT_EQ(statement, original.return_span);
  EXPECT_EQ(integer, original.integer_span);
  auto copy = original;
  auto moved = std::move(copy);
  EXPECT_EQ(moved, original);
  copy = moved;
  moved = ParsedSmokeEntry{{{8}, 1, 34}, {{8}, 22, 32}, {{8}, 29, 31}};
  EXPECT_EQ(copy, original);
  EXPECT_NE(moved, original);
}

TEST(SmokeProfileAst, ExactFieldTypesAndEquality) {
  const ParsedSmokeEntry original{{{1}, 0, 33}, {{1}, 21, 31}, {{1}, 28, 30}};
  auto other = original;
  ++other.entry_span.begin;
  EXPECT_NE(other, original);
  other = original;
  ++other.return_span.end;
  EXPECT_NE(other, original);
  other = original;
  ++other.integer_span.source_id.value;
  EXPECT_NE(other, original);
  other = original;
  EXPECT_EQ(other, original);
}

TEST(SmokeProfileAst, CopiesResolveOriginalSnapshot) {
  SourceManager sources;
  std::string caller = "fn main() -> int32 { return 42; }";
  const auto registration = sources.register_source("same.vyr", caller);
  ASSERT_TRUE(std::holds_alternative<SourceId>(registration));
  const auto id = std::get<SourceId>(registration);
  const ParsedSmokeEntry original{{id, 0, 33}, {id, 21, 31}, {id, 28, 30}};
  auto saved = original;
  caller.assign(100, '?');
  const auto added = sources.register_source("same.vyr", caller);
  ASSERT_TRUE(std::holds_alternative<SourceId>(added));
  EXPECT_NE(std::get<SourceId>(added), id);
  EXPECT_EQ(saved, original);
  const auto entry = sources.text(saved.entry_span);
  const auto statement = sources.text(saved.return_span);
  const auto integer = sources.text(saved.integer_span);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(entry));
  ASSERT_TRUE(std::holds_alternative<std::string_view>(statement));
  ASSERT_TRUE(std::holds_alternative<std::string_view>(integer));
  EXPECT_EQ(std::get<std::string_view>(entry), "fn main() -> int32 { return 42; }");
  EXPECT_EQ(std::get<std::string_view>(statement), "return 42;");
  EXPECT_EQ(std::get<std::string_view>(integer), "42");
}

TEST(SmokeProfileAst, HeaderIsIndependentOfParserTokens) {
  // This translation unit includes no parser, token, lexer or semantic header.
  constexpr ParsedSmokeEntry value{{{9}, 0, 33}, {{9}, 21, 31}, {{9}, 28, 30}};
  static_assert(std::is_trivially_copyable_v<ParsedSmokeEntry>);
  static_assert(value.integer_span.begin == 28);
  EXPECT_EQ(value.entry_span.source_id, SourceId{9});
  EXPECT_EQ(value.return_span, (SourceSpan{{9}, 21, 31}));
}

}  // namespace
