---
phase: 03-m1-experimental-smoke-profile-tokens-and-lexer
plan: "01"
subsystem: design
tags: [m1, experimental-smoke-profile, byte-spans, lexical-recovery, owner-gates]
requires:
  - phase: "02"
    provides: Accepted immutable SourceManager snapshots, checked byte spans and owning diagnostics
provides:
  - Owner-accepted documentation-only experimental M1 smoke profile
  - Deterministic original-byte token and Invalid/Error recovery specification
  - Separately approved dedicated commit and actual verified remote publication
affects: ["03-02", "03-03", "04"]
tech-stack:
  added: []
  patterns: ["profile before syntax-dependent code", "separate review/commit/publication/execution gates"]
key-files:
  created:
    - docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md
    - .planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-01-EXECUTION.md
    - .planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-01-SUMMARY.md
  modified: [.planning/STATE.md, .planning/ROADMAP.md, .planning/REQUIREMENTS.md]
key-decisions:
  - Preserve all fourteen discussion decisions and experimental non-final M1 scope
  - Exclude opener bytes from block terminators and require empty lexical Error related spans
  - Actual remote publication is evidence; a local commit or stale tracking ref is insufficient
patterns-established:
  - Recovered Invalid tokens retain exact primary Error spans and source-end EOF
  - Profile acceptance, dedicated commit, publication and later execution need separate approvals
requirements-completed: [LEX-01]
requirements-acceptance-scope: owner-accepted-documentation-only-LEX-01; canonical ledger completed
coverage:
  - id: P1
    description: Complete experimental profile preserves fourteen decisions and precise byte/recovery contracts
    requirement: LEX-01
    verification:
      - kind: manual_procedural
        ref: "03-01-EXECUTION.md — owner acceptance after 35 recovery rows, five delimiter cases and documentation review"
        status: pass
    human_judgment: true
    rationale: "Profile semantics required explicit owner acceptance, which was granted; formal Plan 03-01 acceptance was separately granted; closeout publication remains separately gated"
  - id: P2
    description: Exact accepted profile was committed and published under separate owner approvals
    requirement: LEX-01
    verification:
      - kind: other
        ref: "03-01-EXECUTION.md — commit 52f598aaf031e549ed3bbe5c598bc43f5f7be6bb and actual origin refs/heads/main observation at 2026-10-17 13:20:11 UTC"
        status: pass
    human_judgment: true
    rationale: "The owner approved commit and publication separately; neither verification nor this summary authorizes Wave 2"
duration: "Owner-gated documentation work on 2026-10-17; active elapsed duration not measured"
completed: 2026-10-17
prepared: 2026-10-17
status: complete
closeout-status: owner-accepted-publication-pending
completion-scope: documentation-only-profile-accepted-and-published
next-wave-authorized: false
---

# Phase 3 Plan 01: Experimental M1 smoke profile summary

**An owner-accepted, non-final M1 profile specifies exact byte-token recovery and
has a dedicated commit with actual verified remote publication.**

Plan 03-01 is formally owner-accepted and complete for its documentation-only
scope. Phase 3 is 1/3 complete and remains in progress; 03-02 and 03-03 remain
unstarted and unauthorized. Canonical GSD `query requirements.mark-complete
LEX-01` updated only LEX-01; the global ledger is 22 accepted / 27 pending.
This is not whole-phase acceptance, native lexer evidence or publication of
this five-file closeout packet. LEX-02–06 remain pending.

## Performance and scope

- Documentation preparation, owner clarification/review, commit and publication:
  2026-10-17. Active duration was not measured; owner wait time is not build time.
- Tasks: Task 1 content/verification delivered; Task 2 profile acceptance,
  dedicated commit and publication gates satisfied. Fresh Wave 2 authorization
  remains a separate future entry condition, not permission implied by closeout.
- Dedicated artifact commit: exactly two files. This owner-accepted closeout packet:
  five planning files, including this new summary; the profile itself is unchanged.
- No native tests/builds, package operations or dependency acquisition apply to
  this documentation-only plan. No new compiler implementation exists here.

## Accomplishments and accepted contract

The profile fills all eight template sections and maps D-01 through D-14. It
defines the experimental `fn main() -> int32 { return 42; }` shape with one root,
fixed entry and integer return. Signed 32-bit results restricted to 0–255 are
later semantic/lowering constraints, not lexical value conversion or final ABI.
Digit-only 256, leading zeros and arbitrarily long sequences remain lexical
IntegerLiteral tokens. No final grammar, general type system or ABI is frozen.

