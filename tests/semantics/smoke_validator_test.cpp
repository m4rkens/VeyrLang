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

namespace {

// Public contract fixtures: no validator implementation, internal tables or parser oracle.
struct SemanticMatrixRow {
  const char* name;
  unsigned scenario;
};

using CheckedAstFailure = std::variant<SourceError, AstError>;

void expect_checked_ast_failure(const SemanticResult& result,
                                const CheckedAstFailure& expected) {
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
  if (const auto* source = std::get_if<SourceError>(&expected)) {
    ASSERT_TRUE(std::holds_alternative<SourceError>(result));
    EXPECT_EQ(std::get<SourceError>(result), *source);
  } else {
    ASSERT_TRUE(std::holds_alternative<AstError>(result));
    EXPECT_EQ(std::get<AstError>(result), std::get<AstError>(expected));
  }
}

std::vector<Diagnostic> conformance_history(SourceId id, bool with_error = true) {
  std::vector<Diagnostic> history{
      {DiagnosticSeverity::Note, "prior note", std::nullopt,
       {{SourceSpan{id, 0, 2}, "note context"}}},
      {DiagnosticSeverity::Warning, "prior warning", SourceSpan{id, 0, 2},
       {{SourceSpan{id, 21, 27}, "warning context"}}}};
  if (with_error) {
    history.push_back({DiagnosticSeverity::Error, "prior error", SourceSpan{id, 21, 27},
                       {{SourceSpan{id, 0, 2}, "error context"}}});
  }
  return history;
}

void emit_history(DiagnosticEngine& engine, const std::vector<Diagnostic>& history) {
  for (const auto& diagnostic : history) {
    engine.emit(diagnostic);
  }
}

class SmokeProfileSemanticStructure
    : public SmokeProfileSemantic,
      public testing::WithParamInterface<SemanticMatrixRow> {};

TEST_P(SmokeProfileSemanticStructure, ExactCheckedFailure) {
  SCOPED_TRACE(GetParam().name);
  ASSERT_NO_FATAL_FAILURE(load("42"));
  const auto history = conformance_history(id);
  emit_history(diagnostics, history);
  const auto secondary = sources.register_source("other.vyr", "fn main() -> int32 { return 42; }");
  ASSERT_TRUE(std::holds_alternative<SourceId>(secondary));
  const auto other = std::get<SourceId>(secondary);
  ASSERT_NE(other, id);
  CheckedAstFailure expected;
  switch (GetParam().scenario) {
    case 0:
      entry.entry_span = {{0}, 0, 33};
      expected = SourceError{SourceErrorKind::UnknownSource, {0}, 0, 33, std::nullopt};
      break;
    case 1:
      entry.return_span = {{0}, 21, 31};
      expected = SourceError{SourceErrorKind::UnknownSource, {0}, 21, 31, std::nullopt};
      break;
    case 2:
      entry.integer_span = {{0}, 28, 30};
      expected = SourceError{SourceErrorKind::UnknownSource, {0}, 28, 30, std::nullopt};
      break;
    case 3:
      entry.entry_span = {id, 33, 0};
      expected = SourceError{SourceErrorKind::ReversedRange, id, 33, 0, 33};
      break;
    case 4:
      entry.return_span = {id, 31, 21};
      expected = SourceError{SourceErrorKind::ReversedRange, id, 31, 21, 33};
      break;
    case 5:
      entry.integer_span = {id, 30, 28};
      expected = SourceError{SourceErrorKind::ReversedRange, id, 30, 28, 33};
      break;
    case 6:
      entry.entry_span = {id, 0, 34};
      expected = SourceError{SourceErrorKind::BeyondSource, id, 0, 34, 33};
      break;
    case 7:
      entry.return_span = {id, 21, 34};
      expected = SourceError{SourceErrorKind::BeyondSource, id, 21, 34, 33};
      break;
    case 8:
      entry.integer_span = {id, 28, 34};
      expected = SourceError{SourceErrorKind::BeyondSource, id, 28, 34, 33};
      break;
    case 9:
      entry.return_span = {other, 21, 31};
      expected = AstError{AstErrorKind::SourceMismatch, AstField::Return,
                          {other, 21, 31}, std::nullopt};
      break;
    case 10:
      entry.integer_span = {other, 28, 30};
      expected = AstError{AstErrorKind::SourceMismatch, AstField::Integer,
                          {other, 28, 30}, std::nullopt};
      break;
    case 11:
      entry.entry_span = {id, 33, 33};  // Source-valid EOF, invalid empty AST entry.
      expected = AstError{AstErrorKind::EmptySpan, AstField::Entry,
                          {id, 33, 33}, std::nullopt};
      break;
    case 12:
      entry.return_span = {id, 31, 31};
      expected = AstError{AstErrorKind::EmptySpan, AstField::Return,
                          {id, 31, 31}, std::nullopt};
      break;
    case 13:
      entry.integer_span = {id, 30, 30};
      expected = AstError{AstErrorKind::EmptySpan, AstField::Integer,
                          {id, 30, 30}, std::nullopt};
      break;
    case 14:
      entry.return_span = {id, 0, 31};
      expected = AstError{AstErrorKind::NonNestedSpan, AstField::Return,
                          {id, 0, 31}, std::nullopt};
      break;
    case 15:
      entry.return_span = {id, 21, 33};
      expected = AstError{AstErrorKind::NonNestedSpan, AstField::Return,
                          {id, 21, 33}, std::nullopt};
      break;
    case 16:
      entry.integer_span = {id, 21, 30};
      expected = AstError{AstErrorKind::NonNestedSpan, AstField::Integer,
                          {id, 21, 30}, std::nullopt};
      break;
    case 17:
      entry.integer_span = {id, 28, 31};
      expected = AstError{AstErrorKind::NonNestedSpan, AstField::Integer,
                          {id, 28, 31}, std::nullopt};
      break;
    default:
      FAIL() << "Unknown structural row";
  }
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_checked_ast_failure(result, expected));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
  EXPECT_TRUE(diagnostics.has_errors());
}

