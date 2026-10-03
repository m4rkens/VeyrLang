---
name: veyr-cross-platform-impact
description: >
  Use whenever touching paths, processes, terminals, dynamic libraries, linking, FFI, sockets, threads, windowing, graphics APIs, package caches, generated bindings, or CI. Requires Windows/Linux impact and future macOS notes.
---

# Skill: Veyr Cross-Platform Impact

## Purpose

Keep Windows and Linux as mandatory Tier 1 platforms from the beginning and avoid blocking future macOS support.

## Trigger

Use this skill when touching:

- paths/filesystem
- process execution, args, env, exit codes
- terminals/ANSI/color
- dynamic libraries
- native linking
- FFI
- sockets/networking
- threads
- windowing
- graphics surfaces
- Vulkan/OpenGL/DX12/Metal
- package caches
- generated bindings
- CI/build profiles

## Required output

Every platform-sensitive plan/change must include:

```text
Platform impact:
- Windows:
- Linux:
- macOS future:
```

## Guardrails

- Do not implement Linux-only behavior without naming it as temporary and filing a future parity task.
- Do not hardcode `/`, `.so`, ANSI behavior, POSIX APIs, or case-sensitive assumptions without platform analysis.
- Do not make Windows path/process/linking an afterthought.
- macOS may be future Tier 2, but early choices must not structurally block it.
