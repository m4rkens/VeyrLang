# Phase 5 — Source-grounded Pattern Map

Baseline: 8e7bb1d8ce1f42cedfda06e498887507774635d9. Read-only mapping; no native
execution or implementation. Inline capability fallback, not an agent-run claim.

| Planned file | Closest actual analog | Assignment |
|---|---|---|
| include/veyr/ast/smoke_entry.hpp | parser.hpp ParsedSmokeEntry; source_span.hpp | Move the same three-field definition; source-span include only |
| include/veyr/parsing/parser.hpp | existing parser.hpp | Include shared header, remove duplicate definition; preserve every other declaration |
| include/veyr/semantics/smoke_validator.hpp | parser.hpp checked alternatives; diagnostic_engine.hpp private state | Own payload, distinct rejections/errors, restricted success construction |
| src/semantics/smoke_validator.cpp | parser.cpp checked source/diagnostic flow | Own header first; private helpers; no source reload, tokens or renderer |
| tests/ast/smoke_entry_test.cpp | tests/lexing/token_test.cpp | Compile the AST header without parser/token/semantic headers; value/field assertions |
| tests/semantics/smoke_validator_test.cpp | parser_test.cpp | Exact variant/error/diagnostic matrices with independent literal oracles |
| tests/semantics/smoke_validator_integration_test.cpp | parser_integration_test.cpp | Real components, scoped destruction, copied diagnostics and independent output |
| CMakeLists.txt | existing veyrcore source list | Add validator source to existing veyrcore only |
| tests/CMakeLists.txt | existing parser_tests/lexer_tests | ast_tests + semantic_tests, existing target policy/links and PRE_TEST |

## Concrete source excerpts and data flow

parser.hpp currently contains `struct ParsedSmokeEntry` with `SourceSpan
entry_span`, `return_span`, `integer_span` and defaulted `operator==`. Preserve
this exact public value in the shared header. Existing parse returns it only
after complete syntax/EOF recognition; no numeric payload is present.

parser.cpp uses `sources.validate(token.span)` then returns `*error` on
`std::get_if<SourceError>(&checked)`. Semantic validation applies the same
checked propagation to the three AST spans, with its own documented full-pass
priority rather than importing the parser's token-stream rules.

The existing explicit initializer is `Diagnostic error{DiagnosticSeverity::Error,
std::move(message), actual.span, {}}`. Semantic range rejection uses its literal
primary and fixed message with the same explicit severity/empty-related policy.

parser_integration_test.cpp's `copied_diagnostics` copies the engine view into
owning Diagnostic values. Its `expect_diagnostic` checks severity, full message,
primary and every related span/label. Follow that field-complete pattern; never
retain engine references across emit or derive expected text from production.

tests/CMakeLists.txt uses `veyr_target_policy(parser_tests)`, private links to
`veyrcore GTest::gtest_main GTest::gmock` and
`gtest_discover_tests(parser_tests DISCOVERY_MODE PRE_TEST)`. Replicate for the two
new native targets; preserve the existing parser target byte-for-byte.

Data flow: original manager → lexer tokens → parser → shared AST → semantic
validator → owned validated AST/constant; diagnostics → independent renderer.
AST and semantic unit TUs intentionally have no parser/token include dependency.
No pattern introduces generic factories, OOM policy, LLVM types or numeric libraries.

Platform impact:
- Windows: use the same policies and PRE_TEST, no executable/path spelling assumptions.
- Linux: use the same target structure and original-byte contracts.
- macOS future: deferred; no OS API is required.