INSTANTIATE_TEST_SUITE_P(
    Conformance, SmokeProfileSemanticStructure,
    testing::Values(
        SemanticMatrixRow{"UnknownEntry", 0}, SemanticMatrixRow{"UnknownReturn", 1},
        SemanticMatrixRow{"UnknownInteger", 2}, SemanticMatrixRow{"ReversedEntry", 3},
        SemanticMatrixRow{"ReversedReturn", 4}, SemanticMatrixRow{"ReversedInteger", 5},
        SemanticMatrixRow{"BeyondEntry", 6}, SemanticMatrixRow{"BeyondReturn", 7},
        SemanticMatrixRow{"BeyondInteger", 8}, SemanticMatrixRow{"ReturnSourceMismatch", 9},
        SemanticMatrixRow{"IntegerSourceMismatch", 10}, SemanticMatrixRow{"EmptyEntry", 11},
        SemanticMatrixRow{"EmptyReturn", 12}, SemanticMatrixRow{"EmptyInteger", 13},
        SemanticMatrixRow{"ReturnBeginTouchesEntry", 14},
        SemanticMatrixRow{"ReturnEndTouchesEntry", 15},
        SemanticMatrixRow{"IntegerBeginTouchesReturn", 16},
        SemanticMatrixRow{"IntegerEndTouchesReturn", 17}),
    [](const testing::TestParamInfo<SemanticMatrixRow>& row) { return row.param.name; });

class SmokeProfileSemanticPriority
    : public SmokeProfileSemantic,
      public testing::WithParamInterface<SemanticMatrixRow> {};

