# M1 Experimental Smoke Profile

**Status:** Draft prepared by Plan 03-01; owner profile review/acceptance pending.
**Authority:** Phase 3 D-01–D-14 and the approved 03-01 planning contract.
**Implementation gate:** Dedicated commit approval, commit, separate publication
approval, verified remote publication and fresh Wave 2 authorization remain pending.

## 1. Warning

The M1 Experimental Smoke Profile is milestone-local and non-final.
Implementation support for this profile does not mean the spelling is accepted
Veyr grammar. Final Veyr syntax and semantics require separate design review and
a decision-log entry. This document is a reviewable draft, not owner approval of
its proposed recovery refinements or permission to implement them.

All concrete spellings, the `int32` result representation and the 0–255 result
restriction below are experimental M1 choices. They do not freeze final function,
module, return-statement, integer-literal, type-system, encoding or ABI policy.

## 2. Purpose

P01, the M1 smoke executable in [DESIGN_PROBES.md](../DESIGN_PROBES.md), forces
only the smallest compiler walking skeleton:

```text
source file -> tokens -> parser -> AST -> semantic smoke -> lowering -> LLVM -> native executable -> exit code
```

This profile supplies a shared milestone-local contract for those later stages.
Plan 03-01 authors documentation only; Phase 3 later implements token/lexer
behavior, not the rest of that pipeline. Existing source management and owned
diagnostics are reused. No lexer generator, dependency, filesystem loader,
general grammar framework or replacement reporting infrastructure is required.

## 3. Program shape

M1 supports one root source file/module, one fixed entry, one braced body,
one integer-literal return and EOF. It supports no imports or module graph.
This root boundary does not define final module or package syntax.

The fixed entry has an empty parameter list and explicit `int32` result annotation.
Its one return statement contains a decimal integer token followed by a mandatory
semicolon. Whitespace may surround lexical units, but cannot split a word,
integer token or contiguous arrow. Extra tokens, statements, declarations,
parameters or entry forms are outside this profile's program shape.

**D-01/D-02:** The eventual M1 result has signed 32-bit integer semantics,
restricted to values 0–255 inclusive. This supports the same expected smoke
exit-code checks on Windows and Linux. Later semantic validation owns range
checking; later lowering/backend work owns the concrete native implementation.
No stable calling convention, exported binary interface or general integer ABI
is selected here. The lexer must not parse or range-check integer values.

## 4. Concrete provisional spelling

**D-03–D-06:** The fixed signature is `fn main() -> int32`; its body uses braces,
`return`, one decimal integer token and a mandatory semicolon.

```text
fn main() -> int32 {
    return 42;
}
```

Other examples within the experimental program/value boundary are:

```text
fn main() -> int32 { return 0; }
fn main() -> int32 { return 255; }
```

The lexer recognizes units; it does not determine whether their order is a
complete program. These distinctions are mandatory:

| Example | Lexical result | Later program/semantic expectation |
|---|---|---|
| `fn main() -> int32 { return 256; }` | Valid tokens, no lexical Error | Reject result outside 0–255 in later semantics |
| `fn main() -> int32 { return 999999999999999999999999999999; }` | One whole IntegerLiteral; no conversion/overflow Error | Reject in later semantics without requiring lexer conversion |
| `fn main() -> int32 { return 00042; }` | One unsigned decimal IntegerLiteral | Decimal 42; no implicit octal interpretation |
| `fn main() -> int32 { return 42 }` | Valid tokens | Later parser rejects missing semicolon |
| `fn main() -> int32 { return 0; return 1; }` | Valid tokens | Later parser rejects a second statement |
| `fn main(1) -> int32 { return 0; }` | Valid tokens | Later parser rejects nonempty parameters |
| `fn main() -> int32 { return -12; }` | Invalid `-12` and Error | Cannot compile successfully |
| `fn main() -> int32 { return 0x10; }` | Invalid `0x10` and Error | Cannot compile successfully |
| `FN main() -> int32 { return 0; }` | Invalid `FN` and Error | No case-insensitive keyword recognition |

These are documentation examples only. No `.vyr` test fixture or compiler
implementation is created by this profile draft.

## 5. Lexical decisions and exact recovery contract

The D-01–D-14 decisions are settled inputs. The precise mixed-input precedence
and numeric-like grouping refinements in this section carry the approved research
and planning proposal into the profile **for owner review**. They become the
implementation contract only after the required acceptance/commit/publication
gates; drafting this document does not silently accept additional language design.

### 5.1 Original bytes, identity and token values

