# Pre-Commit Owner Review Gate

**Status:** mandatory agent/project workflow rule  
**Purpose:** force human review before committing implementation-sensitive files.

This repository uses commits as an audit trail. Build logic, compiler source, tests, and scripts can silently change project behavior, cross-platform guarantees, dependency behavior, or language semantics. They require a dedicated owner review before any commit.

## 1. Sensitive file classes

The following file classes are **implementation-sensitive**:

### CMake / build-system implementation files

- `CMakeLists.txt`
- `cmake/**`
- `**/*.cmake`
- `CMakePresets.json`
- `CMakeUserPresets.json` if it ever becomes intentionally project-owned
- toolchain files such as `**/*Toolchain*.cmake`
- CI/build scripts that configure or invoke CMake/Ninja/CTest

### C/C++ compiler implementation files

- `include/**`
- `src/**`
- `tests/**`
- `lib/**`
- `tools/**`
- `**/*.c`, `**/*.cc`, `**/*.cpp`, `**/*.cxx`
- `**/*.h`, `**/*.hh`, `**/*.hpp`, `**/*.hxx`
- `**/*.ipp`, `**/*.inc`
- C++ module/interface candidates such as `**/*.ixx`, `**/*.cppm`, `**/*.mpp`

### Project scripts and automation

- `scripts/**`
- `.github/workflows/**`
- `**/*.sh`
- `**/*.ps1`
- `**/*.py`
- `**/*.bat`
- `**/*.cmd`
- `**/*.js`, `**/*.ts` when used as project automation
- generated guardrail/checker scripts

### Future Veyr implementation/source files

- `**/*.vyr`
- `Veyr.toml` when it changes project/build behavior
- `Veyr.lock`
- `templates/**` when templates generate build/source/project files

## 2. Mandatory stop before commit

If any sensitive file class is created, modified, deleted, renamed, or staged, the agent must stop before committing and ask for explicit owner review.

The agent must not proceed from successful verification to commit merely because tests pass.

The agent must not assume that a lifecycle-stage commit policy or a previous instruction like `commit after this` covers sensitive files unless the owner has reviewed the sensitive-file summary or explicitly waives this gate.

## 3. Required review packet

Before committing sensitive files, the agent must present this packet:

```text
Sensitive-file pre-commit review required.

Proposed commit:
  <area>: <summary>

Sensitive files changed:
- <path> — <why it changed>

Non-sensitive files changed:
- <path> — <why it changed>

Commands run:
- <command> — <result>

Not run:
- <command> — <reason>

Platform impact:
- Windows: <impact/none/unknown>
- Linux: <impact/none/unknown>
- macOS future: <impact/none/unknown>

Potential risks:
- <risk or none>

Exact approval needed:
  "approve this sensitive-file commit"
```

For CMake/build changes, include the relevant preset names and whether the change can trigger dependency downloads.

For source/test changes, state which compiler layer is affected and whether any language syntax/semantics are final, provisional, or infrastructure-only.

For scripts, state whether the script is cross-platform, platform-specific, or optional.

## 4. Required local checks before review packet

Before asking for review, inspect:

```bash
git status --short
git diff --stat
git diff --check
```

For sensitive files, also inspect the actual diff enough to summarize the behavioral change. Do not send a review packet based only on filenames.

## 5. Approval wording

Acceptable approval examples:

```text
approve this sensitive-file commit
commit these reviewed implementation changes
commit after my review is accepted
```

Ambiguous wording is not enough:

```text
ok
looks good
continue
next
ship it
```

When approval is ambiguous, ask for the exact approval phrase instead of committing.

## 6. Explicit override

The project owner may override this gate for a single commit, but the override must be explicit and narrow:

```text
For this commit only, skip the sensitive-file review gate and commit exactly these staged changes.
```

A general preference or old instruction does not permanently disable this policy.

## 7. Relation to normal commit policy

This policy is an extra gate on top of `docs/development/COMMIT_POLICY.md`.

A lifecycle-stage commit that touches sensitive files requires all of the following:

1. normal explicit commit permission;
2. clean pre-commit inspection;
3. sensitive-file review packet;
4. explicit owner approval after review;
5. compliant commit message.

If any condition is missing, do not commit.
