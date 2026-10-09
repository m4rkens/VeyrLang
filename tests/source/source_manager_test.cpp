#include <veyr/source/source_manager.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace veyr {

class SourceManagerTestAccess {
 public:
  static std::optional<RegistrationError> capacity_error(std::size_t count,
                                                        std::size_t size) {
    return SourceManager::registration_capacity_error(count, size);
  }
};

}  // namespace veyr

namespace {

static_assert(!std::is_copy_constructible_v<veyr::SourceManager>);
static_assert(!std::is_copy_assignable_v<veyr::SourceManager>);
static_assert(!std::is_move_constructible_v<veyr::SourceManager>);
static_assert(!std::is_move_assignable_v<veyr::SourceManager>);

veyr::SourceId register_source(veyr::SourceManager& manager, std::string name,
                               std::string bytes) {
  const auto result = manager.register_source(std::move(name), std::move(bytes));
  EXPECT_TRUE(std::holds_alternative<veyr::SourceId>(result));
  return std::get<veyr::SourceId>(result);
}

std::string_view source_text(const veyr::SourceManager& manager, veyr::SourceId id) {
  return std::get<std::string_view>(manager.source_text(id));
}

std::string_view display_name(const veyr::SourceManager& manager, veyr::SourceId id) {
  return std::get<std::string_view>(manager.display_name(id));
}

TEST(SourceManager, OwnsCallerTextAndDisplayName) {
  veyr::SourceManager manager;
  veyr::SourceId id;
  {
    std::string bytes = "caller text";
    std::string name = "caller name";
    id = register_source(manager, name, bytes);
    bytes.assign("changed");
    name.assign("renamed");
    EXPECT_EQ(source_text(manager, id), "caller text");
    EXPECT_EQ(display_name(manager, id), "caller name");
  }
  EXPECT_EQ(source_text(manager, id), "caller text");
  EXPECT_EQ(display_name(manager, id), "caller name");
}

TEST(SourceManager, EveryRegistrationIsAFreshSnapshot) {
  veyr::SourceManager manager;
  const auto first = register_source(manager, "same.vyr", "same bytes");
  const auto duplicate = register_source(manager, "same.vyr", "same bytes");
  const auto newer = register_source(manager, "same.vyr", "changed bytes");
  EXPECT_NE(first, duplicate);
  EXPECT_NE(first, newer);
  EXPECT_NE(duplicate, newer);
  EXPECT_EQ(source_text(manager, first), "same bytes");
  EXPECT_EQ(source_text(manager, duplicate), "same bytes");
  EXPECT_EQ(source_text(manager, newer), "changed bytes");
  EXPECT_EQ(display_name(manager, first), display_name(manager, newer));
}

TEST(SourceManager, NumericIdsAreLocalToTheOriginatingManager) {
  veyr::SourceManager first_manager;
  veyr::SourceManager second_manager;
  const auto first = register_source(first_manager, "<memory>", "first");
  const auto second = register_source(second_manager, "<memory>", "second");
  EXPECT_EQ(first.value, 1u);
  EXPECT_EQ(first, second);
  EXPECT_EQ(source_text(first_manager, first), "first");
  EXPECT_EQ(source_text(second_manager, second), "second");
}

TEST(SourceManager, ShortLongAndEmptyViewsSurviveGrowth) {
  veyr::SourceManager manager;
  const std::string long_bytes(1024, 'x');
  const auto short_id = register_source(manager, "s", "short");
  const auto long_id = register_source(manager, std::string(1024, 'n'), long_bytes);
  const auto empty_id = register_source(manager, "", "");
  const auto short_view = source_text(manager, short_id);
  const auto long_view = source_text(manager, long_id);
  const auto empty_view = source_text(manager, empty_id);
  const auto short_name = display_name(manager, short_id);
  const auto long_name = display_name(manager, long_id);
  const auto empty_name = display_name(manager, empty_id);
  for (std::size_t index = 0; index < 3000; ++index) {
    static_cast<void>(register_source(manager, std::to_string(index),
                                     std::string(index % 3 == 0 ? 512 : index % 3, 'z')));
  }
  EXPECT_EQ(short_view, "short");
  EXPECT_EQ(long_view, long_bytes);
  EXPECT_TRUE(empty_view.empty());
  EXPECT_EQ(short_view.data(), source_text(manager, short_id).data());
  EXPECT_EQ(long_view.data(), source_text(manager, long_id).data());
  EXPECT_EQ(empty_view.data(), source_text(manager, empty_id).data());
  EXPECT_EQ(short_name, "s");
  EXPECT_EQ(long_name, std::string(1024, 'n'));
  EXPECT_TRUE(empty_name.empty());
  EXPECT_EQ(short_name.data(), display_name(manager, short_id).data());
  EXPECT_EQ(long_name.data(), display_name(manager, long_id).data());
  EXPECT_EQ(empty_name.data(), display_name(manager, empty_id).data());
}

TEST(SourceManager, ValidEmptyFullAndEofSpans) {
  veyr::SourceManager manager;
  const auto id = register_source(manager, "<stdin>", "abc");
  const veyr::SourceSpan spans[] = {{id, 0, 0}, {id, 0, 3},
                                    {id, 1, 1}, {id, 1, 2}, {id, 3, 3}};
  for (const auto span : spans) {
    EXPECT_TRUE(std::holds_alternative<std::monostate>(manager.validate(span)));
    EXPECT_EQ(std::get<std::string_view>(manager.text(span)),
              std::string_view{"abc"}.substr(span.begin, span.end - span.begin));
  }
  const auto empty = register_source(manager, "<empty>", "");
  EXPECT_TRUE(std::holds_alternative<std::monostate>(manager.validate({empty, 0, 0})));
  EXPECT_TRUE(std::get<std::string_view>(manager.text({empty, 0, 0})).empty());
}

TEST(SourceManager, DistinctFailuresRetainOriginalInputs) {
  veyr::SourceManager manager;
  const auto id = register_source(manager, "<memory>", "abc");
  const veyr::SourceSpan invalid[] = {{{0}, 7, 4}, {id, 3, 2}, {id, 1, 4}};
  const veyr::SourceError expected[] = {
      {veyr::SourceErrorKind::UnknownSource, {0}, 7, 4, std::nullopt},
      {veyr::SourceErrorKind::ReversedRange, id, 3, 2, 3},
      {veyr::SourceErrorKind::BeyondSource, id, 1, 4, 3}};
  for (std::size_t index = 0; index < 3; ++index) {
    const auto validation = manager.validate(invalid[index]);
    const auto retrieval = manager.text(invalid[index]);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(validation));
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(retrieval));
    EXPECT_EQ(std::get<veyr::SourceError>(validation), expected[index]);
    EXPECT_EQ(std::get<veyr::SourceError>(retrieval), expected[index]);
  }
}

