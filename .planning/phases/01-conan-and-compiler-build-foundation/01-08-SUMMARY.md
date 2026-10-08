---
phase: 01-conan-and-compiler-build-foundation
plan: "08"
subsystem: build
tags: [conan, llvm, cmake, ci, windows, linux, acceptance, cache, offline]
requires:
  - phase: "01-07"
    provides: Native foundation harness, strict offline rejection and preserved Linux developer evidence
provides:
  - Four successful independently reviewed Ubuntu/Windows Debug/Release native CI rows
  - Project-owned Conan bootstrap, exact lock use and portable dependency snapshot reuse
  - Windows Release ABI-probe configuration and Windows Debug verification/offline acceptance
  - Final Phase 1 evidence and normal planning-only closeout
affects: ["02"]
tech-stack:
  added: []
  patterns: ["independent developer and CI evidence", "early portable Conan snapshot save", "function-local Windows probe configuration"]
key-files:
  created:
    - .planning/phases/01-conan-and-compiler-build-foundation/01-08-SUMMARY.md
  modified:
    - .planning/phases/01-conan-and-compiler-build-foundation/01-08-EXECUTION.md
    - .planning/STATE.md
    - .planning/ROADMAP.md
    - .planning/REQUIREMENTS.md
key-decisions:
  - Conan 2.33.0 owns package mechanics; backend LLVM remains 23.1.2
  - Ubuntu Clang 18/GCC-13 CI and Windows standalone Clang 20.1.8 CI are separate from developer baselines
  - Successful cached package work remains independently validated by native configure/build/test/verify/acceptance
  - Windows developer-host evidence remains independently pending; macOS deferred
  - Phase 2 and closeout commit/push require separate owner authorization
requirements-completed: [BUILD-06, BUILD-07, BUILD-12, BUILD-13, BUILD-14]
phase-requirements-accepted: [BUILD-01, BUILD-02, BUILD-03, BUILD-04, BUILD-05, BUILD-06, BUILD-07, BUILD-08, BUILD-09, BUILD-10, BUILD-11, BUILD-12, BUILD-13, BUILD-14]
duration: "Owner-gated execution 2026-10-05 through 2026-10-08; final evidence review only"
completed: 2026-10-08
status: complete
completion_scope: reviewed-ubuntu-windows-ci-debug-release-plus-preserved-linux-developer-evidence
acceptance_run: "37826767824"
tested_commit: "2741d6514f5e695d36c20cb792bff4985ac5d754"
---

# Plan 01-08 — Four-job foundation CI acceptance

**Plan 01-08 and Phase 1 are complete: 8/8 plans, all four final native CI jobs
successful. This planning-only closeout is uncommitted and awaits owner review;
Phase 2 has not started.**

The owner explicitly authorized inspection of the completed final run and
conditional closeout. Evidence is the actual logs, step outcomes and uploaded
artifacts from [run 37826767824](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824),
tested commit `2741d6514f5e695d36c20cb792bff4985ac5d754`.

## Final native evidence

