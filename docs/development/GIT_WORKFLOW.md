# Git Workflow — Veyr

**Status:** mandatory workflow guidance for agents and humans.

Git is both an implementation history and a GSD lifecycle audit trail.

## 1. Branching default

For early local development, a single branch is acceptable:

```text
main
```

For collaboration/remote review prefer short-lived topic branches.

## 2. Stage boundary discipline

Do not mix unrelated lifecycle states in one commit.

Preferred GSD history when tracked artifacts exist:

```text
discuss -> review -> commit discussion artifacts
plan    -> review -> commit planning artifacts
execute -> review -> commit implementation/task bundle
verify  -> review -> commit verification artifacts
```

Then move to the next phase only after the current boundary is explicitly resolved.

An empty stage does not require an empty commit.

## 3. Separation rules

- discussion commits contain discussion/decision artifacts only;
- planning commits contain `.planning/**` and approved planning docs only;
- execution commits contain implementation/tests/docs produced by execution;
- verification commits contain verification/gap/evidence records only;
- gap-fix implementation is execution work and must not be hidden inside a verification commit;
- generated build directories and machine-local SDK/evidence state are never committed.

## 4. Commit grouping examples

A plausible Phase 1 history:

```text
planning: capture phase 1 build decisions
planning: define phase 1 build foundation plan
build: implement managed toolchain foundation
verification: record phase 1 verification results
```

Large execution may use multiple coherent implementation commits. Do not split mechanically by file.

## 5. Agent confirmation template

Before each commit:

```text
Lifecycle stage:
  <discussion|planning|execution|verification|gap-fix>

Proposed commit:
  <area>: <summary>

Includes:
- ...

Does not include:
- ...

Verification / evidence:
- ...

Potential concerns:
- ...
```

Then wait for explicit owner approval. Sensitive execution files require the additional review packet.

## 6. Sensitive implementation/build/script review

If a proposed commit includes CMake/build-system files, compiler source/header files, tests, scripts, automation, `.vyr`, or project manifest/build behavior, apply `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`.

Successful verification never bypasses that gate.

## 7. Cross-session Git handoff

Before switching environments/sessions, produce the handoff packet defined in `docs/development/SESSION_HANDOFF_POLICY.md`.

The next session must reconstruct state from the repository, including the lifecycle-stage commits and any remaining uncommitted changes. Chat history is not canonical.
