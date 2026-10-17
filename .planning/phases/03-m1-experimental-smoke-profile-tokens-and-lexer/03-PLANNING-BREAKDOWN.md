# Phase 3 — Approved Planning Breakdown and Source Audit

**Scope:** M1 Experimental Smoke Profile, Tokens, and Lexer.
**Lifecycle:** Planning only; execution **0/3**. No profile/source/test/CMake
implementation, native command, new dependency or CI action performed.
**Breakdown approval:** Owner explicitly approved the following exact three-plan,
three-wave structure after the installed GSD interactive confirm_breakdown gate.
This approval permits plan creation/checking, not execution or planning commits.

## Plans, dependencies and ownership

| Plan / wave | Needs | Creates / proves | Tasks | Checkpoints / next gate |
|---|---|---|---|---|
| 03-01 / 1 | Accepted Phase 2; context14 decisions; research proposal; existing profile template | Filled future M1 profile and actual review/commit/publication checkpoint | 1 author documentation; 2 blocking owner gate | Owner review → explicit dedicated commit approval/verification → separate publication approval/push → actual remote containment/content evidence → stop for fresh scoped 03-02 authorization |
| 03-02 / 2 | Exact accepted profile/dedicated SHA, approved successful publication, fresh actual authorized-branch containment and ancestry/content if advanced, accepted 03-01 closeout and fresh Wave 2 authorization | Minimal TokenKind/Token, checked synchronous complete lexer, owned Errors, focused tests and existing CMake wiring | 1 model/native target; 2 complete scanner/recovery and tests | Stop if any prerequisite is unverifiable; no local-commit/stale-ref shortcut or automatic publish/execute; later owner implementation/evidence review and separate closeout/publication; no automatic 03-03 |
| 03-03 / 3 | Complete reviewed 03-02 and unchanged committed profile | Expanded hostile-byte/integration tests; final-code independent Debug/Release evidence and owner packet | 1 adversarial/integration tests; 2 native evidence/owner acceptance boundary | Publication, new four-job hosted evidence, canonical goal verification and final phase acceptance remain separate explicit owner steps |

Dependency edges: **03-01 → 03-02 → 03-03**. The 03-01 edge includes actual
owner-approved profile publication, live remote-state verification and required
ancestry/content checks, not merely local summary/commit existence. Wave numbers are precomputed from
those edges. No same-wave parallelism or file conflicts. 03-02 creates the two
test files before 03-03 extends them. Task 03-02-01 lists only token_test.cpp in
the new lexer_tests target; 03-02-02 adds lexer_test.cpp when created. This avoids
a build referring to a nonexistent future file. The final plan adds no framework
or production subsystem and performs no new package work.

`autonomous: false` and blocking Task 03-01-02 protect the profile gate. Later
implementation plans' `autonomous: true` permits their scoped local tasks only;
the repository's explicit owner review/commit/next-command boundaries still
override GSD automatic commits, summary acceptance and phase advancement.

## Exact planned API and files

Namespace `veyr`: TokenKind has Fn, Main, Int32, Return, LeftParen, RightParen,
Arrow, LeftBrace, RightBrace, Semicolon, IntegerLiteral, Invalid and EndOfFile.
Token has exactly `TokenKind kind` and `SourceSpan span`; no other payload.
LexResult is `std::variant<std::vector<Token>, SourceError>`;
`lex(const SourceManager&, SourceId, DiagnosticEngine&)` is synchronous and
[[nodiscard]]. Returned tokens are recovered output, never compilation success.
Checked infrastructure failure preserves the existing SourceError; lexical
Errors accumulate in the existing owning engine. Future compilation callers
must consult has_errors; no driver-success implementation belongs to Phase 3.

| File | Owning creation task | Later modification |
|---|---|---|
| docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md | 03-01-01 | Owner-reviewed corrections only, before dependent execution |
| include/veyr/lexing/token.hpp | 03-02-01 | None planned |
| tests/lexing/token_test.cpp | 03-02-01 | 03-03-01 expanded lifetime/integration coverage |
| tests/CMakeLists.txt | 03-02-01 lexer_tests with token source | 03-02-02 adds lexer test source |
| include/veyr/lexing/lexer.hpp | 03-02-02 | None planned |
| src/lexing/lexer.cpp | 03-02-02 complete scanner | None planned; a new defect needs bounded owner approval |
| tests/lexing/lexer_test.cpp | 03-02-02 | 03-03-01 expanded adversarial/integration coverage |
| CMakeLists.txt | 03-02-02 adds existing veyrcore source | None planned |

