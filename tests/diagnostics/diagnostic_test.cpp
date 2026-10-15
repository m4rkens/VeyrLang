#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/source/source_manager.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <optional>
#include <span>
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
using veyr::RelatedSpan;
using veyr::SourceSpan;

static_assert(std::is_same_v<decltype(Diagnostic::message), std::string>);
static_assert(std::is_same_v<decltype(Diagnostic::primary), std::optional<SourceSpan>>);
static_assert(std::is_same_v<decltype(Diagnostic::related), std::vector<RelatedSpan>>);
static_assert(std::is_same_v<decltype(RelatedSpan::label), std::string>);
static_assert(std::is_same_v<decltype(std::declval<const DiagnosticEngine&>().diagnostics()),
                             std::span<const Diagnostic>>);
static_assert(!std::is_copy_constructible_v<DiagnosticEngine>);
static_assert(!std::is_copy_assignable_v<DiagnosticEngine>);
static_assert(!std::is_move_constructible_v<DiagnosticEngine>);
static_assert(!std::is_move_assignable_v<DiagnosticEngine>);

TEST(Diagnostic, EmptyEngineHasNoErrorsOrFabricatedDiagnostic) {
  const DiagnosticEngine engine;
  EXPECT_TRUE(engine.diagnostics().empty());
  EXPECT_FALSE(engine.has_errors());
}

TEST(Diagnostic, LocationFreeSeveritiesHaveIndependentErrorSemantics) {
  for (const auto severity : {DiagnosticSeverity::Warning, DiagnosticSeverity::Note,
                              DiagnosticSeverity::Error}) {
    DiagnosticEngine engine;
    engine.emit({severity, "location-free", std::nullopt, {}});
    const auto values = engine.diagnostics();
    ASSERT_EQ(values.size(), 1u);
    EXPECT_EQ(values[0].severity, severity);
    EXPECT_EQ(values[0].message, "location-free");
    EXPECT_FALSE(values[0].primary);
    EXPECT_TRUE(values[0].related.empty());
    EXPECT_EQ(engine.has_errors(), severity == DiagnosticSeverity::Error);
  }
}

TEST(Diagnostic, PrimarySpanIsRetainedWithoutSourceLookup) {
  DiagnosticEngine engine;
  const SourceSpan primary{{17}, 3, 8};
  engine.emit({DiagnosticSeverity::Error, "source-linked", primary, {}});
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 1u);
  ASSERT_TRUE(values[0].primary);
  EXPECT_EQ(*values[0].primary, primary);
  EXPECT_TRUE(values[0].related.empty());
  EXPECT_TRUE(engine.has_errors());
}

TEST(Diagnostic, RelatedSpansRetainLabelsAndCrossSourceOrder) {
  veyr::SourceManager sources;
  const auto first = std::get<veyr::SourceId>(sources.register_source("a", "abc"));
  const auto second = std::get<veyr::SourceId>(sources.register_source("b", "xyz"));
  DiagnosticEngine engine;
  const SourceSpan primary{first, 0, 1};
  const SourceSpan other{second, 1, 3};
  const SourceSpan same{first, 2, 3};
  engine.emit({DiagnosticSeverity::Note, "context", primary,
               {{other, "other source"}, {same, "same source"}}});
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 1u);
  EXPECT_EQ(values[0].primary, primary);
  ASSERT_EQ(values[0].related.size(), 2u);
  EXPECT_EQ(values[0].related[0].span, other);
  EXPECT_EQ(values[0].related[0].label, "other source");
  EXPECT_EQ(values[0].related[1].span, same);
  EXPECT_EQ(values[0].related[1].label, "same source");
  EXPECT_EQ(std::get<std::string_view>(sources.text(values[0].related[0].span)), "yz");
  EXPECT_FALSE(engine.has_errors());
}

TEST(Diagnostic, EmitOwnsMessageAndRelatedLabels) {
  DiagnosticEngine engine;
  {
    Diagnostic caller{DiagnosticSeverity::Warning, "message", std::nullopt,
                      {{{{1}, 0, 0}, "label"}}};
    engine.emit(caller);
    caller.message.assign("changed");
    caller.related[0].label.assign("renamed");
    caller.related.clear();
  }
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 1u);
  EXPECT_EQ(values[0].message, "message");
  ASSERT_EQ(values[0].related.size(), 1u);
  EXPECT_EQ(values[0].related[0].label, "label");
  EXPECT_FALSE(engine.has_errors());
}

TEST(Diagnostic, ErrorStatePersistsAfterWarningsAndNotes) {
  DiagnosticEngine engine;
  engine.emit({DiagnosticSeverity::Warning, "warning", std::nullopt, {}});
  engine.emit({DiagnosticSeverity::Note, "note", std::nullopt, {}});
  EXPECT_FALSE(engine.has_errors());
  engine.emit({DiagnosticSeverity::Error, "error", std::nullopt, {}});
  EXPECT_TRUE(engine.has_errors());
  engine.emit({DiagnosticSeverity::Note, "later note", std::nullopt, {}});
  engine.emit({DiagnosticSeverity::Warning, "later warning", std::nullopt, {}});
  EXPECT_TRUE(engine.has_errors());
  EXPECT_EQ(engine.diagnostics().size(), 5u);
}

