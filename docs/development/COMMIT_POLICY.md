# Commit Policy — Veyr

**Status:** mandatory agent/project workflow rule  
**Purpose:** make Git history a stage-by-stage audit trail for GSD work without permitting silent commits.

## 1. Explicit permission remains mandatory

Agents must not create a commit unless the project owner explicitly approves that specific commit boundary.

The new default is **not** "one commit at the end of a phase". The default is to preserve reviewed GSD lifecycle artifacts in separate commits.

No stage completion automatically grants commit permission.

## 2. GSD lifecycle commit model

When a GSD stage produces tracked changes, the preferred history is:

```text
discuss -> review -> discussion commit
plan    -> review -> planning commit
execute -> review -> execution commit
verify  -> review -> verification commit
next phase only after the intended stage commit boundary is resolved
```

An empty stage does not require an empty commit. If a stage produces no tracked change, record that fact in the review and proceed after owner approval.

### Discussion commit

Contains only durable discussion/decision artifacts produced by that GSD discussion step.

Examples:

```text
docs: capture phase 2 design decisions
planning: record phase 2 discussion context
```

It must not contain implementation.

### Planning commit

Contains only reviewed plan artifacts (`.planning/**` and explicitly approved planning docs).

Example:

```text
planning: define phase 2 implementation plan
```

It must not contain implementation, generated build state, or RED/GREEN test work.

### Execution commit

Contains the implementation/docs/tests created by the approved execution step. It may be unverified in the sense that the dedicated GSD verification step has not yet run, but all execution-time checks required by the plan should be recorded.

Examples:

```text
build: implement managed dependency foundation
compiler: implement source diagnostics foundation
```

If sensitive files are touched, the sensitive-file owner review gate applies before this commit.

### Verification commit

Contains verification artifacts only: GSD verification reports, updated planning state, evidence summaries, gap records, or documentation corrections discovered during verification.

Example:

```text
verification: record phase 2 verification results
```

Do not mix new feature implementation into the verification commit. If verification finds defects requiring code changes, create/execute the appropriate gap-fix work and commit that execution separately before the next verification result commit.

## 3. Commit cadence

Preferred cadence for a normal GSD phase:

```text
0-1 discussion commit
1 planning commit
1-N coherent execution commits
1 verification commit
```

Execution may be split into multiple coherent commits when the approved plan contains independently reviewable task bundles. Do not split mechanically per file or per tiny task.

Bad cadence:

```text
commit after every file
commit after every assertion
commit WIP because the agent is switching tasks
mix planning + implementation + verification in one commit
```

## 4. Required commit-message format

Use:

```text
<area>: <imperative summary>
```

Allowed areas include:

- `planning`
- `verification`
- `docs`
- `design`
- `build`
- `test`
- `compiler`
- `source`
- `diagnostics`
- `lexer`
- `parser`
- `ast`
- `sema`
- `ir`
- `backend`
- `driver`
- `project`
- `tooling`
- `ffi`
- `gfx`
- `runtime`
- `stdlib`
- `ci`
- `repo`

The summary must be action-oriented, preferably under 72 characters, with no trailing punctuation or emojis.

Forbidden vague summaries include `fix`, `wip`, `update files`, `misc`, `codex changes`, and bare `phase N`.

## 5. Commit body expectations

For non-trivial commits use:

```text
Why:
- ...

What:
- ...

Guardrails:
- ...

Verified:
- ...
```

For stage commits, state the GSD phase/stage in the body when useful, for example:

```text
GSD:
- Phase 1 planning artifact
```

Do not use a misleading `Verified:` claim in an execution commit before dedicated verification; say what execution-time checks actually ran.

## 6. Pre-commit inspection

Before every commit inspect:

```bash
git status --short
git diff --stat
git diff --check
```

Inspect the actual diff sufficiently to confirm it belongs to one lifecycle stage.

## 7. Staging rules

Do not use broad staging blindly. Prefer path-specific staging after reviewing status.

Never stage:

- `build/`;
- `.veyr/`;
- `.veyr-dev/`;
- compiler/dependency caches;
- local editor settings unless intentionally project-owned;
- accidental screenshots;
- generated artifacts from a different GSD stage;
- machine-local SDK/provenance evidence.

`.planning/` is intentionally trackable when it represents reviewed GSD discussion/planning/verification state. Do not blanket-ignore it.

## 8. Sensitive-file owner review gate

Before an execution/gap-fix commit that touches CMake/build files, C/C++ implementation, tests, scripts, automation, `.vyr`, or project-manifest behavior, apply `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`.

Planning and verification commits that contain only `.planning/**` or documentation do not become implementation-sensitive merely because they describe sensitive work. If they also modify sensitive implementation files, the gate applies.

## 9. Phase progression

A lifecycle stage may be considered reviewed but not yet committed. The agent must not silently skip the intended stage commit and fold it into a later commit.

Before starting the next GSD stage, explicitly report whether the current stage has:

```text
committed
no tracked changes / no commit needed
owner-approved deferred commit
```

Default: commit reviewed stage artifacts before moving to the next lifecycle stage.

## 10. History rewriting and remotes

Do not run these commands unless explicitly asked:

```bash
git commit --amend
git rebase
git reset --hard
git push --force
git push --force-with-lease
git tag
git branch -D
```

Do not push unless explicitly asked.

## 11. Recovery from accidental commits

Stop and report:

```bash
git log --oneline -10
git status --short
```

Ask whether to keep, revert, reset, or reorganize. Do not rewrite history without explicit approval.
