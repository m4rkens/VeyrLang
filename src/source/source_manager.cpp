#include <veyr/source/source_manager.hpp>

#include <cstdint>
#include <limits>
#include <utility>

namespace veyr {

struct SourceManager::SourceRecord {
  std::string display_name;
  std::string bytes;
};

SourceManager::SourceManager() = default;
SourceManager::~SourceManager() = default;

std::optional<RegistrationError> SourceManager::registration_capacity_error(
    std::size_t record_count, std::size_t source_size) noexcept {
  // Keep both the one-based ID and its index representable before any addition.
  if (record_count == std::numeric_limits<std::size_t>::max() ||
      record_count >= std::numeric_limits<std::uint64_t>::max()) {
    return RegistrationError{RegistrationErrorKind::IdCapacityExceeded};
  }
  // Reserve headroom for a future one-based EOF column/line.
  if (source_size == std::numeric_limits<std::size_t>::max()) {
    return RegistrationError{RegistrationErrorKind::SourceTooLarge};
  }
  return std::nullopt;
}

RegistrationResult SourceManager::register_source(std::string display_name,
                                                  std::string bytes) {
  if (const auto error = registration_capacity_error(sources_.size(), bytes.size())) {
    return *error;
  }
  const SourceId id{static_cast<std::uint64_t>(sources_.size()) + 1};
  auto record = std::make_unique<const SourceRecord>(
      SourceRecord{std::move(display_name), std::move(bytes)});
  sources_.push_back(std::move(record));
  return id;
}

const SourceManager::SourceRecord* SourceManager::lookup(SourceId id) const noexcept {
  if (id.value == 0 || id.value > sources_.size()) {
    return nullptr;
  }
  return sources_[static_cast<std::size_t>(id.value - 1)].get();
}

TextResult SourceManager::source_text(SourceId id) const {
  const auto* source = lookup(id);
  if (!source) {
    return SourceError{SourceErrorKind::UnknownSource, id, 0, 0, std::nullopt};
  }
  return std::string_view{source->bytes};
}

TextResult SourceManager::display_name(SourceId id) const {
  const auto* source = lookup(id);
  if (!source) {
    return SourceError{SourceErrorKind::UnknownSource, id, 0, 0, std::nullopt};
  }
  return std::string_view{source->display_name};
}

ValidationResult SourceManager::validate(SourceSpan span) const {
  const auto* source = lookup(span.source_id);
  if (!source) {
    return SourceError{SourceErrorKind::UnknownSource, span.source_id,
                       span.begin, span.end, std::nullopt};
  }
  if (span.begin > span.end) {
    return SourceError{SourceErrorKind::ReversedRange, span.source_id,
                       span.begin, span.end, source->bytes.size()};
  }
  if (span.end > source->bytes.size()) {
    return SourceError{SourceErrorKind::BeyondSource, span.source_id,
                       span.begin, span.end, source->bytes.size()};
  }
  return std::monostate{};
}

TextResult SourceManager::text(SourceSpan span) const {
  const auto validation = validate(span);
  if (const auto* error = std::get_if<SourceError>(&validation)) {
    return *error;
  }
  return std::string_view{lookup(span.source_id)->bytes}.substr(
      span.begin, span.end - span.begin);
}

}  // namespace veyr
