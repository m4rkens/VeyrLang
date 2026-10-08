# Phase 2: Source Identity, Spans, and Diagnostics - Discussion Log

> **Audit trail only.** Downstream research/planning/execution agents use
> `02-CONTEXT.md`; this log preserves the questions, alternatives, and owner choices.

**Date:** 2026-10-08
**Phase:** 02-source-identity-spans-and-diagnostics
**Mode:** Interactive text discussion; no delegated agents.
**Areas discussed:** Source lifetime and identity; Span validation; Paths and locations; Diagnostic contract.

## Authorization and carried-forward constraints

- Owner invoked `$gsd-discuss-phase 2` and selected **all** four presented areas.
- Phase 1 remains closed at 8/8 with final run 37826767824; closeout commit
  `1f7e67d11ea6a68790d29b27bd3e1750a1c5d29f`. No reopening/redesign unless a concrete regression or missing requirement is revealed.
- Linux remains primary development host; Windows remains Tier 1 compatibility/CI.
  Windows developer-host evidence is independently pending and does not block Phase 2 planning. macOS deferred.
- Discussion only; no automatic research, planning, implementation, build/test, CI, or commit.

## Source lifetime and identity

### D-01: Who owns registered source text?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | SourceManager owns immutable text (Recommended). Text and retrieved views remain valid until the manager is destroyed; adding sources cannot invalidate earlier views. | Yes |
| 2 | Caller owns immutable text. Caller must keep buffers alive and unchanged while any compiler consumer uses them. |  |

**Owner choice:** 1.

**Clarifications and rationale:** SourceManager owns immutable source buffers. Contents and derived views/span backing remain valid for the entire SourceManager lifetime. Adding sources must not invalidate prior views. No caller buffer lifetime requirements.

### D-02: If the same display path is registered twice, how should SourceManager respond?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Fresh SourceId each time (Recommended). Each registration represents a distinct immutable snapshot; path spelling does not determine identity. | Yes |
| 2 | Reuse only identical path and bytes. Changed contents receive a new SourceId. |  |
| 3 | Reject duplicate display paths. Caller must register each path only once. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Fresh SourceId for every registration, even with identical display path and bytes. SourceId identifies an immutable registered snapshot, never a filesystem path. Duplicate display names are allowed; old snapshots remain intact. Future reparsing/incremental/editor use motivates this contract but is not implemented in Phase 2.

### D-03: How far should SourceId identity extend?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | One SourceManager (Recommended). IDs belong to their originating manager; consumers use that manager to resolve them. No persistence across compiler runs. | Yes |
| 2 | Across managers within one process. IDs also distinguish snapshots owned by different managers; no persistence across compiler runs. |  |

**Owner choice:** 1.

**Clarifications and rationale:** One originating SourceManager only. Consumers resolve IDs through that manager. Different managers may reuse numeric IDs for unrelated snapshots. No cross-manager or cross-run identity guarantee in Phase 2.

### D-04: How should sources be named for diagnostics?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Caller-supplied display name (Recommended). Supports file paths and labels such as <memory> or <stdin>. Registration performs no filesystem lookup. | Yes |
| 2 | Filesystem path required. Every snapshot must carry a path, including in-memory inputs; registration still performs no filesystem lookup. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Caller-supplied display name, diagnostic metadata only and not source identity. May be a filesystem path or synthetic label such as <memory>, <stdin>, <repl>, <generated>. Registration performs no filesystem lookup or path canonicalization. SourceId is authoritative.

**Area completion:** Owner selected option 1 to move to the next area after four questions. A discussion checkpoint was saved.

---

## Span validation

### D-05: How should spans describe byte ranges?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Half-open [begin, end) (Recommended). Includes begin, excludes end; begin == end represents an empty location, including EOF. | Yes |
| 2 | Start plus byte length. Zero length represents an empty location; validation must also guard against arithmetic overflow. |  |

**Owner choice:** 1.

**Clarifications and rationale:** SourceSpan stores SourceId, begin byte offset and end byte offset. Half-open [begin, end): begin inclusive, end exclusive. begin == end is an empty span; [source_size, source_size) is valid EOF. Require begin <= end and end <= source byte size. Offsets address original bytes, not decoded characters/code points.

