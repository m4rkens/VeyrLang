---
name: veyr-project-system-steward
description: >
  Use when modifying Veyr.toml, Veyr.lock, .veyr state, project roots, targets, dependency resolution, package caches, cross-platform profiles, or generated native build integration.
---

# Skill: Veyr Project System Steward

## Purpose

Guard the `Veyr.toml` manifest, `Veyr.lock`, `.veyr/` workspace state, target profiles, dependency semantics, and future project/build tool behavior.

## Trigger

Use this skill when a task touches:

- `Veyr.toml`
- `Veyr.lock`
- `.veyr/`
- project root/module selection
- executable/static/shared library targets
- dependency resolution
- package cache layout
- cross-platform project profiles
- generated CMake or native build integration

## Guardrails

- `Veyr.toml` is not CMake replacement implementation yet; it is the future Veyr project manifest/buildsystem contract.
- Do not invent package/version semantics beyond documented decisions.
- Do not make source imports choose dependency versions.
- Do not accept final manifest semantics without project-owner decision.
- Changes that alter build/project behavior are sensitive and require owner review before commit.

## Required output

```text
Project-system impact packet

Files affected:
Manifest/build behavior changed:
Targets affected:
Cross-platform effect:
Compatibility/open questions:
Owner review required before commit: yes/no
```