Original bytes, manager-local SourceId, half-open SourceSpan, LF/CRLF/lone-CR/EOF
location semantics and Phase 2 APIs remain unchanged. Tokens contain kind/span
only. The ordered specification groups whole ASCII words, unsupported numeric
spellings, top-level high bytes and comment-shaped input. Each rejected run has
one Invalid and one explicitly Error-severity diagnostic with exact primary span
and empty related spans, followed eventually by exactly one source-end EOF.
Unknown source remains the original checked infrastructure failure with no
language diagnostic or fabricated EOF. Recovered tokens cannot erase accumulated
Errors or imply successful compilation; no new driver is introduced.

| Accepted decision | Preserved M1 contract |
|---|---|
| D-01 | 0–255 later semantic acceptance; no lexical range check |
| D-02 | Signed 32-bit experimental result; no stable ABI |
| D-03 | Provisional `fn main` |
| D-04 | Provisional `int32` |
| D-05 | Fixed empty-parameter entry signature with explicit result |
| D-06 | Braces, one integer return and mandatory semicolon |
| D-07 | Skip only space/TAB/LF/CR; preserve original bytes and location rules |
| D-08 | Four exact lowercase spellings |
| D-09 | Whole ASCII words; no keyword-fragment salvage or general identifiers |
| D-10 | Contiguous `->` only |
| D-11 | Ordered Invalid plus Error recovery, final EOF and persistent Error state |
| D-12 | Whole unsupported numeric-like runs with independent delimiters preserved |
| D-13 | Consecutive original high-byte runs; no Unicode decoding |
| D-14 | Rejected comment-shaped runs, first eligible closer/EOF, no nesting/trivia |

The owner-reviewed clarification starts block-terminator search at opener offset
plus two, excluding all opening bytes. Thus `/*/return` is Invalid `[0,9)`, one
Error over `[0,9)` and EOF `[9,9)`, with no Return token. No nesting, comment trivia
or new accepted comment syntax was added. Numeric/comment/high-byte precedence
and all earlier examples remain intact.

## Task commit and actual publication evidence

Task 1 and the directly required Task 2 execution checkpoint were committed in
one owner-approved dedicated artifact commit, not artificial per-task commits:

`52f598aaf031e549ed3bbe5c598bc43f5f7be6bb`

Message: `docs: establish experimental M1 smoke profile`.
Parent/planning baseline: `3dd1df75dc6f5e64befdd5a8af6c2863b8d0b307`.
Exact committed files:

- `docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md`
- `.planning/phases/03-m1-experimental-smoke-profile-tokens-and-lexer/03-01-EXECUTION.md`

The accepted profile is 21,726 bytes, SHA-256
`1b5cefb5fa40a4a940ce1c0f4a4d7b8049ce591b81b79ecaf2251e5dfb107864`.
Its original draft-status prose is preserved as the exact reviewed snapshot;
the execution record supplies subsequent owner acceptance/publication status.

Under separate owner publication approval, `git push origin main` exited 0.
Actual `git ls-remote --exit-code --refs origin refs/heads/main` queries exited 0:
before push, main was the planning baseline; after push, observed main was exactly
the dedicated profile SHA. The post-push observation was recorded at
2026-10-17 13:20:11 UTC. Because the observed tip equaled the accepted SHA,
advanced-tip ancestry checking was not applicable. This is actual remote proof,
not an inference from push output or a cached remote-tracking reference.

The existing three unstaged checkpoint files remained byte-identical during
publication. Current closeout authorization now permits their reconciliation.
No additional commit or push is authorized in this preparation step.

## Accepted document evidence

| Document check accepted by the owner | Result |
|---|---|
| Template sections / discussion decisions | 8/8 / 14/14 |
| Exact recovery span/Error/EOF examples | 35/35, including all 34 unchanged prior rows |
| Block-delimiter example checks | 5/5 |
| Complete sample token/EOF spans | 12/12 |
| LF/CRLF/lone-CR/byte-column/EOF example groups | 5/5 |
| Canonical artifact-presence query | 2/2; presence is not owner acceptance |
| Documentation links, fences and whitespace | PASS |
| Accepted implementation/evidence hashes | 36/36 unchanged |
| Tracked Conan locks | 4/4 byte-identical |

