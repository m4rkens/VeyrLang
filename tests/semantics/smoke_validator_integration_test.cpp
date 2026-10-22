#include <veyr/diagnostics/plain_text_renderer.hpp>
#include <veyr/lexing/lexer.hpp>
#include <veyr/parsing/parser.hpp>
#include <veyr/semantics/smoke_validator.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

namespace {

using namespace veyr;

std::vector<Diagnostic> copied_diagnostics(const DiagnosticEngine& engine) {
  const auto view = engine.diagnostics();
  return {view.begin(), view.end()};
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

void expect_history(const DiagnosticEngine& engine, const std::vector<Diagnostic>& expected) {
  const auto actual = copied_diagnostics(engine);  // Reacquire after all emissions.
  ASSERT_EQ(actual.size(), expected.size());
  for (std::size_t index = 0; index < expected.size(); ++index) {
    SCOPED_TRACE(index);
    ASSERT_NO_FATAL_FAILURE(expect_diagnostic(actual[index], expected[index]));
  }
}

Diagnostic range_error(SourceSpan literal) {
  return {DiagnosticSeverity::Error,
          "return value must be between 0 and 255 in M1 Experimental Smoke Profile",
          literal, {}};
}

void expect_rejected(const SemanticResult& result, SemanticRejectionKind kind) {
  ASSERT_TRUE(std::holds_alternative<SemanticRejected>(result));
  EXPECT_EQ(std::get<SemanticRejected>(result).kind, kind);
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
}

void expect_success(const SemanticResult& result, ParsedSmokeEntry ast, std::int32_t constant) {
  ASSERT_TRUE(std::holds_alternative<ValidatedSmokeEntry>(result));
  EXPECT_EQ(std::get<ValidatedSmokeEntry>(result).ast(), ast);
  EXPECT_EQ(std::get<ValidatedSmokeEntry>(result).return_value(), constant);
}

void expect_text(const SourceManager& sources, SourceSpan span, std::string_view expected) {
  const auto text = sources.text(span);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(text));
  EXPECT_EQ(std::get<std::string_view>(text), expected);
}

class SmokeProfileSemanticIntegration : public testing::Test {
 protected:
  void load(std::string bytes, std::string name = "<semantic-integration>") {
    ast.reset();
    tokens.clear();
    const auto registered = sources.register_source(std::move(name), std::move(bytes));
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    id = std::get<SourceId>(registered);
    auto scanned = lex(sources, id, diagnostics);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    tokens = std::get<std::vector<Token>>(std::move(scanned));
    const auto parsed = parse(sources, id, tokens, diagnostics);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(parsed));
    ast = std::get<ParsedSmokeEntry>(parsed);
  }

  // All parameters and local intermediate values die before the caller inspects saved.
  void capture(std::optional<ValidatedSmokeEntry>& saved, std::string bytes,
               std::string name = "owned.vyr") {
    const auto registered = sources.register_source(name, bytes);
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    id = std::get<SourceId>(registered);
    DiagnosticEngine local_engine;
    auto scanned = lex(sources, id, local_engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    auto local_tokens = std::get<std::vector<Token>>(std::move(scanned));
    const auto parsed = parse(sources, id, local_tokens, local_engine);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(parsed));
    auto input_ast = std::get<ParsedSmokeEntry>(parsed);
    const auto result = validate_smoke(sources, input_ast, local_engine);
    ASSERT_TRUE(std::holds_alternative<ValidatedSmokeEntry>(result));
    saved = std::get<ValidatedSmokeEntry>(result);
    EXPECT_FALSE(local_engine.has_errors());
    EXPECT_TRUE(local_engine.diagnostics().empty());
    input_ast = {};  // Mutating the caller's value cannot mutate successful output.
    name.assign(100, 'x');
    bytes.assign(100, 'x');
  }

  SourceManager sources;  // Remains alive across every retained-span resolution.
  SourceId id{};
  DiagnosticEngine diagnostics;
  std::vector<Token> tokens;
  std::optional<ParsedSmokeEntry> ast;
};

