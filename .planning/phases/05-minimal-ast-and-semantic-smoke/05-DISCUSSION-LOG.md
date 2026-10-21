# Phase 5: Minimal AST and Semantic Smoke - Discussion Log

> **Audit trail only.** Downstream research/planning/execution reads [05-CONTEXT.md](05-CONTEXT.md), not this log. This records actual questions, alternatives and owner choices.

**Date:** 2026-10-21
**Phase:** 05-minimal-ast-and-semantic-smoke
**Status:** Discussion captured; owner review and a separately approved discussion commit pending. No research, plans, implementation or native validation.
**Areas discussed:** Shared AST handoff; semantic diagnostics; malformed AST inputs; validated output and Error state.

## Discussion controls

| Checkpoint | Options presented | Owner selection |
|---|---|---|
| Initial area selection | Four listed areas; select numbers or all | `all` |
| AST-area continuation | 1 Next: semantic diagnostics / 2 More AST questions | `1` |
| Diagnostic-area continuation | 1 Next: malformed inputs / 2 More diagnostic questions | `1` |
| Malformed-input continuation | 1 Next: validated output and Error state / 2 More malformed-input questions | `1` |
| Final context checkpoint | 1 Ready to capture context / 2 Explore remaining questions | `1` |

The owner explicitly invoked `$gsd-discuss-phase 5`. The `all` answer selected
areas only: it did not enable --all/--auto, approve automatic answers, authorize
planning or permit a commit. Every substantive question below received its own
`1` answer. Text-mode questions followed the installed GSD Codex adapter.
Four-area checkpoints were preserved during discussion; the completed checkpoint
is retained in ignored `.veyr-dev/discussion/05/` after canonical context creation.

## Shared AST handoff

### D-01: Should the accepted owned ParsedSmokeEntry three-span value become the shared minimal AST?

| Option | Description | Selected |
|---|---|---|
| 1. Reuse the existing value (Recommended) | Use the existing owned three-span value as the common M1 AST, preserving parser API and avoiding duplicate structure. | Yes |
| 2. Introduce a distinct minimal AST value | Preserve the parser result but explicitly convert it into a separate minimal AST value. |  |

**User's choice:** `1` — Reuse the existing value (Recommended).

**Captured contract:** Reuse the existing value as the common M1 AST boundary for parser and semantic clients, preserving the parser API and avoiding duplicate structural representations.

### D-02: Should semantic clients be able to consume that AST independently of the parser/token interfaces?

| Option | Description | Selected |
|---|---|---|
| 1. Independent AST boundary (Recommended) | Expose the shared source-span value independently; semantic clients need no parser/token/lexer interface. | Yes |
| 2. Keep it within the parser interface | Keep the value within the existing parser interface and its dependencies. |  |

**User's choice:** `1` — Independent AST boundary (Recommended).

**Captured contract:** Independent AST boundary. The shared value depends only on source-span types; the parser continues exposing and returning the same value, while semantic clients need no token or lexer interface. This changes organization only, not ownership, spans or syntax behavior.

### D-03: What producer contract should semantic validation require?

| Option | Description | Selected |
|---|---|---|
| 1. Check public AST values (Recommended) | Check documented source/span/literal invariants for public caller-created as well as parser-produced values, without reparsing. | Yes |
| 2. Require conforming parser-produced values | Require conforming parser-produced ASTs as a producer precondition; check literal value and source-access failures. |  |

**User's choice:** `1` — Check public AST values (Recommended).

**Captured contract:** Check public AST values. Accept caller-constructed values as well as parser results, checking documented source/span and literal invariants before producing validated output. Do not reparse the program. Exact checks and precedence remain for the malformed-input discussion.

**Follow-through:** D-09–D-12 later settle the promised checked-input boundary. No parser-origin-only restriction was selected.

### D-04: Should successful semantic output remain usable after the input AST value is destroyed?

| Option | Description | Selected |
|---|---|---|
| 1. Own its required data (Recommended) | Own the validated constant and required spans; input AST destruction does not invalidate the result. | Yes |
| 2. Borrow the input AST | Borrow the input AST and require its lifetime during use. |  |

**User's choice:** `1` — Own its required data (Recommended).

**Captured contract:** Own its required data: retain the validated constant and source spans by value, so destruction of the input AST does not invalidate the result. Source-text resolution still requires the originating SourceManager to remain alive.

**Follow-through:** D-16 later fixes the owned payload as the complete shared AST plus validated constant.

---

## Semantic diagnostics

### D-05: For an out-of-range decimal literal, where should the primary Error span point?

| Option | Description | Selected |
|---|---|---|
| 1. The entire integer literal (Recommended) | Use the exact complete literal primary span; existing renderer bounds the excerpt. | Yes |
| 2. The first digit that establishes the range violation | Highlight the digit that first establishes a value beyond 255. |  |
| 3. The whole return statement | Highlight the complete return statement. |  |