TEST(SourceManager, EveryInBoundsByteRangePreservesOriginalData) {
  veyr::SourceManager manager;
  const std::vector<std::string> fixtures = {
      "", "x", "\r\n", "a\rb\nc\r\nd", std::string{"A\0B\t\r\n", 6},
      std::string{"\xC3\xA9\xFF", 3}};
  for (const auto& bytes : fixtures) {
    SCOPED_TRACE(testing::Message() << "source size=" << bytes.size());
    const auto id = register_source(manager, "<bytes>", bytes);
    EXPECT_EQ(source_text(manager, id), bytes);
    for (std::size_t begin = 0; begin <= bytes.size(); ++begin) {
      for (std::size_t end = begin; end <= bytes.size(); ++end) {
        const veyr::SourceSpan span{id, begin, end};
        EXPECT_TRUE(std::holds_alternative<std::monostate>(manager.validate(span)));
        const auto result = manager.text(span);
        ASSERT_TRUE(std::holds_alternative<std::string_view>(result));
        EXPECT_EQ(std::get<std::string_view>(result),
                  std::string_view{bytes}.substr(begin, end - begin));
      }
    }
  }
}

TEST(SourceManager, RangesMaySplitCrlfWithoutNormalization) {
  veyr::SourceManager manager;
  const auto id = register_source(manager, "<crlf>", "a\r\nb");
  EXPECT_EQ(std::get<std::string_view>(manager.text({id, 1, 2})), "\r");
  EXPECT_EQ(std::get<std::string_view>(manager.text({id, 2, 3})), "\n");
  EXPECT_EQ(std::get<std::string_view>(manager.text({id, 1, 3})), "\r\n");
  EXPECT_EQ(source_text(manager, id), "a\r\nb");
}

