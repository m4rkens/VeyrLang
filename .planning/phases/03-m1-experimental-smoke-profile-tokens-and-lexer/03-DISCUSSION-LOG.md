# Phase 3: M1 Experimental Smoke Profile, Tokens, and Lexer - Discussion Log

> **Audit trail only.** Downstream research/planning/execution consumes
> 03-CONTEXT.md. This log preserves questions, alternatives and owner selections.

**Date:** 2026-10-15
**Phase:** 03-m1-experimental-smoke-profile-tokens-and-lexer
**Areas discussed:** Smoke result contract; provisional spelling; lexical boundaries; recovery and diagnostics.
**Substantive decisions:** 14, each explicitly selected by the owner.

## Area selection

The owner invoked `$gsd-discuss-phase 3` and selected `all` when offered:

| Option | Area | Selected |
|--------|------|----------|
| 1 | Smoke result contract | Yes |
| 2 | Provisional spelling | Yes |
| 3 | Lexical boundaries | Yes |
| 4 | Recovery and diagnostics | Yes |

`all` selected the discussion areas; it did not authorize automatic answers,
research, planning, execution, commits or phase movement. Questions used the
installed workflow's text-mode fallback. Existing phase contracts were carried
forward rather than reopened.

## Smoke result contract

### D-01 — What return-value range should the M1 smoke profile accept?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | 0–255 (recommended): a small range suitable for identical expected exit-code checks on Windows and Linux. | Yes |
| 2 | 0–2,147,483,647: nonnegative signed-32-bit values, with platform exit-code observation rules documented separately. |  |
| 3 | Another range: specify it. |  |

**User's choice:** `1`.

**Notes:** 0–255. This is M1-only. The lexer recognizes decimal digit sequences; range validation belongs to the later semantic phase.

### D-02 — What should the smoke entry's result type represent?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Signed 32-bit integer (recommended): M1 restricts accepted values to 0–255; later lowering/backend work has a concrete result width. | Yes |
| 2 | Width unspecified: the profile defines only 0–255; representation is decided in a later phase. |  |

**User's choice:** `1`.

**Notes:** signed 32-bit integer result semantics, with M1 accepted values restricted to 0–255. This gives later lowering/backend work a concrete width without accepting final Veyr type syntax or a stable ABI.

### Navigation/capture checkpoint — More result semantics or next area?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Next area: provisional spelling | Yes |
| 2 | Discuss more result semantics |  |

**User's choice:** `1`.

**Notes:** next area, provisional spelling.

## Provisional spelling

### D-03 — For the M1-only entry introducer and fixed name, choose a spelling.

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | fn main (recommended): compact spelling for the single fixed entry; general functions remain outside M1. | Yes |
| 2 | entry main: explicitly spells the construct as an entry point. |  |
| 3 | Different spelling: specify the introducer and name. |  |

**User's choice:** `1`.

**Notes:** fn main. The spelling is experimental and does not accept final Veyr function syntax or general functions.

### D-04 — How should the signed 32-bit result type be spelled?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | int32 (recommended): follows the existing type-system document's naming direction. | Yes |
| 2 | i32: a shorter experimental spelling. |  |
| 3 | Another spelling: specify it. |  |

**User's choice:** `1`.

**Notes:** int32. This follows the existing type-system document's naming direction and remains an M1-only spelling choice.

### D-05 — Choose the M1 signature form.

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | fn main() -> int32 (recommended): explicit empty parameter list and result annotation, matching the smoke-profile template's structure. | Yes |
| 2 | fn main(): int32: same meaning, with a colon annotation. |  |
| 3 | Another form: specify it. |  |

**User's choice:** `1`.

**Notes:** fn main() -> int32. M1 accepts only this fixed entry signature with an empty parameter list and explicit result annotation.

### D-06 — Choose the M1 body spelling.

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | { return 42; } (recommended): braces, return keyword and mandatory semicolon give explicit boundaries while whitespace is skipped. | Yes |
| 2 | { return 42 }: braces and return, without a semicolon. |  |
| 3 | Another form: specify it. |  |

**User's choice:** `1`.

**Notes:** { return 42; }. Braces, the return keyword and a mandatory semicolon are required. 42 is illustrative; the body contains one integer-literal return under the selected 0–255 semantic range.

### Navigation/capture checkpoint — More provisional spelling or next area?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Next area: lexical boundaries | Yes |
| 2 | Discuss more provisional spelling |  |

**User's choice:** `1`.

**Notes:** next area, lexical boundaries.

## Lexical boundaries

### D-07 — Which whitespace bytes should M1 skip?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Space, TAB, LF and CR (recommended): ordinary formatting and Windows/Linux line endings with a small explicit set. | Yes |
| 2 | Those plus vertical TAB and form feed: the full ASCII whitespace set. |  |
| 3 | Another explicit set: specify it. |  |

**User's choice:** `1`.

**Notes:** space (0x20), TAB (0x09), LF (0x0A), and CR (0x0D). Source bytes remain unchanged; Phase 2 location mapping still treats lone CR as an ordinary byte.

### D-08 — Should the four fixed words be case-sensitive?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Yes, exact lowercase (recommended): fn, main, int32, return. | Yes |
| 2 | ASCII case-insensitive: accept variants such as FN, Main and RETURN. |  |

**User's choice:** `1`.

