#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/diagnostics/plain_text_renderer.hpp>

#include <gtest/gtest.h>

#include <algorithm>
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

using veyr::Diagnostic;
using veyr::DiagnosticEngine;
using veyr::DiagnosticSeverity;
using veyr::PlainTextRenderer;
using veyr::RenderError;
using veyr::SourceError;
using veyr::SourceErrorKind;
using veyr::SourceId;
using veyr::SourceManager;
using veyr::SourceSpan;

static_assert(std::is_same_v<veyr::RenderResult, std::variant<std::string, RenderError>>);

SourceId add(SourceManager& sources, std::string name, std::string text) {
  return std::get<SourceId>(sources.register_source(std::move(name), std::move(text)));
}

std::string render(const SourceManager& sources, Diagnostic diagnostic) {
  const auto result = PlainTextRenderer{}.render(diagnostic, sources);
  if (!std::holds_alternative<std::string>(result)) {
    ADD_FAILURE() << "Unexpected checked render failure";
    return {};
  }
  return std::get<std::string>(result);
}

std::string primary(const SourceManager& sources, SourceId id,
                    std::size_t begin, std::size_t end) {
  return render(sources, {DiagnosticSeverity::Error, "bad", SourceSpan{id, begin, end}, {}});
}

void expect_error(const SourceManager& sources, const Diagnostic& diagnostic,
                  SourceError expected, std::optional<std::size_t> related_index) {
  const auto result = PlainTextRenderer{}.render(diagnostic, sources);
  ASSERT_TRUE(std::holds_alternative<RenderError>(result));
  const auto& error = std::get<RenderError>(result);
  EXPECT_EQ(error.source_error, expected);
  EXPECT_EQ(error.related_index, related_index);
}

TEST(PlainTextDiagnostic, LocationFreeSeveritiesHaveOnlyAnOwnedMessageHeader) {
  const SourceManager sources;
  const std::array severities = {DiagnosticSeverity::Error, DiagnosticSeverity::Warning,
                                  DiagnosticSeverity::Note};
  const std::array words = {"error", "warning", "note"};
  for (std::size_t index = 0; index < severities.size(); ++index) {
    EXPECT_EQ(render(sources, {severities[index], "message", std::nullopt, {}}),
              std::string(words[index]) + ": message\n");
  }
  EXPECT_EQ(render(sources, {DiagnosticSeverity::Note, "", std::nullopt, {}}), "note: \n");
}

TEST(PlainTextDiagnostic, PrimaryHeaderAndSingleLineUnderlineAreExact) {
  SourceManager sources;
  const auto id = add(sources, "<memory>", "abcdef");
  EXPECT_EQ(primary(sources, id, 1, 4),
            "<memory>:1:2: error: bad\n1 | abcdef\n  |  ^~~\n");
  EXPECT_EQ(render(sources, {DiagnosticSeverity::Warning, "warn", SourceSpan{id, 0, 1}, {}}),
            "<memory>:1:1: warning: warn\n1 | abcdef\n  | ^\n");
  EXPECT_EQ(render(sources, {DiagnosticSeverity::Note, "hint", SourceSpan{id, 5, 6}, {}}),
            "<memory>:1:6: note: hint\n1 | abcdef\n  |      ^\n");
}

TEST(PlainTextDiagnostic, RelatedLocationsKeepCrossSourceInsertionOrderAndDuplicates) {
  SourceManager sources;
  const auto first = add(sources, "first", "a\nb");
  const auto second = add(sources, "second", "xyz");
  EXPECT_EQ(render(sources, {DiagnosticSeverity::Error, "bad", SourceSpan{first, 2, 3},
                            {{SourceSpan{second, 1, 3}, "other"},
                             {SourceSpan{first, 0, 0}, "start"},
                             {SourceSpan{second, 1, 3}, "other"}}}),
            "first:2:1: error: bad\n2 | b\n  | ^\n"
            "note: other (second:1:2)\nnote: start (first:1:1)\nnote: other (second:1:2)\n");
}

