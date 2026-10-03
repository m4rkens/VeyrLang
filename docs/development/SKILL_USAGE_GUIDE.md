# Skill Usage Guide

This document explains when to use Veyr's repository-local Codex skills.

The skills themselves live in:

```text
.codex/skills/
```

The policy overview lives in:

```text
docs/development/LOCAL_CODEX_SKILLS.md
```

This guide is the practical scenario map: when a skill applies, how it should be announced, and what it must stop before doing.

## Core rule

```text
Repository is canonical.
Chat history is not canonical.
```

A skill may inspect, summarize, compare, propose, and prepare review packets.

A skill must not silently:

- advance GSD lifecycle steps;
- run or retry CMake/Ninja/CTest/build commands;
- clean or recreate build directories;
- create commits;
- modify sensitive files during planning;
- convert chat-only design ideas into accepted language semantics;
- continue after ambiguous owner approval.

## Required announcement format

When a skill is used, the agent must say so explicitly:

```text
Applying local skill: <skill-name>
Reason: <specific trigger from this task>
```

If multiple skills apply, list them in the order they will be applied.

Example:

```text
Applying local skills:
1. veyr-session-rehydrate — fresh Windows session after Linux work.
2. veyr-gsd-phase-gate — owner asked to continue near a lifecycle boundary.
3. veyr-cmake-watchdog — next requested action may run CMake/Ninja.
```

## Scenario matrix

| Situation | Required skill(s) | What the agent should do | Must stop before |
|---|---|---|---|
| New Codex chat/session | `veyr-session-rehydrate` | Read repo docs/state, summarize current project state. | Any execution/verification/commit/phase transition. |
| Switching Linux -> Windows or Windows -> Linux | `veyr-session-rehydrate`, `veyr-cross-platform-impact` | Rehydrate from repo and identify platform differences. | Continuing previous chat-only assumptions. |
| Fresh clone, new worktree, tool reinstall | `veyr-session-rehydrate` | Read canonical docs and Git state. | Running setup/build commands without owner instruction. |
| Owner says “continue”, “next”, “go on”, “looks good” near GSD | `veyr-gsd-phase-gate` | Determine exact lifecycle boundary and ask for concrete command if ambiguous. | discussion -> discussion commit -> planning, planning -> planning commit -> execution, execution -> execution commit -> verification, verification -> verification commit -> next phase. |
| Before `$gsd-discuss-phase N` | `veyr-gsd-phase-gate` | Confirm this is discussion only. | Creating implementation files or running builds. |
| After `$gsd-discuss-phase N` completes | `veyr-gsd-phase-gate` | Summarize captured decisions; if tracked artifacts exist, propose the discussion commit. | Auto-commit or automatically running `$gsd-plan-phase N`. |
| Before `$gsd-plan-phase N` | `veyr-gsd-phase-gate`, optionally `veyr-design-probe-planner` | Confirm planning scope and project pressure. | Creating tests/source/CMake or executing plan. |
| After `$gsd-plan-phase N` completes | `veyr-gsd-phase-gate` | Present plan for review and propose a planning commit. | Auto-commit or automatically running `$gsd-execute-phase N`. |
| Before `$gsd-execute-phase N` | `veyr-gsd-phase-gate` | Confirm exact phase/task and owner command. | Verification/commit/next phase. |
| Build/configure/test command will run | `veyr-cmake-watchdog` | Show command/cwd/purpose; monitor output. | Retrying, increasing timeout, launching duplicate process. |
| Conan/dependency setup, profiles/recipes/cache, LLVM integration, setup/doctor scripts | `veyr-developer-bootstrap`, `veyr-cross-platform-impact`, and `veyr-cmake-watchdog` before long commands | Check Conan ownership boundary, exact dependency/profile choices, cache isolation, and Windows/Linux parity. | Reimplementing package-manager mechanics, destructive cache deletion, execution without owner command. |
| CMake/Ninja/CTest/Conan/build produces no new output for ~60s | `veyr-cmake-watchdog` | Inspect liveness and continue if progressing; stop/report only on a real stall or other hard-stop condition. | Automatic termination solely due to silence, retry, or cleanup. |
| Diff touches CMake/source/tests/scripts/`.vyr`/manifest behavior before commit | `veyr-sensitive-commit-review` | Prepare sensitive-file review packet. | Commit. |
| Creating or changing `Veyr.toml`, `Veyr.lock`, target profiles, package/dependency behavior | `veyr-project-system-steward` | Validate project-system semantics and cross-platform impact. | Treating manifest syntax/semantics as final without design review. |
| Planning a language feature | `veyr-design-probe-planner` | Tie the feature to a project/probe and semantic pressure. | Choosing syntax first or implementing feature without pressure. |
| Adding a project card or milestone | `veyr-design-probe-planner` | Document what language capability the project forces. | Feature-driven roadmap drift. |
| Touching paths, files, processes, dynamic libraries, networking, threads, windowing, graphics, packages | `veyr-cross-platform-impact` | Add Windows/Linux mandatory and macOS-future impact notes. | Linux-only assumptions. |
| Editing graphics/windowing/GPU work | `veyr-cross-platform-impact`, `veyr-design-probe-planner`, possibly `veyr-project-system-steward` | Check Win32/Linux/macOS surface strategy and build profiles. | Backend abstraction before real API probes. |
| End of session before switching machine | `veyr-session-rehydrate` | Produce handoff packet: state, branch, git status, next safe step. | Leaving work only in chat. |

