#include <veyr/diagnostics/diagnostic_engine.hpp>

#include <algorithm>
#include <utility>

namespace veyr {

void DiagnosticEngine::emit(Diagnostic diagnostic) {
  diagnostics_.push_back(std::move(diagnostic));
}

std::span<const Diagnostic> DiagnosticEngine::diagnostics() const {
  return diagnostics_;
}

bool DiagnosticEngine::has_errors() const {
  return std::any_of(diagnostics_.begin(), diagnostics_.end(),
                     [](const Diagnostic& diagnostic) {
                       return diagnostic.severity == DiagnosticSeverity::Error;
                     });
}

}  // namespace veyr
