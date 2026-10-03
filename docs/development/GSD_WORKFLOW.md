# GSD Workflow Policy

**Status:** Required workflow policy  
**Purpose:** prevent agent drift, accidental execution, and stage-history collapse.

## 1. Phase lifecycle

Each phase is manual and review-gated:

```text
discuss -> review -> commit discussion artifacts (if any)
-> plan -> review -> commit planning artifacts
-> execute -> review -> commit execution artifacts
-> verify -> review -> commit verification artifacts
-> next phase
```

Every transition and every commit requires explicit project-owner approval. No automatic phase hopping or automatic committing is allowed.

## 2. Allowed actions by mode

| Mode | Allowed | Forbidden |
|---|---|---|
| Discussion | questions, alternatives, captured decisions | code, tests, CMake/Ninja/CTest |
| Planning | `.planning/` plans/tasks/risks | implementation files, builds/tests |
| Execution | approved code/tests/docs | broad redesign, extra phases, unapproved syntax |
| Verification | configure/build/test/checks, verification records | new feature implementation |

## 3. Stage commit boundary

After a stage completes:

1. inspect its tracked diff;
2. present the stage for owner review;
3. if tracked artifacts exist, propose a stage-specific commit;
4. commit only after explicit approval;
5. if there is no tracked diff, report `no commit needed`;
6. stop before the next lifecycle command.

Do not fold discussion/planning/verification artifacts into a later implementation commit merely for convenience.

Execution commits touching sensitive files require `PRE_COMMIT_OWNER_REVIEW_GATE.md`.

Verification that finds implementation defects does not authorize code edits in verification mode. Record the gaps, then plan/execute the gap-fix work and commit that execution separately before rerunning verification.

## 4. Exact command gate

The agent may suggest the next GSD command but must not run it without a concrete owner instruction.

Ambiguous `continue`, `next`, `looks good`, or similar wording is not permission to cross a lifecycle boundary or create a commit.

`$gsd-progress --next` and equivalent automatic progression remain forbidden by default.

## 5. No build during discussion/planning

CMake, Ninja, CTest, compiler invocations, dependency/SDK downloads, RED/GREEN loops, and build verification are execution/verification actions only.

## 6. Phase 3 special rule

Before lexer implementation, Phase 3 must create:

```text
docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md
```

and commit the reviewed design/planning artifact before implementation that depends on it.

## 7. Long-running command supervision

During execution/verification, approximately 60 seconds without new output is a soft liveness
checkpoint.

Do not stop a healthy CMake/Ninja/CTest/Conan/compiler/dependency build merely because it is quiet.
Inspect available liveness signals and continue while credible progress exists.

Stop on a real error, owner interruption, invariant violation, process conflict, an explicit hard
timeout, or credible evidence the command is no longer progressing. Do not retry automatically or
perform destructive cleanup.

See `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md`.

## 8. Recovery from accidental lifecycle mixing

If implementation occurs during discussion/planning, or new implementation occurs during verification:

1. stop;
2. inspect `git status --short`;
3. identify contaminated files;
4. preserve reviewed stage artifacts;
5. revert/move implementation only after owner review;
6. restore the correct lifecycle boundary before continuing.

## 9. Planning output quality

Plans must include scope, non-goals, exact files expected to change, tests, guardrails, rollback, unresolved questions, platform impact when applicable, and a statement that provisional syntax is not final unless recorded.

## 10. Session handoff

A new session/machine/worktree must reconstruct context from repository files, `.planning/`, Git history/status, and the stage-specific commits. Produce a rehydration summary and wait for the owner's exact next step.