### D-06: What should happen when lookup receives an unknown SourceId or invalid span?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Explicit checked failure (Recommended). Report invalid input to the caller; never clamp, fabricate text, or rely only on debug assertions. | Yes |
| 2 | Fatal invariant failure. Treat invalid IDs/spans as compiler programming errors and terminate immediately. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Explicit checked failure. Unknown SourceId, reversed range (begin > end), and end > source_size are invalid compiler-infrastructure inputs. Public/runtime contract must remain checked, deterministic and fail closed. Never clamp, fabricate text, substitute a source, or rely only on debug assertions. Assertions may additionally document internal invariants.

### D-07: Should a span be allowed to split a CRLF pair?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Yes—validate byte bounds only (Recommended). Any in-bounds byte range is valid; retrieval preserves exact bytes and presentation handles newline interpretation separately. | Yes |
| 2 | No—require CRLF pairs to remain intact. Reject otherwise in-bounds spans that split the pair. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Yes: structural byte-bound validation only. SourceId must exist, begin <= end, end <= source_size. Any in-bounds half-open range is valid, including CRLF splits. Retrieval preserves exact original bytes. CRLF interpretation belongs to line/column mapping and diagnostics, never source-span identity or validity.

### D-08: How much detail should checked failures expose?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Distinct failure reasons (Recommended). Unknown source, reversed range and beyond-source range are distinguishable, with offending ID/offset details available. | Yes |
| 2 | One invalid-input failure. Failure plus details without distinct reason categories. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Distinct reasons for at least unknown SourceId, reversed range and beyond-source range. Retain offending SourceId and relevant offsets for caller/test inspection. These are infrastructure failures, not Veyr source-language diagnostics. Do not clamp, normalize or merge cases into one generic category.

**Area completion:** Owner selected option 1 to move to the next area after four questions. A discussion checkpoint was saved.

---

## Paths and locations

### D-09: Which byte sequences should advance the line number?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | LF and CRLF (Recommended). CRLF is one line break; lone CR remains an ordinary source byte. | Yes |
| 2 | LF, CRLF, and lone CR. Each is a line break, with CRLF counting once. |  |

**Owner choice:** 1.

**Clarifications and rationale:** LF and CRLF each count as one line break; CRLF advances exactly once. Lone CR remains an ordinary source byte. Original bytes unchanged. This is location mapping/rendering only, not final lexer whitespace semantics or encoding.

### D-10: What should reported columns mean?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | 1-based byte columns (Recommended). Lines start at 1; tabs occupy one byte and non-ASCII bytes are counted without decoding. Visual alignment is separate. | Yes |
| 2 | 1-based visual columns. Expand tabs and count display positions; requires additional rendering rules. |  |

**Owner choice:** 1.

**Clarifications and rationale:** 1-based line numbers and 1-based byte columns within the original source line. Tab is one byte/column; non-ASCII UTF-8 bytes are counted individually. No Unicode decoding or display-width calculation in SourceLocation mapping. Visual caret alignment, tab expansion, Unicode width and renderer-specific positioning are separate diagnostic-rendering concerns.

### D-11: Approve the proposed boundary-location rules?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Approve (Recommended). | Yes |
| 2 | Describe a different boundary rule. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Approved exactly: LF and both CRLF bytes belong to the preceding line at their respective byte columns. Byte immediately after the break is next line column 1. Empty-source EOF is 1:1. EOF after a trailing break is next line column 1. Otherwise EOF is final line one byte column past contents. Rules remain byte-based and independent from visual rendering.

**Exact proposed boundary rules approved:**

- LF and both CRLF bytes belong to the preceding line at respective byte columns.
- Immediately after a line break: next line, column 1.
- Empty source EOF: 1:1.
- EOF after trailing line break: next line, column 1.
- Other EOF: final line, one byte column past contents.