TEST_F(SmokeProfileSemanticIntegration, RealFixedEntryValidates) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  const ParsedSmokeEntry expected{{id, 0, 33}, {id, 21, 31}, {id, 28, 30}};
  ASSERT_TRUE(ast.has_value());
  EXPECT_EQ(*ast, expected);
  const auto result = validate_smoke(sources, *ast, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_success(result, expected, 42));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.entry_span,
                                     "fn main() -> int32 { return 42; }"));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.return_span, "return 42;"));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.integer_span, "42"));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemanticIntegration, RealLeadingZerosAreDecimal) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 00042; }"));
  const ParsedSmokeEntry expected{{id, 0, 36}, {id, 21, 34}, {id, 28, 33}};
  EXPECT_EQ(*ast, expected);
  ASSERT_NO_FATAL_FAILURE(expect_success(validate_smoke(sources, *ast, diagnostics), expected, 42));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.integer_span, "00042"));
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
}

TEST_F(SmokeProfileSemanticIntegration, RealBoundaryValues) {
  struct Case { const char* literal; std::int32_t constant; std::size_t end; };
  const std::array cases = {Case{"0", 0, 29}, Case{"255", 255, 31}};
  unsigned calls = 0;
  for (const auto& item : cases) {
    SCOPED_TRACE(item.literal);
    ASSERT_NO_FATAL_FAILURE(load(std::string{"fn main() -> int32 { return "} + item.literal + "; }"));
    const ParsedSmokeEntry expected{{id, 0, item.end + 3}, {id, 21, item.end + 1},
                                     {id, 28, item.end}};
    EXPECT_EQ(*ast, expected);
    ASSERT_NO_FATAL_FAILURE(expect_success(validate_smoke(sources, *ast, diagnostics),
                                          expected, item.constant));
    EXPECT_FALSE(diagnostics.has_errors());
    EXPECT_TRUE(diagnostics.diagnostics().empty());
    ++calls;
  }
  EXPECT_EQ(calls, 2u);
}

TEST_F(SmokeProfileSemanticIntegration, RealHugeDecimalRejects) {
  const std::string digits(65536, '9');
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return " + digits + "; }"));
  const ParsedSmokeEntry expected{{id, 0, 65567}, {id, 21, 65565}, {id, 28, 65564}};
  EXPECT_EQ(*ast, expected);  // Parser accepts decimal shape without numeric conversion.
  EXPECT_FALSE(diagnostics.has_errors());
  EXPECT_TRUE(diagnostics.diagnostics().empty());
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.integer_span, digits));
  const auto result = validate_smoke(sources, *ast, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejected(result, SemanticRejectionKind::OutOfRange));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, {range_error({id, 28, 65564})}));
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileSemanticIntegration, OwnedOutputOutlivesInputs) {
  std::optional<ValidatedSmokeEntry> saved;
  ASSERT_NO_FATAL_FAILURE(capture(saved, "fn main() -> int32 { return 00042; }"));
  // Caller strings, LexResult, tokens, ParseResult, input AST and engine have died.
  ASSERT_TRUE(saved.has_value());
  const ParsedSmokeEntry expected{{id, 0, 36}, {id, 21, 34}, {id, 28, 33}};
  EXPECT_EQ(saved->ast(), expected);
  EXPECT_EQ(saved->return_value(), 42);
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, saved->ast().entry_span,
                                     "fn main() -> int32 { return 00042; }"));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, saved->ast().return_span, "return 00042;"));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, saved->ast().integer_span, "00042"));
  const auto name = sources.display_name(id);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(name));
  EXPECT_EQ(std::get<std::string_view>(name), "owned.vyr");
}

