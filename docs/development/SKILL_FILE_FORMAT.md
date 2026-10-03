# Codex Skill File Format

Local Codex skills live under `.codex/skills/<skill-name>/SKILL.md`.

Every `SKILL.md` must start with YAML frontmatter delimited by `---`. Codex uses the frontmatter metadata for skill discovery before it decides whether to read the full skill instructions.

Required fields:

```yaml
---
name: skill-name
description: >
  Short trigger-focused description of when Codex should use this skill.
---
```

Rules:

- `name` must match the skill directory name.
- `description` must be specific and trigger-focused.
- Put operational instructions after the closing `---`.
- Do not put long policy bodies in the frontmatter.
- Do not omit the frontmatter; Codex may skip the skill as invalid.

Minimal example:

```markdown
---
name: veyr-session-rehydrate
description: >
  Reconstruct Veyr project state after switching machines or starting a new Codex session.
---

# Skill: Veyr Session Rehydrate

## Purpose

Reconstruct project context from repository files.
```

When adding or editing a skill, run a quick local check before committing:

```bash
python3 - <<'PY'
from pathlib import Path
for path in Path('.codex/skills').glob('*/SKILL.md'):
    text = path.read_text(encoding='utf-8')
    if not text.startswith('---\n') or '\n---\n' not in text[4:]:
        raise SystemExit(f'invalid skill frontmatter: {path}')
print('skill frontmatter ok')
PY
```