TEST(SourceManager, UnknownSourceOperationsRetainTheOffendingId) {
  veyr::SourceManager manager;
  static_cast<void>(register_source(manager, "known", "bytes"));
  const veyr::SourceId ids[] = {{0}, {2}, {std::numeric_limits<std::uint64_t>::max()}};
  for (const auto id : ids) {
    const veyr::SourceError expected{
        veyr::SourceErrorKind::UnknownSource, id, 0, 0, std::nullopt};
    for (const auto& result : {manager.source_text(id), manager.display_name(id)}) {
      ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
      EXPECT_EQ(std::get<veyr::SourceError>(result), expected);
    }
    const auto span_result = manager.text({id, 9, 3});
    const veyr::SourceError span_expected{
        veyr::SourceErrorKind::UnknownSource, id, 9, 3, std::nullopt};
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(span_result));
    EXPECT_EQ(std::get<veyr::SourceError>(span_result), span_expected);
  }
}

TEST(SourceManager, ExtremeRangesFailClosedWithDeterministicPrecedence) {
  veyr::SourceManager manager;
  const auto id = register_source(manager, "<memory>", "abc");
  const auto maximum = std::numeric_limits<std::size_t>::max();
  const veyr::SourceSpan spans[] = {
      {id, maximum, 0}, {id, maximum, maximum - 1},
      {id, 0, maximum}, {id, maximum, maximum}, {id, 4, 4}};
  for (const auto span : spans) {
    const auto expected_kind = span.begin > span.end
                                   ? veyr::SourceErrorKind::ReversedRange
                                   : veyr::SourceErrorKind::BeyondSource;
    const veyr::SourceError expected{expected_kind, id, span.begin, span.end, 3};
    const auto validation = manager.validate(span);
    const auto result = manager.text(span);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(validation));
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(result));
    EXPECT_EQ(std::get<veyr::SourceError>(validation), expected);
    EXPECT_EQ(std::get<veyr::SourceError>(result), expected);
  }
}

TEST(SourceManager, DisplayNamesRemainExactNonIdentityMetadata) {
  veyr::SourceManager manager;
  const std::vector<std::string> names = {
      R"(C:\Some Dir\..\MixedCase.vyr)", R"(\\server\Share\file.vyr)",
      "./relative/../Name With Spaces.vyr", "<memory>", "<stdin>", "<repl>",
      "<generated>", "", std::string{"a\0b\n\x1B", 5}};
  veyr::SourceId previous;
  for (const auto& name : names) {
    const auto id = register_source(manager, name, "same bytes");
    EXPECT_NE(id, previous);
    EXPECT_EQ(display_name(manager, id), name);
    EXPECT_EQ(source_text(manager, id), "same bytes");
    previous = id;
  }
}