TEST_P(SmokeProfileSemanticPriority, BothFaultsSelectExactWinner) {
  SCOPED_TRACE(GetParam().name);
  const bool late_nondigit = GetParam().scenario == 7;
  const std::string literal = late_nondigit ? std::string(65536, '9') + "x"
                              : GetParam().scenario == 6 ? "42x" : "42";
  ASSERT_NO_FATAL_FAILURE(load(literal));
  const auto history = conformance_history(id);
  emit_history(diagnostics, history);
  ASSERT_TRUE(diagnostics.has_errors());

  if (GetParam().scenario == 5) {
    unsigned calls = 0;
    for (const bool return_end : {false, true}) {
      for (const bool integer_end : {false, true}) {
        SCOPED_TRACE(return_end ? "return end touches" : "return begin touches");
        SCOPED_TRACE(integer_end ? "integer end touches" : "integer begin touches");
        auto broken = entry;
        if (return_end) broken.return_span.end = 33;
        else broken.return_span.begin = 0;
        if (integer_end) broken.integer_span.end = broken.return_span.end;
        else broken.integer_span.begin = broken.return_span.begin;
        ASSERT_TRUE(broken.return_span.begin == broken.entry_span.begin ||
                    broken.return_span.end == broken.entry_span.end);
        ASSERT_TRUE(broken.integer_span.begin == broken.return_span.begin ||
                    broken.integer_span.end == broken.return_span.end);
        for (const auto span : {broken.entry_span, broken.return_span, broken.integer_span}) {
          ASSERT_TRUE(std::holds_alternative<std::monostate>(sources.validate(span)));
          ASSERT_LT(span.begin, span.end);
        }
        const auto result = validate_smoke(sources, broken, diagnostics);
        ASSERT_NO_FATAL_FAILURE(expect_checked_ast_failure(
            result, AstError{AstErrorKind::NonNestedSpan, AstField::Return,
                              broken.return_span, std::nullopt}));
        ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
        ++calls;
      }
    }
    EXPECT_EQ(calls, 4u);
    EXPECT_TRUE(diagnostics.has_errors());
    return;
  }

  CheckedAstFailure expected;
  switch (GetParam().scenario) {
    case 0:
      entry.entry_span.source_id = SourceId{0};
      entry.return_span = {id, 31, 21};
      ASSERT_EQ(entry.entry_span.source_id, SourceId{0});
      ASSERT_GT(entry.return_span.begin, entry.return_span.end);
      expected = SourceError{SourceErrorKind::UnknownSource, {0}, 0, 33, std::nullopt};
      break;
    case 1:
      entry.return_span = {id, 31, 21};
      entry.integer_span = {id, 28, 34};
      ASSERT_GT(entry.return_span.begin, entry.return_span.end);
      ASSERT_GT(entry.integer_span.end, 33u);
      expected = SourceError{SourceErrorKind::ReversedRange, id, 31, 21, 33};
      break;
    case 2:
      entry.return_span.begin = 0;
      entry.integer_span.source_id = SourceId{0};
      ASSERT_EQ(entry.return_span.begin, entry.entry_span.begin);
      ASSERT_EQ(entry.integer_span.source_id, SourceId{0});
      expected = SourceError{SourceErrorKind::UnknownSource, {0}, 28, 30, std::nullopt};
      break;
    case 3: {
      const auto registered = sources.register_source("other.vyr", "fn main() -> int32 { return 42; }");
      ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
      const auto other = std::get<SourceId>(registered);
      entry.return_span.source_id = other;
      entry.integer_span.end = 28;
      ASSERT_NE(other, id);
      ASSERT_TRUE(std::holds_alternative<std::monostate>(sources.validate(entry.return_span)));
      ASSERT_EQ(entry.integer_span.begin, entry.integer_span.end);
      expected = AstError{AstErrorKind::SourceMismatch, AstField::Return,
                          {other, 21, 31}, std::nullopt};
      break;
    }
    case 4:
      entry.entry_span = {id, 33, 33};
      entry.return_span.begin = 0;
      ASSERT_EQ(entry.entry_span.begin, entry.entry_span.end);
      ASSERT_LE(entry.return_span.begin, entry.entry_span.begin);
      expected = AstError{AstErrorKind::EmptySpan, AstField::Entry,
                          {id, 33, 33}, std::nullopt};
      break;
    case 6:
      entry.return_span.begin = 0;
      ASSERT_EQ(entry.return_span.begin, entry.entry_span.begin);
      ASSERT_EQ(literal, "42x");
      ASSERT_EQ(literal[2], 'x');
      expected = AstError{AstErrorKind::NonNestedSpan, AstField::Return,
                          {id, 0, 32}, std::nullopt};
      break;
    case 7:
      ASSERT_EQ(literal.size(), 65537u);
      ASSERT_EQ(literal.substr(0, 65536), std::string(65536, '9'));
      ASSERT_EQ(literal.back(), 'x');
      expected = AstError{AstErrorKind::NonDecimalLiteral, AstField::Integer,
                          {id, 28, 65565}, 65564};
      break;
    default:
      FAIL() << "Unknown priority row";
  }
  const auto result = validate_smoke(sources, entry, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_checked_ast_failure(result, expected));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
  EXPECT_TRUE(diagnostics.has_errors());
}

INSTANTIATE_TEST_SUITE_P(
    Conformance, SmokeProfileSemanticPriority,
    testing::Values(
        SemanticMatrixRow{"EntrySourceBeforeReturnSource", 0},
        SemanticMatrixRow{"ReturnSourceBeforeIntegerSource", 1},
        SemanticMatrixRow{"LaterSourceBeforeEarlierRelationship", 2},
        SemanticMatrixRow{"SourceMismatchBeforeEmpty", 3},
        SemanticMatrixRow{"EmptyBeforeNonNested", 4},
        SemanticMatrixRow{"ReturnRelationshipBeforeIntegerRelationship", 5},
        SemanticMatrixRow{"RelationshipBeforeLiteral", 6},
        SemanticMatrixRow{"NonDecimalAfterRangePrefixBeforeOutOfRange", 7}),
    [](const testing::TestParamInfo<SemanticMatrixRow>& row) { return row.param.name; });

}  // namespace

