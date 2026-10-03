# Windows/Linux CI Matrix Probe

**Track:** Cross-platform systems  
**Status:** validation project card  
**Primary milestone pressure:** M0-M7, recurring gate

## Minimal project

A project-infrastructure probe that defines how every phase will eventually be checked on Windows and Linux without making local development painful.

## Forces in Veyr

- CMake presets that work on both OSes;
- CTest labels;
- platform-specific test gating;
- dependency discovery policy;
- no shell-specific mandatory workflow.

## Must not accidentally force

- expensive full release pipeline;
- package publishing;
- macOS CI before feasibility is reviewed.

## Platform requirement

This project must consider Windows and Linux from the start. macOS behavior should be documented as future/deferred when it cannot yet be implemented.

## Exit criteria

- CI plan covers Windows and Linux;
- platform-specific tests are labeled;
- build commands are documented;
- no required script is single-shell-only.