void expect_diagnostic(const Diagnostic& actual, const Diagnostic& expected) {
  EXPECT_EQ(actual.severity, expected.severity);
  EXPECT_EQ(actual.message, expected.message);
  EXPECT_EQ(actual.primary, expected.primary);
  ASSERT_EQ(actual.related.size(), expected.related.size());
  for (std::size_t index = 0; index < expected.related.size(); ++index) {
    EXPECT_EQ(actual.related[index].span, expected.related[index].span);
    EXPECT_EQ(actual.related[index].label, expected.related[index].label);
  }
}

TEST(Diagnostic, MixedSeveritiesRemainInEmissionRatherThanSourceOrder) {
  DiagnosticEngine engine;
  const std::vector<Diagnostic> emitted = {
      {DiagnosticSeverity::Note, "later source", SourceSpan{{2}, 8, 9}, {}},
      {DiagnosticSeverity::Warning, "earlier source", SourceSpan{{1}, 0, 1}, {}},
      {DiagnosticSeverity::Error, "earlier offset", SourceSpan{{2}, 1, 2}, {}},
      {DiagnosticSeverity::Note, "no location", std::nullopt, {}}};
  for (const auto& diagnostic : emitted) {
    engine.emit(diagnostic);
  }
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), emitted.size());
  for (std::size_t index = 0; index < emitted.size(); ++index) {
    expect_diagnostic(values[index], emitted[index]);
  }
  EXPECT_TRUE(engine.has_errors());
}

TEST(Diagnostic, IdenticalDiagnosticsAndRelatedSpansAreNotDeduplicated) {
  DiagnosticEngine engine;
  const Diagnostic repeated{DiagnosticSeverity::Error, "repeat", SourceSpan{{1}, 0, 1},
                            {{{{2}, 1, 2}, "context"}, {{{2}, 1, 2}, "context"}}};
  engine.emit(repeated);
  engine.emit({DiagnosticSeverity::Warning, "between", std::nullopt, {}});
  engine.emit(repeated);
  engine.emit(repeated);
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 4u);
  expect_diagnostic(values[0], repeated);
  EXPECT_EQ(values[1].message, "between");
  expect_diagnostic(values[2], repeated);
  expect_diagnostic(values[3], repeated);
  EXPECT_TRUE(engine.has_errors());
}

TEST(Diagnostic, LongCallerStorageCanChangeAndDieAfterEmission) {
  DiagnosticEngine engine;
  const std::string expected_message(1024, 'm');
  const std::string expected_label(1024, 'l');
  {
    std::string message = expected_message;
    std::string label = expected_label;
    Diagnostic caller{DiagnosticSeverity::Warning, message, SourceSpan{{1}, 0, 0},
                      {{{{1}, 0, 0}, label}, {{{2}, 0, 1}, "second"}}};
    engine.emit(caller);
    message.assign("external changed");
    label.clear();
    caller.message.assign("diagnostic changed");
    caller.primary.reset();
    caller.related[0].label.assign("diagnostic label changed");
    std::swap(caller.related[0], caller.related[1]);
    caller.related.clear();
  }
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 1u);
  EXPECT_EQ(values[0].message, expected_message);
  ASSERT_TRUE(values[0].primary);
  EXPECT_EQ(values[0].primary->source_id.value, 1u);
  ASSERT_EQ(values[0].related.size(), 2u);
  EXPECT_EQ(values[0].related[0].label, expected_label);
  EXPECT_EQ(values[0].related[0].span.source_id.value, 1u);
  EXPECT_EQ(values[0].related[1].label, "second");
  EXPECT_FALSE(engine.has_errors());
}

TEST(Diagnostic, MovedOwnedInputRemainsIndependentOfReusedCaller) {
  DiagnosticEngine engine;
  const Diagnostic expected{DiagnosticSeverity::Error, "owned", std::nullopt,
                            {{{{1}, 0, 0}, "owned label"}}};
  Diagnostic caller = expected;
  engine.emit(std::move(caller));
  caller.message.assign("reused");
  caller.related = {{{{2}, 7, 9}, "replacement"}};
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 1u);
  expect_diagnostic(values[0], expected);
  EXPECT_TRUE(engine.has_errors());
}