namespace {

class SmokeProfileSemanticDecimal
    : public SmokeProfileSemantic,
      public testing::WithParamInterface<SemanticMatrixRow> {};

TEST_P(SmokeProfileSemanticDecimal, DecimalContentAndRange) {
  SCOPED_TRACE(GetParam().name);
  std::string literal;
  std::optional<std::int32_t> value;
  std::optional<std::size_t> bad_offset;
  bool range_error = false;
  switch (GetParam().scenario) {
    case 0: literal = "0"; value = 0; break;
    case 1: literal = "1"; value = 1; break;
    case 2: literal = "254"; value = 254; break;
    case 3: literal = "255"; value = 255; break;
    case 4: literal = "256"; range_error = true; break;
    case 5: literal = std::string(65536, '9'); range_error = true; break;
    case 6: literal = std::string(65536, '0'); value = 0; break;
    case 7: literal = std::string(65536, '0') + "42"; value = 42; break;
    case 8: literal = std::string(65536, '0') + "255"; value = 255; break;
    case 9: literal = std::string(65536, '0') + "256"; range_error = true; break;
    case 10: literal = "42x"; bad_offset = 30; break;
    case 11: literal = "-1"; bad_offset = 28; break;
    case 12: literal = "+1"; bad_offset = 28; break;
    case 13: literal = "4 2"; bad_offset = 29; break;
    case 14: literal = std::string("4\0" "2", 3); bad_offset = 29; break;
    case 15: literal = std::string{'4', static_cast<char>(0xff), '2'}; bad_offset = 29; break;
    case 16: literal = "4.2"; bad_offset = 29; break;
    case 17: literal = std::string(65536, '9') + "x"; bad_offset = 65564; break;
    default: FAIL() << "Unknown decimal row";
  }
  ASSERT_EQ(static_cast<unsigned>(value.has_value()) + static_cast<unsigned>(range_error) +
                static_cast<unsigned>(bad_offset.has_value()), 1u);
  if ((GetParam().scenario >= 5 && GetParam().scenario <= 9) || GetParam().scenario == 17) {
    ASSERT_GE(literal.size(), 65536u);
  }
  if (GetParam().scenario == 14) {
    ASSERT_EQ(literal.size(), 3u);
    ASSERT_EQ(literal[1], '\0');
    ASSERT_EQ(literal[2], '2');  // Explicit length retained bytes beyond the NUL.
  }
  if (GetParam().scenario == 15) {
    ASSERT_EQ(literal.size(), 3u);
    ASSERT_EQ(static_cast<unsigned char>(literal[1]), 0xffu);
  }
  if (GetParam().scenario == 17) {
    ASSERT_EQ(literal.substr(0, 65536), std::string(65536, '9'));
    ASSERT_EQ(literal.back(), 'x');
  }
  ASSERT_NO_FATAL_FAILURE(load(literal));
  const auto result = validate_smoke(sources, entry, diagnostics);
  if (value) {
    ASSERT_NO_FATAL_FAILURE(expect_success(result, entry, *value));
    EXPECT_FALSE(diagnostics.has_errors());
    EXPECT_TRUE(diagnostics.diagnostics().empty());
  } else if (range_error) {
    ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::OutOfRange));
    ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {expected_range_error()}));
    EXPECT_TRUE(diagnostics.has_errors());
  } else {
    ASSERT_NO_FATAL_FAILURE(expect_checked_ast_failure(
        result, AstError{AstErrorKind::NonDecimalLiteral, AstField::Integer,
                          {id, 28, 28 + literal.size()}, bad_offset}));
    EXPECT_FALSE(diagnostics.has_errors());
    EXPECT_TRUE(diagnostics.diagnostics().empty());
  }

  if (GetParam().scenario == 3) {
    // Enumerate the entire reviewed range. Formatting creates inputs; the loop
    // value is the independent expected constant, never parsed from production.
    unsigned calls = 0;
    for (std::int32_t expected = 0; expected <= 255; ++expected) {
      SCOPED_TRACE(expected);
      ASSERT_NO_FATAL_FAILURE(load(std::to_string(expected)));
      const auto each = validate_smoke(sources, entry, diagnostics);
      ASSERT_NO_FATAL_FAILURE(expect_success(each, entry, expected));
      EXPECT_FALSE(diagnostics.has_errors());
      EXPECT_TRUE(diagnostics.diagnostics().empty());
      ++calls;
    }
    EXPECT_EQ(calls, 256u);
  }
}