TEST_F(SmokeProfileSemanticIntegration, CopiesAndMovesKeepCompleteContext) {
  std::optional<ValidatedSmokeEntry> original, other;
  ASSERT_NO_FATAL_FAILURE(capture(original, "fn main() -> int32 { return 00042; }"));
  const SourceId first = id;
  ASSERT_NO_FATAL_FAILURE(capture(other, "fn main() -> int32 { return 0; }", "other.vyr"));
  ASSERT_TRUE(original.has_value());
  ASSERT_TRUE(other.has_value());
  ASSERT_NE(id, first);
  auto copy = *original;
  auto moved = std::move(copy);
  auto assigned = *other;
  assigned = moved;  // Replace a different, already validated source/value context.
  auto move_assigned = *other;
  move_assigned = std::move(assigned);
  original.reset();
  other.reset();
  const ParsedSmokeEntry expected{{first, 0, 36}, {first, 21, 34}, {first, 28, 33}};
  for (const auto* value : {&moved, &move_assigned}) {
    SCOPED_TRACE(value == &moved ? "move construction" : "copy/move assignment");
    EXPECT_EQ(value->ast(), expected);
    EXPECT_EQ(value->return_value(), 42);
    ASSERT_NO_FATAL_FAILURE(expect_text(sources, value->ast().entry_span,
                                       "fn main() -> int32 { return 00042; }"));
    ASSERT_NO_FATAL_FAILURE(expect_text(sources, value->ast().return_span, "return 00042;"));
    ASSERT_NO_FATAL_FAILURE(expect_text(sources, value->ast().integer_span, "00042"));
  }
}

TEST_F(SmokeProfileSemanticIntegration, ThreeThousandRegistrationsPreserveSnapshot) {
  std::optional<ValidatedSmokeEntry> saved;
  ASSERT_NO_FATAL_FAILURE(capture(saved, "fn main() -> int32 { return 00042; }"));
  ASSERT_TRUE(saved.has_value());
  const SourceId first = id;
  const auto original = sources.source_text(first);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(original));
  const auto view = std::get<std::string_view>(original);
  unsigned calls = 0;
  for (unsigned index = 0; index < 3000; ++index) {
    SCOPED_TRACE(index);
    const auto registered = sources.register_source(index % 2 ? "other.vyr" : "owned.vyr",
                                                    "unrelated " + std::to_string(index));
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    EXPECT_NE(std::get<SourceId>(registered), first);
    ++calls;
  }
  EXPECT_EQ(calls, 3000u);
  EXPECT_EQ(view, "fn main() -> int32 { return 00042; }");
  EXPECT_EQ(saved->ast(), (ParsedSmokeEntry{{first, 0, 36}, {first, 21, 34}, {first, 28, 33}}));
  EXPECT_EQ(saved->return_value(), 42);
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, saved->ast().entry_span, view));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, saved->ast().return_span, "return 00042;"));
  ASSERT_NO_FATAL_FAILURE(expect_text(sources, saved->ast().integer_span, "00042"));
}

TEST_F(SmokeProfileSemanticIntegration, LexicalInvalidCannotReachSuccess) {
  struct Case { const char* name; std::string literal; std::size_t end; const char* message; };
  const std::array cases = {
      Case{"negative", "-1", 30, "unsupported numeric spelling in M1 Experimental Smoke Profile"},
      Case{"hex", "0x10", 32, "unsupported numeric spelling in M1 Experimental Smoke Profile"},
      Case{"comment", "/*x*/", 33, "unsupported comment-shaped input in M1 Experimental Smoke Profile"},
      Case{"high byte", std::string(1, static_cast<char>(0xff)), 29,
           "unsupported non-ASCII byte run in M1 Experimental Smoke Profile"}};
  unsigned calls = 0;
  for (const auto& item : cases) {
    SCOPED_TRACE(item.name);
    DiagnosticEngine engine;
    const std::string bytes = "fn main() -> int32 { return " + item.literal + "; }";
    const auto registered = sources.register_source("lexical.vyr", bytes);
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    const auto selected = std::get<SourceId>(registered);
    const auto scanned = lex(sources, selected, engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    const auto& stream = std::get<std::vector<Token>>(scanned);
    std::vector<SourceSpan> invalid;
    for (const auto& token : stream) if (token.kind == TokenKind::Invalid) invalid.push_back(token.span);
    EXPECT_EQ(invalid, (std::vector<SourceSpan>{{selected, 28, item.end}}));
    const std::vector<Diagnostic> expected{{DiagnosticSeverity::Error, item.message,
                                          SourceSpan{selected, 28, item.end}, {}}};
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, expected));
    const auto parsed = parse(sources, selected, stream, engine);
    ASSERT_TRUE(std::holds_alternative<ParseRejected>(parsed));
    EXPECT_EQ(std::get<ParseRejected>(parsed).kind, ParseRejectionKind::LexicalErrors);
    EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(parsed));
    std::optional<SemanticResult> semantic;
    if (const auto* input = std::get_if<ParsedSmokeEntry>(&parsed))
      semantic = validate_smoke(sources, *input, engine);
    EXPECT_FALSE(semantic.has_value());
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, expected));
    EXPECT_TRUE(engine.has_errors());
    ++calls;
  }
  EXPECT_EQ(calls, 4u);
}

