# LL(1) analysis of MiniLang

**Lab 1, steps B1 and B2.** Student: <TODO> · Date: <TODO>

Work **on paper first**, then type your answers here. Only after you have written an answer may you compare it with Appendix C of `Lab1.md`. Replace **every** placeholder (the word TODO between angle brackets); `ctest --test-dir build -R docs.ll1_analysis` checks that none is left, it does not check that your answers are right.

## 1. Transformations (step B1)

The first column is how a *natural* grammar would write the rule. For each row explain why a recursive-descent parser with one token of lookahead cannot use it as written, name the transformation, and write the resulting rule in EBNF (compare it with the grammar in `Lab1.md`, section 7.2, only afterwards).

| Natural form | Why it is not LL(1) / not directly implementable | Transformation | Resulting rule |
|---|---|---|---|
| orExpr → orExpr "\|\|" andExpr \| andExpr | <TODO> | <TODO> | <TODO> |
| `assignment → IDENT "=" expr ";"` and `callStmt → IDENT "(" [ args ] ")" ";"`, both alternatives of `statement` | <TODO> | <TODO> | <TODO> |
| `variable → IDENT` and `call → IDENT "(" [ args ] ")"`, both alternatives of `primary` | <TODO> | <TODO> | <TODO> |

Short answers:

1. The same left-recursion problem appears in five more rules. Which ones? <TODO>
2. `ifStmt` has an optional `else`. Why does that **not** need left factoring and why is there no dangling-else problem in MiniLang? <TODO>
3. Do the loops (`( "||" andExpr )*`) make `||` left- or right-associative? Which part of the grammar or of the parser code is responsible for that? <TODO>

## 2. FIRST and FOLLOW (step B2)

Compute FIRST and FOLLOW of these non-terminals of the grammar in `Lab1.md`, section 7.2. Write `ε` in FIRST if the non-terminal can derive the empty string (and `EOF` for the end of the input).

| Non-terminal | FIRST | FOLLOW |
|---|---|---|
| `type` | <TODO> | <TODO> |
| `returnType` | <TODO> | <TODO> |
| `block` | <TODO> | <TODO> |
| `statement` | <TODO> | <TODO> |
| `simpleStmt` | <TODO> | <TODO> |
| `forInit` | <TODO> | <TODO> |
| `primary` | <TODO> | <TODO> |
| `args` | <TODO> | <TODO> |

Differences between your answers and Appendix C (what was wrong and why) — write "none" if there are none:

<TODO>

## 3. The LL(1) condition at five decision points

Pick **five different decision points** of your parser (an alternative, an optional part or a loop) and show that the sets that must be disjoint are disjoint. At least one must be a loop, one an optional part and one a choice between alternatives; at least one must involve a FOLLOW set.

| Rule | Decision the parser makes | Set A | Set B | A ∩ B = ∅ |
|---|---|---|---|---|
| <TODO> | <TODO> | <TODO> | <TODO> | <TODO> |
| <TODO> | <TODO> | <TODO> | <TODO> | <TODO> |
| <TODO> | <TODO> | <TODO> | <TODO> | <TODO> |
| <TODO> | <TODO> | <TODO> | <TODO> | <TODO> |
| <TODO> | <TODO> | <TODO> | <TODO> | <TODO> |

## 4. Call trace

Without running the program, write the sequence of calls made by `parse()` for the input below, one call per line, indented by nesting depth, including every successful `expect(...)` (also the last one, made by `parse()` itself at depth 0). Do not list `nextToken()`.

```
x = 1 + 2 * y;
```

Your trace:

```
<TODO>
```

Then (optional but recommended) add `TraceScope trace(*this, "name");` to your methods, run `minilang --trace` on a file with that statement and compare. If they differ, who is right, you or the program? Write what you found: <TODO>
