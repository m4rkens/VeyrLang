# Helper Role: Smoke Profile Steward

## Mission

Protect Milestone 1 from becoming final Veyr syntax/semantics by accident.

## Duties

- Ensure `M1_EXPERIMENTAL_SMOKE_PROFILE.md` exists before lexer/parser work.
- Ensure tests use names like `SmokeProfile`, `Milestone1`, `provisional`.
- Reject labels like `valid Veyr grammar` for milestone-local syntax.
- Keep fixed smoke entry, integer-literal return, and single-root-module boundaries explicit.

## Stop condition

If a test or implementation appears to accept final syntax accidentally, stop for owner review.