TEST_F(SmokeProfileSemanticIntegration, SyntaxFailureCannotReachSuccess) {
  struct Case {
    const char* name; const char* bytes; const char* message;
    std::size_t begin, end; bool opener;
  };
  const std::array cases = {
      Case{"semicolon", "fn main() -> int32 { return 42 }",
           "expected ';', found '}' in M1 Experimental Smoke Profile", 31, 32, false},
      Case{"brace", "fn main() -> int32 { return 42;",
           "expected '}', found end of source in M1 Experimental Smoke Profile", 31, 31, true},
      Case{"trailing", "fn main() -> int32 { return 42; } return",
           "expected end of source, found 'return' in M1 Experimental Smoke Profile", 34, 40, false}};
  unsigned calls = 0;
  for (const auto& item : cases) {
    SCOPED_TRACE(item.name);
    DiagnosticEngine engine;
    const auto registered = sources.register_source("syntax.vyr", item.bytes);
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    const auto selected = std::get<SourceId>(registered);
    const auto scanned = lex(sources, selected, engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    EXPECT_TRUE(engine.diagnostics().empty());
    const auto parsed = parse(sources, selected, std::get<std::vector<Token>>(scanned), engine);
    ASSERT_TRUE(std::holds_alternative<ParseRejected>(parsed));
    EXPECT_EQ(std::get<ParseRejected>(parsed).kind, ParseRejectionKind::SyntaxError);
    EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(parsed));
    Diagnostic expected{DiagnosticSeverity::Error, item.message,
                         SourceSpan{selected, item.begin, item.end}, {}};
    if (item.opener) expected.related.push_back({{selected, 19, 20}, "opening '{' is here"});
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, {expected}));
    std::optional<SemanticResult> semantic;
    if (const auto* input = std::get_if<ParsedSmokeEntry>(&parsed))
      semantic = validate_smoke(sources, *input, engine);
    EXPECT_FALSE(semantic.has_value());
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, {expected}));
    EXPECT_TRUE(engine.has_errors());
    ++calls;
  }
  EXPECT_EQ(calls, 3u);
}

TEST_F(SmokeProfileSemanticIntegration, PriorErrorBlocksValidPipeline) {
  unsigned calls = 0;
  for (const bool syntax_error : {false, true}) {
    SCOPED_TRACE(syntax_error ? "prior parser Error" : "prior lexer Error");
    DiagnosticEngine engine;
    const auto registered = sources.register_source("earlier.vyr", syntax_error ? "fn" : "@");
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    const auto earlier = std::get<SourceId>(registered);
    const auto scanned = lex(sources, earlier, engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    if (syntax_error) {
      EXPECT_TRUE(engine.diagnostics().empty());
      const auto parsed = parse(sources, earlier, std::get<std::vector<Token>>(scanned), engine);
      ASSERT_TRUE(std::holds_alternative<ParseRejected>(parsed));
      EXPECT_EQ(std::get<ParseRejected>(parsed).kind, ParseRejectionKind::SyntaxError);
    }
    const Diagnostic expected{DiagnosticSeverity::Error,
        syntax_error ? "expected 'main', found end of source in M1 Experimental Smoke Profile"
                     : "unsupported byte in M1 Experimental Smoke Profile",
        syntax_error ? SourceSpan{earlier, 2, 2} : SourceSpan{earlier, 0, 1}, {}};
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, {expected}));
    ASSERT_TRUE(engine.has_errors());
    const auto valid = sources.register_source("current.vyr", "fn main() -> int32 { return 42; }");
    ASSERT_TRUE(std::holds_alternative<SourceId>(valid));
    const auto selected = std::get<SourceId>(valid);
    ASSERT_NE(selected, earlier);
    const auto lexed = lex(sources, selected, engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(lexed));
    const auto parsed = parse(sources, selected, std::get<std::vector<Token>>(lexed), engine);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(parsed));
    EXPECT_EQ(std::get<ParsedSmokeEntry>(parsed),
              (ParsedSmokeEntry{{selected, 0, 33}, {selected, 21, 31}, {selected, 28, 30}}));
    ASSERT_NO_FATAL_FAILURE(expect_rejected(
        validate_smoke(sources, std::get<ParsedSmokeEntry>(parsed), engine),
        SemanticRejectionKind::BlockedByErrors));
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, {expected}));
    EXPECT_TRUE(engine.has_errors());
    ++calls;
  }
  EXPECT_EQ(calls, 2u);
}

