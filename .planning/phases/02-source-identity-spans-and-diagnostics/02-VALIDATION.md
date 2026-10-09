---
phase: 02
slug: source-identity-spans-and-diagnostics
status: draft
nyquist_compliant: true
wave_0_complete: false
created: 2026-10-09
---

# Phase 2 — Validation Strategy

This is a future execution contract. `nyquist_compliant: true` describes complete
planned feedback coverage, not passing tests. No implementation tests, compiler,
CMake, Ninja, CTest, Conan, CI or formatter ran during planning. Test files are
created within their owning implementation tasks, not a new infrastructure wave.
`wave_0_complete: false` records that those future test artifacts do not exist yet.

## Existing infrastructure and selection

| Property | Contract |
|---|---|
| Framework | Existing Conan GoogleTest/GoogleMock 1.18.0; no new dependency |
| Configuration | Existing root/tests CMake, C++23 target policy and PRE_TEST discovery |
| Native targets | New source_tests / diagnostic_tests alongside unchanged foundation/LLVM/driver/policy tests |
| Quick cases | SourceManager.*, SourceLocation.*, Diagnostic.*, PlainTextDiagnostic.* |
| Full verification | Existing dev.py test and verify; include-policy remains in CTest |
| Runtime | Small in-memory tests expected seconds after incremental build; not measured. First builds/CI/package work may be long and remain liveness-supervised |

Use the same **explicit selection on every command**. On the primary Omarchy
host, the reviewed base interpreter is `/usr/bin/python3`; existing Conan home
and prepared dependencies are reused. Obtain selected absolute build directory
from the read-only doctor's `selection.build_dir` JSON or `build-dir` text output.
Do not guess its fingerprint, reuse the other configuration, activate a venv or
use a Windows/Ubuntu baseline on Omarchy. No ordinary command implicitly sets up
missing dependencies. If existing preparation is missing/incompatible, stop for
owner review before package work; this phase adds no dependencies.

Future Linux command template (`<Debug|Release>` and `<selected-build-dir>` are
substitutions from the selection, not literal arguments):

```text
/usr/bin/python3 scripts/dev.py doctor --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed --json
/usr/bin/python3 scripts/dev.py configure --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py build --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
ctest --test-dir <selected-build-dir> -R "^(SourceManager|SourceLocation|Diagnostic|PlainTextDiagnostic)\." --output-on-failure
/usr/bin/python3 scripts/dev.py test --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/dev.py verify --profile linux-x86_64-clang --environment linux-developer --config <Debug|Release> --llvm-provider managed
/usr/bin/python3 scripts/check_includes.py --root .
```

The include checker requires `--root`; use the checkout root as shown above. Run commands sequentially, stop
on the first nonzero exit, and do not rerun already accepted unchanged gates
without a new reason. Configure explicitly after changing CMake source/test lists;
build may configure and test may build per accepted chaining semantics. `all`
includes setup and is not needed merely to run Phase 2 incremental tests.
Use the selected dedicated home consistently if overriding `--conan-home`.

For native hosted CI, use existing Build foundation selections `ubuntu-ci` and
`windows-ci`, the existing `VEYR_BASE_PYTHON` absolute interpreter and unchanged
shared commands/cache/acceptance steps. No workflow edit or new Windows developer
baseline is needed. A push/dispatch requires separate owner authorization; no
automatic retry. Independent Windows developer inspection stays pending and
non-blocking, distinct from Windows CI evidence. macOS stays deferred.

## Sampling and task verification map

Focused native tests follow each task's implementation in Debug **and** Release;
no task commit is automatic. At each plan boundary run selected full CTest/verify
when integration changed; after 02-04 require actual four-row CI evidence before
claiming complete Phase 2 acceptance. Use actual discovered test counts, not the
old foundation count 35. No watch mode, hard 60-second kill or duplicate launch.

| Task | Wave | Requirements | Decision / behavior | Automated future gate | Test file availability | Status |
|---|---|---|---|---|---|---|
| 02-01-01 | 1 | SRC-01/02/03 | D-01–D-08 ownership, fresh IDs, checked exact spans | build + SourceManager CTest | Created in this task | pending |
| 02-01-02 | 1 | SRC-01/02/03 | Original bytes, lifetime growth, invalid payload/arithmetic boundaries | SourceManager CTest + full verify | Task 1 | pending |
| 02-02-01 | 2 | SRC-04 | D-09–D-11 exact newline/EOF locations | build + SourceLocation CTest | Extended in this task | pending |
| 02-02-02 | 2 | SRC-04 | D-04/D-07/D-09–D-12 byte oracle, exact display metadata | SourceManager/SourceLocation CTest + verify | Earlier tasks | pending |
| 02-03-01 | 3 | DIAG-01/03 | D-13/D-14 values, optional primary, error state | build + Diagnostic CTest | Created in this task | pending |
| 02-03-02 | 3 | DIAG-01/03 | D-16 order/duplicates and caller-owned input lifetime | Diagnostic CTest + verify | Task 1 | pending |
| 02-04-01 | 4 | DIAG-02 | D-12/D-15 checked rendering/escaping/bounds | build + PlainTextDiagnostic CTest | Created in this task | pending |
| 02-04-02 | 4 | DIAG-02; all regression | D-01–D-16 complete in-memory slice/hostile boundary bytes | Focused + full native CTest/test/verify/include checker | Earlier tasks | pending |
| 02-04-03 | 4 | all seven | Independent Windows/Linux Debug/Release compatibility and acceptance | Existing four-job CI, verify/prepared/offline gates | Existing workflow; new tests from tasks above | owner-gated pending |

Each implementation task has automated feedback; no three-task gap. These are
contract/behavior tests, not tests mirroring a private representation. All seven
requirements get direct native checks; all sixteen decisions are mapped in
02-PLAN-REVIEW.md. Arithmetic extremes use bounded numeric-helper tests, not
huge allocations. Invalid-input tests must run in Release, not debug asserts only.
No threat-model/schema infrastructure is applicable; malformed source references
and terminal-control bytes are covered by fail-closed/escaping regressions.

## Wave 0 / test creation ownership

Existing infrastructure is sufficient; no framework install or preliminary wave.
The currently missing C++ tests are explicitly created in owning tasks before
their first automated gate:

- 02-01-01: tests/source/source_manager_test.cpp plus source_tests integration.
- 02-03-01: tests/diagnostics/diagnostic_test.cpp plus diagnostic_tests integration.
- 02-04-01: tests/diagnostics/plain_text_renderer_test.cpp plus existing target extension.

Do not create placeholder files during planning or report future tests as passed.

## Owner-only review and sign-off

Owner reviews bounded C++ API, renderer limits/layout/escapes, sensitive-file
changes and separate publication/closeout. Determinism, clipping, error categories,
ordering and byte boundaries are automated; no subjective terminal/color/Unicode
rendering gate. Cross-platform native results are evidence, not manual assumption.

- [x] Every task specifies automated verification or existing CI behind an owner gate.
- [x] All missing test files have same-task creation ownership; no unowned prerequisite.
- [x] Debug/Release, original bytes, Release runtime failures and Tier 1 CI covered.
- [x] No watch mode, implicit package setup, auto retry/commit or phase transition.
- [ ] Execute tests and record observed latency/results; planned latency is not proven.
- [ ] Obtain separately authorized four-row Phase 2 CI evidence and owner closeout.

Approval: pending owner review of planning packet. Execution has not started.
