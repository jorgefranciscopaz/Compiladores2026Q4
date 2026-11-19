# LL(1) analysis of MiniLang

**Lab 1, steps B1 and B2.** Student: Jorge Francisco Paz Sauceda · Date: 11/18/2026

Work **on paper first**, then type your answers here. Only after you have written an answer may you compare it with Appendix C of `Lab1.md`. Replace **every** placeholder (the word TODO between angle brackets); `ctest --test-dir build -R docs.ll1_analysis` checks that none is left, it does not check that your answers are right.

## 1. Transformations (step B1)

The first column is how a _natural_ grammar would write the rule. For each row explain why a recursive-descent parser with one token of lookahead cannot use it as written, name the transformation, and write the resulting rule in EBNF (compare it with the grammar in `Lab1.md`, section 7.2, only afterwards).

| Natural form                                                                                                    | Why it is not LL(1) / not directly implementable                                                        | Transformation                                                     | Resulting rule                |
| --------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------ | ----------------------------- |
| orExpr → orExpr "\|\|" andExpr \| andExpr                                                                       | Because it starts with itself, being that left recursion                                                | `orExpr → andExpr orExprP` and `orExprP → "\|\|" andExpr orExprP`  | We create the rule `orExprP`  |
| `assignment → IDENT "=" expr ";"` and `callStmt → IDENT "(" [ args ] ")" ";"`, both alternatives of `statement` | They both starts with `IDENT`, so the parser doesnt know which path to take                             | `stmt → IDEDNT stmtP`, `stmtP → "=" expr ";" \| "(" [args] ")"`    | We create the rule `stmtP`    |
| `variable → IDENT` and `call → IDENT "(" [ args ] ")"`, both alternatives of `primary`                          | Same reason as before, both productions starts with IDENT, so the parser doesnt know which path to take | `primary → IDENT primaryP`, `primaryP → "(" [args] ")" \| epsilon` | We create the rule `primaryP` |

Short answers:

1. The same left-recursion problem appears in five more rules. Which ones?

- andExpr, equality, relational, additive, term

2. `ifStmt` has an optional `else`. Why does that **not** need left factoring and why is there no dangling-else problem in MiniLang?

- En MiniLang no hay dangling-else porque los cuerpos del if siempre son blocks con { }, así que siempre se sabe a qué if pertenece el else. Es una decisión del lenguaje, no una regla de "el if más cercano".

3. Do the loops (`( "||" andExpr )*`) make `||` left- or right-associative? Which part of the grammar or of the parser code is responsible for that?

- Lo que da la asociatividad izquierda es el loop while dentro de orExpr(): va combinando de izquierda a derecha, ((a || b) || c).

## 2. FIRST and FOLLOW (step B2)

Compute FIRST and FOLLOW of these non-terminals of the grammar in `Lab1.md`, section 7.2. Write `ε` in FIRST if the non-terminal can derive the empty string (and `EOF` for the end of the input).

| Non-terminal | FIRST                                                         | FOLLOW                                                                                                                                 |
| ------------ | ------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------- |
| `type`       | {int, bool}                                                   | {IDENT}                                                                                                                                |
| `returnType` | {int, bool, void}                                             | {IDENT}                                                                                                                                |
| `block`      | { `{` }                                                       | {`'EOF'`, `'func'`, `'int'`, `'bool'`, `'if'`, `'else'`, `'while'`, `'for'`, `'return'`, `'print'`, `'read'`, `'IDENT'`, `'{'`, `'}'`} |
| `statement`  | {int, bool, IDENT, id, while, for, return, print, read, "(" } | `'func'`, `'EOF'`, `'int'`, `'bool'`                                                                                                   |
| `simpleStmt` | {`IDENT`, `(`}                                                | `'}'`, `'+'`, `'EOF'`, `'+'`                                                                                                           |
| `forInit`    | {`int`, `bool`, `IDENT`}                                      | `';'`                                                                                                                                  |
| `primary`    | {`NUMBER`, `true`, `false`, `IDENT`, `(`}                     | {`'*'`, `'/'`, `'+'`, `'-'`, `'<'`, `'<='`, `'>'`, `'>='`, `'='`, `'=='`, `'!='`, `'&&'`}                                              |
| `args`       | {`-`, `!`, `NUMBER`, `true`, `false`, `IDENT`, `(`}           | {`")"`}                                                                                                                                |