SourceManager owns immutable snapshots. Each registration creates a fresh
manager-local SourceId, even for identical display names and bytes. Use the
originating manager; equal numeric IDs from unrelated managers are not detected
as foreign identities by the current API. Display names are metadata, with no
filesystem lookup or canonicalization during registration.

Each token contains exactly TokenKind and SourceSpan. It owns no lexeme string,
numeric value, source pointer, line/column cache or trivia. Recover lexemes using
the originating SourceManager's checked span retrieval. Its views survive later
registrations, but not manager destruction; token storage does not extend the
manager's lifetime. Lexing borrows source bytes only synchronously.

All spans use the original SourceId and zero-based half-open byte offsets
`[begin,end)`. Non-EOF tokens are nonempty, source-ordered and nonoverlapping.
Whitespace is the only omitted source region. Never normalize bytes, decode
Unicode, drop embedded NUL, clamp spans or fabricate text. Checked span validity
requires an existing source, `begin <= end` and `end <= source_size`.

### 5.2 Supported units and whitespace

| Original spelling | Planned TokenKind | Boundary |
|---|---|---|
| `fn` | Fn | Exact whole ASCII word |
| `main` | Main | Exact whole ASCII word |
| `int32` | Int32 | Exact whole ASCII word |
| `return` | Return | Exact whole ASCII word |
| `(` / `)` | LeftParen / RightParen | One byte each |
| `{` / `}` | LeftBrace / RightBrace | One byte each |
| `;` | Semicolon | One byte |
| `->` | Arrow | Exactly two contiguous bytes |
| One or more ASCII `0`–`9` bytes | IntegerLiteral | The entire numeric-like run must be digits |
| Rejected run | Invalid | Same span as its one Error |
| Source end | EndOfFile | Exactly one `[source_size,source_size)` |

**D-07:** Skip only space `20`, TAB `09`, LF `0A` and CR `0D` (hex), emitting
no trivia tokens. VT `0B` and FF `0C` are not whitespace in this profile.
Classify explicit ASCII/unsigned byte values, independent of locale or host
signed-char behavior. Embedded NUL is a source byte, never an implicit EOF.

**D-08/D-09:** First consume the whole word `[A-Za-z_][A-Za-z0-9_]*`, then
recognize only the four exact lowercase spellings. Any other whole word is one
Invalid/Error. `return42` and `fnmain` are not keyword fragments. This grouping
does not introduce general identifier support.

**D-10:** Recognize `->` before minus/numeric handling. Whitespace may surround
the arrow, but `- >` is two unsupported independent bytes.

### 5.3 Deterministic precedence

At each original-byte cursor, apply the first matching rule:

1. Skip the four defined whitespace bytes.
2. At `//` or `/*`, group and reject a complete comment-shaped run (§5.5).
3. Recognize contiguous `->`.
4. Recognize one of `( ) { } ;`.
5. Consume a complete ASCII word and classify it as in §5.2.
6. Consume a numeric-like run as in §5.4.
7. Group consecutive top-level bytes `80`–`FF` as in §5.6.
8. Reject any remaining ASCII byte individually, including NUL, VT, FF and a
   slash without a comment opener.
9. At source end, append exactly one zero-width EndOfFile and finish.

Each non-EOF scan branch advances. Lookahead must be bounded by the available
source length. No recursive comment scanner, retry at the same cursor or
unbounded lookahead is implied. Recovery consumes the smallest related run
defined here and leaves independent following bytes available.

### 5.4 Numeric-like grouping for rejection

**D-12, with the proposed exact boundary refinement:** Start a numeric-like run
at an ASCII digit, at a dot immediately followed by a digit, or at `+`/`-`
immediately followed by a digit or by dot-plus-digit. After an optional leading
sign/dot, continue ASCII letters, digits, underscores and dots.

An internal `+`/`-` joins the run only immediately after `e`, `E`, `p` or `P`
and immediately before an ASCII digit. This groups an unsupported exponent-like
spelling for rejection; it does not support exponents or arithmetic.

Stop before whitespace, `( ) { } ;`, contiguous `->`, slash, a high byte or
any other independent byte. Accept IntegerLiteral only if the **entire** run
is one or more ASCII decimal digits. Otherwise emit one Invalid and one Error
for the entire run. Do not salvage apparently valid digit fragments.

Thus `+12`, `-12`, `1..2`, `0x10`, `12_3`, `12u`, `1.5`, `.5` and `1e+2`
are each one rejected run. `12+34` is IntegerLiteral `12` then Invalid `+34`;
`+ 12` is Invalid `+` then IntegerLiteral `12`. Leading zeros are decimal,
and arbitrarily long digit-only sequences remain lexically valid. Signs,
prefixes, separators, suffixes and non-decimal literal forms remain unsupported.

