# Terminal and Diagnostics Portability Probe

**Track:** Cross-platform systems  
**Status:** validation project card  
**Primary milestone pressure:** M2-M7, M39

## Minimal project

A diagnostics renderer probe that prints colored and non-colored messages, paths, line/column info, and stable plain-text output on Windows and Linux terminals.

## Forces in Veyr

- color capability policy;
- stderr/stdout separation;
- Unicode/ASCII fallback thinking;
- snapshot-test stability;
- IDE vs terminal diagnostics split.

## Must not accidentally force

- final UI theme system;
- full Unicode source policy;
- localization/internationalization.

## Platform requirement

This project must consider Windows and Linux from the start. macOS behavior should be documented as future/deferred when it cannot yet be implemented.

## Exit criteria

- diagnostics have a portable plain-text mode;
- color can be disabled;
- Windows/Linux terminal differences are documented;
- tests do not depend on one console.