TEST(PlainTextDiagnostic, RelatedOnlyContextDoesNotFabricateAPrimaryOrExcerpt) {
  SourceManager sources;
  const auto id = add(sources, "context", "abc");
  EXPECT_EQ(render(sources, {DiagnosticSeverity::Note, "info", std::nullopt,
                            {{SourceSpan{id, 3, 3}, "end"}, {SourceSpan{id, 0, 1}, ""}}}),
            "note: info\nnote: end (context:1:4)\nnote:  (context:1:1)\n");
}

TEST(PlainTextDiagnostic, EmptySourceAndTrailingBreakEOFHaveOneBoundaryCaret) {
  SourceManager sources;
  for (const auto& bytes : {std::string{}, std::string{"\n"}, std::string{"\r\n"}}) {
    const auto id = add(sources, "empty", bytes);
    const auto line = bytes.empty() ? "1" : "2";
    EXPECT_EQ(primary(sources, id, bytes.size(), bytes.size()),
              std::string("empty:") + line + ":1: error: bad\n" + line + " | \n  | ^\n");
  }
}

TEST(PlainTextDiagnostic, InteriorAndNonTrailingEOFUseExactByteBoundaries) {
  SourceManager sources;
  const auto id = add(sources, "x", "abc");
  EXPECT_EQ(primary(sources, id, 0, 0), "x:1:1: error: bad\n1 | abc\n  | ^\n");
  EXPECT_EQ(primary(sources, id, 1, 1), "x:1:2: error: bad\n1 | abc\n  |  ^\n");
  EXPECT_EQ(primary(sources, id, 3, 3), "x:1:4: error: bad\n1 | abc\n  |    ^\n");
}

TEST(PlainTextDiagnostic, LFByteBelongsToPrecedingLineAndEscapesIntoTwoCells) {
  SourceManager sources;
  const auto id = add(sources, "lf", "a\nb");
  EXPECT_EQ(primary(sources, id, 1, 2), "lf:1:2: error: bad\n1 | a\\n\n  |  ^~\n");
  EXPECT_EQ(primary(sources, id, 2, 3), "lf:2:1: error: bad\n2 | b\n  | ^\n");
}

TEST(PlainTextDiagnostic, EitherCRLFByteAndSplitRangesUseSeparateByteCells) {
  SourceManager sources;
  const auto id = add(sources, "crlf", "a\r\nb");
  EXPECT_EQ(primary(sources, id, 1, 2),
            "crlf:1:2: error: bad\n1 | a\\r\\n\n  |  ^~\n");
  EXPECT_EQ(primary(sources, id, 2, 3),
            "crlf:1:3: error: bad\n1 | a\\r\\n\n  |    ^~\n");
  EXPECT_EQ(primary(sources, id, 1, 3),
            "crlf:1:2: error: bad\n1 | a\\r\\n\n  |  ^~~~\n");
  EXPECT_EQ(primary(sources, id, 2, 2),
            "crlf:1:3: error: bad\n1 | a\\r\\n\n  |    ^\n");
  EXPECT_EQ(primary(sources, id, 3, 4), "crlf:2:1: error: bad\n2 | b\n  | ^\n");
}

TEST(PlainTextDiagnostic, LoneCRIsAnOrdinaryByteOnTheSameLine) {
  SourceManager sources;
  const auto id = add(sources, "cr", "a\rb");
  EXPECT_EQ(primary(sources, id, 1, 3), "cr:1:2: error: bad\n1 | a\\rb\n  |  ^~~\n");
  EXPECT_EQ(primary(sources, id, 3, 3), "cr:1:4: error: bad\n1 | a\\rb\n  |     ^\n");
}

