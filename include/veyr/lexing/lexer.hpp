#pragma once

#include <veyr/diagnostics/diagnostic_engine.hpp>
#include <veyr/lexing/token.hpp>
#include <veyr/source/source_manager.hpp>

#include <variant>
#include <vector>

namespace veyr {

using LexResult = std::variant<std::vector<Token>, SourceError>;

// Synchronously scans the accepted M1 Experimental Smoke Profile only. Success
// owns recovered tokens, not a compilation-success indication: clients must also
// inspect diagnostics.has_errors(). Existing diagnostics/order are preserved.
// Unknown source returns the original checked error, with no diagnostic or EOF.
// Source views are call-local; later lexeme resolution needs the originating
// SourceManager to remain alive. No diagnostic view is retained across emit().
// Allocation exceptions retain ordinary C++ semantics, without batch rollback.
[[nodiscard]] LexResult lex(const SourceManager& sources, SourceId source_id,
                            DiagnosticEngine& diagnostics);

}  // namespace veyr
