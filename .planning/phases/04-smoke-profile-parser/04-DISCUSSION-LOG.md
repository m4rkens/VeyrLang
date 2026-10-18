# Phase 4: Smoke Profile Parser - Discussion Log

> **Audit trail only.** Downstream agents use [04-CONTEXT.md](04-CONTEXT.md), not this log. This records alternatives and owner choices.

**Date:** 2026-10-18
**Phase:** 04-smoke-profile-parser
**Status:** Discussion captured; owner review and dedicated commit remain pending. No research, plans or execution.
**Areas discussed:** Parse result and source relationships; syntax failure and recovery; lexical-error handoff; parser diagnostic contract; token/source consistency.

## Discussion controls

| Checkpoint | Options presented | Owner selection |
|---|---|---|
| Initial area selection | Four listed areas; select numbers or all | `all` |
| Result-area continuation | 1 Next: syntax failure / 2 Explore further | `1` |
| Failure-area continuation | 1 Next: lexical-error handoff / 2 Explore further | `1` |
| Handoff-area continuation | 1 Next: parser diagnostics / 2 Explore further | `1` |
| End-of-diagnostics follow-up | 1 Explore token/source consistency / 2 Prepare context with that research question open | `1` |
| Final context checkpoint | 1 Prepare context/log / 2 Explore more | `1` |

The explicit `all` selected discussion areas, not automatic answers or an auto-chain flag. Each substantive choice below was separately answered `1`. The additional consistency question was resolved before context creation.

## Parse result and source relationships

### D-01: Should a well-formed parse result remain usable after input token storage is destroyed?

| Option | Description | Selected |
|---|---|---|
| 1. Independent value result (recommended) | Own minimal structure/spans without borrowing tokens or copying text; source resolution still requires its manager. | Yes |
| 2. Borrowed token-backed result | Borrow tokens and require caller-retained token storage plus SourceManager. |  |

**User's choice:** `1` — Independent value result (recommended).

**Captured contract:** Independent value result: own minimal structural data and original SourceSpans; no borrowed token references or copied source text. The originating SourceManager must outlive source resolution.

### D-02: Which source spans should the minimal result retain?

| Option | Description | Selected |
|---|---|---|
| 1. Entry, return statement and integer literal (recommended) | Retain entry, return-statement and literal boundaries for later diagnostics and AST handoff. | Yes |
| 2. Entry and integer literal only | Retain only entry and literal boundaries; reconstruct return boundaries if needed. |  |

**User's choice:** `1` — Entry, return statement and integer literal (recommended).

**Captured contract:** Retain entry, return-statement and integer-literal spans, without a general syntax tree. Numeric conversion and range validation remain later semantic work.

### D-03: How should the entry span treat leading and trailing whitespace?

| Option | Description | Selected |
|---|---|---|
| 1. Syntax bounds only (recommended) | Bound entry/return by syntax tokens and retain exact literal bounds; exclude surrounding whitespace. | Yes |
| 2. Whole-source entry span | Include leading/trailing whitespace in the entry span only. |  |

**User's choice:** `1` — Syntax bounds only (recommended).

**Captured contract:** Tight syntax bounds: entry from fn through closing brace; return from return through semicolon; literal exact token span. Internal whitespace remains within structural spans; surrounding whitespace remains in SourceManager. All offsets are original half-open byte offsets.

### D-04: Should the minimal parse result support ordinary copying and moving?

| Option | Description | Selected |
|---|---|---|
| 1. Copyable and movable value (recommended) | Ordinary inexpensive copy/move semantics for the span values. | Yes |
| 2. Move-only result | Explicit single-owner transfer despite containing only span values. |  |

**User's choice:** `1` — Copyable and movable value (recommended).

**Captured contract:** Copyable and movable value containing the three spans; no source/token ownership dependency. This is C++ infrastructure, not Veyr language ownership semantics.

## Syntax failure and recovery

### D-05: For an otherwise lexically valid token stream, what happens after the first syntax error?

| Option | Description | Selected |
|---|---|---|
| 1. Stop at the first syntax error (recommended) | One precise Error at the first syntax mismatch; no later recovery cascades. | Yes |
| 2. Recover at defined boundaries | Continue at defined delimiters for additional syntax Errors while still failing. |  |

