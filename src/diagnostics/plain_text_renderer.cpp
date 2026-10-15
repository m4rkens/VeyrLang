#include <veyr/diagnostics/plain_text_renderer.hpp>

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string_view>

namespace veyr {
namespace {

constexpr std::size_t max_lines = 3;
constexpr std::size_t max_bytes = 80;
constexpr std::size_t context_bytes = 40;

void append_escaped(std::string& output, std::string_view bytes) {
  constexpr char hex[] = "0123456789ABCDEF";
  for (const unsigned char byte : bytes) {
    switch (byte) {
      case '\t': output += "\\t"; break;
      case '\r': output += "\\r"; break;
      case '\n': output += "\\n"; break;
      default:
        if (byte >= 0x20 && byte <= 0x7e) {
          output += static_cast<char>(byte);
        } else {
          output += "\\x";
          output += hex[byte >> 4];
          output += hex[byte & 0x0f];
        }
    }
  }
}

std::string_view severity_name(DiagnosticSeverity severity) {
  switch (severity) {
    case DiagnosticSeverity::Error: return "error";
    case DiagnosticSeverity::Warning: return "warning";
    case DiagnosticSeverity::Note: return "note";
  }
  throw std::invalid_argument("Diagnostic severity must be initialized to a supported value");
}

std::optional<SourceError> append_location(std::string& output,
                                         const SourceManager& sources,
                                         SourceSpan span) {
  const auto name = sources.display_name(span.source_id);
  if (const auto* error = std::get_if<SourceError>(&name)) {
    return *error;
  }
  const auto location = sources.location(span.source_id, span.begin);
  if (const auto* error = std::get_if<SourceError>(&location)) {
    return *error;
  }
  const auto& value = std::get<SourceLocation>(location);
  append_escaped(output, std::get<std::string_view>(name));
  output += ':' + std::to_string(value.line) + ':' + std::to_string(value.column);
  return std::nullopt;
}

std::optional<SourceError> append_excerpt(std::string& output,
                                        const SourceManager& sources,
                                        SourceSpan primary) {
  auto offset = primary.begin;
  for (std::size_t row = 0; row < max_lines; ++row) {
    const auto checked_line = sources.line_span_at(primary.source_id, offset);
    if (const auto* error = std::get_if<SourceError>(&checked_line)) {
      return *error;
    }
    const auto line = std::get<SourceSpan>(checked_line);
    const auto checked_text = sources.text(line);
    if (const auto* error = std::get_if<SourceError>(&checked_text)) {
      return *error;
    }
    const auto checked_location = sources.location(primary.source_id, offset);
    if (const auto* error = std::get_if<SourceError>(&checked_location)) {
      return *error;
    }
    const auto bytes = std::get<std::string_view>(checked_text);
    const auto local_begin = offset - line.begin;
    const auto window_begin = row == 0 && local_begin > context_bytes
                                  ? local_begin - context_bytes : 0;
    const auto window_size = std::min(max_bytes, bytes.size() - window_begin);
    const auto window = bytes.substr(window_begin, window_size);

    // At most 81 byte boundaries and 320 escaped cells. Never allocate or
    // iterate markers according to an unbounded original source range.
    std::array<std::size_t, max_bytes + 1> cells{};
    std::string escaped;
    for (std::size_t index = 0; index < window.size(); ++index) {
      append_escaped(escaped, window.substr(index, 1));
      cells[index + 1] = escaped.size();
    }
    const auto first_byte = line.begin + window_begin;
    const auto last_byte = first_byte + window_size;
    const auto selected_begin = std::max(primary.begin, first_byte) - first_byte;
    const auto selected_end = std::min(primary.end, last_byte) - first_byte;
    const auto marker_begin = cells[selected_begin];
    const auto marker_size = std::max<std::size_t>(1, cells[selected_end] - marker_begin);
    const bool clipped_left = window_begin != 0;
    const bool clipped_right = window_size < bytes.size() - window_begin;
    const auto digits = std::to_string(std::get<SourceLocation>(checked_location).line);
    output += digits + " | ";
    if (clipped_left) output += "...";
    output += escaped;
    if (clipped_right) output += "...";
    output += '\n';
    output.append(digits.size(), ' ');
    output += " | ";
    output.append((clipped_left ? 3 : 0) + marker_begin, ' ');
    output += '^';
    output.append(marker_size - 1, '~');
    output += '\n';

    // Empty spans only show their containing line. A nonempty half-open span
    // ending at a line boundary never includes the following line.
    if (primary.begin == primary.end || primary.end <= line.end) {
      return std::nullopt;
    }
    offset = line.end;
  }
  output += "...\n";
  return std::nullopt;
}

}  // namespace

RenderResult PlainTextRenderer::render(const Diagnostic& diagnostic,
                                       const SourceManager& sources) const {
  // Validate all references before constructing any successful output. Preserve
  // the exact original range/error and primary-first, related-insertion order.
  if (diagnostic.primary) {
    const auto checked = sources.validate(*diagnostic.primary);
    if (const auto* error = std::get_if<SourceError>(&checked)) {
      return RenderError{*error, std::nullopt};
    }
  }
  for (std::size_t index = 0; index < diagnostic.related.size(); ++index) {
    const auto checked = sources.validate(diagnostic.related[index].span);
    if (const auto* error = std::get_if<SourceError>(&checked)) {
      return RenderError{*error, index};
    }
  }

  std::string output;
  if (diagnostic.primary) {
    if (const auto error = append_location(output, sources, *diagnostic.primary)) {
      return RenderError{*error, std::nullopt};
    }
    output += ": ";
  }
  output += severity_name(diagnostic.severity);
  output += ": ";
  append_escaped(output, diagnostic.message);
  output += '\n';
  if (diagnostic.primary) {
    if (const auto error = append_excerpt(output, sources, *diagnostic.primary)) {
      return RenderError{*error, std::nullopt};
    }
  }
  for (std::size_t index = 0; index < diagnostic.related.size(); ++index) {
    const auto& related = diagnostic.related[index];
    output += "note: ";
    append_escaped(output, related.label);
    output += " (";
    if (const auto error = append_location(output, sources, related.span)) {
      return RenderError{*error, index};
    }
    output += ")\n";
  }
  return output;
}

}  // namespace veyr