INSTANTIATE_TEST_SUITE_P(
    Conformance, SmokeProfileSemanticDecimal,
    testing::Values(
        SemanticMatrixRow{"Zero", 0}, SemanticMatrixRow{"One", 1},
        SemanticMatrixRow{"TwoHundredFiftyFour", 2},
        SemanticMatrixRow{"TwoHundredFiftyFive", 3},
        SemanticMatrixRow{"TwoHundredFiftySix", 4}, SemanticMatrixRow{"HugeDigits", 5},
        SemanticMatrixRow{"ManyZeros", 6}, SemanticMatrixRow{"ManyZerosThen42", 7},
        SemanticMatrixRow{"ManyZerosThen255", 8}, SemanticMatrixRow{"ManyZerosThen256", 9},
        SemanticMatrixRow{"LetterSuffix", 10}, SemanticMatrixRow{"NegativeSpelling", 11},
        SemanticMatrixRow{"PlusSpelling", 12}, SemanticMatrixRow{"EmbeddedSpace", 13},
        SemanticMatrixRow{"EmbeddedNul", 14}, SemanticMatrixRow{"HighByte", 15},
        SemanticMatrixRow{"Dot", 16}, SemanticMatrixRow{"RangePrefixThenLetter", 17}),
    [](const testing::TestParamInfo<SemanticMatrixRow>& row) { return row.param.name; });

class SmokeProfileSemanticDiagnostics
    : public SmokeProfileSemantic,
      public testing::WithParamInterface<SemanticMatrixRow> {};

TEST_P(SmokeProfileSemanticDiagnostics, ExactOwningHistoryAndErrorGate) {
  SCOPED_TRACE(GetParam().name);
  const auto scenario = GetParam().scenario;
  ASSERT_LE(scenario, 4u);
  const std::string literal = scenario == 1 ? std::string(65536, '0') + "256"
                              : scenario == 3 ? "000256"
                              : scenario == 4 ? "42x" : "42";
  ASSERT_NO_FATAL_FAILURE(load(literal));
  auto history = conformance_history(id, scenario != 2);
  emit_history(diagnostics, history);
  if (scenario == 4) {
    entry.integer_span.end = 28;
    ASSERT_EQ(entry.integer_span.begin, entry.integer_span.end);
    const auto result = validate_smoke(sources, entry, diagnostics);
    ASSERT_NO_FATAL_FAILURE(expect_checked_ast_failure(
        result, AstError{AstErrorKind::EmptySpan, AstField::Integer,
                          {id, 28, 28}, std::nullopt}));
  } else if (scenario == 3) {
    unsigned calls = 0;
    for (unsigned index = 0; index < 2; ++index) {
      SCOPED_TRACE(index);
      const auto result = validate_smoke(sources, entry, diagnostics);
      ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::OutOfRange));
      history.push_back(expected_range_error());
      ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
      ++calls;
    }
    EXPECT_EQ(calls, 2u);
    const auto actual = diagnostics.diagnostics();
    ASSERT_EQ(actual.size(), 5u);
    EXPECT_NE(&actual[3], &actual[4]);
    ASSERT_NO_FATAL_FAILURE(expect_diagnostic(actual[3], actual[4]));
  } else {
    const auto result = validate_smoke(sources, entry, diagnostics);
    if (scenario == 0) {
      ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::BlockedByErrors));
    } else if (scenario == 1) {
      ASSERT_GE(literal.size(), 65536u);
      ASSERT_EQ(literal.substr(0, 65536), std::string(65536, '0'));
      ASSERT_NO_FATAL_FAILURE(expect_rejection(result, SemanticRejectionKind::OutOfRange));
      history.push_back(expected_range_error());
    } else {
      ASSERT_NO_FATAL_FAILURE(expect_success(result, entry, 42));
    }
  }
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
  EXPECT_EQ(diagnostics.has_errors(), scenario != 2);
}

INSTANTIATE_TEST_SUITE_P(
    Conformance, SmokeProfileSemanticDiagnostics,
    testing::Values(
        SemanticMatrixRow{"ValidWithPriorErrorBlocked", 0},
        SemanticMatrixRow{"RangeWithPriorErrorAppendsOne", 1},
        SemanticMatrixRow{"ValidWithNoteAndWarningSucceeds", 2},
        SemanticMatrixRow{"DuplicateRangeErrorsPreserveOrder", 3},
        SemanticMatrixRow{"MalformedAstPreservesAllPriorFields", 4}),
    [](const testing::TestParamInfo<SemanticMatrixRow>& row) { return row.param.name; });

}  // namespace
