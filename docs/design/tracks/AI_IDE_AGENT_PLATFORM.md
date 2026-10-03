# Agent IDE / Developer Tooling Probe Track

**Status:** Long-term validation track

## 1. Motivation

A future project idea is a free/open Cursor-like IDE or wrapper around Codex/Claude/local LLMs: provider-agnostic, potentially based on VS Code/Code-OSS, with workflows/adapters and local/self-hosted model support.

This is not an early compiler milestone, but it is a strong long-term pressure test for Veyr tooling.

## 2. IDE-01 — Diagnostic navigator

Forces:

- structured diagnostics;
- JSON output;
- source spans;
- project graph;
- CLI integration.

## 3. IDE-02 — Project indexer

Forces:

- filesystem traversal;
- incremental cache;
- module graph;
- concurrency;
- cancellation candidate.

## 4. IDE-03 — LSP quality probe

Forces:

- recovered lexer/parser output;
- semantic queries;
- incremental document model;
- stable compiler library API.

## 5. IDE-04 — Agent workflow runner

Forces:

- process spawning;
- workspace sandboxing;
- structured logs;
- plugin architecture;
- configuration files.

## 6. Why this matters

This track prevents Veyr tooling from becoming only a batch compiler. It pressures diagnostics, project metadata, LSP, and developer UX.

## Project cards

Smaller validation projects for this track are split into dedicated cards under:

```text
docs/design/tracks/projects/
```

Use those cards when planning milestones so the track develops through small concrete programs rather than one oversized project.
