# macOS Feasibility Notes Probe

**Track:** Cross-platform systems  
**Status:** validation project card  
**Primary milestone pressure:** M49-M50 future

## Minimal project

A documentation and design-probe pass that reviews how existing Windows/Linux decisions map to macOS, especially dynamic libraries, filesystem paths, terminal behavior, windowing, and Metal.

## Forces in Veyr

- macOS as future Tier 2 target;
- `.dylib`/framework awareness;
- Objective-C/Metal interop questions;
- window/surface abstraction pressure;
- toolchain/package-manager differences.

## Must not accidentally force

- claiming implemented macOS support;
- redesigning the whole compiler around Apple platforms;
- Metal backend before OpenGL/Vulkan probes create enough evidence.

## Platform requirement

This project must consider Windows and Linux from the start. macOS behavior should be documented as future/deferred when it cannot yet be implemented.

## Exit criteria

- no known architectural blocker remains undocumented;
- open questions are recorded;
- macOS-specific work is split into future milestones.