**User's choice:** `1` — The entire integer literal (Recommended).

**Captured contract:** The entire integer literal, preserving its exact original-byte span, including leading zeros and very long digit sequences. The existing renderer bounds its displayed excerpt. The accepted range remains 0–255 inclusive.

### D-06: How should the range diagnostic be worded?

| Option | Description | Selected |
|---|---|---|
| 1. One fixed message (Recommended) | Use the exact fixed range message for every out-of-range decimal value, leaving digits to the source excerpt. | Yes |
| 2. Include a bounded digit preview | Include a truncated digit preview in addition to the range restriction. |  |

**User's choice:** `1` — One fixed message (Recommended).

**Captured contract:** One fixed message for every out-of-range decimal value, including extremely long literals: return value must be between 0 and 255 in M1 Experimental Smoke Profile. Do not copy digits into the diagnostic message; the source excerpt displays the offending bytes.

### D-07: Should that Error include related spans?

| Option | Description | Selected |
|---|---|---|
| 1. No related spans (Recommended) | One full-literal primary Error with an explicitly empty related-span list. | Yes |
| 2. One return-statement context span | Add one return-statement related span with owning label return statement is here. |  |

**User's choice:** `1` — No related spans (Recommended).

**Captured contract:** No related spans. One explicitly initialized Error uses the full literal as primary and an explicitly empty related-span list; rendering remains separate.

### D-08: If the DiagnosticEngine already contains an earlier Error, should semantic validation still report a new out-of-range literal?

| Option | Description | Selected |
|---|---|---|
| 1. Report the new range Error (Recommended) | Check the current input and append its range Error without removing/reordering earlier diagnostics. | Yes |
| 2. Stop on the earlier Error | Stop because of the earlier Error without diagnosing the current literal. |  |

**User's choice:** `1` — Report the new range Error (Recommended).

**Captured contract:** Report the new range Error: check the current input and append its Error, preserving every earlier diagnostic, insertion order and Error state. The successful-output gate with prior Errors remains for the later validated-output discussion.

**Follow-through:** D-13 later blocks successful validated output when prior Errors remain, while retaining current-input checks and diagnostics.

---

## Malformed AST inputs

### D-09: How should a structurally inconsistent caller-created AST be reported when individual source spans are valid?

| Option | Description | Selected |
|---|---|---|
| 1. A distinct checked AST error (Recommended) | Return an inspectable checked AST infrastructure error without adding a source-language diagnostic. | Yes |
| 2. A semantic Error diagnostic | Return rejection and append a semantic Error for invalid M1 structure. |  |

**User's choice:** `1` — A distinct checked AST error (Recommended).

**Captured contract:** Return a distinct checked AST infrastructure error with inspectable details and add no source-language diagnostic. Existing SourceManager failures retain their original checked SourceError details.

### D-10: When an AST has multiple faults, which checking order should apply?

| Option | Description | Selected |
|---|---|---|
| 1. Source checks first (Recommended) | First validate entry,return,literal source spans, then AST relationships, then literal content/range; preserve first checked source failure. | Yes |
| 2. Check each field completely before advancing | Validate each span and its relationships before advancing; an earlier relationship fault can precede a later SourceError. |  |

**User's choice:** `1` — Source checks first (Recommended).

**Captured contract:** Validate all three SourceManager spans first, in entry-return-literal order, preserving the first SourceError. Then check AST relationships, then literal content and range. Stop at the first failure and return no successful validated value.

### D-11: How should a literal span resolving to something other than one or more ASCII decimal digits be reported?

| Option | Description | Selected |
|---|---|---|
| 1. Checked AST error (Recommended) | Treat malformed literal bytes as a checked AST infrastructure failure, with no added language Error. | Yes |
| 2. Semantic Error diagnostic | Append a separate semantic Error for invalid literal spelling. |  |

**User's choice:** `1` — Checked AST error (Recommended).

**Captured contract:** Return a checked AST infrastructure error, not an added language diagnostic. Such malformed caller-created literals violate the shared AST literal contract and cannot be produced by the accepted lexer/parser.

### D-12: Accept the proposed structural span contract or adjust it?

| Option | Description | Selected |
|---|---|---|
| 1. Accept this structural contract (Recommended) | Require one SourceId, nonempty strict nested entry/return/literal spans, allow surrounding whitespace, and do not reparse keywords/punctuation. | Yes |
| 2. Adjust the contract - describe the required change | Owner may describe a different required structural contract. |  |

**User's choice:** `1` — Accept this structural contract (Recommended).

**Captured contract:** Accept: all three spans have the same SourceId, every span is nonempty, the return lies strictly inside the entry and the literal strictly inside the return. Surrounding source whitespace is allowed, so entry need not cover the whole source. These checks do not certify keywords or punctuation by reparsing.

