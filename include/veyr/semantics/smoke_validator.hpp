#pragma once

#include <veyr/ast/smoke_entry.hpp>
#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/source/source_manager.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <variant>

namespace veyr {

enum class AstField { Entry, Return, Integer };

enum class AstErrorKind {
  SourceMismatch,
  EmptySpan,
  NonNestedSpan,
  NonDecimalLiteral
};

// Checked caller-contract failure, not a source-language diagnostic.
struct AstError {
  AstErrorKind kind;
  AstField field;
  SourceSpan span;
  // Original absolute offset of the first nondigit; absent for other errors.
  std::optional<std::size_t> byte_offset;

  bool operator==(const AstError&) const = default;
};

enum class SemanticRejectionKind { OutOfRange, BlockedByErrors };

struct SemanticRejected {
  SemanticRejectionKind kind;

  bool operator==(const SemanticRejected&) const = default;
};

class ValidatedSmokeEntry;

// A default-constructed result is a rejection, never proof of validation.
using SemanticResult =
    std::variant<SemanticRejected, SourceError, AstError, ValidatedSmokeEntry>;

// Synchronous experimental M1 validation; caller must use the originating live
// SourceManager. Numeric SourceIds cannot detect a foreign manager on collision.
// Validate all entry/return/integer source spans before AST relationships, then
// complete ASCII digit conformance and the inclusive decimal range 0..255.
// Source/AST infrastructure failures emit nothing. OutOfRange appends one Error
// on the entire literal with no related spans; existing diagnostics persist.
// Valid input with accumulated Errors returns BlockedByErrors without emission.
// Every failure has no validated value. Notes/Warnings alone do not block success.
// These checks do not reparse surrounding source syntax: use the accepted parser
// or an equivalent conforming producer for the fixed complete program shape.
// No input AST/token/text/engine view is retained. Later compilation handoffs
// must still honor Errors added after this call. Ordinary allocation exceptions
// propagate; no new OOM, language exception or rollback guarantee is introduced.
[[nodiscard]] SemanticResult validate_smoke(const SourceManager& sources,
                                          const ParsedSmokeEntry& entry,
                                          DiagnosticEngine& diagnostics);

// Owns the complete common AST and constant. Only successful validation can
// create a payload; copies/moves preserve already validated values. Text resolution
// still needs the live originating manager. No final AST, ABI or numeric policy.
class ValidatedSmokeEntry {
 public:
  ValidatedSmokeEntry(const ValidatedSmokeEntry&) = default;
  ValidatedSmokeEntry(ValidatedSmokeEntry&&) = default;
  ValidatedSmokeEntry& operator=(const ValidatedSmokeEntry&) = default;
  ValidatedSmokeEntry& operator=(ValidatedSmokeEntry&&) = default;

  [[nodiscard]] ParsedSmokeEntry ast() const { return ast_; }
  [[nodiscard]] std::int32_t return_value() const { return return_value_; }

 private:
  ValidatedSmokeEntry(ParsedSmokeEntry ast, std::int32_t return_value)
      : ast_(ast), return_value_(return_value) {}

  ParsedSmokeEntry ast_;
  std::int32_t return_value_;

  friend SemanticResult validate_smoke(const SourceManager&, const ParsedSmokeEntry&,
                                       DiagnosticEngine&);
};

}  // namespace veyr
