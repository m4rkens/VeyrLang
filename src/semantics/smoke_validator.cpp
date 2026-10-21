#include <veyr/semantics/smoke_validator.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>
#include <variant>

namespace veyr {

SemanticResult validate_smoke(const SourceManager& sources,
                              const ParsedSmokeEntry& entry,
                              DiagnosticEngine& diagnostics) {
  const std::array spans = {
      std::pair{AstField::Entry, entry.entry_span},
      std::pair{AstField::Return, entry.return_span},
      std::pair{AstField::Integer, entry.integer_span}};

  // Finish the source-validation pass before any AST relationship checks.
  for (const auto& [field, span] : spans) {
    const auto checked = sources.validate(span);
    if (const auto* error = std::get_if<SourceError>(&checked)) {
      return *error;
    }
  }
  for (std::size_t index = 1; index < spans.size(); ++index) {
    const auto& [field, span] = spans[index];
    if (span.source_id != entry.entry_span.source_id) {
      return AstError{AstErrorKind::SourceMismatch, field, span, std::nullopt};
    }
  }
  for (const auto& [field, span] : spans) {
    if (span.begin == span.end) {
      return AstError{AstErrorKind::EmptySpan, field, span, std::nullopt};
    }
  }
  if (entry.entry_span.begin >= entry.return_span.begin ||
      entry.return_span.end >= entry.entry_span.end) {
    return AstError{AstErrorKind::NonNestedSpan, AstField::Return,
                    entry.return_span, std::nullopt};
  }
  if (entry.return_span.begin >= entry.integer_span.begin ||
      entry.integer_span.end >= entry.return_span.end) {
    return AstError{AstErrorKind::NonNestedSpan, AstField::Integer,
                    entry.integer_span, std::nullopt};
  }

  const auto text = sources.text(entry.integer_span);
  if (const auto* error = std::get_if<SourceError>(&text)) {
    return *error;
  }
  const auto literal = std::get<std::string_view>(text);
  std::uint32_t value = 0;
  bool out_of_range = false;
  for (std::size_t index = 0; index < literal.size(); ++index) {
    const auto byte = static_cast<unsigned char>(literal[index]);
    if (byte < static_cast<unsigned char>('0') ||
        byte > static_cast<unsigned char>('9')) {
      // A checked span bounds begin+index by its original end offset.
      return AstError{AstErrorKind::NonDecimalLiteral, AstField::Integer,
                      entry.integer_span, entry.integer_span.begin + index};
    }
    const auto digit = static_cast<std::uint32_t>(byte - static_cast<unsigned char>('0'));
    if (!out_of_range) {
      if (value > 25 || (value == 25 && digit > 5)) {
        out_of_range = true;
      } else {
        value = value * 10 + digit;  // Always in [0,255]; no host overflow.
      }
    }
    // Even a known excessive prefix must not hide a later malformed byte.
  }

  if (out_of_range) {
    diagnostics.emit({DiagnosticSeverity::Error,
                      "return value must be between 0 and 255 in M1 Experimental Smoke Profile",
                      entry.integer_span, {}});
    return SemanticRejected{SemanticRejectionKind::OutOfRange};
  }
  if (diagnostics.has_errors()) {
    return SemanticRejected{SemanticRejectionKind::BlockedByErrors};
  }
  return ValidatedSmokeEntry{entry, static_cast<std::int32_t>(value)};
}

}  // namespace veyr
