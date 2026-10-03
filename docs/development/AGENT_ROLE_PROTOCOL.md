# Agent Role Protocol

Veyr may use helper agent roles to structure reviews, but roles are advisory and constrained.

## Activation

A role must announce itself:

```text
Role active: <role name>
Scope: <narrow task>
Authority: advisory only; no execution/verification/commit/phase transition without explicit owner command.
```

## Deactivation

A role ends when its packet is produced or when the owner switches tasks.

## Role packets

Roles should produce short packets, not broad rewrites:

- what was inspected;
- what risk was found;
- what concrete next owner command is needed;
- what the role will not do automatically.

## Interaction with skills

Local skills under `.codex/skills/` define operational checklists. Agent roles under `docs/agents/` define review responsibilities.

When both apply, use both:

```text
Role active: Build Sentinel
Applying local skill: veyr-cmake-watchdog
```
