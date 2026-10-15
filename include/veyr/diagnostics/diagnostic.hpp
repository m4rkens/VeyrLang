#pragma once

#include <veyr/source/source_span.hpp>

#include <optional>
#include <string>
#include <vector>

namespace veyr {

enum class DiagnosticSeverity { Error, Warning, Note };

struct RelatedSpan {
  SourceSpan span;
  std::string label;
};

// Owns message/label bytes. Spans refer to the originating SourceManager;
// checked source resolution and rendering remain separate operations.
struct Diagnostic {
  DiagnosticSeverity severity;
  std::string message;
  std::optional<SourceSpan> primary;
  std::vector<RelatedSpan> related;
};

}  // namespace veyr
