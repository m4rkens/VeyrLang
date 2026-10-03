# Cybersecurity and Code-Analysis Probe Track

**Status:** Long-term validation track

## 1. Motivation

Veyr should eventually be useful for security tooling, static analysis, log processing, vulnerability-pattern detection, and compiler-like code analysis.

This track connects Veyr to projects such as:

- Sysmon/Splunk log parsing;
- MITRE ATT&CK rule matching;
- Atomic Red Team output parsing;
- web/security scanner utilities;
- source-code tokenization;
- AST/CFG/DDG graph export;
- vulnerability-pattern scanning;
- thesis-inspired AST/GNN/NLP data preparation.

## 2. SEC-01 — Log parser

Forces:

- files;
- large text processing;
- structured data;
- error aggregation;
- CLI reports.

## 3. SEC-02 — Rule matcher

Forces:

- maps/sets;
- pattern matching;
- tagged unions;
- domain-specific diagnostics;
- config parsing.

## 4. SEC-03 — Sigma-like subset

Forces:

- YAML/JSON-like parser pressure;
- expressions;
- rule validation;
- explainable diagnostics.

## 5. SEC-04 — Web/security scanner utility

Forces:

- HTTP/client or command execution later;
- output formats;
- report generation;
- error classification.

## 6. CODE-01 — Source tokenizer

Forces:

- source spans;
- diagnostics;
- lexing ergonomics;
- recovered token streams.

## 7. CODE-02 — AST extractor

Forces:

- recursive structures;
- tagged unions;
- ownership of tree data;
- source-location preservation.

## 8. CODE-03 — Graph export

Goal:

```text
source -> tokens -> AST-ish graph -> CFG/DDG-like edges -> JSON/CSV/export
```

Forces:

- graph structures;
- serialization;
- stable IDs;
- memory layout choices;
- large-data performance.

## 9. CODE-04 — Vulnerability-pattern scanner

Forces:

- analyzer-like flow;
- API misuse rules;
- diagnostics with remediation;
- false-positive handling;
- SARIF/JSON output candidates.

## 10. Why this track matters

This track validates Veyr as both:

- a systems language;
- a language for building analyzers, compilers, and security tools.

## Project cards

Smaller validation projects for this track are split into dedicated cards under:

```text
docs/design/tracks/projects/
```

Use those cards when planning milestones so the track develops through small concrete programs rather than one oversized project.
