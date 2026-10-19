#include <veyr/parsing/parser.hpp>

#include <array>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <variant>

namespace veyr {
namespace {

bool supported_kind(TokenKind kind) {
  switch (kind) {
    case TokenKind::Fn:
    case TokenKind::Main:
    case TokenKind::Int32:
    case TokenKind::Return:
    case TokenKind::LeftParen:
    case TokenKind::RightParen:
    case TokenKind::Arrow:
    case TokenKind::LeftBrace:
    case TokenKind::RightBrace:
    case TokenKind::Semicolon:
    case TokenKind::IntegerLiteral:
    case TokenKind::Invalid:
    case TokenKind::EndOfFile:
      return true;
  }
  return false;
}

struct Expectation {
  TokenKind kind;
  std::string_view label;
};

constexpr std::array expected = {
    Expectation{TokenKind::Fn, "'fn'"},
    Expectation{TokenKind::Main, "'main'"},
    Expectation{TokenKind::LeftParen, "'('"},
    Expectation{TokenKind::RightParen, "')'"},
    Expectation{TokenKind::Arrow, "'->'"},
    Expectation{TokenKind::Int32, "'int32'"},
    Expectation{TokenKind::LeftBrace, "'{'"},
    Expectation{TokenKind::Return, "'return'"},
    Expectation{TokenKind::IntegerLiteral, "decimal integer literal"},
    Expectation{TokenKind::Semicolon, "';'"},
    Expectation{TokenKind::RightBrace, "'}'"},
    Expectation{TokenKind::EndOfFile, "end of source"}};

std::optional<std::string_view> found_label(TokenKind kind) {
  for (const auto& item : expected) {
    if (item.kind == kind) {
      return item.label;
    }
  }
  return std::nullopt;
}

}  // namespace

ParseResult parse(const SourceManager& sources, SourceId source_id,
                  std::span<const Token> tokens, DiagnosticEngine& diagnostics) {
  const auto source = sources.source_text(source_id);
  if (const auto* error = std::get_if<SourceError>(&source)) {
    return *error;
  }
  const auto source_size = std::get<std::string_view>(source).size();

  // Check the complete stream before deciding lexical or syntax rejection.
  // Increasing indices and this per-token order define structural precedence.
  bool has_invalid = false;
  for (std::size_t index = 0; index < tokens.size(); ++index) {
    const auto& token = tokens[index];
    const auto checked = sources.validate(token.span);
    if (const auto* error = std::get_if<SourceError>(&checked)) {
      return *error;
    }
    if (token.span.source_id != source_id) {
      return TokenStreamError{TokenStreamErrorKind::SourceMismatch, source_id, index};
    }
    if (!supported_kind(token.kind)) {
      return TokenStreamError{TokenStreamErrorKind::UnsupportedTokenKind,
                              source_id, index};
    }
    if (token.kind == TokenKind::EndOfFile) {
      if (index != tokens.size() - 1) {
        return TokenStreamError{TokenStreamErrorKind::NonFinalEndOfFile,
                                source_id, index};
      }
      if (token.span.begin != source_size || token.span.end != source_size) {
        return TokenStreamError{TokenStreamErrorKind::InvalidEndOfFileSpan,
                                source_id, index};
      }
    } else if (token.span.begin == token.span.end) {
      return TokenStreamError{TokenStreamErrorKind::EmptyTokenSpan, source_id, index};
    }
    if (index > 0 && token.span.begin < tokens[index - 1].span.end) {
      return TokenStreamError{TokenStreamErrorKind::OutOfOrderOrOverlappingSpan,
                              source_id, index};
    }
    has_invalid = has_invalid || token.kind == TokenKind::Invalid;
  }
  if (tokens.empty() || tokens.back().kind != TokenKind::EndOfFile) {
    return TokenStreamError{TokenStreamErrorKind::MissingEndOfFile,
                            source_id, std::nullopt};
  }
  if (has_invalid) {
    return ParseRejected{ParseRejectionKind::LexicalErrors};
  }

  for (std::size_t position = 0; position < expected.size(); ++position) {
    // A validated actual EOF will mismatch before a short stream is exhausted.
    // Keep the index guard explicit; never manufacture an EOF or read past it.
    if (position >= tokens.size()) {
      return TokenStreamError{TokenStreamErrorKind::MissingEndOfFile,
                              source_id, std::nullopt};
    }
    const auto& actual = tokens[position];
    if (actual.kind != expected[position].kind) {
      const auto found = found_label(actual.kind);
      // Structural validation and the Invalid gate guarantee a known label.
      if (!found) {
        return TokenStreamError{TokenStreamErrorKind::UnsupportedTokenKind,
                                source_id, position};
      }
      std::string message{"expected "};
      message += expected[position].label;
      message += ", found ";
      message += *found;
      message += " in M1 Experimental Smoke Profile";
      Diagnostic error{DiagnosticSeverity::Error, std::move(message), actual.span, {}};
      if (position == 3) {
        error.related.push_back({tokens[2].span, "opening '(' is here"});
      } else if (position == 10) {
        error.related.push_back({tokens[6].span, "opening '{' is here"});
      }
      diagnostics.emit(std::move(error));
      return ParseRejected{ParseRejectionKind::SyntaxError};
    }
  }

  // Matching EOF also proves there are no trailing tokens: it was validated final.
  return ParsedSmokeEntry{{source_id, tokens[0].span.begin, tokens[10].span.end},
                          {source_id, tokens[7].span.begin, tokens[9].span.end},
                          tokens[8].span};
}

}  // namespace veyr