TEST(Diagnostic, CollectionViewsAreReadOnlyAndReacquiredAfterEmission) {
  DiagnosticEngine engine;
  {
    const auto empty = engine.diagnostics();
    EXPECT_TRUE(empty.empty());
  }
  constexpr std::size_t count = 4096;
  for (std::size_t index = 0; index < count; ++index) {
    const auto severity = index == 511 ? DiagnosticSeverity::Error
                                       : DiagnosticSeverity::Note;
    engine.emit({severity, std::to_string(index), std::nullopt, {}});
    // This view leaves scope before the next emit; no stale span is dereferenced.
    const auto current = engine.diagnostics();
    ASSERT_EQ(current.size(), index + 1);
    EXPECT_EQ(current.back().message, std::to_string(index));
    EXPECT_EQ(current.back().severity, severity);
  }
  const auto values = std::as_const(engine).diagnostics();
  ASSERT_EQ(values.size(), count);
  for (std::size_t index = 0; index < count; ++index) {
    EXPECT_EQ(values[index].message, std::to_string(index));
    EXPECT_FALSE(values[index].primary);
    EXPECT_TRUE(values[index].related.empty());
  }
  EXPECT_TRUE(engine.has_errors());
}

TEST(Diagnostic, InvalidReferencesRemainExactForCheckedConsumers) {
  veyr::SourceManager sources;
  const auto id = std::get<veyr::SourceId>(sources.register_source("<memory>", "abc"));
  const std::vector<SourceSpan> invalid = {{{0}, 9, 1}, {id, 2, 1}, {id, 0, 4}};
  const std::vector<veyr::SourceErrorKind> kinds = {
      veyr::SourceErrorKind::UnknownSource, veyr::SourceErrorKind::ReversedRange,
      veyr::SourceErrorKind::BeyondSource};
  DiagnosticEngine engine;
  for (const auto span : invalid) {
    engine.emit({DiagnosticSeverity::Error, "original error", span, {{span, "context"}}});
  }
  ASSERT_TRUE(engine.has_errors());
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), invalid.size());
  for (std::size_t index = 0; index < invalid.size(); ++index) {
    ASSERT_TRUE(values[index].primary);
    EXPECT_EQ(*values[index].primary, invalid[index]);
    ASSERT_EQ(values[index].related.size(), 1u);
    EXPECT_EQ(values[index].related[0].span, invalid[index]);
    EXPECT_EQ(values[index].message, "original error");
    const auto checked = sources.validate(*values[index].primary);
    ASSERT_TRUE(std::holds_alternative<veyr::SourceError>(checked));
    const auto error = std::get<veyr::SourceError>(checked);
    EXPECT_EQ(error.kind, kinds[index]);
    EXPECT_EQ(error.source_id, invalid[index].source_id);
    EXPECT_EQ(error.begin, invalid[index].begin);
    EXPECT_EQ(error.end, invalid[index].end);
  }
  // Source resolution failure cannot erase or replace accumulated Error state.
  EXPECT_TRUE(engine.has_errors());
  EXPECT_EQ(engine.diagnostics().size(), invalid.size());
}

TEST(Diagnostic, EmptyAndControlBytePayloadsAreStoredWithoutRendering) {
  const std::vector<std::string> bytes = {"", std::string{"\0\r\n\t\x1B\xC3\xA9", 7}};
  for (const auto& payload : bytes) {
    for (const auto severity : {DiagnosticSeverity::Error, DiagnosticSeverity::Warning,
                                DiagnosticSeverity::Note}) {
      DiagnosticEngine engine;
      engine.emit({severity, payload, std::nullopt, {{{{1}, 0, 0}, payload}}});
      const auto values = engine.diagnostics();
      ASSERT_EQ(values.size(), 1u);
      EXPECT_EQ(values[0].message, payload);
      EXPECT_FALSE(values[0].primary);
      ASSERT_EQ(values[0].related.size(), 1u);
      EXPECT_EQ(values[0].related[0].label, payload);
      EXPECT_EQ(engine.has_errors(), severity == DiagnosticSeverity::Error);
    }
  }
}

TEST(Diagnostic, PrimaryLessRelatedContextKeepsOriginatingManagerContract) {
  veyr::SourceManager first;
  veyr::SourceManager second;
  const auto id = std::get<veyr::SourceId>(first.register_source("first", "abc"));
  const auto reused_id = std::get<veyr::SourceId>(second.register_source("second", "xyz"));
  ASSERT_EQ(id, reused_id);
  DiagnosticEngine engine;
  const SourceSpan span{id, 0, 3};
  engine.emit({DiagnosticSeverity::Note, "context without primary", std::nullopt,
               {{span, "first context"}, {span, "second context"}}});
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 1u);
  EXPECT_FALSE(values[0].primary);
  ASSERT_EQ(values[0].related.size(), 2u);
  EXPECT_EQ(values[0].related[0].label, "first context");
  EXPECT_EQ(values[0].related[1].label, "second context");
  EXPECT_EQ(std::get<std::string_view>(first.text(values[0].related[0].span)), "abc");
  EXPECT_EQ(std::get<std::string_view>(second.text(values[0].related[0].span)), "xyz");
  EXPECT_FALSE(engine.has_errors());
}

}  // namespace