### 5.5 Comment-shaped input

**D-14:** Comments are unsupported, not accepted whitespace/trivia.

- A `//` run includes its opener and stops before LF, before the CR starting
  CRLF, or at EOF. A lone CR is ordinary content within this rejected run.
- After recognizing the complete two-byte `/*` opener at byte offset `b`, begin
  searching for a complete closing `*/` at offset `b + 2`, immediately after
  the opener. Only closing pairs beginning at offsets greater than or equal to
  `b + 2` are eligible. No byte belonging to the opening delimiter may
  participate in the closing delimiter. The earliest eligible complete pair
  terminates the run, which includes both delimiters. If no eligible closing
  pair exists, the Invalid run extends to EOF. Newlines stay in the run;
  there is no nested-comment handling.
- Emit exactly one Invalid and one Error for the whole run. Its contents,
  including numeric-looking, control and non-ASCII bytes, produce no nested
  lexical Errors. This precedence does not claim those bytes are valid Veyr.

For `12/*x*/34`, numeric scanning ends before `/`; tokens are IntegerLiteral,
Invalid comment, IntegerLiteral. Comment-shaped rejection cannot swallow the
surrounding numbers or program delimiters after the rejected run ends.

For the exact input `/*/return`, the only apparent `*/` begins at offset 1
and would reuse the opener's `*`; it is ineligible. There is no eligible closing
pair. Emit Invalid `[0,9)`, one lexical Error with primary span `[0,9)` and an
empty related-span collection, then EndOfFile `[9,9)`. No Return token is emitted.

### 5.6 Non-ASCII and other unsupported bytes

**D-13:** Outside comment-shaped runs, every maximal consecutive byte run in
`80`–`FF` is one Invalid/Error. Resume at the next ASCII byte. Adjacent high
bytes are grouped without UTF-8 decoding or validation. ASCII embedded between
high-byte runs divides them, and high bytes divide ASCII word/numeric runs.
This rejects an M1 limitation without defining the final source encoding.

Remaining independent ASCII bytes are individually Invalid/Error, even when
adjacent. Do not interpolate hostile raw bytes into fixed diagnostic messages;
presentation/escaping belongs to the existing checked plain-text renderer.

### 5.7 Exact byte-span examples and recovery order

Notation: `I` = IntegerLiteral, `X` = Invalid, `F` = Fn, `R` = Return,
`S` = Semicolon, `A` = Arrow. Every interval is `[begin,end)` in the same
registered snapshot. The notation `\n`, `\r`, `\t`, `\xHH` denotes exactly
one original byte; it is documentation notation, not literal backslash bytes.
Each `X` has exactly one Error with that same primary span, in `X` encounter
order. Other tokens and EOF emit no lexical diagnostic. The final column is
the exact EndOfFile span, not an extra Invalid or Error.

| Input bytes | Ordered tokens before EOF | Error count | EOF |
|---|---|---|---|
| `+12` | X[0,3) | 1 | [3,3) |
| `1..2` | X[0,4) | 1 | [4,4) |
| `-12;` | X[0,3), S[3,4) | 1 | [4,4) |
| `0x10` | X[0,4) | 1 | [4,4) |
| `12_3` | X[0,4) | 1 | [4,4) |
| `12u` | X[0,3) | 1 | [3,3) |
| `1.5` | X[0,3) | 1 | [3,3) |
| `.5` | X[0,2) | 1 | [2,2) |
| `1e+2` | X[0,4) | 1 | [4,4) |
| `12+34` | I[0,2), X[2,5) | 1 | [5,5) |
| `+ 12` | X[0,1), I[2,4) | 1 | [4,4) |
| `12//x\nreturn` | I[0,2), X[2,5), R[6,12) | 1 | [12,12) |
| `12/*x*/34` | I[0,2), X[2,7), I[7,9) | 1 | [9,9) |
| `//\xC3\xA9\n` | X[0,4) | 1 | [5,5) |
| `/*\xC3\xA9*/` | X[0,6) | 1 | [6,6) |
| `\xC3\xA9fn` | X[0,2), F[2,4) | 1 | [4,4) |
| `fn\xC3\xA9return` | F[0,2), X[2,4), R[4,10) | 1 | [10,10) |
| `12\xC3\xA9_` | I[0,2), X[2,4), X[4,5) | 2 | [5,5) |
| `\x80a\xFF` | X[0,1), X[1,2), X[2,3) | 3 | [3,3) |
| `return42` | X[0,8) | 1 | [8,8) |
| `fnmain` | X[0,6) | 1 | [6,6) |
| `- >` | X[0,1), X[2,3) | 2 | [3,3) |
| `12->` | I[0,2), A[2,4) | 0 | [4,4) |
| `//x\ry\n` | X[0,5) | 1 | [6,6) |
| `//x\r\n` | X[0,3) | 1 | [5,5) |
| `//x` | X[0,3) | 1 | [3,3) |
| `/*x` | X[0,3) | 1 | [3,3) |
| `/*/return` | X[0,9) | 1 | [9,9) |
| `/*a/*b*/return` | X[0,8), R[8,14) | 1 | [14,14) |
| `\x00return` | X[0,1), R[1,7) | 1 | [7,7) |
| `\x0B\x0C` | X[0,1), X[1,2) | 2 | [2,2) |
| ` \t\r\n` | none | 0 | [4,4) |
| Empty source (zero bytes) | none | 0 | [0,0) |
| `256` | I[0,3) | 0 | [3,3) |
| `00042` | I[0,5) | 0 | [5,5) |