---

## Validated output and Error state

### D-13: If current AST/value are valid but the DiagnosticEngine contains an earlier Error, should validation return a successful value ready for lowering?

| Option | Description | Selected |
|---|---|---|
| 1. No successful validated value (Recommended) | Finish checking current input; valid input plus accumulated Errors returns explicit blocked outcome with no duplicate Error and no validated value. Notes/Warnings alone do not block. | Yes |
| 2. Return a value valid only for this input | Return a locally valid value despite prior Errors and require a separate downstream error-state check. |  |

**User's choice:** `1` — No successful validated value (Recommended).

**Captured contract:** No successful validated value. Finish current input checks, preserve diagnostics, then return an explicit blocked-by-errors outcome without adding a duplicate diagnostic. Notes/Warnings alone remain nonblocking.

### D-14: How should the API prevent unchecked data from masquerading as validated output?

| Option | Description | Selected |
|---|---|---|
| 1. Construction restricted to successful validation (Recommended) | Only successful validation creates the new output type; clients inspect but cannot arbitrarily construct/mutate its constant and spans. | Yes |
| 2. Publicly constructible value | Allow public construction with invariants checked by downstream consumers. |  |

**User's choice:** `1` — Construction restricted to successful validation (Recommended).

**Captured contract:** Construction restricted to successful validation. Callers can inspect constant/spans but cannot directly construct or mutate arbitrary validated data. This applies only to the new validated-output type, not the ordinary shared syntax AST.

### D-15: Should the validated-output value be copyable?

| Option | Description | Selected |
|---|---|---|
| 1. Copyable and movable (Recommended) | Ordinary copy/move and replacement with another validated value; no input AST lifetime dependency or new language ownership/ABI commitment. | Yes |
| 2. Move-only | Move-only value to make infrastructure transfers explicit. |  |

**User's choice:** `1` — Copyable and movable (Recommended).

**Captured contract:** Copyable and movable. Own the constant/span payload without input-AST lifetime dependence; assignment can replace it with another already-validated value. This is C++ infrastructure, not Veyr ownership semantics or a final ABI commitment.

### D-16: Which source context should validated output retain?

| Option | Description | Selected |
|---|---|---|
| 1. The complete shared three-span AST plus the validated constant (Recommended) | Own the complete shared three-span AST and validated constant; preserve entry,return,literal context without tokens/text copies. | Yes |
| 2. Only the literal span plus the validated constant | Keep only constant/literal span, leaving entry/return context with caller. |  |

**User's choice:** `1` — The complete shared three-span AST plus the validated constant (Recommended).

**Captured contract:** The complete shared three-span AST plus the validated constant, all by value. Preserve entry, return and literal spans for later diagnostics/lowering; no token references or copied source text. Original SourceManager remains required for text resolution.

---

## Inherited contracts, not new discussion choices

The published profile already fixes the experimental signature, unsigned decimal
spelling, decimal leading-zero interpretation, inclusive 0–255 range and signed
32-bit result semantics. Phases 2/3/4 already fix original byte spans, manager-local
source identity/lifetime, checked SourceError payloads, persistent ordered owning
diagnostics, independent rendering and the accepted parser's syntax-only behavior.
The Phase 5 choices preserve those contracts rather than reopen them.

Runtime AST source/relationship/literal checks do not certify arbitrary surrounding
source syntax or foreign-manager provenance. Full program syntax remains a parser/
conforming-producer responsibility. Validated output does not authorize later
compilation stages to ignore Errors added after validation; any Error continues
to prevent successful compilation. These are inherited boundaries and consequences
of the selected contracts, not additional silently selected language decisions.

## The agent's Discretion

No substantive choice was delegated through "you decide". Exact C++ names/header
organization, checked-outcome representation and error payload/tie-break details,
resource-bounded conversion technique and test/build integration remain for later
research and the owner-reviewed plan within D-01–D-16. None was implemented here.

## Deferred Ideas

No new out-of-scope capability was proposed. Preserve the existing general-language,
ABI, module, lowering/backend/driver exclusions and non-blocking diagnostic/renderer/
lexer follow-ups. Stage 2 performance remediation remains independently deferred;
held Stage 1/Stage 2/abandoned CI-policy candidates remain isolated and unpublished.

## Final stage boundary

Only discussion records and the supported STATE session handoff were prepared.
AST-01/02 and SEMA-01/02 remain Pending; global 30 accepted/19 pending; Phase 5 has
no implementation plans or completed execution work. The next boundary is owner
review and an explicitly approved dedicated discussion commit. Research/planning
requires a later explicit `$gsd-plan-phase 5` authorization. No automatic command,
build/test, staging, commit, push or next-stage execution follows this discussion.