TEST(SourceManager, RegistrationCapacityKeepsIdsAndEofColumnsRepresentable) {
  const auto maximum = std::numeric_limits<std::size_t>::max();
  EXPECT_FALSE(veyr::SourceManagerTestAccess::capacity_error(0, 0));
  EXPECT_FALSE(veyr::SourceManagerTestAccess::capacity_error(maximum - 1, maximum - 1));
  const auto id_error = veyr::SourceManagerTestAccess::capacity_error(maximum, 0);
  ASSERT_TRUE(id_error);
  EXPECT_EQ(id_error->kind, veyr::RegistrationErrorKind::IdCapacityExceeded);
  const auto source_error = veyr::SourceManagerTestAccess::capacity_error(0, maximum);
  ASSERT_TRUE(source_error);
  EXPECT_EQ(source_error->kind, veyr::RegistrationErrorKind::SourceTooLarge);
  const auto both = veyr::SourceManagerTestAccess::capacity_error(maximum, maximum);
  ASSERT_TRUE(both);
  EXPECT_EQ(both->kind, veyr::RegistrationErrorKind::IdCapacityExceeded);
  // Pure numeric boundary checks above allocate no huge string or source array.
}

TEST(SourceLocation, ApprovedNewlineAndEofBoundaryTable) {
  struct Case {
    std::string bytes;
    std::vector<std::pair<std::size_t, std::size_t>> positions;
  };
  const std::vector<Case> cases = {
      {"", {{1, 1}}},
      {"ab", {{1, 1}, {1, 2}, {1, 3}}},
      {"a\n", {{1, 1}, {1, 2}, {2, 1}}},
      {"a\r\n", {{1, 1}, {1, 2}, {1, 3}, {2, 1}}},
      {"a\r\nb", {{1, 1}, {1, 2}, {1, 3}, {2, 1}, {2, 2}}},
      {"\n\r\n", {{1, 1}, {2, 1}, {2, 2}, {3, 1}}},
      {"a\rb\r", {{1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}}}};
  veyr::SourceManager manager;
  for (const auto& item : cases) {
    SCOPED_TRACE(testing::Message() << "bytes=" << testing::PrintToString(item.bytes));
    const auto id = register_source(manager, "<boundary>", item.bytes);
    ASSERT_EQ(item.positions.size(), item.bytes.size() + 1);
    for (std::size_t offset = 0; offset <= item.bytes.size(); ++offset) {
      SCOPED_TRACE(testing::Message() << "offset=" << offset);
      const auto result = manager.location(id, offset);
      ASSERT_TRUE(std::holds_alternative<veyr::SourceLocation>(result));
      const veyr::SourceLocation expected{
          id, offset, item.positions[offset].first, item.positions[offset].second};
      EXPECT_EQ(std::get<veyr::SourceLocation>(result), expected);
    }
    EXPECT_EQ(source_text(manager, id), item.bytes);
  }
}

TEST(SourceLocation, ContainingLineSpansIncludeOriginalTerminators) {
  veyr::SourceManager manager;
  const auto id = register_source(manager, "<lines>", "a\r\nb\nc\r");
  const std::vector<veyr::SourceSpan> expected = {
      {id, 0, 3}, {id, 0, 3}, {id, 0, 3}, {id, 3, 5},
      {id, 3, 5}, {id, 5, 7}, {id, 5, 7}, {id, 5, 7}};
  for (std::size_t offset = 0; offset < expected.size(); ++offset) {
    const auto result = manager.line_span_at(id, offset);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceSpan>(result));
    EXPECT_EQ(std::get<veyr::SourceSpan>(result), expected[offset]);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(manager.validate(expected[offset])));
  }
  for (const auto& bytes : {std::string{}, std::string{"a\n"}, std::string{"a\r\n"}}) {
    const auto trailing = register_source(manager, "<trailing>", bytes);
    const auto result = manager.line_span_at(trailing, bytes.size());
    ASSERT_TRUE(std::holds_alternative<veyr::SourceSpan>(result));
    const veyr::SourceSpan empty{trailing, bytes.size(), bytes.size()};
    EXPECT_EQ(std::get<veyr::SourceSpan>(result), empty);
    EXPECT_TRUE(std::get<std::string_view>(manager.text(empty)).empty());
  }
}

