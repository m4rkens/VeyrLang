#pragma once

#include <veyr/source/source_span.hpp>

namespace veyr {

// Independent original-byte spans, with no token borrows or numeric value.
// Resolution requires the originating SourceManager to remain alive.
struct ParsedSmokeEntry {
  SourceSpan entry_span;
  SourceSpan return_span;
  SourceSpan integer_span;

  bool operator==(const ParsedSmokeEntry&) const = default;
};

}  // namespace veyr
