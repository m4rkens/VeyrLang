---
phase: 1
slug: conan-and-compiler-build-foundation
status: draft
nyquist_compliant: true
wave_0_complete: false
created: 2026-10-04
---

# Phase 1 — Validation Strategy

Planning-only contract. Breakdown approved and eight PLAN.md files written. No tests/infrastructure created or run. nyquist_compliant records reviewed plan coverage only; wave_0_complete remains false because implementation has not started.

## Test Infrastructure

| Property | Value |
|---|---|
| Framework | Python stdlib unittest; GoogleTest/GoogleMock gtest/1.18.0 through Conan |
| Configuration | Future tests/development/ and tests/CMakeLists.txt; no existing harness |
| Quick command | python -m unittest discover -s tests/development -p 'test_*.py' |
| Full command | python scripts/dev.py verify --profile <reviewed-native-profile> --config <Debug-or-Release> |
| Recipe integration | Conan test_package during explicit setup/native recipe validation |
| Runtime | Quick target <30s after infrastructure exists; native/source-build duration requires measurement |

These are proposed future commands, not commands authorized or executed by planning. Quick checks use fake tool commands or stdlib fixtures, never acquire packages. Run only the relevant available subset while infrastructure is introduced; do not run nonexistent tests or automatically initiate setup.

## Sampling Rate

- After each authorized implementation task: relevant quick checks; no auto-commit.
- After each completed wave: relevant native full checks once required infrastructure exists, with prepared dependencies; explicit setup separately when needed.
- Before dedicated phase verification: both native Debug/Release combinations on both platforms and passing CI required.
- Quick feedback target: <30 seconds; do not impose this on LLVM/source/native build work.
- Quiet ~60 seconds triggers inspection; healthy work continues. Respect a separately established command/job hard limit.

## Per-Task Native/Behavior Verification Map

All files below are missing until execution creates them. No row is green.

| Task | Wave | Requirement | Test type / meaningful behavior | Proposed verification | Status |
|---|---:|---|---|---|---|
| 01-01-01 | 1 | BUILD-05, BUILD-07, BUILD-13 | Host identity/profile inventory | Review actual versions, CRT/SDK/libc/linker and exact profile inputs | Pending |
| 01-01-02 | 1 | BUILD-05, BUILD-07, BUILD-13 | Owner checkpoint | Resolve missing identity or unsupported baseline before package work | Pending |
| 01-01-03 | 1 | BUILD-07 | Unit: missing/mismatched tools/SDK, paths | unittest test_doctor | Missing infrastructure |
| 01-02-01 | 2 | BUILD-03, BUILD-04 | Recipe/options/source-integrity | Review exact source/digest/options and Conan-native graph | Pending |
| 01-02-02 | 2 | BUILD-03, BUILD-13 | Native recipe integration | Conan test_package verifies native exports and CRT-compatible link/run | Missing infrastructure |
| 01-03-01 | 3 | BUILD-02, BUILD-12 | Conan profile/lock resolution | Review Conan-generated exact refs/revisions across native combinations | Pending |
| 01-03-02 | 3 | BUILD-06, BUILD-12 | Unit + native prepared/cache boundary | unittest test_conan_ops; prepared cache-only configure and missing-package failure | Missing infrastructure |
| 01-04-01 | 4 | BUILD-01, BUILD-08, BUILD-09 | Native CMake discovery/build | Configure/build with prepared packages; inspect compile database/provider diagnostics | Missing infrastructure |
| 01-04-02 | 4 | BUILD-10 | GoogleTest/GoogleMock and LLVM probe | CTest infrastructure suite on matching runtime | Missing infrastructure |
| 01-05-01 | 5 | BUILD-11 | Unit include positives/negatives | unittest test_include_policy | Missing infrastructure |
| 01-05-02 | 5 | BUILD-11 | Build/CTest reachability | check-includes target and CTest include-policy entry | Missing infrastructure |
| 01-06-01 | 6 | BUILD-06, BUILD-12 | Unit command chaining/clean | unittest test_dev_commands; no setup in build/test; preserve cache/other builds | Missing infrastructure |
| 01-06-02 | 6 | BUILD-06 | Unit child/liveness | unittest test_runner; healthy quiet child continues, confirmed stall stops once | Missing infrastructure |
| 01-07-01 | 7 | BUILD-08, BUILD-10, BUILD-12, BUILD-13 | Unit acceptance harness contract | unittest test_foundation_contract; wrong version/runtime/triple and offline negatives | Missing infrastructure |
| 01-07-02 | 7 | BUILD-08, BUILD-10, BUILD-12, BUILD-13 | Native acceptance | check_foundation for actual native profile/config/provider combinations | Missing infrastructure |
| 01-08-01 | 8 | BUILD-14 | CI | Four native jobs, distinct bootstrap/native steps; cache-disabled source path | No remote evidence |
| 01-08-02 | 8 | BUILD-06, BUILD-07, BUILD-12, BUILD-13, BUILD-14 | Docs/CI capacity | Compare documented local/CI commands; record duration/resource/cold/warm identities | Pending |
| 01-08-03 | 8 | BUILD-14 | Owner checkpoint | Authorized publication/CI runs, passing URLs or capacity decision | Pending |

Threat domain: Native tooling/integrity; no active web ASVS feature. Profile/runtime checks prevent incompatible linkage; argument arrays and clean-boundary checks protect process/path handling. This is not a security implementation verdict.

## Exact Quick Feedback Map