**User's choice:** `1` — Stop at the first syntax error (recommended).

**Captured contract:** Stop at the first syntax error, emit one precise parser Error and return no successful parse result. Lexer recovery is unchanged.

### D-06: Should a failed parse expose any partial structural result?

| Option | Description | Selected |
|---|---|---|
| 1. No partial result (recommended) | Expose complete structure only after the required shape and EOF are recognized. | Yes |
| 2. Explicit partial result | Expose explicitly partial structure separately from successful parsing. |  |

**User's choice:** `1` — No partial result (recommended).

**Captured contract:** No partial parse representation. Return the entry/return/literal representation only after the complete required M1 shape and EOF are recognized.

### D-07: How should callers observe ordinary syntax failure?

| Option | Description | Selected |
|---|---|---|
| 1. Explicit checked outcome (recommended) | Explicit checked syntax failure plus engine Error; no ordinary syntax exception. | Yes |
| 2. Exception-based outcome | Append a diagnostic and throw a dedicated syntax exception. |  |

**User's choice:** `1` — Explicit checked outcome (recommended).

**Captured contract:** Explicit checked syntax-failure outcome with the Error appended to DiagnosticEngine; malformed source does not throw a C++ syntax exception. Existing checked infrastructure errors and ordinary allocation-exception behavior remain distinct and unchanged.

## Lexical-error handoff

### D-08: How should the parser handle a stream containing Invalid tokens?

| Option | Description | Selected |
|---|---|---|
| 1. Do not attempt syntax parsing (recommended) | Reject the Invalid-containing stream without syntax parsing or added parser Errors. | Yes |
| 2. Parse the recovered stream | Inspect recovered valid tokens, suppress duplicate Invalid complaints and possibly report an independent syntax Error. |  |

**User's choice:** `1` — Do not attempt syntax parsing (recommended).

**Captured contract:** Do not attempt syntax parsing. Return an explicit failure, preserve lexical diagnostics and emit no additional parser Errors for that stream. No successful parse representation or compilation success follows lexical Errors.

### D-09: What if a well-formed stream without Invalid tokens is parsed using an engine containing earlier Errors?

| Option | Description | Selected |
|---|---|---|
| 1. Parse this stream normally (recommended) | A separate well-formed stream may yield a result while prior Errors remain authoritative. | Yes |
| 2. Refuse parsing whenever the engine has Errors | Refuse all parsing while any earlier Error is in the engine. |  |

**User's choice:** `1` — Parse this stream normally (recommended).

**Captured contract:** Parse this stream normally; a complete result describes this input syntax only. Preserve earlier Errors, which still prevent successful compilation.

### D-10: Where should the lexer-to-parser call boundary sit?

| Option | Description | Selected |
|---|---|---|
| 1. Caller supplies the token stream (recommended) | Caller invokes lex and supplies the completed stream, originating manager and engine; synchronous read only. | Yes |
| 2. Parser invokes the lexer internally | Parser accepts a source identity and invokes lexing internally. |  |

**User's choice:** `1` — Caller supplies the token stream (recommended).

**Captured contract:** Caller invokes lex and supplies its completed tokens, originating SourceManager and DiagnosticEngine. Parser reads synchronously without retaining borrowed token references or invoking lex internally.

### D-11: Should token-stream integrity be checked before syntax parsing or Invalid-token rejection?

| Option | Description | Selected |
|---|---|---|
| 1. Checked structural validation (recommended) | Check source/spans/kinds/order/nonempty non-EOF spans/final EOF first; malformed structure is an infrastructure failure. | Yes |
| 2. Trust the lexer contract | Treat malformed streams primarily as asserted/documented producer misuse. |  |

**User's choice:** `1` — Checked structural validation (recommended).

**Captured contract:** Checked structural validation first: source identity, checked span bounds, valid token kinds, ordered/nonoverlapping nonempty non-EOF spans, and exactly one final zero-width EOF at source end. Malformed streams produce a distinct checked infrastructure failure, not a language diagnostic. Preserve original SourceError payloads where applicable. No re-lexing is required.

