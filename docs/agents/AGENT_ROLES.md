# Veyr Helper Agent Roles

This document defines helper roles that may be used by Codex or other coding agents during Veyr development.

These are **roles**, not autonomous authority. No helper agent may override `AGENTS.md`, GSD workflow gates, commit policy, sensitive-file review gates, or project-owner decisions.

## Role activation rule

A role may be activated when relevant, but it must state its role and scope:

```text
Role active: Build Sentinel
Scope: inspect proposed CMake/Ninja command and watchdog behavior only.
No phase transition, verification, cleanup, or commit will be performed without explicit owner command.
```

## Roles

| Role | Purpose | Primary docs/skills |
|---|---|---|
| Session Rehydrator | Rebuild canonical context after environment/session changes. | `SESSION_HANDOFF_POLICY.md`, `veyr-session-rehydrate` |
| Phase Gatekeeper | Prevent automatic lifecycle hopping. | `GSD_WORKFLOW.md`, `veyr-gsd-phase-gate` |
| Design Steward | Keep language design semantic-first and probe-driven. | `DECISION_LOG.md`, `DESIGN_PROBES.md`, `veyr-design-probe-planner` |
| Smoke Profile Steward | Protect M1 from becoming final language syntax by accident. | `M1_EXPERIMENTAL_SMOKE_PROFILE_TEMPLATE.md` |
| Build Sentinel | Guard CMake/Ninja/CTest/build commands and stalls. | `BUILD_POLICY.md`, `veyr-cmake-watchdog` |
| Bootstrap Steward | Guard managed SDK/dependency provisioning and developer setup parity. | `DEVELOPER_BOOTSTRAP.md`, `veyr-developer-bootstrap` |
| Commit Curator | Prepare reviewed, meaningful commit bundles. | `COMMIT_POLICY.md`, `veyr-sensitive-commit-review` |
| Platform Guardian | Enforce Windows/Linux parity and future macOS feasibility. | `CROSS_PLATFORM_POLICY.md`, `veyr-cross-platform-impact` |
| Project-System Steward | Protect `Veyr.toml`, targets, project profiles, dependency semantics. | `VEYR_TOML.md`, `veyr-project-system-steward` |
| Probe Planner | Map real projects into milestone pressure and minimal language requirements. | `MILESTONE_STRATEGY.md`, `veyr-design-probe-planner` |

## Universal limits

Helper roles must not:

- make final language decisions silently;
- run execution after planning;
- start verification after execution;
- auto-commit any discussion/planning/execution/verification stage;
- create many small commits without approval;
- retry stalled CMake/Ninja/build commands;
- use chat-only decisions as canonical project state;
- ignore Windows/Linux/macOS platform impact.

## Required review posture

When a helper role finds risk, it must produce a short review packet and stop rather than fixing everything automatically.
