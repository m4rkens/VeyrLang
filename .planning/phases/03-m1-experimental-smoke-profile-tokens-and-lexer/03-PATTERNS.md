# Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer - Pattern Map

**Mapped:** 2026-10-15
**Files classified:** 8 candidate new/modified files
**Analogs found:** 8 / 8 for file structure; no existing lexer algorithm

This map consumes `03-CONTEXT.md` and `03-RESEARCH.md`. File names and API names
below remain planning proposals. All fourteen discussion decisions remain locked;
research's mixed-input recovery rules remain proposals for the separately reviewed
profile. No implementation, filled profile, tests or targets were created here.

## File Classification

| New/Modified File | Role | Data Flow | Closest Analog | Match Quality |
|---|---|---|---|---|
| `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md` | config/documented contract | request-response: owner review | `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE_TEMPLATE.md` | exact template |
| `include/veyr/lexing/token.hpp` | model | transform: source bytes to kind/span value | `include/veyr/source/source_span.hpp`; `include/veyr/diagnostics/diagnostic.hpp` | role-match |
| `include/veyr/lexing/lexer.hpp` | service/API | batch transform: checked source to recovered tokens | `include/veyr/diagnostics/plain_text_renderer.hpp`; `include/veyr/source/source_manager.hpp` | role-match |
| `src/lexing/lexer.cpp` | component | batch transform with diagnostic accumulation | `src/source/source_manager.cpp`; `src/diagnostics/diagnostic_engine.cpp` | partial: checked input/output, not scanning algorithm |
| `tests/lexing/token_test.cpp` | test | batch validation of values/lifetimes | `tests/diagnostics/diagnostic_test.cpp`; `tests/source/source_manager_test.cpp` | role-match |
| `tests/lexing/lexer_test.cpp` | test | batch validation of bytes/recovery/integration | `tests/source/source_manager_test.cpp`; `tests/diagnostics/plain_text_renderer_test.cpp` | role-match |
| `CMakeLists.txt` | config | generated native build graph | existing `veyrcore` source list in same file | exact extension |
| `tests/CMakeLists.txt` | config | native test discovery | existing `diagnostic_tests` definition in same file | exact extension |

The profile is documentation, not an executable configuration system. These role
labels do not authorize a new controller/provider/framework or file-I/O layer.

## Pattern Assignments

### `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md`

**Analog:** `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE_TEMPLATE.md`.
Use its warning, purpose, program-shape, spelling, lexical-decision, unsupported
construct, test-name and diagnostics sections as the document skeleton.

**Non-final contract** (lines 11–17):

```markdown
## 1. Warning

The M1 Experimental Smoke Profile is milestone-local and non-final.

Implementation support for this profile does not mean the spelling is accepted Veyr grammar.

Final Veyr syntax and semantics require separate design review and decision-log entry.
```

Preserve this warning while filling D-01–D-14 and the exact mixed-input recovery
table. In particular, `+12`, `1..2`, comment adjacency and non-ASCII inside comments
need deterministic approved boundaries. Research recommendations are not additional
owner decisions. The first implementation wave must remain documentation-only:
author profile → stop for owner review → separately authorized dedicated profile
commit → separate publication approval/push → fresh actual remote containment/
content and ancestry verification if advanced → stop for fresh execution
authorization before syntax-dependent code/tests. Neither
test scaffolding nor a token spelling table may bypass this gate.

### `include/veyr/lexing/token.hpp`

**Analog:** `include/veyr/source/source_span.hpp` (lines 1–15):

```cpp
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
```

Copy the small namespaced value/header style and canonical angle-bracket includes,
not these offset fields into Token. Token has exactly `TokenKind` and `SourceSpan`;
its header includes `<veyr/source/source_span.hpp>`. The scoped enum pattern comes
from `diagnostic.hpp:11` (`enum class DiagnosticSeverity { Error, Warning, Note };`).
Use profile-specific token kinds, including Invalid and EOF, without lexeme strings,
parsed integer values, cached locations or diagnostic payloads. Equality is a small
implementation convenience if selected by planning; it is not a new semantic field.

### `include/veyr/lexing/lexer.hpp`

**Analogs:** `plain_text_renderer.hpp:19–28` and `source_manager.hpp:36–40,53–58`.

```cpp
using RenderResult = std::variant<std::string, RenderError>;

// Stateless, synchronous rendering through the originating SourceManager.
// The caller must initialize diagnostic.severity. No diagnostic/source views
// are retained: success owns LF-terminated ASCII text, failure owns the original
// checked error. Allocation failures retain ordinary C++ exception semantics.
class PlainTextRenderer {
 public:
  [[nodiscard]] RenderResult render(const Diagnostic& diagnostic,
                                    const SourceManager& sources) const;
};
```

Copy the synchronous checked-result and documented ownership style, not renderer
formatting or `RenderError`. Research proposes `LexResult =
std::variant<std::vector<Token>, SourceError>` and `lex(const SourceManager&,
SourceId, DiagnosticEngine&)`. Keep the originating manager requirement explicit,
the output token vector owning, and the source borrow call-local. Lexical Errors
still yield recovered tokens; unknown SourceId returns the original SourceError.
No token success alternative may be described as successful compilation.