Normal per-plan execution/checkpoint/summary records follow existing lifecycle
conventions only under their respective authorizations; local logs/manifests
remain ignored. No profile or implementation file is created during planning.

## Multi-source coverage audit

Every row is **COVERED in planned tasks**, not implemented or accepted evidence.
Deferred capabilities in CONTEXT/RESEARCH and later roadmap phases are excluded
intentionally, not silently omitted. All six LEX requirements remain Pending.

### GOAL — roadmap outcome and all five acceptance criteria

| Source ID | Outcome | Owning task(s) | Status |
|---|---|---|---|
| GOAL | Owner-reviewed milestone-local boundary plus recovered source-linked tokens/diagnostics | 03-01-01/02; 03-02-01/02; 03-03-01/02 | COVERED |
| GOAL-1 | Complete non-final profile with spellings, examples, forms, diagnostics and range boundary | 03-01-01 | COVERED |
| GOAL-2 | Owner-reviewed dedicated profile commit and separately approved verified publication before syntax-dependent code/tests | 03-01-02; 03-02 live remote/ancestry/content entry check | COVERED |
| GOAL-3 | Kind/span only, SourceManager lexemes, zero-width explicit EOF | 03-02-01/02; 03-03-01 | COVERED |
| GOAL-4 | Profile tokens/ASCII digits/whitespace and M1-limit diagnostics | 03-02-02; 03-03-01 | COVERED |
| GOAL-5 | Smallest related offending-run consumption, spans and continuing recovery | 03-02-02; 03-03-01 | COVERED |

### REQ — unique primary delivery owner plus explicit regression coverage

| Requirement | Primary owner | Supplemental evidence owner | Status |
|---|---|---|---|
| LEX-01 | 03-01-01/02 profile and actual review/commit/publication gate | 03-02 rechecks live containment/ancestry/content; 03-03-02 checks gate/evidence ordering | COVERED |
| LEX-02 | 03-02-01 token model; 03-02-02 API | 03-03-01 lifetime/integration; 03-03-02 native evidence | COVERED |
| LEX-03 | 03-02-02 complete word/delimiter/digit/numeric scanning | 03-03-01 hostile/mixed cases; 03-03-02 native evidence | COVERED |
| LEX-04 | 03-02-02 whitespace/comment/high-byte handling | 03-03-01 all-byte/precedence cases; 03-03-02 native evidence | COVERED |
| LEX-05 | 03-02-02 source-end EOF | 03-03-01 empty/error/tail cases; 03-03-02 native evidence | COVERED |
| LEX-06 | 03-02-02 Invalid/Error recovery/checked API | 03-03-01 progress/order/error-state/renderer integration; 03-03-02 native evidence | COVERED |

Repeated requirement IDs in the regression plan denote supplemental verification,
not duplicated production ownership or competing APIs.

### CONTEXT — all fourteen locked decisions

| Decision | Accepted contract | Task coverage | Status |
|---|---|---|---|
| D-01 | M1 return0–255, later semantic range check | 03-01-01; 03-02-02/03-03-01 valid256/huge digits | COVERED |
| D-02 | Signed32 result, experimental/no stable ABI | 03-01-01; 03-02-02 no semantic conversion | COVERED |
| D-03 | Experimental fn main | 03-01-01; 03-02-01/02 | COVERED |
| D-04 | Experimental int32 | 03-01-01; 03-02-01/02 | COVERED |
| D-05 | Fixed fn main() -> int32 | 03-01-01; 03-02-01/02 | COVERED |
| D-06 | Braces, return, integer, mandatory semicolon | 03-01-01; 03-02-01/02 | COVERED |
| D-07 | Skip exactly space/TAB/LF/CR; source bytes unchanged | 03-01-01; 03-02-02; 03-03-01 | COVERED |
| D-08 | Four exact lowercase words | 03-01-01; 03-02-02 | COVERED |
| D-09 | Whole ASCII word matching, not general identifiers | 03-01-01; 03-02-02; 03-03-01 | COVERED |
| D-10 | Contiguous arrow | 03-01-01; 03-02-02; 03-03-01 | COVERED |
| D-11 | Explicit Invalid plus Error; ordered recovery; Errors block success | 03-01-01; 03-02-01/02; 03-03-01/02 | COVERED |
| D-12 | One Invalid/Error per whole unsupported numeric spelling | 03-01-01; 03-02-02; 03-03-01 | COVERED |
| D-13 | Consecutive top-level high-byte run, no decoding | 03-01-01; 03-02-02; 03-03-01 | COVERED |
| D-14 | Rejected // and /* grouping, first terminator/EOF | 03-01-01; 03-02-02; 03-03-01 | COVERED |

