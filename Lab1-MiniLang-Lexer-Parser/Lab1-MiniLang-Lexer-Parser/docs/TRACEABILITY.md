# DFA ↔ C++ traceability

**Lab 1, step A6.** Student: Jorge Francisco Paz Sauceda · Date: 11-17-2026

This document proves that your hand-written `Lexer::nextToken()` implements **the DFA that RE/flex
built from your `spec/MiniLang.l`**, and not some other automaton. Rules:

- Use the state names of `docs/MiniLang.numbered.svg` (`S0` = start state; `[n]` = accepting, rule `n` of your spec).
- **Every** state of the picture needs a row in section 1 and a `case k:` in `src/Lexer.cpp` with a `// Sk` comment on the same line.
- `ctest --test-dir build -R docs.traceability` checks that no state is missing in the graph, in the code or here. It cannot check that what you write is *true*: that is up to you, and it is what you will be asked to do in the in-person evaluation.
- Replace **every** placeholder (the word TODO between angle brackets) and keep the section headings.

## 1. State table

One row per state, in numeric order. In *Arrows* write the character or class that labels each outgoing arrow and the state it leads to (`'=' → S12`, `[0-9] → S11`). "none" means the state has no outgoing arrows. In *Code* write what your `case` does when no arrow matches the current character.

| State | Kind | Arrows leaving the state | Code in `Lexer.cpp` (`case`, line) | When no arrow matches |
|---|---|---|---|---|
| S0 | start | `[\t\n\r ]` → S3, `'!'` → S11, `'&'` → S12, `'('` → S14, `')'` → S15, `'*'` → S7, `'+'` → S5, `','` → S18, `'-'` → S6, `'/'` → S4, `[0-9]` → S2, `';'` → S19, `'<'` → S8, `'='` → S10, `'>'` → S9, `[A-Za-z_]` → S1, `'{'` → S16, `'\|'` → S13, `'}'` → S17 | `case 0`, line 84 | `EOF`: returns `EndOfFile` (line 164). Any other character: consumes it and returns a one-character `Unknown` (lines 166-168) |
| S1 | accepting [1] (identifier) | `[A-Za-z0-9_]` → S1 | `case 1`, line 172 | returns `identifierKind(text)` (a keyword or `Ident`) without consuming `ch` (line 178) |
| S2 | accepting [2] (number) | `[0-9]` → S2 | `case 2`, line 182 | returns `Number` without consuming `ch` (line 188) |
| S3 | accepting [3] (whitespace, ignored) | `[\t\n\r ]` → S3 | `case 3`, line 192 | returns no token: `state = 0` without consuming `ch`, so S0 starts a new lexeme (line 197) |
| S4 | accepting [8] (`'/'`) | `[/]` → S20 | `case 4`, line 201 | returns `OpDiv` `'/'` without consuming `ch` (line 207) |
| S5 | accepting [5] (`'+'`) | none | `case 5`, line 211 | returns `OpAdd` `'+'` without consuming `ch` (line 212) |
| S6 | accepting [6] (`'-'`) | none | `case 6`, line 214 | returns `OpSub` `'-'` without consuming `ch` (line 215) |
| S7 | accepting [7] (`'*'`) | none | `case 7`, line 217 | returns `OpMul` `'*'` without consuming `ch` (line 218) |
| S8 | accepting [9] (`'<'`)| `[=]` → S21 | `case 8`, line 220 | returns `OpLess` `'<'` without consuming `ch` (line 226) |
| S9 | accepting [11] (`'>'`) | `[=]` → S22 | `case 9`, line 230 | returns `OpGreater` `'>'` without consuming `ch` (line 236) |
| S10 | accepting [18] (`'='`)| `[=]` → S23 | `case 10`, line 240 | returns `Assign` `'='` without consuming `ch` (line 246) |
| S11 | accepting [17] (`'!'`) | `[=]` → S24 | `case 11`, line 250 | returns `OpNot` `'!'` without consuming `ch` (line 256) |
| S12 | non-accepting (`'&'` alone) | `[&]` → S25 | `case 12`, line 260 | returns `Unknown` `'&'` without consuming `ch` (line 266) |
| S13 | non-accepting (`'\|'` alone) | `[\|]` → S26 | `case 13`, line 270 | returns `Unknown` `'\|'` without consuming `ch` (line 276) |
| S14 | accepting [19] (`'('`) | none | `case 14`, line 280 | always returns `LParen` `'('` (no arrows: `ch` is never examined) (line 281) |
| S15 | accepting [20] (`')'`) | none | `case 15`, line 283 | always returns `RParen` `')'` (line 284) |
| S16 | accepting [21] (`'{'`) | none | `case 16`, line 286 | always returns `LBrace` `'{'` (line 287) |
| S17 | accepting [22] (`'}'`) | none | `case 17`, line 289 | always returns `RBrace` `'}'` (line 290) |
| S18 | accepting [23] (`','`) | none | `case 18`, line 292 | always returns `Comma` `','` (line 293) |
| S19 | accepting [24] (`';'`) | none | `case 19`, line 295 | always returns `Semicolon` `';'` (line 296) |
| S20 | accepting [4] (`//` comment, ignored) | `[\0-\t]` → S20, `[\v-\xff]` → S20 (every byte except `\n`) | `case 20`, line 298 | at `\n` or `EOF`: returns no token, `state = 0` without consuming `ch`, so the line feed is left for S0/S3 (line 303) |
| S21 | accepting [10] (`'<='`) | none | `case 21`, line 307 | always returns `OpLessEqual` `'<='` (line 308) |
| S22 | accepting [12] (`'>='`) | none | `case 22`, line 310 | always returns `OpGreaterEqual` `'>='` (line 311) |
| S23 | accepting [13] (`'=='`) | none | `case 23`, line 313 | always returns `OpEqual` `'=='` (line 314) |
| S24 | accepting [14] (`'!='`) | none | `case 24`, line 316 | always returns `OpNotEqual` `'!='` (line 317) |
| S25 | accepting [15] (`'&&'`) | none | `case 25`, line 319 | always returns `OpAnd` `'&&'` (line 320) |
| S26 | accepting [16] (`'\|\|'`) | none | `case 26`, line 322 | always returns `OpOr` `'\|\|'` (line 323) |

