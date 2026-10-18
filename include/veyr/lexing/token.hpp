#pragma once

#include <veyr/source/source_span.hpp>

namespace veyr {

// Spellings belong only to the owner-reviewed M1 Experimental Smoke Profile.
enum class TokenKind {
  Fn,
  Main,
  Int32,
  Return,
  LeftParen,
  RightParen,
  Arrow,
  LeftBrace,
  RightBrace,
  Semicolon,
  IntegerLiteral,
  Invalid,
  EndOfFile
};

// Initialize both fields. Resolve original bytes through the originating
// SourceManager, which must outlive that resolution. No lexeme/value is cached.
struct Token {
  TokenKind kind;
  SourceSpan span;

  bool operator==(const Token&) const = default;
};

}  // namespace veyr