### `src/lexing/lexer.cpp`

**Checked error propagation:** `src/source/source_manager.cpp:91–98`:

```cpp
TextResult SourceManager::text(SourceSpan span) const {
  const auto validation = validate(span);
  if (const auto* error = std::get_if<SourceError>(&validation)) {
    return *error;
  }
  return std::string_view{lookup(span.source_id)->bytes}.substr(
      span.begin, span.end - span.begin);
}
```

Apply that exact variant-discrimination pattern to the public `source_text(id)`
result once, before scanning. Do not call private lookup or duplicate SourceManager
validation. Preserve the exact SourceError, with no fabricated token/EOF or language
diagnostic on unknown source.

**Owning emission call-site:** `tests/diagnostics/diagnostic_test.cpp:56–65`:

```cpp
  DiagnosticEngine engine;
  const SourceSpan primary{{17}, 3, 8};
  engine.emit({DiagnosticSeverity::Error, "source-linked", primary, {}});
  const auto values = engine.diagnostics();
  ASSERT_EQ(values.size(), 1u);
  ASSERT_TRUE(values[0].primary);
  EXPECT_EQ(*values[0].primary, primary);
  EXPECT_TRUE(values[0].related.empty());
  EXPECT_TRUE(engine.has_errors());
```

Use an explicit initialized Error, a fixed M1-specific owned message, exact offending
primary span and empty related list where unnecessary. Each rejected run emits one
Invalid token and one Error. `diagnostic_engine.cpp:8–9` already moves each owning
Diagnostic into its vector; `:16–20` derives persistent Error state. Do not add an
error accumulator or clear prior errors. Reacquire borrowed diagnostic views after
emissions (`diagnostic_engine.hpp:19–23`).

There is no existing scanner to copy. Implement only the approved profile's ordered,
unsigned-byte scan, bounded lookahead and guaranteed cursor advancement. Never use
locale-sensitive character classification, NUL termination, numeric conversion,
newline normalization, parser/AST/LLVM dependencies or a lexer-generator framework.

### `tests/lexing/token_test.cpp`

**Type-contract analog:** `tests/diagnostics/diagnostic_test.cpp:24–29`:

```cpp
static_assert(std::is_same_v<decltype(Diagnostic::message), std::string>);
static_assert(std::is_same_v<decltype(Diagnostic::primary), std::optional<SourceSpan>>);
static_assert(std::is_same_v<decltype(Diagnostic::related), std::vector<RelatedSpan>>);
static_assert(std::is_same_v<decltype(RelatedSpan::label), std::string>);
static_assert(std::is_same_v<decltype(std::declval<const DiagnosticEngine&>().diagnostics()),
                             std::span<const Diagnostic>>);
```

Adapt to the exact two Token field types and result type, without copying unrelated
DiagnosticEngine move restrictions onto token values. Resolve lexemes only through
`sources.text(token.span)` and assert exact original bytes.

**Lifetime analog:** `tests/source/source_manager_test.cpp:90–118` saves views,
registers 3000 sources and checks both bytes and `.data()` identity. Apply this to
tokens from an old immutable snapshot after newer registration, preserving the
originating-manager contract. Its `:120–132` covers empty/full/EOF spans and
`:175–181` proves CRLF-split ranges retain bytes. No filesystem fixture is needed.

### `tests/lexing/lexer_test.cpp`

**Fixture structure:** use canonical `<veyr/...>` includes, `<gtest/gtest.h>`, an
anonymous namespace and local helpers as in `diagnostic_test.cpp:1–23` and
`plain_text_renderer_test.cpp:32–57`. Use M1/SmokeProfile test names, not claims
about final Veyr grammar (`M1_EXPERIMENTAL_SMOKE_PROFILE_TEMPLATE.md:89–101`).

**Table-based exact boundary assertions:** `source_manager_test.cpp:269–282`:

```cpp
  for (const auto& item : cases) {
    SCOPED_TRACE(testing::Message() << "bytes=" << testing::PrintToString(item.bytes));
    const auto id = register_source(manager, "<boundary>", item.bytes);
    ASSERT_EQ(item.positions.size(), item.bytes.size() + 1);
    for (std::size_t offset = 0; offset <= item.bytes.size(); ++offset) {
      SCOPED_TRACE(testing::Message() << "offset=" << offset);
      const auto result = manager.location(id, offset);
      ASSERT_TRUE(std::holds_alternative<veyr::SourceLocation>(result));
      const veyr::SourceLocation expected{
          id, offset, item.positions[offset].first, item.positions[offset].second};
      EXPECT_EQ(std::get<veyr::SourceLocation>(result), expected);
    }
    EXPECT_EQ(source_text(manager, id), item.bytes);
```

