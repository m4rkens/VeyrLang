#pragma once

#include <veyr/diagnostics/diagnostic.hpp>

#include <span>
#include <vector>

namespace veyr {

class DiagnosticEngine {
 public:
  DiagnosticEngine() = default;
  // Keep the collection-view lifetime tied to this engine and emit().
  DiagnosticEngine(const DiagnosticEngine&) = delete;
  DiagnosticEngine& operator=(const DiagnosticEngine&) = delete;
  DiagnosticEngine(DiagnosticEngine&&) = delete;
  DiagnosticEngine& operator=(DiagnosticEngine&&) = delete;

  void emit(Diagnostic diagnostic);
  // Preserves insertion order and duplicates. Reacquire after every emit;
  // views/references are valid only until the next emit or engine destruction.
  [[nodiscard]] std::span<const Diagnostic> diagnostics() const;
  [[nodiscard]] bool has_errors() const;

 private:
  std::vector<Diagnostic> diagnostics_;
};

}  // namespace veyr
