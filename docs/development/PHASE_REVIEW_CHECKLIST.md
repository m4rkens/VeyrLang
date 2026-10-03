# Phase Review Checklist

Use this before crossing or committing any GSD lifecycle stage.

## Discussion review

- Did the discussion capture all owner decisions?
- Did it separate accepted decisions from provisional syntax?
- Did it identify non-goals?
- Did it avoid implementation?

## Planning review

- Does the plan modify only the intended layer?
- Are files-to-change listed?
- Are forbidden files excluded?
- Are tests named with appropriate scope language?
- Are final-language implications avoided?
- Is there a rollback path?
- Does the plan stop before execution?

## Execution review

- Did the diff match the plan?
- Did it add only necessary files?
- Did it avoid silent design decisions?
- Did it avoid unrelated formatting churn?
- Are diagnostics precise?
- Are tests meaningful and not overfitted?

## Verification review

- Did configure/build/test run with expected presets?
- Did dependency resolution behave correctly?
- Is `git status --short` clean except intended changes before commit?
- Are generated files ignored?

# Stage commit review addendum

Before any discussion/planning/execution/verification commit, check:

- Was the commit explicitly requested?
- Does the commit contain only artifacts from the current lifecycle stage?
- For execution commits, are execution-time checks accurately reported without claiming dedicated verification already passed?
- For verification commits, are implementation fixes excluded and represented as gaps instead?
- Are there accidental implementation files from discussion/planning/verification?
- Are build artifacts/caches excluded?
- Is the commit message specific and allowed by `COMMIT_POLICY.md`?

Required commands before committing:

```bash
git status --short
git diff --stat
git diff --check
```


# Automatic progression / command watchdog

Before approving any phase transition, check:

- Did the agent stop after the previous lifecycle step?
- Did the project owner explicitly request the next exact command?
- Did the agent avoid `$gsd-progress --next` and equivalent automatic transitions?
- Were no implementation files changed during discussion/planning?
- Were no CMake/Ninja/CTest/build commands run during discussion/planning?
- For long execution/verification commands, did ~60 seconds of silence trigger liveness inspection rather than automatic termination?
- If a command actually stalled, did the agent report the exact command, last output, and liveness evidence without duplicate retries or destructive cleanup?

# Sensitive-file commit gate

Before committing, check whether the diff touches any sensitive files:

- `CMakeLists.txt`, `cmake/**`, `**/*.cmake`, `CMakePresets.json`;
- `include/**`, `src/**`, `tests/**`, `lib/**`, `tools/**`;
- C/C++ source/header/module files;
- scripts or automation files;
- `.vyr` source files;
- `Veyr.toml` / `Veyr.lock` when build/project behavior changes.

If yes, stop before commit and request owner review using `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`.

Do not commit sensitive files until the owner explicitly approves after reviewing the sensitive-file packet.


# Session handoff review

Before continuing work in a new Codex chat, new machine, reinstalled toolchain, or new worktree, check:

- Did the agent read `docs/development/SESSION_HANDOFF_POLICY.md`?
- Did it reconstruct state from repository files rather than prior chat history?
- Did it inspect `.planning/STATE.md` and current phase files if present?
- Did it inspect `git status --short` and recent commits?
- Did it produce a session rehydration summary?
- Did it stop before execution/verification/commit/phase progression?
- Are any chat-only decisions either confirmed again or written into docs/planning artifacts?