TEST_F(SmokeProfileSemanticIntegration, PriorWarningNoteAllowsPipeline) {
  const auto context = sources.register_source("context.vyr", "ok");
  ASSERT_TRUE(std::holds_alternative<SourceId>(context));
  const auto earlier = std::get<SourceId>(context);
  const std::vector<Diagnostic> history{
      {DiagnosticSeverity::Note, "prior note", std::nullopt,
       {{SourceSpan{earlier, 0, 2}, "note context"}}},
      {DiagnosticSeverity::Warning, "prior warning", SourceSpan{earlier, 0, 2},
       {{SourceSpan{earlier, 1, 2}, "warning context"}}}};
  for (const auto& item : history) diagnostics.emit(item);
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  EXPECT_NE(id, earlier);
  ASSERT_NO_FATAL_FAILURE(expect_success(validate_smoke(sources, *ast, diagnostics),
      {{id, 0, 33}, {id, 21, 31}, {id, 28, 30}}, 42));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
  EXPECT_FALSE(diagnostics.has_errors());
}

TEST_F(SmokeProfileSemanticIntegration, DuplicateRangeErrorsRemainOrdered) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 000256; }"));
  EXPECT_EQ(*ast, (ParsedSmokeEntry{{id, 0, 37}, {id, 21, 35}, {id, 28, 34}}));
  std::vector<Diagnostic> history{{DiagnosticSeverity::Error, "prior error", SourceSpan{id, 0, 2},
                                  {{SourceSpan{id, 21, 27}, "earlier context"}}}};
  diagnostics.emit(history[0]);
  unsigned calls = 0;
  for (unsigned index = 0; index < 2; ++index) {
    SCOPED_TRACE(index);
    ASSERT_NO_FATAL_FAILURE(expect_rejected(validate_smoke(sources, *ast, diagnostics),
                                          SemanticRejectionKind::OutOfRange));
    history.push_back(range_error({id, 28, 34}));
    ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
    EXPECT_TRUE(diagnostics.has_errors());
    ++calls;
  }
  EXPECT_EQ(calls, 2u);
  const auto actual = diagnostics.diagnostics();
  ASSERT_EQ(actual.size(), 3u);
  EXPECT_NE(&actual[1], &actual[2]);
  ASSERT_NO_FATAL_FAILURE(expect_diagnostic(actual[1], actual[2]));
}