For the exact no-newline sample `fn main() -> int32 { return 42; }`, the full
stream is Fn[0,2), Main[3,7), LeftParen[7,8), RightParen[8,9), Arrow[10,12),
Int32[13,18), LeftBrace[19,20), Return[21,27), IntegerLiteral[28,30),
Semicolon[30,31), RightBrace[32,33), EndOfFile[33,33). It emits no lexical Error.

### 5.8 Source locations and boundaries

The approved Phase 2 mapping remains unchanged: one-based line numbers and
one-based original-byte columns. TAB is one byte/column; UTF-8 bytes count
individually without decoding or visual-width calculation. LF and both CRLF
bytes belong to the preceding line. CRLF advances the line once; lone CR never
advances it, even though the lexer can skip it outside comment-shaped runs.

| Source bytes | Exact offset-to-location expectations |
|---|---|
| Empty source | EOF offset 0 = 1:1 |
| `a\n` | offset 0 = 1:1; LF offset 1 = 1:2; EOF offset 2 = 2:1 |
| `a\r\n` | offset 0 = 1:1; CR offset 1 = 1:2; LF offset 2 = 1:3; EOF offset 3 = 2:1 |
| `a\rb` | offsets 0/1/2 = 1:1/1:2/1:3; EOF offset 3 = 1:4 |
| `\t\xC3\xA9` | offsets 0/1/2 = 1:1/1:2/1:3; EOF offset 3 = 1:4 |

EOF after a trailing line break is next line, column 1; otherwise it is one
byte column past the final line's contents. Empty spans and in-bounds spans
splitting CRLF remain structurally valid. Lexer grouping imposes no additional
SourceSpan validity rule and never rewrites source text for newline handling.

### 5.9 Recovery, ownership and infrastructure failure

**D-11:** Every rejected run remains an explicit Invalid token plus an owned
Error diagnostic. Preserve valid recovered tokens and insertion order, then
append exactly one EOF even for empty, whitespace-only or error-only known
sources. Do not stop after the first lexical Error or emit errors for EOF.

The planned checked synchronous call uses existing SourceManager and
DiagnosticEngine. Unknown SourceId returns the exact original checked SourceError
from source lookup, including kind/ID/offsets/optional size, with no language
diagnostic, recovered vector or fabricated EOF. Existing distinct UnknownSource,
ReversedRange and BeyondSource failures remain infrastructure errors; never
clamp, reinterpret or merge them into a language diagnostic. Scanner-produced
spans must remain valid by construction and retain runtime checked resolution.

Diagnostics own message/label bytes; source spans refer to the originating
manager. Each lexical Error explicitly initializes severity, uses the exact
offending primary span and has an explicitly empty related-span collection.
Preserve previous diagnostics,
duplicates and accumulated Error state. Reacquire borrowed engine views after
every emission; do not store them across emit or beyond engine lifetime.

A recovered token vector is **not successful compilation**. Any accumulated
Error prevents successful compilation; consumers must inspect existing
`DiagnosticEngine::has_errors()` before any later success handoff. Warning/Note
alone do not set that state. Phase 3 proves recovery and Error preservation;
it does not add a compiler driver or new success API. Rendering is a separate
consumer and must not clear Error state. Ordinary allocation exceptions retain
existing C++ behavior; no transactional rollback, OOM or final language exception
policy is introduced.

