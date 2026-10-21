---
phase: 05
slug: minimal-ast-and-semantic-smoke
status: draft
nyquist_compliant: true
wave_0_complete: false
created: 2026-10-21
---

# Phase 5 — Validation Strategy

Planning contract only; no native result or owner execution approval is implied.
See [05-CONTEXT.md](05-CONTEXT.md), [05-RESEARCH.md](05-RESEARCH.md#validation-architecture)
and the three executable plans. Compliance denotes planned sampling/producer
coverage checked inline, not independent agent review or completed validation.

## Test Infrastructure

| Property | Value |
|---|---|
| Framework | Existing GoogleTest/GoogleMock 1.18.0, CTest PRE_TEST |
| Configuration | Existing root/tests CMake, C++23 target policy |
| New targets | ast_tests, semantic_tests |
| Quick command | `ctest --test-dir <selected-build-dir> -R 'SmokeProfile(Ast|Semantic)' --output-on-failure --stop-on-failure --no-tests=error` |
| Full command | Selected dev.py test and verify, separately Debug/Release |
| Latency | Focused target under 30 seconds after build; unmeasured, no hard timeout |

## Exact Future Native Commands

Run only during separately authorized execution, one supervised command at a time.
Substitute Debug and Release independently; obtain each absolute build directory
from that configuration's doctor. Worktree root and selected compiler must match.

```text
/usr/bin/python3 scripts/dev.py doctor --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed --json
/usr/bin/python3 scripts/dev.py configure --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py build --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
ctest --test-dir <selected-build-dir> --show-only=json-v1 -R 'SmokeProfile(Ast|Semantic)'
ctest --test-dir <selected-build-dir> -R 'SmokeProfile(Ast|Semantic)' --output-on-failure --stop-on-failure --no-tests=error
ctest --test-dir <selected-build-dir> -R '^(SourceManager|SourceLocation|Diagnostic|PlainTextDiagnostic|SmokeProfileToken|SmokeProfileLexer|SmokeProfileParser)' --output-on-failure --stop-on-failure --no-tests=error
/usr/bin/python3 scripts/dev.py test --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py verify --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 -B scripts/check_includes.py --root .
```

Configure after CMake source/target edits; use existing prepared pinned packages.
No setup/all/download/build-missing/cache purge is implicit. Ordinary supported
Conan bookkeeping is permitted by existing policy; a genuine missing dependency
or changed package identity requires owner review. No manual hosted run without
authorization. Preserve all four existing automatic native jobs and foundation
prepared/offline/integrity/isolated-empty-cache acceptance; do not edit workflows.

## Sampling Rate

- Test-first each coherent implementation task; record expected RED separately
  from unexpected failure, then independent Debug/Release GREEN and focused names.
- Each plan checkpoint: full local test, strict verify and direct include checks
  in both configurations, not only the focused matrix. Keep accepted regressions.
- Before phase acceptance: published implementation, fresh successful Ubuntu and
  Windows Debug/Release hosted evidence, canonical phase goal verification, then
  explicit requirement/phase owner acceptance. Pending documentation CI alone
  does not block local planning or separately authorized local implementation.
- Apply watchdog: around 60 seconds quiet is a soft liveness check, not a kill or
  retry condition. Stop on real failure; do not modify accepted production under
  test-only later plans without bounded owner correction authorization.

## Per-Task Verification Map

| Task ID | Plan | Wave | Requirements | Threat Ref | Secure Behavior | Test Type | Automated Command | File Exists | Status |
|---|---|---|---|---|---|---|---|---|---|
| 05-01-01 | 01 | 1 | AST-01, AST-02, SEMA-02 | T-05-HANDOFF | Same three spans, independent header, no public forged success | native values/static assertions | Quick command, SmokeProfileAst + semantic value tests | Same-task producer | pending |
| 05-01-02 | 01 | 1 | all four | T-05-INPUT/T-05-RANGE | Full checked validator; bounded digits; Error gate | baseline unit | Quick + full commands | Same-task source/test producer | pending |
| 05-02-01 | 02 | 2 | AST-01, SEMA-02 | T-05-INPUT | Exact source/AST failures and deterministic priority | adversarial matrix | Quick + named row reconciliation | Extends 05-01 test | pending |
| 05-02-02 | 02 | 2 | SEMA-01, SEMA-02 | T-05-RANGE/T-05-ERRORS | Long literals, late nondigits, exact diagnostics/no partial success | conformance matrix | Quick + full commands | Extends prior task | pending |
| 05-03-01 | 03 | 3 | all four | T-05-LIFETIME/T-05-HANDOFF | Real pipeline, destruction/ownership, safe rendering | integration | Quick + names | Same-task integration producer | pending |
| 05-03-02 | 03 | 3 | all four | all | Complete independent local evidence and protected baselines | full native/static | Full command list + hash reconciliation | Existing tools/prior tests | pending |

## Independent Oracles and Discovery

Use complete literal expected messages, SourceError/AstError payloads, field and
byte offsets, rejection alternatives and all three AST spans. No implementation
conversion/error/priority table may generate expected data. Name parameter rows;
retain discovery JSON and Passed/XML identities, comparing exact sets per config.
Reject zero selection, disabled/skipped required rows, absent producer or a loop
that lacks SCOPED_TRACE. Historical 142 parser/561 full counts are baseline
references, not predictions or future proof. Retain all 110 parser unit/conformance
and 32 parser integration identities unchanged. Report actual new/full counts.

Every owned C++ translation unit must use the selected compiler/directory/C++23.
Current 16 entries are historical: 05-01 adds validator.cpp, smoke_entry_test.cpp,
smoke_validator_test.cpp (expected 19 if no other authorized TU changes); 05-03
adds integration_test.cpp (expected 20). Independently verify exact path membership,
not only the aggregate. AST header compilation has no parser/token include.

## Wave 0 Requirements

Existing framework/prepared dependencies suffice. No scaffolding, install or
planning-time stub. 05-01-01 creates AST/semantic headers and value tests/targets;
05-01-02 creates validator.cpp and registers it; 05-03-01 creates and registers
integration source only when present. Every missing reference has a producer.

## Manual-Only Verifications

| Behavior | Requirement | Why manual | Instructions |
|---|---|---|---|
| No duplicate AST/reparse or LLVM/general language expansion | AST-02 | Includes/tests alone do not prove architecture | Inspect actual dependencies/dataflow; preserve parser behavior |
| Private successful construction and read-only payload | SEMA-02 | API review complements type traits | Inspect private constructor/friend/getters; tests reject public construction/mutation |
| Plan/phase/publication approval | All | Owner authority | Review actual diff/evidence; no auto-commit or summary discovery |

## Evidence and Acceptance Gates

Archive original mutable inputs before each plan; preserve hashes and prior
receipts, including Phase 4/discussion STATE changes through explicit mappings.
Store logs/manifests/review under ignored .veyr-dev/execution/05-01, 05-02, 05-03.
Execution records may be tracked; canonical SUMMARY files may be created only at
separately owner-authorized formal closeout. Their filenames are completion gates.
All AST/SEMA requirements remain Pending across plan checkpoints unless complete
phase coverage is technically verified and owner acceptance is explicitly granted.
No requirement helper, unrestricted phase.complete, automatic commit or Phase 6.

## Validation Sign-Off

- [x] All six automated tasks have focused verification and concrete test producers.
- [x] No three consecutive tasks lack automated checks; no watch mode.
- [x] Every missing test/source has a same-task or preceding producer.
- [x] Independent oracles and discovered/executed identities are required.
- [x] Full per-plan and hosted phase acceptance remain explicit separate gates.
- [ ] Actual native/hosted execution (not authorized by planning).

Approval: pending owner review; inline planning checks only.

Platform impact:
- Windows: preserve native clang-cl/CRT and hosted Debug/Release; developer-host
  inspection independently pending/non-blocking. No Linux result proves Windows.
- Linux: independent Debug/Release local selections; Ubuntu hosted is separate.
- macOS future: deferred; no OS-specific AST/validator assumptions.