TEST(PlainTextDiagnostic, MultilineSpanExcludesItsEndAtTheNextLineStart) {
  SourceManager sources;
  const auto id = add(sources, "lines", "ab\ncd\r\nef");
  EXPECT_EQ(primary(sources, id, 1, 3),
            "lines:1:2: error: bad\n1 | ab\\n\n  |  ^~~\n");
  EXPECT_EQ(primary(sources, id, 1, 7),
            "lines:1:2: error: bad\n1 | ab\\n\n  |  ^~~\n"
            "2 | cd\\r\\n\n  | ^~~~~~\n");
  EXPECT_EQ(primary(sources, id, 1, 8),
            "lines:1:2: error: bad\n1 | ab\\n\n  |  ^~~\n"
            "2 | cd\\r\\n\n  | ^~~~~~\n3 | ef\n  | ^\n");
}

TEST(PlainTextDiagnostic, TabAndUTF8BytesRemainByteColumnsWithExpandedMarkers) {
  SourceManager sources;
  const auto id = add(sources, "bytes", "a\t\xC3\xA9z");
  EXPECT_EQ(primary(sources, id, 1, 4),
            "bytes:1:2: error: bad\n1 | a\\t\\xC3\\xA9z\n  |  ^~~~~~~~~~\n");
  EXPECT_EQ(primary(sources, id, 4, 5),
            "bytes:1:5: error: bad\n1 | a\\t\\xC3\\xA9z\n  |            ^\n");
}

TEST(PlainTextDiagnostic, HostileMessageNameLabelAndExcerptAreEscapedIndependently) {
  SourceManager sources;
  const std::string bytes{"\0\t\r\n\x1B\x7F\x80\xFF", 8};
  const auto id = add(sources, "name\n\x1B", bytes);
  EXPECT_EQ(render(sources, {DiagnosticSeverity::Error, bytes, SourceSpan{id, 0, 4},
                            {{SourceSpan{id, 4, 8}, bytes}}}),
            "name\\n\\x1B:1:1: error: \\x00\\t\\r\\n\\x1B\\x7F\\x80\\xFF\n"
            "1 | \\x00\\t\\r\\n\n  | ^~~~~~~~~~\n"
            "note: \\x00\\t\\r\\n\\x1B\\x7F\\x80\\xFF (name\\n\\x1B:2:1)\n");
}

TEST(PlainTextDiagnostic, WindowsUNCRelativeAndSyntheticNamesPreserveCallerSpelling) {
  SourceManager sources;
  for (const auto& name : {std::string{R"(C:\Mixed Case\..\file.vyr)"},
                           std::string{R"(\\Server\Share\a.vyr)"},
                           std::string{"./a/../b.vyr"}, std::string{"<stdin>"}}) {
    const auto id = add(sources, name, "x");
    EXPECT_EQ(primary(sources, id, 0, 1), name + ":1:1: error: bad\n1 | x\n  | ^\n");
    EXPECT_EQ(std::get<std::string_view>(sources.display_name(id)), name);
  }
}

TEST(PlainTextDiagnostic, EveryInputByteProducesOnlySafeASCIIAndLFSeparators) {
  SourceManager sources;
  for (unsigned int value = 0; value <= 255; ++value) {
    SCOPED_TRACE(value);
    const std::string byte(1, static_cast<char>(value));
    const auto id = add(sources, byte, byte);
    const auto text = render(sources, {DiagnosticSeverity::Note, byte, SourceSpan{id, 0, 1},
                                       {{SourceSpan{id, 0, 1}, byte}}});
    for (const unsigned char output : text) {
      EXPECT_TRUE(output == '\n' || (output >= 0x20 && output <= 0x7e));
    }
    EXPECT_EQ(std::count(text.begin(), text.end(), '\n'), 4);
    EXPECT_EQ(std::get<std::string_view>(sources.source_text(id)), byte);
    EXPECT_EQ(std::get<std::string_view>(sources.display_name(id)), byte);
  }
}

TEST(PlainTextDiagnostic, HugeLineClipsBothEdgesWithoutClampingTheHeader) {
  SourceManager sources;
  const auto id = add(sources, "huge", std::string(1'000'000, 'X'));
  EXPECT_EQ(primary(sources, id, 500'000, 900'000),
            "huge:1:500001: error: bad\n1 | ..." + std::string(80, 'X') + "...\n  | " +
            std::string(43, ' ') + '^' + std::string(39, '~') + '\n');
}