See [03-01-EXECUTION.md](03-01-EXECUTION.md) for the actual workflow, approval
sequence and evidence limits. Retained ignored records are
`.veyr-dev/execution/03-01/document-verification.json` (including revision history),
`profile-commit.json` and `profile-publication.json`. Completed checks and remote
operations were not repeated merely to prepare this summary. Closeout integrity
checks inspect the actual files/receipts and accepted hashes again.

Build foundation run [39036114634](https://github.com/m4rkens/VeyrLang/actions/runs/39036114634)
was automatically triggered by publication and observed queued at that time.
Current CI status is not asserted or queried by this preparation. No historical
or skipped CI job is credited as validation of unimplemented lexer code.

Preparation checks previously validated the summary frontmatter and installed
`verify-summary`; the final canonical checks and integrity results are recorded
in [03-01-EXECUTION.md](03-01-EXECUTION.md). The summary is now complete under
explicit owner acceptance; its separate closeout commit/publication is pending.
These are document/metadata checks, not native lexer tests.

## Decisions, issues and deviations

All fourteen accepted choices remain intact. The delimiter-overlap ambiguity
was resolved under explicit bounded owner authorization before final profile
acceptance; lexical Error related spans were made explicitly empty. No accepted
source/diagnostic API, dependency, profile, lock, baseline or workflow changed.

Installed GSD bookkeeping actually ran: state.advance-plan, state.update-progress,
state.record-metric, state.record-session, three state.add-decision operations,
roadmap.update-plan-progress and requirements.mark-complete LEX-01. The roadmap
reported 1/3 In Progress; the requirement helper completed only LEX-01. The next
plan pointer is administrative and does not start 03-02.

Generic state metadata again mis-inferred milestone identity/completed phases
and counted support documents as plans (13/20 rather than 13/15); bounded
reconciliation preserves the actual M1 name, two completed phases and 1/3 Phase 3.
One file-input decision query rejected a /tmp path before mutation; supported
literal arguments then recorded all three decisions. No additional repository
artifact was created. The execution record preserves the actual results.

Project approval gates override generic automatic commit and advancement steps.
No commit/publication, phase-goal verification or next-wave execution is claimed.

## User Setup Required

None for this documentation-only closeout. No new service, package or toolchain
configuration is introduced.

## Closeout publication and next-plan gates

Owner acceptance and canonical Plan 03-01 bookkeeping are complete. Only this
five-file closeout packet's commit/publication awaits separate owner approval.
No extra required plan-closeout artifact was identified; phase-wide verification
and LEX-02–06 acceptance belong to later authorized work.

After closeout, Wave 2 still requires a fresh explicit owner instruction. Its
entry checks must re-observe the authorized remote branch, verify the exact
accepted profile/commit, and check ancestry plus unchanged profile content if
the tip advanced. The publication receipt here is historical evidence, not a
substitute for that future fresh check. Do not automatically execute 03-02.

Platform impact:
- Windows: Same experimental byte/recovery specification. Prior Phase 1/2 hosted
  CI remains accepted; new lexer native/hosted evidence is future work.
  Developer-host inspection remains independently pending/non-blocking.
- Linux: Primary Omarchy developer host; Ubuntu CI remains an independent
  reviewed baseline. Only documentation/file integrity checks ran here.
- macOS future: Deferred; no platform, terminal, filesystem or ABI dependency added.

## Self-Check: PASSED

- Accepted profile and both required checkpoint/summary files exist; exact
  dedicated SHA 52f598aaf031e549ed3bbe5c598bc43f5f7be6bb contains the two approved
  artifact files, and the saved actual remote publication receipt identifies it.
- The generic commit-subject search for 03-01 returned no match because the owner
  approved a descriptive subject without that identifier. Direct SHA inspection
  supplies the real commit identity; history remains unchanged.
- Both tasks' documentary acceptance criteria and plan verification were reviewed
  against actual content, accepted document checks and completed owner gates.
  Fresh Wave 2 permission remains a later entry condition.
- Canonical summary schema and artifact checks pass; lifecycle queries agree
  with 1/3 Phase 3, while only 03-02/03-03 await execution. LEX-01 alone is newly
  complete; the global ledger has 22 accepted and 27 pending requirements.
- All 14 decisions, 36 accepted hashes, four locks and published profile bytes
  are unchanged. Links, fences, whitespace and exact five-file scope pass.
- No native lexer validation, new CI outcome or whole-phase acceptance is
  implied. Closeout commit/publication and Wave 2 remain separately owner-gated.
