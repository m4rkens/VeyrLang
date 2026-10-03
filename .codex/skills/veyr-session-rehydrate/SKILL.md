---
name: veyr-session-rehydrate
description: >
  Reconstruct Veyr project state from repository files after switching environments, starting a new Codex session, reinstalling tools, or when prior chat context is unavailable. Use before continuing work; do not modify files or run build/phase commands.
---

# Skill: Veyr Session Rehydrate

## Purpose

Reconstruct Veyr project state from repository files when moving between Linux, Windows, future macOS, IDE sessions, CLI sessions, worktrees, or after reinstalling Codex/GSD/plugins.

This skill exists because Codex chat history is not canonical. The repository is canonical.

## Trigger

Use this skill when:

- the owner switches from Linux to Windows or Windows to Linux;
- a new Codex chat/session starts;
- tools/plugins/hooks were reinstalled;
- the working copy was freshly cloned;
- the owner says `rehydrate`, `new environment`, `read state`, `continue from repo`, or similar;
- the agent is unsure whether prior chat context is available.

## Hard rules

- Do not run CMake, Ninja, CTest, builds, tests, formatters, or verification.
- Do not modify files.
- Do not start/continue a GSD phase.
- Do not commit.
- Do not assume hidden context from another chat.
- If a decision exists only in chat, treat it as non-canonical and ask whether to record it in repository docs.

## Required read order

Read or inspect:

1. `AGENTS.md`
2. `README.md`
3. `docs/development/SESSION_HANDOFF_POLICY.md`
4. `docs/development/AGENT_STEP_AND_COMMAND_POLICY.md`
5. `docs/development/GSD_WORKFLOW.md`
6. `docs/development/COMMIT_POLICY.md`
7. `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`
8. `docs/development/CROSS_PLATFORM_POLICY.md`
9. `docs/development/LOCAL_CODEX_SKILLS.md`
10. `docs/agents/AGENT_ROLES.md`
11. `docs/design/DECISION_LOG.md`
12. `docs/design/milestones/MILESTONE_STRATEGY.md`
13. `.planning/STATE.md` if present
14. current `.planning/phases/**` artifacts if present
15. `git status --short`
16. `git log --oneline -5`

## Output format

Produce exactly this kind of summary and then stop:

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
- Planning/execution/verification status:

Sensitive state:
- Modified sensitive files:
- Untracked sensitive files:
- Build directories present:
- Suspicious generated files:

Project state:
- Intended next safe step:
- Required owner command before continuing:
- Platform concerns:

I will not run CMake/Ninja/CTest, execute a phase, verify, commit, or advance to another phase until you explicitly instruct me with the concrete command or exact step.
```

## Completion

After the summary, wait. Do not continue automatically.
