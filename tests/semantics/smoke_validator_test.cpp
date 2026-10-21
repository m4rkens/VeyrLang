#include <veyr/semantics/smoke_validator.hpp>
#include <veyr/diagnostics/diagnostic.hpp>

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

namespace {

using namespace veyr;

static_assert(!std::is_default_constructible_v<ValidatedSmokeEntry>);
static_assert(!std::is_constructible_v<ValidatedSmokeEntry, ParsedSmokeEntry, std::int32_t>);
static_assert(!std::is_aggregate_v<ValidatedSmokeEntry>);
static_assert(std::is_copy_constructible_v<ValidatedSmokeEntry>);
static_assert(std::is_move_constructible_v<ValidatedSmokeEntry>);
static_assert(std::is_copy_assignable_v<ValidatedSmokeEntry>);
static_assert(std::is_move_assignable_v<ValidatedSmokeEntry>);
static_assert(std::is_same_v<decltype(std::declval<const ValidatedSmokeEntry&>().ast()),
                             ParsedSmokeEntry>);
static_assert(std::is_same_v<decltype(std::declval<const ValidatedSmokeEntry&>().return_value()),
                             std::int32_t>);
static_assert(std::variant_size_v<SemanticResult> == 4);
static_assert(std::is_same_v<std::variant_alternative_t<0, SemanticResult>, SemanticRejected>);

TEST(SmokeProfileSemanticValue, CheckedAlternativesRemainDistinct) {
  const SemanticResult initial{};
  EXPECT_TRUE(std::holds_alternative<SemanticRejected>(initial));
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(initial));
  const SourceError source{SourceErrorKind::UnknownSource, {0}, 8, 9, std::nullopt};
  SemanticResult result{source};
  ASSERT_TRUE(std::holds_alternative<SourceError>(result));
  EXPECT_EQ(std::get<SourceError>(result), source);
  const AstError ast{AstErrorKind::NonDecimalLiteral, AstField::Integer,
                     {{1}, 28, 31}, 30};
  result = ast;
  ASSERT_TRUE(std::holds_alternative<AstError>(result));
  EXPECT_EQ(std::get<AstError>(result), ast);
  result = SemanticRejected{SemanticRejectionKind::BlockedByErrors};
  ASSERT_TRUE(std::holds_alternative<SemanticRejected>(result));
  EXPECT_EQ(std::get<SemanticRejected>(result).kind, SemanticRejectionKind::BlockedByErrors);
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
}

TEST(SmokeProfileSemanticValue, SuccessCannotBePubliclyForged) {
  EXPECT_FALSE((std::is_default_constructible_v<ValidatedSmokeEntry>));
  EXPECT_FALSE((std::is_constructible_v<ValidatedSmokeEntry, ParsedSmokeEntry, std::int32_t>));
  EXPECT_FALSE((std::is_aggregate_v<ValidatedSmokeEntry>));
  EXPECT_FALSE((std::is_convertible_v<ParsedSmokeEntry, ValidatedSmokeEntry>));
}

TEST(SmokeProfileSemanticValue, SuccessSupportsCopyMove) {
  EXPECT_TRUE((std::is_copy_constructible_v<ValidatedSmokeEntry>));
  EXPECT_TRUE((std::is_move_constructible_v<ValidatedSmokeEntry>));
  EXPECT_TRUE((std::is_copy_assignable_v<ValidatedSmokeEntry>));
  EXPECT_TRUE((std::is_move_assignable_v<ValidatedSmokeEntry>));
}

TEST(SmokeProfileSemanticValue, AccessorsExposeOwnedValuesOnly) {
  using AstValue = decltype(std::declval<const ValidatedSmokeEntry&>().ast());
  using Constant = decltype(std::declval<const ValidatedSmokeEntry&>().return_value());
  EXPECT_TRUE((std::is_same_v<AstValue, ParsedSmokeEntry>));
  EXPECT_TRUE((std::is_same_v<Constant, std::int32_t>));
  EXPECT_FALSE((std::is_reference_v<AstValue>));
  EXPECT_FALSE((std::is_reference_v<Constant>));
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
  // Only acquire this view after all emissions; expected owns its fields.
  const auto actual = engine.diagnostics();
  ASSERT_EQ(actual.size(), expected.size());
  for (std::size_t index = 0; index < expected.size(); ++index) {
    SCOPED_TRACE(index);
    ASSERT_NO_FATAL_FAILURE(expect_diagnostic(actual[index], expected[index]));
  }
}

void expect_success(const SemanticResult& result, const ParsedSmokeEntry& expected,
                    std::int32_t value) {
  ASSERT_TRUE(std::holds_alternative<ValidatedSmokeEntry>(result));
  const auto& validated = std::get<ValidatedSmokeEntry>(result);
  EXPECT_EQ(validated.ast(), expected);
  EXPECT_EQ(validated.return_value(), value);
}

void expect_rejection(const SemanticResult& result, SemanticRejectionKind kind) {
  ASSERT_TRUE(std::holds_alternative<SemanticRejected>(result));
  EXPECT_EQ(std::get<SemanticRejected>(result).kind, kind);
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
}

