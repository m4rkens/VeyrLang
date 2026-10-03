# Session Handoff Policy

**Status:** Required workflow policy  
**Purpose:** make Linux/Windows/macOS work portable without relying on Codex chat history.

This policy is mandatory for every AI agent, Codex session, GSD phase, terminal session, IDE integration, and machine switch used for Veyr.

## 1. Repository is canonical

Codex chat history is not project state.

The canonical project state is:

```text
repository files
+ committed design docs
+ committed or explicitly reviewed `.planning/` artifacts
+ git history
+ current `git status --short`
```

A previous chat on Linux, Windows, macOS, a web session, an IDE session, or another Codex thread is only background conversation. It must not be treated as authoritative unless the relevant decision is written into repository files.

## 2. Multiple chat histories are allowed

It is acceptable to have separate Codex chat histories for:

- Linux development;
- Windows development;
- future macOS feasibility work;
- IDE/editor integration;
- focused review sessions.

However, those chats must all reconstruct context from the repository. They must not depend on hidden memory from another machine or another chat.

## 3. Required rehydration before continuing

When starting a new Codex session, switching machines, reinstalling tooling, opening a new worktree, or returning after a long pause, the agent must rehydrate context before doing work.

At minimum, read or inspect:

1. `AGENTS.md`
2. `README.md`
3. `docs/development/GSD_WORKFLOW.md`
4. `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md`
5. `docs/development/SESSION_HANDOFF_POLICY.md`
6. `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`
7. `docs/development/COMMIT_POLICY.md`
8. `docs/development/CROSS_PLATFORM_POLICY.md`
9. relevant `docs/design/**`
10. relevant `docs/project-system/**`
11. relevant `docs/tooling/**`
12. `.planning/STATE.md` if it exists
13. current phase context/plan files if `.planning/` exists
14. `git status --short`
15. recent commits, for example `git log --oneline -5`

The agent must not assume `.planning/` exists in a clean starter pack. If it does not exist, treat the project as pre-GSD-init and ask for the intended next command.

## 4. Session rehydration summary

Before executing, planning, verifying, committing, or advancing anything in a new or transferred session, the agent must produce a short rehydration summary.

Required format:

```text
Session rehydration summary

Environment:
- OS / shell:
- Working directory:
- Branch:
- Git status:
- Recent commits:

Canonical docs read:
- ...

GSD state:
- Current phase:
- Last completed lifecycle step:
- Pending review gates:

Project state:
- Intended next safe step:
- Files currently modified/untracked:
- Platform concerns:

I will not run CMake/Ninja/CTest, execute a phase, verify, commit, or advance to another phase until you explicitly instruct me with the concrete command or exact step.
```

After producing this summary, the agent must stop and wait for the project owner.

## 5. Explicit continuation required

A new or transferred session may not continue merely because the previous chat had momentum.

The project owner must explicitly request the next action, for example:

```text
run $gsd-discuss-phase 3
run $gsd-plan-phase 3
execute Phase 3 task 1 only
review the Phase 3 plan only
run the verification command shown above
```

Ambiguous statements such as `continue`, `next`, `go on`, `looks good`, or `you know what to do` are not enough after a session or machine handoff.

## 6. Handoff packet at the end of a session

When finishing a work session, especially before switching Linux -> Windows or Windows -> Linux, the agent should produce a handoff packet.

Required format:

```text
Session handoff packet

Completed:
- ...

Changed files:
- ...

Commands run:
- ...

Verification:
- ...

Current git status:
- ...

GSD state:
- ...

Unresolved questions:
- ...

Next safe command suggestion:
- ...

Do not run the next command automatically.
```

If the handoff is incomplete, the next session must treat the repository as uncertain and ask for review before continuing.

## 7. Decisions must be written down

If a decision exists only in chat, it is not canonical.

Before relying on a chat-only decision, the agent must either:

- ask the project owner to confirm it again; or
- add it to the appropriate repository document or `.planning/` artifact and request review.

Accepted language-design decisions belong in `docs/design/DECISION_LOG.md`. Milestone-local provisional decisions belong in the relevant milestone/profile/planning document, not automatically in the decision log.

## 8. Cross-machine rules

When moving between Linux and Windows, or later macOS:

- do not copy build directories as project state;
- do not trust stale generated CMake state;
- do not assume path separators, executable suffixes, dynamic-library suffixes, shell syntax, or filesystem case behavior;
- re-run configuration only after explicit permission;
- prefer a clean `git pull` / clean working tree before continuing;
- report platform impact for platform-sensitive changes.

The repository must be sufficient to reconstruct project context on another machine.

## 9. If history and repo disagree

If Codex chat history, agent memory, or user recollection conflicts with repository files:

1. stop;
2. identify the conflict;
3. show the repository evidence;
4. ask the project owner which source should win;
5. write the resolution into the appropriate document before continuing.

Do not silently merge conflicting histories.

## 10. No hidden continuation from old sessions

The agent must never say or imply:

```text
I remember from the other machine, so I continued.
```

Correct behavior is:

```text
I reconstructed the state from repository files, here is the rehydration summary, and I am waiting for your explicit next command.
```
