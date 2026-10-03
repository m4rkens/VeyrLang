# Realtime Audio Buffer Probe

**Track:** audio  
**Related milestones:** M42, M46  
**Status:** validation project card, not implementation plan.

## Purpose

Small no-allocation audio buffer processing experiment.

This project exists to force concrete Veyr design pressure. It should stay intentionally small until the required language/compiler capability is proven.

## Minimal scope

- one focused executable or library;
- small inputs and deterministic outputs;
- clear failure diagnostics;
- no feature expansion unless the project cannot be completed without it.

## Forces Veyr to support

- slices;
- no-allocation sections;
- float handling;
- real-time constraints;

## Must not accidentally force

- complete audio engine;

## Exit criteria

- the project can be built through the documented Veyr project flow;
- expected success and failure cases are tested;
- any new language feature needed by this project has a design note, valid examples, invalid examples, and expected diagnostics;
- the project does not silently accept final syntax or semantics outside an accepted decision.

## Design questions to record

- Which missing capability blocked the simplest version?
- Was the capability language-level, standard-library-level, project-system-level, or tooling-level?
- Did the probe expose an ownership/resource-lifetime issue?
- Did diagnostics explain failures precisely enough?
- Should the feature graduate to accepted design, remain milestone-local, or stay a candidate?