## Parser diagnostic contract

### D-12: How informative should a syntax Error be?

| Option | Description | Selected |
|---|---|---|
| 1. Expected and found token (recommended) | Controlled expected/found labels explicitly identify the experimental M1 profile. | Yes |
| 2. Uniform generic message | Use a uniform unexpected-token message and rely on the primary span. |  |

**User's choice:** `1` — Expected and found token (recommended).

**Captured contract:** Identify expected spelling and actual token using controlled labels, explicitly naming M1 Experimental Smoke Profile; do not interpolate raw source bytes. Rendering remains separate. Example: expected semicolon, found closing brace in M1 Experimental Smoke Profile.

### D-13: Which primary span should an unexpected real token use?

| Option | Description | Selected |
|---|---|---|
| 1. Complete unexpected token span (recommended) | Highlight the whole actual unexpected token with its original half-open range. | Yes |
| 2. Empty span before the token | Point to an empty insertion location before that token. |  |

**User's choice:** `1` — Complete unexpected token span (recommended).

**Captured contract:** The complete unexpected token span with exact original half-open byte bounds, including when a different required token was missing before it.

### D-14: Where should an incomplete-input Error point?

| Option | Description | Selected |
|---|---|---|
| 1. Exact EOF span (recommended) | Point to [source_size,source_size), including [0,0) for empty source, and report end of source. | Yes |
| 2. Previous token when available | Highlight the preceding real token where possible; otherwise use EOF. |  |

**User's choice:** `1` — Exact EOF span (recommended).

**Captured contract:** Exact zero-width source-end EOF span [source_size,source_size), including [0,0) for empty source; report the expected spelling with found end of source. Existing SourceManager location rules apply unchanged.

### D-15: Should an Error expecting a closing delimiter include its opener as related context?

| Option | Description | Selected |
|---|---|---|
| 1. One related opener span (recommended) | One related recognized-opener span for expected closing delimiters; no separate diagnostic. | Yes |
| 2. No related spans | Every parser Error has only its primary span and message. |  |

**User's choice:** `1` — One related opener span (recommended).

**Captured contract:** For expected right parenthesis or right brace, include one related span for the already recognized matching opener, with a controlled label such as opening brace is here. Other syntax Errors have no related spans. This remains one Error, not an additional diagnostic.

## Token/source consistency

### D-16: Which lexical-consistency contract should the parser require?

| Option | Description | Selected |
|---|---|---|
| 1. Lexer-conforming producer (recommended) | Trust lexical classification from an accepted/equivalent conforming producer; retain structural checks without a second lexer. | Yes |
| 2. Additional kind/spelling checks | Add kind/spelling guards, while recognizing they do not prove full lexical grouping/boundary conformance. |  |

**User's choice:** `1` — Lexer-conforming producer (recommended).

**Captured contract:** Require tokens from the accepted lexer or an equivalent conforming producer. Runtime structural validation remains required, but lexical classification and boundaries are producer preconditions. Do not duplicate lexical recognition or promise checked rejection of every forged kind/spelling combination. A Fn token spanning return violates the producer contract.

## Agent discretion

No substantive choice was delegated. C++ names, exact checked-outcome/error representation, deterministic precedence among multiple structural violations and full controlled wording tables remain bounded research/planning details, subject to owner review. No new language semantics, dependency or architecture redesign was selected.

## Deferred ideas

No new out-of-scope feature was requested. Alternatives not selected do not authorize later work. Preserve the existing deferred diagnostics/test-maintenance follow-ups and separate Windows developer-host/macOS scope.

## Stage and evidence boundary

The discussion used installed GSD 1.6.1 default interaction with its text-mode overlay, prior accepted context, the actual Phase 4 roadmap/requirements and a lightweight public-API scout. No matching pending todo, separate SPEC, pre-existing Phase 4 context/plan or blocking handoff artifact existed at entry. No research or native validation was performed.

Completed Phase 3 records and the published experimental profile remain unchanged. The final Phase 3 documentation CI was last observed queued; no later success is inferred. No commit, push, manual CI action or automatic next-stage command occurred. The final context and this log replace the incremental checkpoint; its final bytes are retained locally before cleanup.