### D-12: How should diagnostic display names appear?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Preserve caller spelling (Recommended). Keep spelling, escape control characters only when rendering; no source identity or stored metadata change. | Yes |
| 2 | Normalize path-like names for display. Standardized separators/components without filesystem lookup; synthetic labels unchanged. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Preserve exact caller spelling: separators, casing, drive letters, relative components, spaces and synthetic labels. No presentation normalization/canonicalization. Stored metadata unchanged. Renderer escapes controls so names cannot inject lines or terminal control sequences. Display spelling does not affect SourceId/source identity.

**Area completion:** Owner selected option 1 to move to the next area after four questions. A discussion checkpoint was saved.

---

## Diagnostic contract

### D-13: Should every diagnostic require a primary span?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Primary span optional (Recommended). Source-linked messages carry primary and ordered labelled related spans; location-free messages allowed without fake sources. | Yes |
| 2 | Primary span required. Every diagnostic has a primary span; related spans allowed, location-free failures use a separate reporting path. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Primary span optional. One structured model supports location-free messages and an optional primary SourceSpan plus zero or more ordered, labelled related spans. Never fabricate sources or dummy spans. Related spans are supplemental and do not replace an existing primary. Independent of LLVM and rendering.

### D-14: Which severities should Phase 2 support?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Error, warning, note (Recommended). Errors prevent successful compilation; warnings/notes alone do not, independently of rendering. | Yes |
| 2 | Error and note only. Same error-state rule; warning support deferred. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Error, Warning, Note. Accumulated Error sets has_errors() true and any Error prevents successful compilation. Warning/Note alone do not affect has_errors(). Rendering never determines severity semantics. Language warning policy, groups, suppression and warnings-as-errors are outside Phase 2 unless explicitly required later.

### D-15: What should plain-text rendering include?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Header plus source excerpt (Recommended). Severity/message, primary location, bounded excerpt and ASCII marker, ordered labelled related locations; no required color/terminal features. | Yes |
| 2 | Location headers only. Severity/message and primary/related locations, without excerpts or carets. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Severity/message; primary stored display-name:line:column when present; bounded source excerpt around primary; ASCII caret/underline; ordered labelled related locations afterward. Location-free messages have no fabricated source context. Deterministic plain text only: no required color, terminal-specific capabilities, Unicode-width dependency or filesystem lookup. Use SourceManager stored snapshot/display name.

### D-16: How should accumulated diagnostics be ordered?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Emission order (Recommended). Preserve insertion order and repeated diagnostics; no automatic sorting, deduplication or severity filtering. | Yes |
| 2 | Source/location order. Deterministic SourceId/offset sorting; preserve duplicates and separately position location-free messages. |  |

**Owner choice:** 1.

**Clarifications and rationale:** Caller insertion/emission order. No automatic sorting by SourceId/location, deduplication or severity reordering. Repeated diagnostics remain observable. Future grouping/deduplication belongs to explicit presentation/query operations, not the accumulator.

**Completion choice:** After the fourth area, owner selected option 1, Ready to write Phase 2 CONTEXT and DISCUSSION-LOG, rather than explore more gray areas.

---

## Remaining implementation details

No substantive preference was delegated through a "you decide" answer. Later
research/planning must choose and review bounded implementation details within
the recorded contracts: storage/API representation, capacity handling, excerpt
limits and marker layout, escaping/byte presentation, checked renderer failures,
and integration/test breakdown. These are not new selected defaults or permission
to execute. No final language encoding, whitespace, grammar, or warning policy was accepted.

## Deferred Ideas

Incremental/editor/reparse use, synthetic labels, and explicit presentation grouping
motivate the contracts but do not authorize new implementations. Canonical later-phase
compiler work, virtual filesystem/file discovery, final encoding, advanced warning policy,
colored/JSON/SARIF/LSP output, cross-run identity and macOS bring-up remain outside this phase.

## Artifact and lifecycle boundary

- Discussion outputs: `02-CONTEXT.md`, this log, and the Phase 2 discussion session update in `.planning/STATE.md`.
- The temporary checkpoint is superseded by the complete context/log and removed after their validation.
- No research, plans, implementation edits, builds/tests, CI actions, commit, push, or next-stage transition occurred.
- Next gate: owner review and separately approved discussion-only commit; then an explicit `$gsd-plan-phase 2` command.