| Job | Doctor | Configure/build | CTest | Verify | Prepared/native | Controlled offline | Final |
|---|---|---|---|---|---|---|---|
| [Ubuntu Debug / 113481511231](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511231) | 22/22 | Passed | 35/35 | Passed; 4 C++23 entries | Passed; 35/35 | Passed | Success |
| [Ubuntu Release / 113481511547](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511547) | 22/22 | Passed | 35/35 | Passed; 4 C++23 entries | Passed; 35/35 | Passed | Success |
| [Windows Debug / 113481511602](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511602) | 20/20 | Passed | 35/35 | Passed; 4 C++23 entries | Passed; 35/35 | Passed with raw CRLF | Success |
| [Windows Release / 113481511855](https://github.com/m4rkens/VeyrLang/actions/runs/37826767824/job/113481511855) | 20/20 | Passed | 35/35 | Passed; 4 C++23 entries | Passed; 35/35 | Passed with raw CRLF | Success |

Every row also passed observation, reviewed policy, exact base Python,
auxiliary CMake/Ninja, project-owned Conan 2.33.0 bootstrap and tracked native
lock checks. No success is inferred from skipped work or a different job.

## Packages, locks and portable snapshots

All jobs restored/imported an archive into a fresh job-owned Conan home, reused
LLVM/GoogleTest, passed explicit setup and cache integrity, exported a fresh
archive and saved it before project validation. Debug restored the prior run's
Release snapshot; each Release job restored this run's same-platform Debug
snapshot. Configuration-specific package IDs and Windows CRT variants were
preserved. Setup elapsed: Ubuntu Debug 12.5952447s, Ubuntu Release 9.2827918s,
Windows Debug 27.840804s, Windows Release 33.4474755s.

No LLVM/GoogleTest package rebuild occurred in this final run. The LLVM recipe
test_package itself configured/built/ran against cached LLVM and passed its
Support allocation/destruction check. Native project validation remained required
after cache reuse; cache hits were not acceptance evidence on their own.

Exact recipe identities remained:

- `llvm/23.1.2@veyr/m1#aa4cad25b3148c3bdd22ed96fba04654`
- `gtest/1.18.0#e2507fc4efe2ec9e2db9e5c91eefab9f`
- build-context `cmake/4.4.3#8b46ce9dbf1937af1c05f19579c1f105`

All four locks remain valid Conan 0.5 JSON with unchanged dependency categories,
versions/revisions and bytes. Linux Debug/Release SHA-256:
`b76a5f857ba40da4a59d2bdcd8690fba6d12d15d53bd3b0dc6aceb252585f30b`.
Windows Debug/Release SHA-256:
`53d9fc6a882485c51c01a93ce6f9cdd11f277a94377c8d952d2af939b780e9be`.
Final-run input hashes agree with those tracked locks.

## Windows corrections resolved

The separately reviewed implementation checkpoints preserved validated DevShell
reuse, normalized only CMake linker serialization, accepted only supported
dynamic CRT expressions, established CMake 3.31 fixture policies, compared compiler
aliases through native filesystem identity, recognized exact clang-cl C++23
forwarding, normalized only CRLF diagnostic matching and selected the parent
configuration for the function-local Windows ABI scratch probe.

Final Windows Release normal and prepared configure logs both prove
`VEYR_CONSUMER_RUNTIME_COMPATIBLE` passes with `MultiThreadedDLL`,
`/O2 /Ob2 /DNDEBUG -clang:-std=c++23 -MD`, followed by successful native linking.
Neither Release probe uses `-MDd`. Debug keeps `MultiThreadedDebugDLL` and `-MDd`.
The existing _DLL/_MT/_DEBUG/iterator checks remain strict.

Both Windows compilation databases verify four selected C++23 entries; both
jobs pass all 35 tests, including 22 ConsumerCRT fixtures. Their isolated negative
logs retain CRLF bytes while the exact rejection is accepted. No earlier VCVars,
linker-escape, CRT fixture, include-policy or compiler-identity defect reappears.

## Scope and remaining evidence limits

- Ubuntu CI and preserved Omarchy developer acceptance remain separate;
  Windows hosted CI does not establish Windows developer-host acceptance.
  Independent Windows developer inspection remains pending; macOS deferred.
- Earlier successful source builds establish observed hosted capacity: Linux
  run 37670754053 dependency setup ~51m42s Debug / ~40m29s Release;
  Windows run 37786566284 LLVM build/package ~29m12s Debug-runtime /
  ~24m02s Release-runtime, with GoogleTest built afterward. These are previous
  cold/missing-package observations, not a new cold run or universal resource bound.
- Peak resources were not measured; a manual cold_build=true dispatch is not
  claimed. The final run proves warm reuse and native acceptance.
- Non-blocking action-runtime Node deprecations and existing Linux generated
  toolchain quotation warnings remain recorded; no new blocker is evidenced.
- Offline success means one expected Conan exit 1 in an isolated empty home,
  with no download/build/retry. It covers missing recipe/package cache, not a
  binary-only miss or every provider/runtime/ABI surface.
- Generic pending/phase-verification labels in generated harness JSON were
  not rewritten. Current acceptance relies on actual per-gate native results,
  successful job metadata and this explicit owner-authorized review.

Phase-level BUILD-01 through BUILD-14 acceptance combines this final matrix with
preserved Plans 01-01 through 01-07 evidence; it does not transfer their
implementation ownership to Plan 01-08. All eight plans/waves, earlier summaries,
discussion and implementation inputs remain unchanged. REQUIREMENTS.md records
14 accepted foundation requirements and 35 pending later-phase requirements.

## Review boundary

Only REQUIREMENTS.md, ROADMAP.md, STATE.md, 01-08-EXECUTION.md and this new SUMMARY
were edited. No new builds/tests/package operations, implementation changes,
CI dispatch/retry, automatic GSD verification command, commit or push occurred.

Platform impact:
- Windows: Hosted Debug/Release foundation accepted; independent developer evidence pending.
- Linux: Ubuntu Debug/Release accepted separately from preserved Omarchy evidence.
- macOS future: Deferred, without profile or acceptance promotion.

Proposed dedicated commit: `docs: close Phase 1 build-foundation acceptance`.
Owner review and separate commit authorization remain required; Phase 2 unstarted.
