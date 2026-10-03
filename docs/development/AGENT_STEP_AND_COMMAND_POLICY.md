# Agent Step and Command Policy

**Status:** Required workflow policy  
**Purpose:** prevent silent phase transitions, accidental execution, and stuck build/tool commands.

This policy is mandatory for all AI agents, Codex sessions, GSD phases, and project automation used in this repository.

## 1. Absolute explicit-step gate

The agent must never decide that the project should move to the next lifecycle step or create the next stage commit on its own.

Normal lifecycle:

```text
discuss -> owner review -> discussion commit if tracked changes
-> plan -> owner review -> planning commit
-> execute -> owner review -> execution commit
-> verify -> owner review -> verification commit
-> next phase
```

Each arrow requires explicit project-owner permission. Each commit is separately permissioned.

A completed command does not grant permission for the next command or commit. If a stage has no tracked changes, explicitly report `no commit needed` rather than creating an empty commit.

## 2. Exact command requirement

For GSD phase transitions, the project owner should provide the concrete command to run.

The agent may suggest a next command, but must not run it unless the project owner explicitly asks for that command or asks the agent to run that exact step.

Preferred pattern:

```text
Agent: Phase 3 plan is ready. Review it before execution.
Agent: Next command, if approved: $gsd-execute-phase 3
Owner: run $gsd-execute-phase 3
Agent: runs exactly that command
```

Ambiguous statements such as `continue`, `next`, `go on`, `proceed`, or `looks good` are not enough for GSD phase transitions when there is any risk of crossing from planning into execution or from a lifecycle stage into its commit or from one stage into the next. Ask for a concrete command.

## 3. Forbidden automatic progress commands

Do not use automatic phase-progress commands unless the project owner explicitly re-enables them after the workflow is trusted.

Forbidden by default:

```text
$gsd-progress --next
$gsd-progress --auto
any equivalent command that chooses the next phase automatically
```

The default workflow is manual, reviewed, and command-by-command.

## 4. If automatic progression happens

If the environment, hook, plugin, or agent advances to the next step without explicit project-owner permission:

1. stop immediately;
2. do not continue running commands;
3. do not create more files;
4. do not commit;
5. inspect `git status --short`;
6. report exactly what changed;
7. ask the project owner whether to revert, preserve, or inspect the changes.

If execution started during discussion or planning, implementation changes must be treated as contaminated until reviewed.

## 5. Build/test command permission boundary

CMake, Ninja, CTest, compiler invocations, dependency downloads, RED/GREEN loops, and build verification are allowed only during execution or verification after explicit project-owner permission.

They are forbidden during:

- discussion;
- planning;
- plan review;
- design-only review;
- commit-message preparation.

## 6. Long-running command announcement

Before running any potentially long-running command, the agent must state:

- exact command;
- working directory;
- why it is being run;
- whether it may configure, build, test, or download dependencies.

Examples of potentially long-running commands:

```text
cmake --preset ...
cmake --build --preset ...
ctest --preset ...
ninja
npm install
pip install
git submodule update
any dependency fetch/bootstrap command
```

## 7. Long-running command supervision

For CMake/Ninja/CTest/Conan/build/dependency commands:

1. Execute only during an owner-authorized execution/verification step.
2. Do not spawn duplicate/background retries.
3. After roughly 60 seconds without new output, emit/record a liveness heartbeat rather than
   assuming failure.
4. Inspect available process state, children, CPU time, I/O, and relevant artifact changes.
5. Continue while credible progress exists.
6. Stop on a real error, owner interruption, invariant violation, process conflict, explicit hard
   timeout, or credible lack of progress.
7. Report the exact command/cwd/last output/liveness evidence when stopping.

Silence is not itself a hard timeout.


## 8. CMake/Ninja special restrictions

CMake and Ninja are allowed to run only when the current phase explicitly permits execution/verification.

Additional restrictions:

- no hidden configure/build/test during planning;
- no nested CMake/Ninja invocations;
- no parallel duplicate CMake/Ninja processes unless explicitly requested;
- no silent dependency bootstrap that appears as a hung configure;
- no automatic cleanup/reconfigure loop after a timeout;
- no switching presets to `dev-debug`, LLVM-enabled, or dependency-heavy modes without approval.

If CMake appears stuck, the correct response is to stop and hand control back to the project owner, not to keep waiting indefinitely.

## 9. Manual recovery command report

When handing a failed/stuck command back to the project owner, report in this format:

```text
Command stopped after liveness checks indicated a real stall or another hard-stop condition.

Working directory:
<path>

Command:
<exact command>

Last output:
<last relevant lines>

Suggested manual check:
<same command or a safer diagnostic command>
```

Do not hide the actual command that was run.

## 10. Project-owner override

The project owner may explicitly override this policy for a single command or session.

The override must be explicit and narrow, for example:

```text
For this one verify command, wait up to 10 minutes even if output is quiet.
```

Any explicit hard timeout must be stated before the command; otherwise the soft liveness policy remains active.

## 11. Commit boundary for sensitive files

This command policy also applies at the commit boundary. If a change set touches CMake/build files, C/C++ implementation files, tests, scripts, automation, `.vyr` source files, or project manifest/build behavior, the agent must stop before commit and request owner review according to `docs/development/PRE_COMMIT_OWNER_REVIEW_GATE.md`.

Do not use successful verification as permission to commit these files.


## 12. Session handoff boundary

A new Codex session, machine switch, tool reinstall, or worktree switch is also a lifecycle boundary.

Before continuing after such a boundary, the agent must follow `docs/development/SESSION_HANDOFF_POLICY.md`: rehydrate from repository files, produce a rehydration summary, and wait for an explicit project-owner command.

Do not treat prior chat history as permission to continue execution, verification, commit, or phase progression.
