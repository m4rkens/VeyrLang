#pragma once

#include <veyr/source/source_id.hpp>

#include <cstddef>

namespace veyr {

// Offset is an original-byte position; line and column are one-based bytes.
struct SourceLocation {
  SourceId source_id;
  std::size_t offset;
  std::size_t line;
  std::size_t column;

  bool operator==(const SourceLocation&) const = default;
};

}  // namespace veyr