## 2. Walkthroughs

For each input, list the states visited from `S0` (including the last one), the lexeme, the token returned, and the line of `Lexer.cpp` that decides it. "Input" is exactly the characters shown (spaces included).

| Input | States visited | Lexeme and token returned | Why the DFA stops there (maximal munch / no arrow) | Deciding line |
|---|---|---|---|---|
| `<=x` | S0 → S8 → S21 → S0 → S1 | (returns `OpLessEqual`, `'<='`), (returns `Ident`, `'x'`) | First it stops at `'='` because the final state for that case is `'<='`, then it consumes the `'x'` as a new lexeme starting from S0 | (`'OpLessEqual'`, line 308), (`'Ident'`, line 178) |
| `while1 ` | S0 → S1 → S1 → S1 → S1 → S1 → S1 | (returns `Ident`, `'while1'`) | The DFA doesnt stop at while as a keyword, but continues and reads 1 as part of while, so its an Identifier | (`'Ident'`, line 178) |
| `&x` | S0 → S12 → S0 → S1  | (returns `Unknown`, `'&'`), (returns `Ident`, `'x'`) | First it consumes the `'&'`, which alone doesnt have an accepting state, then conusmes de `'x'` in S0 and returns `Ident`, `'x'` | line 266 |
| `// c` then end of file | S0 → S4 → S20 → S20 → S20 → S0 | no token for the comment; then `EndOfFile` (at 1:5) | S20 loops on every byte except '\n'; here the input ends, and EOF has no arrow from S20, so the comment ends (rule [4], ignored) and the code returns to S0 without consuming; S0 sees EOF and returns EndOfFile | lines 303 and 164 |

## 3. Where the code goes beyond the DFA

A DFA only decides *which characters belong together*. Your lexer also does things the picture does not show. List each one, say which state(s) it happens in, and why it is not a transition of the DFA.

| Behaviour | State(s) where it happens | Why it is not in the DFA |
|---|---|---|
| Distinguishing keywords from identifiers | S1 | The DFA has one accepting state for every identifier. Recognizing the 13 keywords in the DFA would need a separate path of states per keyword, interleaved with the identifier states (a much larger DFA). Instead the code looks up the whole lexeme in a table after the DFA stops in S1 |
| Returning `EndOfFile` | S0 | Because EOF is not a character of the input. |
| Returning `Unknown` | S0, S12, S13 | The DFA only describes the strings that match a rule, it has no rule for invalid input |
| Recording the line and column of the token | S0 | The position (Line, Column) is information about where a character is, not a token or character to read, so it cannot be an arrow or state in the DFA |

## 4. If I had to add `%` (modulo) to MiniLang

Without writing code: which files change and what exactly changes in each (spec, DFA picture, `Lexer.cpp`, `TokenId`, ...)? Which states of the DFA are affected?

Files that change:
- `spec/MiniLang.l`: new rule `"%"   // OpMod`
- `docs/MiniLang.gv`, `.svg`, `.numbered.gv`, `.numbered.svg`: regenerated with reflex, dot and NumberDfa
- `src/Lexer.hpp`: new enumerator `TokenId::OpMod` in `enum class TokenId`
- `src/Lexer.cpp`:
  - in S0, a new arrow `'%'` → new state;
  - a new `case` for that accepting state that always returns `TokenId::OpMod`;
  - `tokenToString`: `case TokenId::OpMod: return "OpMod";`.
- `docs/TRACEABILITY.md`: the S0 row in the First section (`# 1. State table`) gets the new arrow, plus one new row for the new state.
- `tests/`: add lexer tests with `%`.
- (Part B) grammar and `Parser::term()`: `term → unary ( ( "*" | "/" | "%" ) unary )*`; FOLLOW(unary) and FOLLOW(primary) gain `%`.

States affected: S0, and one new accepting state with no outgoing arrows, like S5. Because reflex numbers the states in creation order, the numbers of other states may change.