**Notes:** exact lowercase fn, main, int32, return, for the M1 smoke profile only.

### D-09 — How should fixed words interact with adjacent letters, digits and underscores?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Whole-word matching (recommended): recognize only complete fixed spellings; return42 and fnmain are unsupported M1 words. | Yes |
| 2 | Fixed-word prefix matching: permit return42 as return plus 42, and fnmain as fn plus main. |  |
| 3 | Another boundary rule: describe it. |  |

**User's choice:** `1`.

**Notes:** whole-word matching. Scan an ASCII letter/underscore-led word containing following ASCII letters, digits or underscores, then recognize only the four fixed spellings. return42 and fnmain are unsupported M1 words; no general identifier capability is introduced.

### D-10 — Must the result arrow be contiguous?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Yes, -> is one two-byte token (recommended): whitespace may surround it, but - > is unsupported. | Yes |
| 2 | Allow whitespace between - and >: treat the separated form as the same annotation. |  |

**User's choice:** `1`.

**Notes:** -> is one two-byte token. Whitespace may surround it, but - > is unsupported M1 signature punctuation.

### Navigation/capture checkpoint — More lexical boundaries or next area?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Next area: recovery and diagnostics | Yes |
| 2 | Discuss more lexical boundaries |  |

**User's choice:** `1`.

**Notes:** next area, recovery and diagnostics.

## Recovery and diagnostics

### D-11 — After an invalid run is consumed, what should the recovered token stream contain?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Valid tokens plus EOF (recommended): emit an Error for the offending span, omit that run from the token stream, and continue. |  |
| 2 | Valid tokens, explicit Invalid tokens, and EOF: retain each offending span as an Invalid token as well as reporting it. | Yes |

**User's choice:** `2`.

**Notes:** valid tokens, explicit Invalid tokens, and EOF. Retain each offending span as an Invalid token as well as reporting an Error. Preserve token order and final EOF; accumulated Errors still prevent successful compilation.

### D-12 — How should unsupported numeric spellings be recovered?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | One Invalid token and Error per whole spelling (recommended): each unsupported numeric spelling forms one offending run; preserve surrounding program delimiters. | Yes |
| 2 | Preserve valid digit fragments: emit decimal tokens for recognizable portions and Invalid tokens/Errors for unsupported portions. |  |

**User's choice:** `1`.

**Notes:** one Invalid token and one Error per whole unsupported numeric spelling, preserving surrounding program delimiters. Discussed examples: -12, 0x10, 12_3, 12u, and 1.5. These spellings remain unsupported in M1.

### D-13 — How should non-ASCII bytes be grouped?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | One Invalid token and Error per consecutive non-ASCII byte run (recommended): group adjacent bytes 0x80–0xFF and retain exact byte spans without Unicode decoding. | Yes |
| 2 | One Invalid token and Error per byte: report each unsupported byte separately. |  |

**User's choice:** `1`.

**Notes:** one Invalid token and Error per consecutive non-ASCII byte run (adjacent bytes 0x80–0xFF), retaining exact original-byte spans with no Unicode decoding. Final source encoding remains open.

### D-14 — How should comment-shaped input be rejected?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Group it for rejection (recommended): // ends before LF/CRLF or at EOF; /*...*/ ends at first */; unterminated blocks run to EOF. Emit one Invalid token and Error per run. | Yes |
| 2 | Handle slash bytes individually: report unsupported bytes and tokenize remaining contents normally. |  |

**User's choice:** `1`.

**Notes:** group comment-shaped input for rejection. // runs end before LF/CRLF or at EOF; /*...*/ runs end at the first */. An unterminated block runs to EOF. Emit one Invalid token and Error per run; comments are not accepted as trivia.

### Navigation/capture checkpoint — All four areas are covered. Ready to capture context, more recovery questions, or explore another gray area?

| Option | Description | Selected |
|--------|-------------|----------|
| 1 | Ready to capture the discussion context | Yes |
| 2 | More recovery questions |  |
| 3 | Explore another gray area |  |

**User's choice:** `1`.

**Notes:** ready to capture the discussion context.

## Agent discretion

No substantive preference was delegated to the agent. Implementation/API/algorithm
choices within the captured contracts belong to separately authorized research
and planning. The owner explicitly chose Invalid tokens (D-11 option 2), rather
than the suggested omission of invalid runs; retain that choice downstream.

## Deferred ideas and inherited limits

No new out-of-scope feature was requested. Existing final grammar, encoding,
general language and later-stage deferrals remain unchanged. The three Phase 2
non-blocking diagnostic follow-ups remain separate, without implementation
permission. Windows developer-host inspection is independently pending/non-blocking;
macOS is deferred. Phases 1 and 2 remain closed at 8/8 and 4/4 respectively.

## Discussion completion and owner gate

The owner selected ready to capture context after all four areas. Created the
canonical context and this log, and recorded session continuity. Incremental
checkpoints covered completed areas and are removed after successful final capture.
No profile document, research, PLAN, implementation, test or CMake artifact was
created. No build/test/dependency command, CI operation, staging, commit or push
was performed during discussion.

Next gate: owner review and explicit authorization for a dedicated discussion
commit. Later research/planning requires its own concrete owner command. The
filled M1 profile requires review and a separately approved artifact commit before
syntax-dependent implementation/tests. This discussion captures M1-only choices,
not final Veyr language acceptance.
