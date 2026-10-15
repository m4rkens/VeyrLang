#pragma once

#include <veyr/diagnostics/diagnostic.hpp>
#include <veyr/source/source_manager.hpp>

#include <cstddef>
#include <optional>
#include <string>
#include <variant>

namespace veyr {

struct RenderError {
  SourceError source_error;
  // Absent for an invalid primary; otherwise the offending related-span index.
  std::optional<std::size_t> related_index;
};

using RenderResult = std::variant<std::string, RenderError>;

// Stateless, synchronous rendering through the originating SourceManager.
// The caller must initialize diagnostic.severity. No diagnostic/source views
// are retained: success owns LF-terminated ASCII text, failure owns the original
// checked error. Allocation failures retain ordinary C++ exception semantics.
class PlainTextRenderer {
 public:
  [[nodiscard]] RenderResult render(const Diagnostic& diagnostic,
                                    const SourceManager& sources) const;
};

}  // namespace veyr