TEST_F(SmokeProfileSemanticIntegration, OriginalSpansAcrossLineEndings) {
  struct Case {
    const char* name; const char* bytes;
    std::size_t end, return_begin, return_end, integer_begin, integer_end;
    std::size_t line, column, eof_line, eof_column;
  };
  const std::array cases = {
      Case{"LF", "fn main() -> int32 {\nreturn 256;\n}", 34, 21, 32, 28, 31, 2, 8, 3, 2},
      Case{"CRLF", "fn main() -> int32 {\r\nreturn 256;\r\n}", 36, 22, 33, 29, 32, 2, 8, 3, 2},
      Case{"lone CR", "fn main() -> int32 {\rreturn 256;\r}", 34, 21, 32, 28, 31, 1, 29, 1, 35}};
  unsigned calls = 0;
  for (const auto& item : cases) {
    SCOPED_TRACE(item.name);
    DiagnosticEngine engine;
    const auto registered = sources.register_source("lines.vyr", item.bytes);
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    const auto selected = std::get<SourceId>(registered);
    const auto scanned = lex(sources, selected, engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    const auto& stream = std::get<std::vector<Token>>(scanned);
    ASSERT_FALSE(stream.empty());
    EXPECT_EQ(stream.back().kind, TokenKind::EndOfFile);
    EXPECT_EQ(stream.back().span, (SourceSpan{selected, item.end, item.end}));
    const auto parsed = parse(sources, selected, stream, engine);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(parsed));
    const ParsedSmokeEntry expected{{selected, 0, item.end},
        {selected, item.return_begin, item.return_end}, {selected, item.integer_begin, item.integer_end}};
    EXPECT_EQ(std::get<ParsedSmokeEntry>(parsed), expected);
    ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.entry_span, item.bytes));
    ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.return_span, "return 256;"));
    ASSERT_NO_FATAL_FAILURE(expect_text(sources, expected.integer_span, "256"));
    const auto location = sources.location(selected, item.integer_begin);
    ASSERT_TRUE(std::holds_alternative<SourceLocation>(location));
    EXPECT_EQ(std::get<SourceLocation>(location),
              (SourceLocation{selected, item.integer_begin, item.line, item.column}));
    const auto eof = sources.location(selected, item.end);
    ASSERT_TRUE(std::holds_alternative<SourceLocation>(eof));
    EXPECT_EQ(std::get<SourceLocation>(eof),
              (SourceLocation{selected, item.end, item.eof_line, item.eof_column}));
    EXPECT_TRUE(engine.diagnostics().empty());
    ASSERT_NO_FATAL_FAILURE(expect_rejected(
        validate_smoke(sources, std::get<ParsedSmokeEntry>(parsed), engine), SemanticRejectionKind::OutOfRange));
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, {range_error(expected.integer_span)}));
    EXPECT_TRUE(engine.has_errors());
    ++calls;
  }
  EXPECT_EQ(calls, 3u);
}

TEST_F(SmokeProfileSemanticIntegration, CheckedSourceFailureRemainsDistinct) {
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 42; }"));
  const std::vector<Diagnostic> history{
      {DiagnosticSeverity::Note, "retained note", std::nullopt, {}},
      {DiagnosticSeverity::Error, "retained error", SourceSpan{id, 0, 2},
       {{SourceSpan{id, 21, 27}, "retained context"}}}};
  for (const auto& item : history) diagnostics.emit(item);
  auto broken = *ast;
  broken.entry_span.source_id = SourceId{0};
  const auto result = validate_smoke(sources, broken, diagnostics);
  ASSERT_TRUE(std::holds_alternative<SourceError>(result));
  EXPECT_EQ(std::get<SourceError>(result),
            (SourceError{SourceErrorKind::UnknownSource, {0}, 0, 33, std::nullopt}));
  EXPECT_FALSE(std::holds_alternative<ValidatedSmokeEntry>(result));
  const auto scanned = lex(sources, SourceId{0}, diagnostics);
  ASSERT_TRUE(std::holds_alternative<SourceError>(scanned));
  EXPECT_EQ(std::get<SourceError>(scanned),
            (SourceError{SourceErrorKind::UnknownSource, {0}, 0, 0, std::nullopt}));
  const auto parsed = parse(sources, SourceId{0}, std::span<const Token>{}, diagnostics);
  ASSERT_TRUE(std::holds_alternative<SourceError>(parsed));
  EXPECT_EQ(std::get<SourceError>(parsed), std::get<SourceError>(scanned));
  EXPECT_FALSE(std::holds_alternative<ParsedSmokeEntry>(parsed));
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, history));
  EXPECT_TRUE(diagnostics.has_errors());
}