Adapt the table/original-byte assertions to full expected kind/span/lexeme sequences,
Error pairing/order and source-end EOF. Assert variant alternatives before access.
Use independently written expected tables, not a second copy of scanner branches.
Include all-byte inputs as in `plain_text_renderer_test.cpp:192–206`; use explicit
byte lengths for embedded NUL as in `source_manager_test.cpp:153–169`.

**Integration and failure analogs:** `diagnostic_test.cpp:108–118` proves Error state
persists after later notes/warnings; `:238–268` compares original checked input
failures. `plain_text_renderer_test.cpp:381–413` consumes an engine view only while
no emit occurs, preserves source/order/duplicates and keeps Errors after render
failure. Reuse the existing renderer only at the client-test boundary; the lexer
must not depend on it. Test unknown SourceId with unchanged engine, preexisting
Errors, repeated lexing, old snapshots and exact recovery after malformed input.

### `CMakeLists.txt`

**Exact extension point:** existing lines 43–49:

```cmake
add_library(veyrcore STATIC
  src/core/build_info.cpp
  src/diagnostics/diagnostic_engine.cpp
  src/diagnostics/plain_text_renderer.cpp
  src/source/source_manager.cpp)
veyr_target_policy(veyrcore)
target_include_directories(veyrcore PUBLIC "${PROJECT_SOURCE_DIR}/include")
```

Add `src/lexing/lexer.cpp` to this source list only after the profile gate. Do not
create a new library, link lexer code to LLVM or alter provider/profile/build-path
policy. Existing `veyrcore` already exports the canonical include root.

### `tests/CMakeLists.txt`

**Exact executable/discovery analog:** existing lines 19–24:

```cmake
add_executable(diagnostic_tests
  diagnostics/diagnostic_test.cpp
  diagnostics/plain_text_renderer_test.cpp)
veyr_target_policy(diagnostic_tests)
target_link_libraries(diagnostic_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(diagnostic_tests DISCOVERY_MODE PRE_TEST)
```

Use one analogous `lexer_tests` executable for token/lexer test translation units,
the same target policy, existing dependencies and PRE_TEST discovery. Keep source,
diagnostic, CRT/ABI and include-policy tests intact. No new package or CI framework.

## Shared Patterns

- **Checked infrastructure failures:** exact `SourceError` via variant, no assertions
  as the sole runtime check; unknown source is distinct from a lexical Error.
- **Ownership:** stable SourceManager-owned bytes, kind/span tokens, owning token
  vector and owning diagnostics. SourceManager must outlive later token resolution;
  do not claim IDs from another manager can be detected when numeric IDs coincide.
- **Error semantics:** use existing `has_errors()`; recovered tokens never erase
  prior/new Errors or authorize successful compilation. Final driver enforcement
  remains later-phase work, not a driver implementation added here.
- **Imports/style:** `<veyr/...>` before standard headers; public headers use
  `#pragma once`, namespace `veyr` and `[[nodiscard]]` checked APIs. No quoted or
  relative Veyr includes.
- **Validation boundaries:** preserve original half-open byte offsets, LF/CRLF and
  lone-CR location mapping, and exact EOF. The lexer skips four approved whitespace
  bytes; it does not change SourceManager's location rules.
- **No auth/logging layer:** this in-process compiler component has no authentication,
  middleware, filesystem lookup, terminal output or acquisition responsibility.
- **Execution evidence later:** follow existing explicit doctor/configure/build/test/
  verify/include-check contracts under separate execution authorization; record actual
  independent Debug/Release results, never infer new tests passed from Phase 2 logs.

## No Analog Found

| File / responsibility | Role | Data Flow | Reason / planner action |
|---|---|---|---|
| `src/lexing/lexer.cpp`: scanning precedence and recovery | component | byte-stream transform | No token/lexer exists. Use research's bounded proposal only after exact profile review and dedicated commit; surrounding checked-source/diagnostic patterns above are established. |

No other missing analog requires new infrastructure. Do not search external
frameworks to fill this intentionally small algorithmic gap.

## Metadata

**Analog search scope:** `include/veyr/source`, `include/veyr/diagnostics`,
`src/source`, `src/diagnostics`, `tests/source`, `tests/diagnostics`, root/test CMake
integration and the existing smoke-profile template. 13 relevant code/header/test/
template/build files read, plus context/research and applicable policies/skill indexes.
Five strong behavioral analog groups were sufficient: span values, checked source
resolution, owning diagnostic emission, source boundary/lifetime tests and renderer
integration tests. Supporting public headers/template/CMake provide exact wiring.

**Coverage:** 3 exact template/extension assignments, 4 role matches, 1 partial
implementation match. File names remain the eight candidate files from research.

Platform impact:
- Windows: unsigned original-byte scanning and existing native target/CRT policies;
  hosted CI must validate new code later, developer-host inspection stays independent.
- Linux: primary Omarchy development and independent Ubuntu CI remain unchanged;
  no locale-dependent byte classes or filesystem fixtures.
- macOS future: deferred; no new platform API or newline conversion introduced.

Only this pattern map was written. No compiler, CMake, CTest, Conan, dependency,
commit, push or CI operation was run by the pattern mapper.