## 6. Unsupported constructs

M1 does not support comments, imports, variables, arithmetic, general functions,
function calls, multiple statements, general expressions, string/char literals,
non-decimal numeric forms, Unicode identifiers, type inference,
ownership/borrowing syntax, module graphs or packages. This is the P01 walking
skeleton, not a language-completeness claim. Unsupported in M1 does not mean
rejected from future Veyr. A lexically valid token stream does not imply a valid
program or semantic result.

## 7. Test naming rules

Future native suites use `SmokeProfileToken.*` and `SmokeProfileLexer.*`;
examples of descriptive names are `TokenizesMilestone1FixedEntry` and
`RecognizesProvisionalReturnSpelling`. Do not name these `VeyrGrammarTest`,
`ParsesValidVeyrFunction` or `SupportsVeyrReturnStatement` as if final language
design had been accepted. Exact byte-span/Error assertions follow §5.7 only
after profile acceptance, dedicated commit, verified publication and fresh
scoped implementation authorization. No syntax-dependent tests are created now.

## 8. Diagnostics wording

Use these exact fixed lexical messages, each with explicitly initialized Error
severity, the offending primary span and an explicitly empty related-span collection:

| Rejected class | Owned message |
|---|---|
| Unsupported complete word | `unsupported word in M1 Experimental Smoke Profile` |
| Unsupported numeric-like spelling | `unsupported numeric spelling in M1 Experimental Smoke Profile` |
| Top-level non-ASCII run | `unsupported non-ASCII byte run in M1 Experimental Smoke Profile` |
| Comment-shaped run, including unterminated | `unsupported comment-shaped input in M1 Experimental Smoke Profile` |
| Other independent ASCII byte | `unsupported byte in M1 Experimental Smoke Profile` |

Later parser diagnostics may identify an `unexpected token in M1 Experimental
Smoke Profile`; their implementation and exact wording remain later-phase work.
Do not use `invalid Veyr integer literal`, `invalid Veyr source encoding` or
`invalid final Veyr syntax` for this experimental subset. The existing renderer
handles hostile display names/source bytes; the lexer performs no terminal I/O
or presentation escaping and has no LLVM dependency.

## Decision traceability and owner gate

| Decision | Preserved contract | Profile section |
|---|---|---|
| D-01 | 0–255 accepted later semantic results; no lexer range check | 3, 4, 5.4 |
| D-02 | Signed 32-bit M1 result, no stable ABI | 1, 3 |
| D-03 | Provisional `fn main` | 4, 5.2 |
| D-04 | Provisional `int32` | 4, 5.2 |
| D-05 | Fixed empty-parameter signature with explicit result | 3, 4 |
| D-06 | Braces, one integer return, mandatory semicolon | 3, 4 |
| D-07 | Exactly space/TAB/LF/CR; bytes unchanged | 5.1, 5.2, 5.8 |
| D-08 | Four exact lowercase spellings | 5.2 |
| D-09 | Whole ASCII words, no general identifier support | 5.2, 5.7 |
| D-10 | Contiguous arrow | 5.2, 5.3, 5.7 |
| D-11 | Explicit Invalid/Error, ordered recovery and EOF; Errors block success | 5.7, 5.9 |
| D-12 | Whole unsupported numeric run; delimiters retained | 5.4, 5.7 |
| D-13 | Original high-byte runs; no decoding | 5.6, 5.7 |
| D-14 | Comment-shaped rejection; first terminator or EOF | 5.5, 5.7 |

Next: owner reviews this whole draft and its mixed-input table. Profile acceptance
does not authorize a commit. A dedicated profile commit needs explicit approval;
publication needs separate approval naming the remote/branch. Actual successful
push and a fresh remote observation must prove containment, with ancestry and
unchanged-profile checks if the branch advanced. Record actual SHA/remote/branch
and verification evidence in 03-01-EXECUTION.md. Local commit existence, prepared
push commands and stale tracking refs are insufficient. Wave 2 still needs fresh
explicit owner authorization after those gates; nothing advances automatically.

Platform impact:
- Windows: Same immutable byte/token/diagnostic contract and eventual 0–255
  smoke results. Hosted compatibility is Tier 1; new lexer evidence is future
  work. Developer-host inspection remains independently pending/non-blocking.
- Linux: Primary Omarchy development, separate Ubuntu CI baseline; no locale,
  filesystem or newline normalization is introduced.
- macOS future: Deferred; no POSIX, Win32, terminal or native ABI dependency is
  added by this profile.