TEST_F(SmokeProfileSemanticIntegration, RangeDiagnosticRendersIndependently) {
  const std::string expected =
      "range.vyr:1:29: error: return value must be between 0 and 255 in M1 Experimental Smoke Profile\n"
      "1 | fn main() -> int32 { return 256; }\n"
      "  | " "                            " "^~~\n";
  std::vector<Diagnostic> saved;
  std::string rendered;
  {
    DiagnosticEngine engine;
    std::string bytes = "fn main() -> int32 { return 256; }";
    const auto registered = sources.register_source("range.vyr", bytes);
    ASSERT_TRUE(std::holds_alternative<SourceId>(registered));
    id = std::get<SourceId>(registered);
    const auto scanned = lex(sources, id, engine);
    ASSERT_TRUE(std::holds_alternative<std::vector<Token>>(scanned));
    const auto parsed = parse(sources, id, std::get<std::vector<Token>>(scanned), engine);
    ASSERT_TRUE(std::holds_alternative<ParsedSmokeEntry>(parsed));
    const auto input = std::get<ParsedSmokeEntry>(parsed);
    ASSERT_NO_FATAL_FAILURE(expect_rejected(validate_smoke(sources, input, engine),
                                          SemanticRejectionKind::OutOfRange));
    saved = copied_diagnostics(engine);
    ASSERT_EQ(saved.size(), 1u);
    ASSERT_NO_FATAL_FAILURE(expect_diagnostic(saved[0], range_error({id, 28, 31})));
    const auto output = PlainTextRenderer{}.render(saved[0], sources);
    ASSERT_TRUE(std::holds_alternative<std::string>(output));
    rendered = std::get<std::string>(output);
    EXPECT_EQ(rendered, expected);
    ASSERT_NO_FATAL_FAILURE(expect_history(engine, saved));
    EXPECT_TRUE(engine.has_errors());
    bytes.assign(100, 'x');
  }  // Engine, tokens, parse result and input AST are dead; the manager is live.
  ASSERT_EQ(saved.size(), 1u);
  ASSERT_NO_FATAL_FAILURE(expect_diagnostic(saved[0], range_error({id, 28, 31})));
  const auto repeated = PlainTextRenderer{}.render(saved[0], sources);
  ASSERT_TRUE(std::holds_alternative<std::string>(repeated));
  EXPECT_EQ(std::get<std::string>(repeated), expected);
  saved.clear();
  EXPECT_EQ(rendered, expected);
}

TEST_F(SmokeProfileSemanticIntegration, HostileNameIsEscaped) {
  const std::string name = "../private\\bad\x1B\nname.vyr";
  ASSERT_NO_FATAL_FAILURE(load("fn main() -> int32 { return 256; }", name));
  const auto result = validate_smoke(sources, *ast, diagnostics);
  ASSERT_NO_FATAL_FAILURE(expect_rejected(result, SemanticRejectionKind::OutOfRange));
  const auto saved = copied_diagnostics(diagnostics);
  ASSERT_EQ(saved.size(), 1u);
  ASSERT_NO_FATAL_FAILURE(expect_diagnostic(saved[0], range_error({id, 28, 31})));
  const std::string expected =
      "../private\\bad\\x1B\\nname.vyr:1:29: error: return value must be between 0 and 255 in M1 Experimental Smoke Profile\n"
      "1 | fn main() -> int32 { return 256; }\n"
      "  | " "                            " "^~~\n";
  const auto output = PlainTextRenderer{}.render(saved[0], sources);
  ASSERT_TRUE(std::holds_alternative<std::string>(output));
  const auto rendered = std::get<std::string>(output);
  EXPECT_EQ(rendered, expected);
  EXPECT_EQ(std::count(rendered.begin(), rendered.end(), '\n'), 3);
  for (const unsigned char byte : rendered) EXPECT_TRUE(byte == '\n' || (byte >= 0x20 && byte <= 0x7e));
  const auto original_name = sources.display_name(id);
  ASSERT_TRUE(std::holds_alternative<std::string_view>(original_name));
  EXPECT_EQ(std::get<std::string_view>(original_name), name);  // Metadata was not rewritten or opened.
  ASSERT_NO_FATAL_FAILURE(expect_history(diagnostics, saved));
  EXPECT_TRUE(diagnostics.has_errors());
}

}  // namespace
