# Audio / Application Tooling Probe Track

**Status:** Long-term validation track

## 1. Motivation

The user is interested in music production and tools such as REAPER/FL Studio. Veyr can later validate real-time-ish application design, plugins, buffers, and UI/tooling through small audio-oriented probes.

## 2. AUD-01 — Sample metadata indexer

Forces:

- filesystem traversal;
- binary/text metadata;
- string handling;
- database/index structures later.

## 3. AUD-02 — Audio buffer processor

Forces:

- slices;
- numeric types;
- tight loops;
- allocation visibility;
- explicit-cost abstractions.

## 4. AUD-03 — Plugin host sketch

Forces:

- dynamic libraries;
- ABI boundaries;
- callbacks;
- thread-affine resources;
- real-time constraints.

## 5. AUD-04 — DAW project/config parser

Forces:

- structured parsing;
- versioned file formats;
- diagnostics;
- large object graphs.

## Project cards

Smaller validation projects for this track are split into dedicated cards under:

```text
docs/design/tracks/projects/
```

Use those cards when planning milestones so the track develops through small concrete programs rather than one oversized project.
