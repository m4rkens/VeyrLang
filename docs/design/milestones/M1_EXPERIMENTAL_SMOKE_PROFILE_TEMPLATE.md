# M1 Experimental Smoke Profile Template

**This is a template.** Phase 3 must copy/fill this into:

```text
docs/design/milestones/M1_EXPERIMENTAL_SMOKE_PROFILE.md
```

The filled document becomes the canonical syntax source for M1 only.

## 1. Warning

The M1 Experimental Smoke Profile is milestone-local and non-final.

Implementation support for this profile does not mean the spelling is accepted Veyr grammar.

Final Veyr syntax and semantics require separate design review and decision-log entry.

## 2. Purpose

The profile exists only to test the compiler walking skeleton:

```text
source file -> tokens -> parser -> AST -> semantic smoke -> lowering -> LLVM -> native executable -> exit code
```

## 3. Program shape

M1 supports exactly:

- a single root source file/module;
- one fixed entry form;
- one body form;
- one integer-literal return;
- EOF.

No imports, no module graph, no user functions, no expressions beyond the integer literal required by the fixed return form.

## 4. Concrete provisional spelling

Phase 3 must fill this section before lexer implementation.

Example structure:

```text
<fixed-entry-keyword> <fixed-entry-name>() -> <integer-type-token> {
    <return-keyword> <decimal-integer-literal>;
}
```

Do not write tests or lexer code before this section is filled.

## 5. Lexical decisions already captured

- ASCII-only smoke tokens.
- Whitespace skipped; no trivia tokens.
- No comments in M1.
- Token stores only `TokenKind` and `SourceSpan`.
- Lexeme recovered from SourceManager by span.
- Explicit EOF token with zero-width span at source end.
- Integer literal token shape: unsigned ASCII decimal digit sequence, one or more digits.
- No signs, prefixes, separators, suffixes, hex/bin/oct/other literal forms in M1.
- Non-ASCII byte runs are unsupported in M1; do not define final source encoding.
- Invalid token starts and unsupported numeric spellings produce diagnostics.
- Lexer recovers by consuming the smallest clearly related offending run.
- Lexing returns recovered tokens plus diagnostics.

## 6. Unsupported constructs

List explicitly:

- comments;
- imports;
- variables;
- arithmetic;
- general functions;
- function calls;
- multiple statements;
- general expressions;
- string/char literals;
- non-decimal numeric forms;
- Unicode identifiers;
- type inference;
- ownership/borrowing syntax;
- modules/packages.

Unsupported in M1 does not mean rejected from Veyr.

## 7. Test naming rules

Tests tied to this profile must use names such as:

- `SmokeProfileLexerTest`;
- `TokenizesMilestone1FixedEntry`;
- `RecognizesProvisionalReturnSpelling`.

Avoid names such as:

- `VeyrGrammarTest`;
- `ParsesValidVeyrFunction`;
- `SupportsVeyrReturnStatement`.

## 8. Diagnostics wording

Diagnostics should say:

```text
unsupported numeric spelling in M1 Experimental Smoke Profile
unsupported byte in M1 Experimental Smoke Profile
unexpected token in M1 Experimental Smoke Profile
```

Do not say:

```text
invalid Veyr integer literal
invalid Veyr source encoding
invalid final Veyr syntax
```
