#pragma once

#include <veyr/source/source_id.hpp>

#include <cstddef>

namespace veyr {

// Original byte offsets, half-open [begin, end); empty spans include EOF.
struct SourceSpan {
  SourceId source_id;
  std::size_t begin = 0;
  std::size_t end = 0;

  bool operator==(const SourceSpan&) const = default;
};

}  // namespace veyr