TEST(SourceLocation, UnknownSourceRetainsPointInputAndTakesPrecedence) {
  veyr::SourceManager manager;
  static_cast<void>(register_source(manager, "known", "abc"));
  const veyr::SourceId ids[] = {{0}, {2}, {std::numeric_limits<std::uint64_t>::max()}};
  const std::size_t offsets[] = {0, 4, std::numeric_limits<std::size_t>::max()};
  for (const auto id : ids) {
    for (const auto offset : offsets) {
      const veyr::SourceError expected{
          veyr::SourceErrorKind::UnknownSource, id, offset, offset, std::nullopt};
      const auto location = manager.location(id, offset);
      const auto line = manager.line_span_at(id, offset);
      ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(location));
      ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(line));
      EXPECT_EQ(std::get<veyr::SourceError>(location), expected);
      EXPECT_EQ(std::get<veyr::SourceError>(line), expected);
    }
  }
}

TEST(SourceLocation, BeyondSourceRetainsExactOffsetWithoutClamping) {
  veyr::SourceManager manager;
  for (const auto& bytes : {std::string{}, std::string{"a\r\nb"}}) {
    const auto id = register_source(manager, "<bounds>", bytes);
    for (const auto offset : {bytes.size() + 1, std::numeric_limits<std::size_t>::max()}) {
      const veyr::SourceError expected{
          veyr::SourceErrorKind::BeyondSource, id, offset, offset, bytes.size()};
      const auto location = manager.location(id, offset);
      const auto line = manager.line_span_at(id, offset);
      ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(location));
      ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(line));
      EXPECT_EQ(std::get<veyr::SourceError>(location), expected);
      EXPECT_EQ(std::get<veyr::SourceError>(line), expected);
    }
  }
}

// Independent byte scan: no access to, or reconstruction of, the stored index.
void expect_byte_mapping(veyr::SourceManager& manager, const std::string& bytes) {
  SCOPED_TRACE(testing::Message() << "bytes=" << testing::PrintToString(bytes));
  const auto id = register_source(manager, "<oracle>", bytes);
  for (std::size_t offset = 0; offset <= bytes.size(); ++offset) {
    SCOPED_TRACE(testing::Message() << "offset=" << offset);
    std::size_t line = 1;
    std::size_t column = 1;
    std::size_t begin = 0;
    for (std::size_t index = 0; index < offset; ++index) {
      if (bytes[index] == '\n') {
        ++line;
        column = 1;
        begin = index + 1;
      } else {
        ++column;
      }
    }
    std::size_t end = offset;
    while (end < bytes.size()) {
      if (bytes[end++] == '\n') {
        break;
      }
    }
    const auto location = manager.location(id, offset);
    const auto span = manager.line_span_at(id, offset);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceLocation>(location));
    ASSERT_TRUE(std::holds_alternative<veyr::SourceSpan>(span));
    const veyr::SourceLocation expected_location{id, offset, line, column};
    const veyr::SourceSpan expected_span{id, begin, end};
    EXPECT_EQ(std::get<veyr::SourceLocation>(location), expected_location);
    EXPECT_EQ(std::get<veyr::SourceSpan>(span), expected_span);
    EXPECT_TRUE(std::holds_alternative<std::monostate>(manager.validate(expected_span)));
    EXPECT_EQ(std::get<std::string_view>(manager.text(expected_span)),
              std::string_view{bytes}.substr(begin, end - begin));
  }
  EXPECT_EQ(source_text(manager, id), bytes);
}

TEST(SourceLocation, MixedNewlinesTabsUtf8AndNulUseOriginalByteColumns) {
  veyr::SourceManager manager;
  const std::vector<std::string> fixtures = {
      "a\r\nb\nc\rd\n\r\n", "\r\n\n\n\r\r\n", "\t\tX\r", "\r\r",
      std::string{"\xC3\xA9\t\xF0\x9F\x98\x80\r\nZ", 10},
      std::string{"A\0B\r\n\0\t\n", 8}};
  for (const auto& bytes : fixtures) {
    expect_byte_mapping(manager, bytes);
  }
}