Quick commands are planned checks, not execution evidence. Native/behavior checks above remain required; Python syntax/fixture checks do not establish LLVM/CRT/CI acceptance. Sixteen implementation tasks have automated commands; two manual owner checkpoints are excluded from implementation sampling. No commands were run during planning.

| Task | Wave | Exact quick command | Infrastructure dependency |
|---|---:|---|---|
| 01-01-01 | 1 | `python -m unittest discover -s tests/development -p test_doctor.py` | Created by this task or an earlier task in its plan |
| 01-01-02 | 1 | Owner checkpoint; manual evidence review | Manual checkpoint; exempt from implementation sampling |
| 01-01-03 | 1 | `python -m unittest discover -s tests/development -p test_doctor.py` | Created by this task or an earlier task in its plan |
| 01-02-01 | 2 | `python -m py_compile conan/recipes/llvm/conanfile.py` | Created by this task or an earlier task in its plan |
| 01-02-02 | 2 | `python -m py_compile conan/recipes/llvm/test_package/conanfile.py` | Created by this task or an earlier task in its plan |
| 01-03-01 | 3 | `python -m py_compile conanfile.py` | Created by this task or an earlier task in its plan |
| 01-03-02 | 3 | `python -m unittest discover -s tests/development -p test_conan_ops.py` | 01-03-02 creates tests/development/test_conan_ops.py |
| 01-04-01 | 4 | `python -m unittest discover -s tests/development -p test_conan_ops.py` | 01-03-02 creates tests/development/test_conan_ops.py |
| 01-04-02 | 4 | `python -m unittest discover -s tests/development -p test_conan_ops.py` | 01-03-02 creates tests/development/test_conan_ops.py |
| 01-05-01 | 5 | `python -m unittest discover -s tests/development -p test_include_policy.py` | Created by this task or an earlier task in its plan |
| 01-05-02 | 5 | `python scripts/check_includes.py --root .` | Created by this task or an earlier task in its plan |
| 01-06-01 | 6 | `python -m unittest discover -s tests/development -p test_dev_commands.py` | 01-06-01 creates tests/development/test_dev_commands.py |
| 01-06-02 | 6 | `python -m unittest discover -s tests/development -p test_runner.py` | Created by this task or an earlier task in its plan |
| 01-07-01 | 7 | `python -m unittest discover -s tests/development -p test_foundation_contract.py` | 01-07-01 creates tests/development/test_foundation_contract.py |
| 01-07-02 | 7 | `python -m unittest discover -s tests/development -p test_foundation_contract.py` | 01-07-01 creates tests/development/test_foundation_contract.py |
| 01-08-01 | 8 | `python -m unittest discover -s tests/development -p test_foundation_contract.py` | 01-07-01 creates tests/development/test_foundation_contract.py |
| 01-08-02 | 8 | `python -m unittest discover -s tests/development -p test_dev_commands.py` | 01-06-01 creates tests/development/test_dev_commands.py |
| 01-08-03 | 8 | Owner checkpoint; manual evidence review | Manual checkpoint; exempt from implementation sampling |

## Wave 0 Requirements

Wave 0 means infrastructure prerequisites allocated within the written plans, not a new executed phase. A task creates its required test/recipe before its own check; consumers reference the creator task or a strictly earlier dependent plan.

- [ ] Plan 01: doctor fixtures and exact baseline/profile prerequisite observations.
- [ ] Plan 02: narrow LLVM test_package and exact official source integrity.
- [ ] Plan 03: conan operation tests and Conan-generated locks.
- [ ] Plan 04: GoogleTest/GoogleMock CMake infrastructure and native LLVM probe.
- [ ] Plan 05: owned-header include fixtures and CTest integration.
- [ ] Plan 06: command/process fixtures, including quiet progress and destructive-boundary negatives.
- [ ] Plan 07: provider/runtime/offline/cache acceptance harness.
- [ ] Plan 08: GitHub Actions matrix, cache-disabled path and real CI evidence.

No pytest or extra Python test-framework dependency required. Final PLAN.md mapping must provide automated checks or an explicit upstream infrastructure dependency for every task.

## Manual-Only Verifications

| Behavior | Requirement | Why manual | Later instructions |
|---|---|---|---|
| Exact baseline acceptance | BUILD-05, BUILD-07, BUILD-13 | Real Windows/Linux host evidence and owner-selected support policy | Review observed VS toolset/CRT/SDK and libc/linker identities |
| ABI conflict/capacity resolution | BUILD-13, BUILD-14 | Policy/capacity choice belongs to owner | Report failed combination or measured CI constraint; no silent fallback |
| Remote publication/runs | BUILD-14 | Local workflow file is not CI evidence | Obtain explicit remote authorization; retain four passing job/run URLs |
| Quiet build supervision when telemetry unavailable | BUILD-06 | Process progress cannot always be inferred from captured output | Inspect child/process/artifact signals; continue credible progress, stop confirmed stall once |

## Validation Sign-Off

- [x] Breakdown approved and executable plans written
- [x] All implementation tasks have automated verification or explicit infrastructure dependencies; owner checkpoints use manual evidence
- [x] Sampling continuity: every implementation task has automated quick feedback
- [x] Missing infrastructure is allocated before consumers run; not created during planning
- [x] No watch-mode flags or duplicate command retries in planned verification
- [ ] Quick feedback target measured separately from native/source-build timing
- [ ] Four native CI combinations passed; source-build capacity recorded
- [x] nyquist_compliant records final plan coverage review; wave_0_complete remains false until actual infrastructure exists

Plan coverage check: Complete, inline. Owner completed-planning review: Pending. No green native/test/CI evidence is claimed.
