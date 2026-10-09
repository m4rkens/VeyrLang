# Phase 2 — Existing Patterns and Integration Map

Planning-only mapping, 2026-10-09. Generated inline under the repository's
explicit-request-only delegation policy; no agent execution or implementation.
The committed context/log/research are authoritative. Existing code supplies
integration conventions, not an existing source/diagnostic implementation.

## Closest analogs

`include/veyr/build_info.hpp` establishes the public namespace/include pattern:

```cpp
#pragma once
#include <string_view>
namespace veyr {
[[nodiscard]] std::string_view build_version() noexcept;
}
```

`src/core/build_info.cpp` includes its public header as `<veyr/build_info.hpp>`.
The returned build-version view has static backing; it is **not** evidence that
views into vector-owned strings remain stable. Phase 2 must use the researched
heap-stable immutable records and checked results instead.

Root `CMakeLists.txt` currently integrates core without an LLVM link:

```cmake
add_library(veyrcore STATIC src/core/build_info.cpp)
veyr_target_policy(veyrcore)
target_include_directories(veyrcore PUBLIC "${PROJECT_SOURCE_DIR}/include")
```

Extend this source list; do not create a second core library, provider system,
fetch route or configuration policy. Existing top-level LLVM discovery stays.

`tests/CMakeLists.txt` supplies the reusable native test pattern:

```cmake
add_executable(foundation_tests infrastructure/build_info_test.cpp)
veyr_target_policy(foundation_tests)
target_link_libraries(foundation_tests PRIVATE veyrcore GTest::gtest_main GTest::gmock)
gtest_discover_tests(foundation_tests DISCOVERY_MODE PRE_TEST)
```

Use this exact policy/link/discovery structure for `source_tests` and
`diagnostic_tests`. `tests/infrastructure/build_info_test.cpp` shows direct
GoogleTest expectations and existing GoogleMock availability. Pure source and
renderer checks need direct values/strings, not mocks of the implementation.
No additional test framework, golden-file service or property-test package.

## Planned files and data flow

| Files | Role / closest analog | Owner |
|---|---|---|
| include/veyr/source/source_id.hpp, source_span.hpp | Small public value headers; build_info.hpp namespace/include style | 02-01 |
| include/veyr/source/source_manager.hpp; src/source/source_manager.cpp | Owned source service; public-header/translation-unit seam as build_info, new checked storage semantics from research | 02-01; indexed lookup extended by 02-02 |
| include/veyr/source/source_location.hpp | Byte location value; no existing location implementation to copy | 02-02 |
| tests/source/source_manager_test.cpp | Ownership/span/location value regressions; infrastructure/build_info_test.cpp test style | 02-01/02-02 |
| include/veyr/diagnostics/diagnostic.hpp, diagnostic_engine.hpp; src/diagnostics/diagnostic_engine.cpp | Owned values and insertion-order accumulator; new domain logic, existing header/source structure | 02-03 |
| tests/diagnostics/diagnostic_test.cpp | Error state, ownership, order and duplicate checks | 02-03 |
| include/veyr/diagnostics/plain_text_renderer.hpp; src/diagnostics/plain_text_renderer.cpp | Checked returned-string renderer; no terminal/backend framework analog | 02-04 |
| tests/diagnostics/plain_text_renderer_test.cpp | Exact output + independent bounds/byte/control assertions; no external fixture files | 02-04 |
| CMakeLists.txt | Append production sources to veyrcore only; retain all existing targets and ABI policy | 02-01/02-03/02-04 sequentially |
| tests/CMakeLists.txt | Add two policy-bound native test targets; append renderer cases to diagnostic_tests | 02-01/02-03/02-04 sequentially |

Data flow: owned source snapshots / SourceId / SourceSpan → checked text and
locations → owned diagnostic values and accumulator → checked plain renderer.
Source APIs do not import diagnostics. Diagnostic values do not import renderer,
LLVM or driver. Renderer resolves spans through the supplied originating manager.
No reverse dependency, filesystem loading, lexer/module identity or CLI work.

## Reuse and exclusions

- `scripts/dev.py`: existing explicit setup, configure/build/test/verify chaining,
  selected build directory and exact compile-database identity. Reuse unchanged.
- `scripts/check_includes.py`: same owned-header guardrail; new headers use
  `<veyr/...>`. No new formatter/checker framework.
- Existing Build foundation CI: independently reviewed Ubuntu/Windows Debug and
  Release, shared commands and portable Conan snapshot cache. Publication stays
  owner-gated; planning does not run CI or confer acceptance on new code.
- `src/driver/main.cpp`: remains version-only. No fake compiler success/exit path
  to exercise diagnostics; test the library handoff/error state directly.
- No source/diagnostic implementation exists yet. No generated code, database,
  ORM/schema push, AI or UI contract is required. No codebase-map artifact was
  found; this file does not claim one or import old Veyr evidence.

## Design probe planning packet

Project/probe pressure: P01 requires durable text, spans and failures for the
walking skeleton; PLAT_01/PLAT_03 require exact metadata and portable plain output.
Minimal task: in-memory snapshots through checked spans/locations into ordered
structured/plain diagnostics. Capability forced: stable compiler provenance and
useful deterministic failures. Semantics first: D-01–D-16 stay settled; exact
C++ API and bounded renderer layout are explicit plan proposals. Interactions:
no final ownership/error/encoding semantics, no module graph or LLVM coupling.
Open questions: none blocking within Phase 2 scope after bounded proposals;
owner reviews those proposals before execution. Non-goals: lexer/syntax,
filesystem/VFS, incremental engine, Unicode width, colors and reporting plugins.
M1 fit: substrate only; Phase 3 smoke-profile gate remains mandatory.
DECISION_LOG update required: no new final language decision is proposed.

Platform impact:
- Windows: Encode CRLF/path/control fixtures explicitly; existing C++23 policy/CRT and CI remain untouched. Developer-host confirmation independently pending/non-blocking.
- Linux: Primary Omarchy development plus separate Ubuntu Clang 18/GCC-13 CI. Local variant APIs avoid std::expected compatibility changes.
- macOS future: Deferred; standard C++ byte/string APIs avoid host filesystem/terminal assumptions.