## Concrete examples

### Example 1 — new Windows session after Linux work

Trigger:

```text
Owner opens Codex on Windows after working on Linux.
```

Expected behavior:

```text
Applying local skill: veyr-session-rehydrate
Reason: fresh Windows session after Linux work.

I will read AGENTS.md, README.md, docs/development/*, docs/design/*,
.planning/STATE.md if present, current phase artifacts if present,
git status, and recent commits.

I will then summarize the project state and stop before running commands.
```

The agent must not continue from memory of a previous Linux chat.

### Example 2 — owner says “dobra, next” after planning before the planning commit

Trigger:

```text
Owner says: dobra, next
```

Expected behavior:

```text
Applying local skill: veyr-gsd-phase-gate
Reason: ambiguous owner instruction at the planning commit / execution boundary.

"Next" is not enough to create the planning commit or start execution.
First review and explicitly approve the planning commit when tracked planning artifacts exist; after that, give the concrete execution command, for example:
$gsd-execute-phase 3
```

The agent must not infer permission to execute.

### Example 3 — quiet CMake command and confirmed stall

Trigger:

```text
cmake --preset foundation-debug
```

No new output appears for about 60 seconds. This is a soft liveness checkpoint, not a failure
or a reason to rerun the command.

Expected behavior while progress remains credible:

```text
Applying local skill: veyr-cmake-watchdog
Reason: CMake command is quiet; checking liveness at the soft threshold.

Process is still active and build artifacts/CPU time are changing, so I continue waiting.
Command: cmake --preset foundation-debug
CWD: <repo>
Last output:
<last visible lines>

I will continue supervising the original command and checking liveness while it progresses.
I will not launch a duplicate command, retry, or clean the build directory.
```

If subsequent process, child-process, CPU/I/O, and artifact observations confirm a real stall,
stop the original command once and report the evidence:

```text
Build supervisor stop
Command: cmake --preset foundation-debug
CWD: <repo>
Elapsed: <elapsed time>
Last output: <last visible lines>
Process/liveness evidence: <observations confirming a real stall>
Reason for stop: confirmed lack of progress, not silence alone

The original command has stopped. I will not retry, launch duplicate work, or clean the build
directory automatically. I will wait for the owner's concrete next command.
```

### Example 4 — CMake/source commit

Trigger:

