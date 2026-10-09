#pragma once

#include <cstdint>

namespace veyr {

// Identifies a snapshot only within its originating SourceManager. Zero is invalid.
struct SourceId {
  std::uint64_t value = 0;

  bool operator==(const SourceId&) const = default;
};

}  // namespace veyr