class SmokeProfileSemantic : public testing::Test {
 protected:
  void load(const std::string& literal) {
    // Specification-derived fixed prefix/suffix; no lexer/parser/validator oracle.
    const std::string bytes = "fn main() -> int32 { return " + literal + "; }";
    ASSERT_EQ(bytes.size(), 31u + literal.size());
    const auto registered = sources.register_source("<semantic>", bytes);
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    id = std::get<SourceId>(registered);
    entry = {{id, 0, 31 + literal.size()}, {id, 21, 29 + literal.size()},
             {id, 28, 28 + literal.size()}};
  }

  Diagnostic expected_range_error() const {
    return {DiagnosticSeverity::Error,
            "return value must be between 0 and 255 in M1 Experimental Smoke Profile",
            entry.integer_span, {}};
  }

  SourceManager sources;
  DiagnosticEngine diagnostics;
  SourceId id{};
  ParsedSmokeEntry entry{};
};

TEST_F(SmokeProfileSemantic, AcceptsZero) {
  ASSERT_NO_FATAL_FAILURE(load("0"));
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_success(result, entry, 0));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemantic, Accepts255) {
  ASSERT_NO_FATAL_FAILURE(load("255"));
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_success(result, entry, 255));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemantic, LeadingZerosAreDecimal) {
  ASSERT_NO_FATAL_FAILURE(load("00042"));
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_success(result, entry, 42));
  auto copy = std::get<ValidatedSmokeEntry>(result);
  auto moved = std::move(copy);
  EXPECT_EQ(moved.ast(), entry);
  EXPECT_EQ(moved.return_value(), 42);
  copy = moved;
  moved = std::move(copy);
  auto editable_copy = moved.ast();
  editable_copy.integer_span.begin = 0;
  EXPECT_EQ(moved.ast(), entry);
  EXPECT_EQ(std::get<ValidatedSmokeEntry>(result).ast(), entry);
  EXPECT_EQ(std::get<ValidatedSmokeEntry>(result).return_value(), 42);
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemantic, LongLeadingZerosRemainInRange) {
  ASSERT_NO_FATAL_FAILURE(load(std::string(65536, '0') + "42"));
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_success(result, entry, 42));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemantic, Rejects256WithExactError) {
  ASSERT_NO_FATAL_FAILURE(load("256"));
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::OutOfRange));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {expected_range_error()}));
  EXPECT_EQ(entry.integer_span, (SourceSpan{id, 28, 31}));
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileSemantic, HugeDecimalUsesFixedError) {
  ASSERT_NO_FATAL_FAILURE(load(std::string(65536, '9')));
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::OutOfRange));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {expected_range_error()}));
  EXPECT_EQ(entry.integer_span, (SourceSpan{id, 28, 65564}));
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileSemantic, UnknownSourcePreservesPayload) {
  ASSERT_NO_FATAL_FAILURE(load("42"));
  entry.entry_span.source_id = SourceId{0};
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_TRUE(std::holds_alternative<SourceError>(result));
  EXPECT_EQ(std::get<SourceError>(result),
            (SourceError{SourceErrorKind::UnknownSource, {0}, 0, 33, std::nullopt}));
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemantic, InvalidRelationshipIsChecked) {
  ASSERT_NO_FATAL_FAILURE(load("42"));
  entry.return_span.begin = 0;
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_TRUE(std::holds_alternative<AstError>(result));
  EXPECT_EQ(std::get<AstError>(result),
            (AstError{AstErrorKind::NonNestedSpan, AstField::Return,
                      {id, 0, 31}, std::nullopt}));
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemantic, MalformedDigitsAreChecked) {
  ASSERT_NO_FATAL_FAILURE(load("42x"));
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_TRUE(std::holds_alternative<AstError>(result));
  EXPECT_EQ(std::get<AstError>(result),
            (AstError{AstErrorKind::NonDecimalLiteral, AstField::Integer,
                      {id, 28, 31}, 30}));
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemantic, PriorErrorBlocksValidOutput) {
  ASSERT_NO_FATAL_FAILURE(load("42"));
  const Diagnostic earlier{DiagnosticSeverity::Error, "earlier error", SourceSpan{id, 0, 2},
                            {{SourceSpan{id, 21, 27}, "earlier context"}}};
  diagnostics.emit(earlier);
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::BlockedByErrors));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {earlier}));
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileSemantic, PriorErrorDoesNotSuppressRangeError) {
  ASSERT_NO_FATAL_FAILURE(load("256"));
  const Diagnostic earlier{DiagnosticSeverity::Error, "earlier error", SourceSpan{id, 0, 2},
                            {{SourceSpan{id, 21, 27}, "earlier context"}}};
  diagnostics.emit(earlier);
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::OutOfRange));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {earlier, expected_range_error()}));
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileSemantic, WarningsAndNotesPermitSuccess) {
  ASSERT_NO_FATAL_FAILURE(load("42"));
  const Diagnostic note{DiagnosticSeverity::Note, "earlier note", std::nullopt, {}};
  const Diagnostic warning{DiagnosticSeverity::Warning, "earlier warning", SourceSpan{id, 0, 2},
                            {{SourceSpan{id, 21, 27}, "warning context"}}};
  diagnostics.emit(note);
  diagnostics.emit(warning);
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_success(result, entry, 42));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {note, warning}));
  EXPECT_FALSE(diagnostics.has_errors());
}

}  // namespace