```text
Diff contains:
M CMakeLists.txt
M src/lexer/lexer.cpp
A tests/lexing/lexer_test.cpp
```

Expected behavior:

```text
Applying local skill: veyr-sensitive-commit-review
Reason: commit would include CMake/source/test files.

Sensitive-file review packet:
- Files changed: ...
- Why each changed: ...
- Tests run: ...
- Risks: ...
- Platform impact: ...
- Exact proposed commit message: ...

I will not commit until you explicitly say:
commit these reviewed implementation changes
```

Ambiguous approval such as `ok`, `looks good`, or `continue` is not commit permission.

### Example 5 — adding a new language feature

Trigger:

```text
Owner asks to add closures or pattern guards.
```

Expected behavior:

```text
Applying local skill: veyr-design-probe-planner
Reason: requested work may introduce a language feature.

Project/probe pressure:
- Which project requires this?
- What semantic problem is blocked?
- Can an existing feature solve it?
- What invalid examples and diagnostics are needed?
- Is syntax being chosen before semantics?
```

The agent must not jump directly to syntax or implementation.

### Example 6 — project-system change

Trigger:

```text
Owner asks to add build profiles, dependencies, target kinds, or package scripts to Veyr.toml.
```

Expected behavior:

```text
Applying local skill: veyr-project-system-steward
Reason: task touches Veyr.toml/project-system semantics.

I will check:
- target kind semantics;
- platform profiles;
- dependency source/pinning;
- generated file/cache behavior;
- lockfile impact;
- Windows/Linux/macOS future behavior.
```

The agent must not silently invent final package-manager semantics.

### Example 7 — platform-sensitive code

Trigger:

```text
Task touches filesystem paths, dynamic libraries, sockets, terminal output, windowing, Vulkan/OpenGL/DX12/Metal, or generated bindings.
```

Expected behavior:

```text
Applying local skill: veyr-cross-platform-impact
Reason: task touches platform-sensitive behavior.

Platform impact:
- Windows: ...
- Linux: ...
- macOS future: ...
```

The agent must not ship Linux-only assumptions for Tier 1 work.

## Skill composition patterns

### Phase execution pattern

For a normal implementation phase:

```text
1. veyr-gsd-phase-gate
2. veyr-design-probe-planner if new language capability is involved
3. veyr-project-system-steward if manifest/build semantics are touched
4. veyr-cross-platform-impact if platform-sensitive files are touched
5. veyr-cmake-watchdog when build/test commands run
6. veyr-sensitive-commit-review before any sensitive-file commit
```

### Environment switch pattern

For Linux -> Windows or Windows -> Linux:

```text
1. veyr-session-rehydrate
2. veyr-cross-platform-impact if continuing platform-sensitive work
3. veyr-gsd-phase-gate before continuing a GSD phase
```

### Graphics/GPU pattern

For windowing/GPU/Vulkan/OpenGL/DX12/Metal work:

```text
1. veyr-design-probe-planner
2. veyr-cross-platform-impact
3. veyr-project-system-steward
4. veyr-sensitive-commit-review before commit
```

### Commit pattern

For any commit:

```text
1. inspect git status/diff
2. if sensitive files are present -> veyr-sensitive-commit-review
3. if platform-sensitive files are present -> veyr-cross-platform-impact
4. propose exact commit message
5. wait for explicit owner commit approval
```

## When not to use a skill

Do not invoke a skill just to sound formal when the task is a small, non-sensitive documentation edit that does not cross lifecycle boundaries, touch build/source/scripts/manifests, or introduce design decisions.

Even then, normal AGENTS.md rules still apply.

## Owner override

The project owner may explicitly instruct the agent to use a specific skill:

```text
Use veyr-session-rehydrate and summarize the repo state.
```

or:

```text
Apply the CMake watchdog for this command.
```

An owner override may request a skill, but it does not waive safety gates unless the owner also gives an explicit concrete command for the gated action.
