---
name: veyr-design-probe-planner
description: >
  Use when planning language features, milestones, project cards, probes, examples, or capabilities. Forces real project pressure before semantics, syntax, implementation, and final design acceptance.
---

# Skill: Veyr Design Probe Planner

## Purpose

Convert real project pressure into language/compiler requirements without prematurely accepting syntax or overbuilding features.

## Trigger

Use this skill when planning a feature, milestone, project-card, design probe, new `.vyr` example, or language/library capability.

## Required order

Use this order:

```text
real project pressure
-> smallest project/probe
-> semantic problem
-> alternatives
-> selected semantic direction
-> interactions with ownership/types/modules/errors/FFI/platforms
-> syntax candidates
-> valid examples
-> invalid examples
-> diagnostics
-> compiler lowering implications
-> tests
-> decision log if final
```

## Guardrails

- Do not add syntax because it is familiar from another language.
- Do not treat examples as final grammar unless accepted in `DECISION_LOG.md`.
- Do not let implementation tests freeze syntax accidentally.
- Prefer smaller probes over broad feature implementation.
- Every design change must name the project/probe that forces it.

## Output format

```text
Design probe planning packet

Project/probe pressure:
Minimal task:
Capability forced:
Semantics first:
Open questions:
Non-goals:
Cross-platform impact:
M1/Mx milestone fit:
Does this require DECISION_LOG update? yes/no
```