TEST(SourceLocation, EveryOffsetOfBoundedByteCombinationsMatchesScanOracle) {
  veyr::SourceManager manager;
  const std::string alphabet{"x\r\n\t\0\xC3\xA9", 7};
  std::size_t combinations = 1;
  std::size_t checked = 0;
  for (std::size_t length = 0; length <= 4; ++length) {
    for (std::size_t code = 0; code < combinations; ++code) {
      std::string bytes(length, 'x');
      auto remaining = code;
      for (auto& byte : bytes) {
        byte = alphabet[remaining % alphabet.size()];
        remaining /= alphabet.size();
      }
      expect_byte_mapping(manager, bytes);
      ++checked;
    }
    combinations *= alphabet.size();
  }
  EXPECT_EQ(checked, 2801u);
}

TEST(SourceLocation, IndexedSourcesPreserveExactDisplayNameBytes) {
  veyr::SourceManager manager;
  const std::vector<std::string> names = {
      R"(C:\Some Dir\..\MixedCase.vyr)", R"(c:/Some Dir/../MixedCase.vyr)",
      R"(\\server\Share\file.vyr)", "./relative/../Name With Spaces.vyr",
      "<memory>", "<stdin>", "<repl>", "<generated>", "",
      std::string{"a\0b\r\n\t\x1B", 7}};
  veyr::SourceId previous;
  for (const auto& name : names) {
    const auto id = register_source(manager, name, "a\r\nb\n");
    EXPECT_NE(id, previous);
    for (std::size_t offset = 0; offset <= 5; ++offset) {
      ASSERT_TRUE(std::holds_alternative<veyr::SourceLocation>(manager.location(id, offset)));
      ASSERT_TRUE(std::holds_alternative<veyr::SourceSpan>(manager.line_span_at(id, offset)));
      EXPECT_EQ(display_name(manager, id), name);
    }
    previous = id;
  }
}

TEST(SourceLocation, IndexedTextNameAndLineViewsSurviveFurtherRegistration) {
  veyr::SourceManager manager;
  const std::string bytes = "a\r\n" + std::string(1024, 'x') + "\n";
  const auto id = register_source(manager, R"(C:\Keep Name\..\file.vyr)", bytes);
  const auto text = source_text(manager, id);
  const auto name = display_name(manager, id);
  const auto line_span = std::get<veyr::SourceSpan>(manager.line_span_at(id, 3));
  const auto line_text = std::get<std::string_view>(manager.text(line_span));
  const auto location = std::get<veyr::SourceLocation>(manager.location(id, 3));
  for (std::size_t index = 0; index < 3000; ++index) {
    static_cast<void>(register_source(manager, std::to_string(index),
                                     index % 2 == 0 ? "\n\r\n" : std::string{"\t\0\r", 3}));
  }
  EXPECT_EQ(text, bytes);
  EXPECT_EQ(name, R"(C:\Keep Name\..\file.vyr)");
  EXPECT_EQ(text.data(), source_text(manager, id).data());
  EXPECT_EQ(name.data(), display_name(manager, id).data());
  EXPECT_EQ(line_text, std::string(1024, 'x') + "\n");
  EXPECT_EQ(line_text.data(), std::get<std::string_view>(manager.text(line_span)).data());
  EXPECT_EQ(std::get<veyr::SourceLocation>(manager.location(id, 3)), location);
  EXPECT_EQ(std::get<veyr::SourceSpan>(manager.line_span_at(id, 3)), line_span);
  const veyr::SourceLocation eof{id, bytes.size(), 3, 1};
  EXPECT_EQ(std::get<veyr::SourceLocation>(manager.location(id, bytes.size())), eof);
}

}  // namespace