### RESEARCH — in-scope recommendations and constraints

| Research ID | Feature / constraint | Task coverage | Status |
|---|---|---|---|
| R-01 | Documentation-first reviewed profile; dedicated commit and fresh command | 03-01-01/02; 03-02 entry conditions | COVERED |
| R-02 | Minimal exact token/API/files; SourceManager checked original SourceError | 03-02-01/02 | COVERED |
| R-03 | Exact precedence and numeric-like continuation/stop grammar | 03-01-01 owner proposal; 03-02-02 after approval; 03-03-01 | COVERED |
| R-04 | +12, 1..2, numeric/comment adjacency, high bytes inside comments | Same three owning tasks; no silent final semantics | COVERED |
| R-05 | Original byte spans/location, whitespace vs lone-CR distinction | 03-02-02; 03-03-01 | COVERED |
| R-06 | Checked lookup once, owning output, call-local borrow, stable snapshots | 03-02-01/02; 03-03-01 | COVERED |
| R-07 | Explicit owned Error messages, preserved engine views/order/state | 03-02-02; 03-03-01 | COVERED |
| R-08 | ASCII unsigned classification, bounded lookahead, monotonic progress, NUL as data | 03-02-02; 03-03-01 all256/long/tail cases | COVERED |
| R-09 | Digit shape separate from signed32/range semantics and leading-zero octal | 03-01-01; 03-02-02; 03-03-01 | COVERED |
| R-10 | Separate safe renderer integration; hostile source/display bytes | 03-03-01, no production rendering dependency | COVERED |
| R-11 | Existing veyrcore/lexer_tests/PRE_TEST/C++23/include policy | 03-02-01/02; 03-03-02 | COVERED |
| R-12 | All-byte, mixed-run, long-run, repeated-call and prior-Error tests | 03-03-01, complete basic behavior already 03-02-02 | COVERED |
| R-13 | Exact reviewed commands, prepared dependencies, no package framework/work | 03-02 verification; 03-03-02; 03-VALIDATION | COVERED |
| R-14 | Independent Debug/Release actual counts/evidence manifest | 03-03-02, per-task native feedback also 03-02 | COVERED |
| R-15 | Windows/Linux hosted evidence, independent developer inspection, macOS deferral | 03-03-02 gates and every plan platform note | COVERED |
| R-16 | Security trust boundaries, progress, ownership, fixed messages, no unapproved size/OOM policy | Every plan threat model; 03-02-02/03-03-01 | COVERED |
| R-17 | Canonical phase-goal verification and explicit owner acceptance/publication | 03-03-02 records gates; later authorization required | COVERED |
| R-18 | Existing accepted APIs/dependencies, no downstream compiler stages | Every plan guardrails and scoped files | COVERED |

No uncovered in-scope item remains. Research's general language/Unicode/package/
parser/driver ideas and Phase 2 non-blocking hardening follow-ups are explicitly
excluded by owner scope, not tasks in this set.

## Proposed mixed boundaries and approval fidelity

The complete proposed scanner precedence and numeric continuation rules are in
03-01 Task1 and 03-02 Task2; both are contingent on the real committed profile.
Key outcomes: +12 and 1..2 each one Invalid/Error; 12/*x*/34 yields integer,
rejected comment, integer; // content containing high bytes yields only one
comment Invalid/Error; top-level high bytes retain D-13 grouping. Comments take
precedence only within a recognized comment-shaped run; slash terminates numeric
grouping. An ASCII whitespace boundary or independent delimiter is not swallowed.
The signed32/0–255 semantic rule does not reject decimal256 lexically.

## Validation and remaining gates

03-VALIDATION maps all six tasks to exact future feedback. Installed
frontmatter/structure checks and an independent plan-checker validate plans, not
implementation. Planning cannot establish native counts, cross-platform
acceptance or currently missing profile approval. The required progression is
planning owner review/commit → explicit wave1 → profile owner review/dedicated
artifact commit → separate publication approval/push → live authorized-branch
containment/content and ancestry if advanced → stop for fresh wave2 authorization
→ implementation review/closeout → fresh wave3 →
local owner review → separately authorized publication and actual four-job CI →
explicit canonical verification/phase acceptance. No arrow grants automatic
commit, push, execution or phase transition authority.

Platform impact:
- Windows: Same byte/recovery/lifetime contracts and existing hosted Debug/Release
  target; developer-host inspection independently pending/non-blocking.
- Linux: Primary Omarchy native validation; Ubuntu CI remains an independent
  reviewed toolchain. Prepared dependency/lock/cache policy unchanged.
- macOS future: Deferred, without new OS/filesystem/terminal dependencies.
