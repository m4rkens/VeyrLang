# Phase 5 — Planning Breakdown and Traceability

Planning candidate only; no execution authorization or requirement acceptance.
Binding discussion published in 8e7bb1d8ce1f42cedfda06e498887507774635d9.

| Plan | Wave | Depends on | Complete reviewable unit | Technical scope |
|---|---|---|---|---|
| 05-01 | 1 | accepted Phase 4 | Shared AST + complete checked semantic functionality, 20 baseline cases | common header, parser-header exposure, semantic API/source, two unit TUs, CMake |
| 05-02 | 2 | 05-01 formal closeout + owner command | Independent adversarial conformance, 49 new rows | semantic unit tests only |
| 05-03 | 3 | 05-02 formal closeout + owner command | Real integration/lifetime/rendering, 16 cases + final local evidence | integration TU and test source registration |

All plans have two auto tasks followed by a blocking owner checkpoint and
autonomous:false. Dependencies reflect a real API producer/test consumer and
accepted sequential owner-review boundaries. There is no parallel conflicting
write to shared tests and no arbitrary model-only/API-only implementation plan.

| Requirement | Implementation | Independent evidence |
|---|---|---|
| AST-01 | 05-01 shared unchanged ParsedSmokeEntry | AST values; 05-02 checked spans; 05-03 original bytes/lifetime |
| AST-02 | 05-01 AST-only header and semantic include boundary | standalone AST TU; unchanged parser tests; real common-value handoff |
| SEMA-01 | 05-01 complete bounded decimal validator | 05-02 0/255/256, long zeros/digits/content; real pipeline |
| SEMA-02 | 05-01 checked failures/Error gate/private success | exact priority/error/diagnostic matrices; failed pipeline and rendering integration |

| Decisions | Planned behavior and task evidence |
|---|---|
| D-01, D-02 | 05-01-01 same value/shared independent include; 05-03-01 real handoff |
| D-03 | 05-01-02 public AST validation; 05-02-01 full checked matrix |
| D-04 | 05-01-01 owned success; 05-03-01 actual input destruction |
| D-05, D-06, D-07 | 05-01-02 full-literal fixed Error; 05-02-02 exact fields/long inputs; real renderer |
| D-08 | 05-02-02 prior fields/order/duplicates; 05-03-01 real repeated emissions |
| D-09, D-10 | 05-01-02 full checked pass; 05-02-01 exact payload/combined faults |
| D-11 | 05-01-02 full ASCII validation; 05-02-02 late nondigit after huge prefix |
| D-12 | 05-02-01 same-source/empty/strict nesting matrix |
| D-13 | 05-01-02 final Error gate; 05-02-02 and 05-03-01 blocked/no-success cases |
| D-14 | 05-01-01 private construction/type traits; no public mutation in integration |
| D-15, D-16 | 05-01 value contract; 05-03 actual copy/move, all spans and constant survive |

Roadmap criteria: (1) three-span AST → 05-01/03; (2) same independent common
boundary → 05-01/03; (3) valid range/rejections and useful diagnostics → all three;
(4) no invalid successful lowering handoff → private output and Error/failure tests
in all three. Malformed source syntax receives existing parser diagnostics;
caller-created AST contract violations are checked infrastructure failures, not a
new conflicting semantic language diagnostic. No second parser is authorized.

## Planned scope and counts

05-01 creates five technical files (AST header, semantic header/source, AST and
semantic unit tests) and modifies parser.hpp plus root/tests CMake; execution
record is a ninth tracked candidate at that later stage. 05-02 changes only the
semantic test plus execution record. 05-03 creates integration test and modifies
test CMake plus execution record. Counts are future planned identities, not run
results: 20 + 49 + 16 = 85 new AST/semantic tests; historical full baseline 561
and parser baseline 142 remain independent regression obligations.

## Boundaries

At planning review: Phase 5 0/3 executed; Phases 1–4 accepted; requirements
30 accepted/19 pending with AST/SEMA Pending. Eighteen completed prior plans plus
three planned plans is 18/21 written, not execution progress. No summary is
created and no requirement helper runs. Formal per-plan closeout requires owner
approval; defer all four AST/SEMA acceptances to verified complete phase coverage.
Separate sensitive-file commit/publication and hosted/canonical acceptance gates.

No unresolved language choice, added grammar/AST hierarchy/numeric type system,
LLVM/ABI/lowering/driver, dependency/profile/lock/cache/CI change. P01 forces only
a validated constant return. Preserve held infrastructure and earlier evidence.