Differences between your answers and Appendix C (what was wrong and why) — write "none" if there are none:

- `type`, `returnType`, `block`, `forInit`, `args`: no differences.
- `statement`, FIRST: I wrote `id` instead of `if` and `(` instead of `{`. The `{` comes from the alternative `statement → block`, and FIRST(block) = { `{` }; `(` cannot start a statement.
- `statement`, FOLLOW: I only had `func`, `EOF`, `int`, `bool`. It is missing the rest of FIRST(statement) (`if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`) and `}`. Because of `block → "{" statement* "}"`, a statement can be followed by another statement (all of FIRST(statement)) or by the closing `}`; because of `item → statement` and `program → item* EOF`, it also gets FOLLOW(item) = FIRST(item) ∪ { EOF }, which adds `func` and `EOF`.
- `simpleStmt`, FIRST: only { IDENT }. `(` is not in FIRST: it comes _after_ the IDENT (`IDENT ( "=" expr | "(" [args] ")" ) ";"`), so it is only the second token.
- `simpleStmt`, FOLLOW: `+` was wrong (and was repeated). A simpleStmt always ends with `;`, and it is a complete alternative of `statement`, so FOLLOW(simpleStmt) = FOLLOW(statement) = { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` }.
- `primary`, FOLLOW: `=` was wrong. Assignment is a statement, not an operator in an expression, so `=` never follows a primary. It was also missing `||`, `)`, `,` and `;`. `primary` is at the end of `unary`, `unary` at the end of `term`, and so on up to `expr`, so FOLLOW(primary) collects every operator of the chain (`* / + - < <= > >= == != && ||`) plus FOLLOW(expr) = { `)`, `,`, `;` }.

## 3. The LL(1) condition at five decision points

Pick **five different decision points** of your parser (an alternative, an optional part or a loop) and show that the sets that must be disjoint are disjoint. At least one must be a loop, one an optional part and one a choice between alternatives; at least one must involve a FOLLOW set.

| Rule   | Decision the parser makes | Set A  | Set B  | A ∩ B = ∅ |
| ------ | ------------------------- | ------ | ------ | --------- |
| <TODO> | <TODO>                    | <TODO> | <TODO> | <TODO>    |
| <TODO> | <TODO>                    | <TODO> | <TODO> | <TODO>    |
| <TODO> | <TODO>                    | <TODO> | <TODO> | <TODO>    |
| <TODO> | <TODO>                    | <TODO> | <TODO> | <TODO>    |
| <TODO> | <TODO>                    | <TODO> | <TODO> | <TODO>    |

## 4. Call trace

Without running the program, write the sequence of calls made by `parse()` for the input below, one call per line, indented by nesting depth, including every successful `expect(...)` (also the last one, made by `parse()` itself at depth 0). Do not list `nextToken()`.

```
x = 1 + 2 * y;
```

Your trace:

```
program
  item
    statement
      simpleStmt
        expect(IDENT)
        expect(=)
        expr
          orExpr
            andExpr
              equality
                relational
                  additive
                    term
                      unary
                        primary
                          expect(NUMBER)
                    term
                      unary
                        primary
                          expect(NUMBER)
                      unary
                        primary
                          expect(IDENT)
        expect(;)
expect(EOF)

```

Then (optional but recommended) add `TraceScope trace(*this, "name");` to your methods, run `minilang --trace` on a file with that statement and compare. If they differ, who is right, you or the program? Write what you found: 

The structure matched exactly: the nesting, the two term calls under additive (because * binds tighter than +) and the two unary calls in the second term. The difference was in the expect lines: my hand-made trace had expect(=) and an expect for each NUMBER/IDENT in primary, but my parser checks those tokens in an if and consumes them with nextToken(), so they are not printed. The program is right about what my code does; both versions accept the same input. I updated my trace to match.
