#pragma once

#include <veyr/source/source_span.hpp>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace veyr {

enum class SourceErrorKind { UnknownSource, ReversedRange, BeyondSource };

struct SourceError {
  SourceErrorKind kind;
  SourceId source_id;
  std::size_t begin;
  std::size_t end;
  std::optional<std::size_t> source_size;

  bool operator==(const SourceError&) const = default;
};

enum class RegistrationErrorKind { IdCapacityExceeded, SourceTooLarge };

struct RegistrationError {
  RegistrationErrorKind kind;

  bool operator==(const RegistrationError&) const = default;
};

using RegistrationResult = std::variant<SourceId, RegistrationError>;
using TextResult = std::variant<std::string_view, SourceError>;
using ValidationResult = std::variant<std::monostate, SourceError>;

// Owns immutable snapshots. Returned text/name views remain valid until destruction,
// including across registration of further snapshots. No filesystem operations.
class SourceManager {
 public:
  SourceManager();
  ~SourceManager();
  SourceManager(const SourceManager&) = delete;
  SourceManager& operator=(const SourceManager&) = delete;
  SourceManager(SourceManager&&) = delete;
  SourceManager& operator=(SourceManager&&) = delete;

  [[nodiscard]] RegistrationResult register_source(std::string display_name,
                                                   std::string bytes);
  [[nodiscard]] TextResult source_text(SourceId id) const;
  [[nodiscard]] TextResult display_name(SourceId id) const;
  [[nodiscard]] TextResult text(SourceSpan span) const;
  [[nodiscard]] ValidationResult validate(SourceSpan span) const;

 private:
  struct SourceRecord;
  std::vector<std::unique_ptr<const SourceRecord>> sources_;

  [[nodiscard]] const SourceRecord* lookup(SourceId id) const noexcept;
  [[nodiscard]] static std::optional<RegistrationError> registration_capacity_error(
      std::size_t record_count, std::size_t source_size) noexcept;
  friend class SourceManagerTestAccess;
};

}  // namespace veyr
