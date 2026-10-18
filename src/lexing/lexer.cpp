#include <veyr/lexing/lexer.hpp>

#include <cstddef>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace veyr {
namespace {

bool is_digit(unsigned char byte) {
  return byte >= '0' && byte <= '9';
}

bool is_word_start(unsigned char byte) {
  return (byte >= 'a' && byte <= 'z') || (byte >= 'A' && byte <= 'Z') || byte == '_';
}

bool is_word_continue(unsigned char byte) {
  return is_word_start(byte) || is_digit(byte);
}

// All callers have position <= size; subtract before doing lookahead arithmetic.
bool pair_at(std::string_view text, std::size_t position, char first, char second) {
  return text.size() - position >= 2 && text[position] == first &&
         text[position + 1] == second;
}

bool starts_number(std::string_view text, std::size_t position) {
  const auto remaining = text.size() - position;
  const auto byte = static_cast<unsigned char>(text[position]);
  if (is_digit(byte)) {
    return true;
  }
  if (byte == '.') {
    return remaining >= 2 && is_digit(static_cast<unsigned char>(text[position + 1]));
  }
  if (byte == '+' || byte == '-') {
    return (remaining >= 2 && is_digit(static_cast<unsigned char>(text[position + 1]))) ||
           (remaining >= 3 && text[position + 1] == '.' &&
            is_digit(static_cast<unsigned char>(text[position + 2])));
  }
  return false;
}

TokenKind word_kind(std::string_view word) {
  if (word == "fn") {
    return TokenKind::Fn;
  }
  if (word == "main") {
    return TokenKind::Main;
  }
  if (word == "int32") {
    return TokenKind::Int32;
  }
  if (word == "return") {
    return TokenKind::Return;
  }
  return TokenKind::Invalid;
}

}  // namespace

LexResult lex(const SourceManager& sources, SourceId source_id,
              DiagnosticEngine& diagnostics) {
  const auto input = sources.source_text(source_id);
  if (const auto* error = std::get_if<SourceError>(&input)) {
    return *error;
  }
  const auto text = std::get<std::string_view>(input);
  std::vector<Token> tokens;
  std::size_t cursor = 0;
  while (cursor < text.size()) {
    const auto byte = static_cast<unsigned char>(text[cursor]);
    if (byte == ' ' || byte == '\t' || byte == '\n' || byte == '\r') {
      ++cursor;
      continue;
    }

    const auto begin = cursor;
    auto kind = TokenKind::Invalid;
    std::string_view message = "unsupported byte in M1 Experimental Smoke Profile";
    if (pair_at(text, cursor, '/', '/')) {
      cursor += 2;
      while (cursor < text.size() && text[cursor] != '\n' &&
             !pair_at(text, cursor, '\r', '\n')) {
        ++cursor;
      }
      message = "unsupported comment-shaped input in M1 Experimental Smoke Profile";
    } else if (pair_at(text, cursor, '/', '*')) {
      // Search starts after the complete opener: its '*' cannot close the run.
      const auto closing = text.find("*/", cursor + 2);
      cursor = closing == std::string_view::npos ? text.size() : closing + 2;
      message = "unsupported comment-shaped input in M1 Experimental Smoke Profile";
    } else if (pair_at(text, cursor, '-', '>')) {
      kind = TokenKind::Arrow;
      cursor += 2;
    } else if (byte == '(' || byte == ')' || byte == '{' || byte == '}' || byte == ';') {
      switch (byte) {
        case '(':
          kind = TokenKind::LeftParen;
          break;
        case ')':
          kind = TokenKind::RightParen;
          break;
        case '{':
          kind = TokenKind::LeftBrace;
          break;
        case '}':
          kind = TokenKind::RightBrace;
          break;
        case ';':
          kind = TokenKind::Semicolon;
          break;
      }
      ++cursor;
    } else if (is_word_start(byte)) {
      ++cursor;
      while (cursor < text.size() &&
             is_word_continue(static_cast<unsigned char>(text[cursor]))) {
        ++cursor;
      }
      kind = word_kind(text.substr(begin, cursor - begin));
      message = "unsupported word in M1 Experimental Smoke Profile";
    } else if (starts_number(text, cursor)) {
      bool digits_only = true;
      if (byte == '+' || byte == '-') {
        digits_only = false;
        ++cursor;
      }
      while (cursor < text.size()) {
        const auto current = static_cast<unsigned char>(text[cursor]);
        if (is_word_continue(current) || current == '.') {
          digits_only = digits_only && is_digit(current);
          ++cursor;
        } else if ((current == '+' || current == '-') && cursor > begin &&
                   (text[cursor - 1] == 'e' || text[cursor - 1] == 'E' ||
                    text[cursor - 1] == 'p' || text[cursor - 1] == 'P') &&
                   text.size() - cursor >= 2 &&
                   is_digit(static_cast<unsigned char>(text[cursor + 1]))) {
          digits_only = false;
          ++cursor;
        } else {
          break;
        }
      }
      kind = digits_only ? TokenKind::IntegerLiteral : TokenKind::Invalid;
      message = "unsupported numeric spelling in M1 Experimental Smoke Profile";
    } else if (byte >= 0x80) {
      ++cursor;
      while (cursor < text.size() && static_cast<unsigned char>(text[cursor]) >= 0x80) {
        ++cursor;
      }
      message = "unsupported non-ASCII byte run in M1 Experimental Smoke Profile";
    } else {
      ++cursor;
    }

    const SourceSpan span{source_id, begin, cursor};
    tokens.push_back({kind, span});
    if (kind == TokenKind::Invalid) {
      diagnostics.emit({DiagnosticSeverity::Error, std::string{message}, span, {}});
    }
  }
  tokens.push_back({TokenKind::EndOfFile, {source_id, text.size(), text.size()}});
  return tokens;
}

}  // namespace veyr
