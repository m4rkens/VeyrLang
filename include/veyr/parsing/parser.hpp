#pragma once

#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/lexing/token.hpp>
#include <veyr/source/source_manager.hpp>

#include <cstddef>
#include <optional>
#include <span>
#include <variant>

namespace veyr {

// Independent original-byte spans, with no token borrows or numeric value.
// Resolution requires the originating SourceManager to remain alive.
struct ParsedSmokeEntry {
  SourceSpan entry_span;
  SourceSpan return_span;
  SourceSpan integer_span;

  bool operator==(const ParsedSmokeEntry&) const = default;
};

enum class ParseRejectionKind { LexicalErrors, SyntaxError };

struct ParseRejected {
  ParseRejectionKind kind;

  bool operator==(const ParseRejected&) const = default;
};

enum class TokenStreamErrorKind {
  SourceMismatch,
  UnsupportedTokenKind,
  EmptyTokenSpan,
  OutOfOrderOrOverlappingSpan,
  NonFinalEndOfFile,
  InvalidEndOfFileSpan,
  MissingEndOfFile
};

struct TokenStreamError {
  TokenStreamErrorKind kind;
  SourceId source_id;  // The selected source.
  std::optional<std::size_t> token_index;  // Absent only for MissingEndOfFile.

  bool operator==(const TokenStreamError&) const = default;
};

using ParseResult =
    std::variant<ParsedSmokeEntry, ParseRejected, SourceError, TokenStreamError>;

// Synchronously recognizes only the M1 Experimental Smoke Profile. The caller
// supplies initialized, completed tokens from lex() or an equivalent conforming
// producer, with token storage valid throughout this call. Classification,
// spelling and lexical boundaries are producer preconditions, not rechecked here.
// Use the originating manager: coincident numeric IDs cannot detect a foreign one.
// Whole-stream structural checks precede Invalid rejection and syntax matching.
// Infrastructure/lexical failures emit nothing; the first syntax mismatch appends
// one Error. Only complete recognition yields a value; failures expose no partial
// result. Default aggregate/variant construction does not validate syntax.
// Prior diagnostics persist; prior Errors do not gate an independent valid stream.
// A syntax value does not establish compilation success: also inspect has_errors().
// No token/source/diagnostic view is retained. Ordinary C++ allocation exceptions
// propagate without batch rollback or a new language allocation/exception policy.
[[nodiscard]] ParseResult parse(const SourceManager& sources, SourceId source_id,
                                std::span<const Token> tokens,
                                DiagnosticEngine& diagnostics);

}  // namespace veyr