TEST(PlainTextDiagnostic, ClippedEndAndZeroWidthWindowsRetainTrueEOFAndLocation) {
  SourceManager sources;
  const auto id = add(sources, "huge", std::string(1'000'000, 'X'));
  EXPECT_EQ(primary(sources, id, 999'990, 1'000'000),
            "huge:1:999991: error: bad\n1 | ..." + std::string(50, 'X') + "\n  | " +
            std::string(43, ' ') + '^' + std::string(9, '~') + '\n');
  EXPECT_EQ(primary(sources, id, 500'000, 500'000),
            "huge:1:500001: error: bad\n1 | ..." + std::string(80, 'X') + "...\n  | " +
            std::string(43, ' ') + "^\n");
  EXPECT_EQ(primary(sources, id, 1'000'000, 1'000'000),
            "huge:1:1000001: error: bad\n1 | ..." + std::string(40, 'X') + "\n  | " +
            std::string(43, ' ') + "^\n");
}

TEST(PlainTextDiagnostic, WindowStartsAtFortyByteContextAndEightyByteLimit) {
  SourceManager sources;
  const auto id = add(sources, "x", std::string(81, 'x'));
  EXPECT_EQ(primary(sources, id, 40, 41),
            "x:1:41: error: bad\n1 | " + std::string(80, 'x') + "...\n  | " +
            std::string(40, ' ') + "^\n");
  EXPECT_EQ(primary(sources, id, 41, 42),
            "x:1:42: error: bad\n1 | ..." + std::string(80, 'x') + "\n  | " +
            std::string(43, ' ') + "^\n");
}

TEST(PlainTextDiagnostic, ManyLineSpanShowsThreeSourceRowsAndStandaloneClipping) {
  SourceManager sources;
  std::string bytes;
  for (std::size_t index = 0; index < 100'000; ++index) bytes += "a\n";
  const auto id = add(sources, "many", bytes);
  EXPECT_EQ(primary(sources, id, 0, bytes.size()),
            "many:1:1: error: bad\n1 | a\\n\n  | ^~~\n"
            "2 | a\\n\n  | ^~~\n3 | a\\n\n  | ^~~\n...\n");
}

TEST(PlainTextDiagnostic, ThreeLineEndBoundaryDoesNotAddFalseVerticalClipping) {
  SourceManager sources;
  const auto id = add(sources, "x", "a\nb\nc\nd\n");
  EXPECT_EQ(primary(sources, id, 0, 6),
            "x:1:1: error: bad\n1 | a\\n\n  | ^~~\n"
            "2 | b\\n\n  | ^~~\n3 | c\\n\n  | ^~~\n");
}

TEST(PlainTextDiagnostic, MultidigitGuttersAndSubsequentLineWindowsAreExact) {
  SourceManager sources;
  const auto id = add(sources, "x", std::string(9, '\n') + "a\nbc");
  EXPECT_EQ(primary(sources, id, 9, 12),
            "x:10:1: error: bad\n10 | a\\n\n   | ^~~\n11 | bc\n   | ^\n");
  const auto wide = add(sources, "wide", "a\n" + std::string(100, 'b'));
  EXPECT_EQ(primary(sources, wide, 1, 102),
            "wide:1:2: error: bad\n1 | a\\n\n  |  ^~\n2 | " + std::string(80, 'b') +
            "...\n  | ^" + std::string(79, '~') + '\n');
}

TEST(PlainTextDiagnostic, EscapedContentAndMarkersAreIndependentlyBounded) {
  SourceManager sources;
  const std::string line(100'000, static_cast<char>(0xff));
  const auto id = add(sources, "bound", line + '\n' + line + '\n' + line + '\n' + line);
  const auto text = primary(sources, id, 0, 400'003);
  std::size_t rows = 0;
  std::size_t start = text.find('\n') + 1;
  while (start < text.size()) {
    const auto end = text.find('\n', start);
    const auto row = std::string_view(text).substr(start, end - start);
    const auto gutter = row.find(" | ");
    if (gutter != std::string_view::npos) {
      const auto content = row.substr(gutter + 3);
      if (row.front() >= '0' && row.front() <= '9') {
        ++rows;
        ASSERT_TRUE(content.ends_with("..."));
        const auto escaped = content.substr(0, content.size() - 3);
        EXPECT_LE(escaped.size(), 320u);
        ASSERT_EQ(escaped.size() % 4, 0u);
        EXPECT_LE(escaped.size() / 4, 80u);
        for (std::size_t index = 0; index < escaped.size(); index += 4) {
          EXPECT_EQ(escaped.substr(index, 4), "\\xFF");
        }
      } else {
        EXPECT_LE(content.size(), 323u);
        EXPECT_EQ(std::count(content.begin(), content.end(), '^'), 1);
        EXPECT_LE(std::count(content.begin(), content.end(), '~'), 319);
      }
    }
    start = end + 1;
  }
  EXPECT_EQ(rows, 3u);
  EXPECT_LT(text.size(), 2100u);  // Independent of the 400k-byte highlighted span.
}

TEST(PlainTextDiagnostic, InvalidPrimaryPreservesAllCheckedErrorKindsAndInputFields) {
  SourceManager sources;
  const auto id = add(sources, "x", "abc");
  const std::array spans = {SourceSpan{{0}, 9, 1}, SourceSpan{id, 2, 1}, SourceSpan{id, 0, 4}};
  const std::array errors = {
      SourceError{SourceErrorKind::UnknownSource, {0}, 9, 1, std::nullopt},
      SourceError{SourceErrorKind::ReversedRange, id, 2, 1, 3},
      SourceError{SourceErrorKind::BeyondSource, id, 0, 4, 3}};
  for (std::size_t index = 0; index < spans.size(); ++index) {
    expect_error(sources, {DiagnosticSeverity::Error, "bad", spans[index], {}},
                 errors[index], std::nullopt);
  }
}

TEST(PlainTextDiagnostic, InvalidRelatedPreservesErrorAndIndexWithoutPartialSuccess) {
  SourceManager sources;
  const auto id = add(sources, "x", "abc");
  const std::array spans = {SourceSpan{{999}, 1, 7}, SourceSpan{id, 3, 2}, SourceSpan{id, 2, 4}};
  const std::array errors = {
      SourceError{SourceErrorKind::UnknownSource, {999}, 1, 7, std::nullopt},
      SourceError{SourceErrorKind::ReversedRange, id, 3, 2, 3},
      SourceError{SourceErrorKind::BeyondSource, id, 2, 4, 3}};
  for (std::size_t index = 0; index < spans.size(); ++index) {
    for (const bool with_primary : {false, true}) {
      Diagnostic diagnostic{DiagnosticSeverity::Error, "bad", std::nullopt,
                            {{SourceSpan{id, 0, 1}, "valid"}, {spans[index], "invalid"},
                             {SourceSpan{{0}, 0, 0}, "later invalid"}}};
      if (with_primary) diagnostic.primary = SourceSpan{id, 0, 1};
      expect_error(sources, diagnostic, errors[index], 1);
    }
  }
}

TEST(PlainTextDiagnostic, PrimaryFailurePrecedesRelatedAndFirstRelatedFailureWins) {
  SourceManager sources;
  const auto id = add(sources, "x", "abc");
  Diagnostic diagnostic{DiagnosticSeverity::Error, "bad", SourceSpan{id, 2, 1},
                        {{SourceSpan{id, 0, 4}, "first"}, {SourceSpan{{0}, 0, 0}, "second"}}};
  expect_error(sources, diagnostic, {SourceErrorKind::ReversedRange, id, 2, 1, 3},
               std::nullopt);
  diagnostic.primary.reset();
  expect_error(sources, diagnostic, {SourceErrorKind::BeyondSource, id, 0, 4, 3}, 0);
}

TEST(PlainTextDiagnostic, RenderedStringOwnsItsBytesAfterAllInputsAreDestroyed) {
  veyr::RenderResult result;
  {
    SourceManager sources;
    const auto id = add(sources, "owned", "abc");
    Diagnostic diagnostic{DiagnosticSeverity::Warning, "message", SourceSpan{id, 1, 2}, {}};
    result = PlainTextRenderer{}.render(diagnostic, sources);
    diagnostic.message.assign("changed");
    // Registering more sources cannot invalidate the snapshot used for rendering.
    for (std::size_t index = 0; index < 128; ++index) add(sources, "other", "different");
    EXPECT_EQ(std::get<std::string_view>(sources.source_text(id)), "abc");
  }
  ASSERT_TRUE(std::holds_alternative<std::string>(result));
  EXPECT_EQ(std::get<std::string>(result),
            "owned:1:2: warning: message\n1 | abc\n  |  ^\n");
}

TEST(PlainTextDiagnostic, RenderingPreservesExactSourceBytesAndDisplayMetadata) {
  SourceManager sources;
  const std::string bytes{"a\0\r\n\t\x80z", 7};
  const std::string name{"C:\\a\r\n\x1B", 7};
  const auto id = add(sources, name, bytes);
  const auto view = std::get<std::string_view>(sources.source_text(id));
  const auto name_view = std::get<std::string_view>(sources.display_name(id));
  const auto text = primary(sources, id, 1, 7);
  EXPECT_FALSE(text.empty());
  EXPECT_EQ(view, bytes);
  EXPECT_EQ(name_view, name);
  EXPECT_EQ(std::get<std::string_view>(sources.text({id, 1, 3})), std::string_view(bytes).substr(1, 2));
}

TEST(PlainTextDiagnostic, EngineIntegrationKeepsOrderDuplicatesAndErrorsAfterRenderFailure) {
  SourceManager sources;
  const auto first = add(sources, "same", "old");
  const auto second = add(sources, "same", "new");
  DiagnosticEngine engine;
  engine.emit({DiagnosticSeverity::Note, "no source", std::nullopt, {}});
  engine.emit({DiagnosticSeverity::Warning, "new", SourceSpan{second, 0, 3},
               {{SourceSpan{first, 0, 3}, "old snapshot"}}});
  const Diagnostic repeated{DiagnosticSeverity::Error, "old", SourceSpan{first, 0, 3}, {}};
  engine.emit(repeated);
  engine.emit(repeated);
  engine.emit({DiagnosticSeverity::Error, "invalid", SourceSpan{first, 0, 4}, {}});
  std::vector<std::string> outputs;
  {
    // No emit while consuming this view; the renderer stores none of it.
    const auto diagnostics = engine.diagnostics();
    ASSERT_EQ(diagnostics.size(), 5u);
    for (std::size_t index = 0; index < 4; ++index) {
      outputs.push_back(render(sources, diagnostics[index]));
    }
    expect_error(sources, diagnostics[4],
                 {SourceErrorKind::BeyondSource, first, 0, 4, 3}, std::nullopt);
  }
  EXPECT_EQ(outputs[0], "note: no source\n");
  EXPECT_EQ(outputs[1], "same:1:1: warning: new\n1 | new\n  | ^~~\n"
                        "note: old snapshot (same:1:1)\n");
  EXPECT_EQ(outputs[2], "same:1:1: error: old\n1 | old\n  | ^~~\n");
  EXPECT_EQ(outputs[3], outputs[2]);
  EXPECT_TRUE(engine.has_errors());
  engine.emit({DiagnosticSeverity::Note, "after rendering", std::nullopt, {}});
  EXPECT_EQ(engine.diagnostics().size(), 6u);
  EXPECT_EQ(outputs[0], "note: no source\n");
  EXPECT_TRUE(engine.has_errors());
}

}  // namespace
