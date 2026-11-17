# MiniLang Language Reference

**Compiladores I (CCC501) · Lab 1 · authoritative definition of the language used in the lab.**

MiniLang is the course language: a small, C-like imperative language with integer and boolean variables, assignment, input/output, `if` / `while` / `for`, and functions with calls. This file is extracted from the course topics document (*Topics: AST, Semantic Analysis and Interpretation*, §1 and its appendices). The tag **[§x.y]** after a statement tells you where it comes from.

Anything that is **not** in the topics document, but that the lab had to decide so that the tests are unambiguous, is marked **[Lab 1 convention]** and collected in section 7. If a convention surprises you, ask; do not guess.

## 1. What belongs to Lab 1

MiniLang has three layers of rules. Lab 1 implements only the first two:

| Layer | Question it answers | In Lab 1? |
|---|---|---|
| Lexical (section 2) | which characters form which token? | **yes**: Part A |
| Syntactic (section 3) | which token sequences are programs? | **yes**: Part B |
| Semantic (sections 4 and 5) | does the program *make sense*, and what does it *do*? | **no** (later units) |

The consequence is important: **a program such as `int x = true;` is a valid input for Lab 1.** It is syntactically correct; it is only wrong for the semantic analysis that you will write later.

## 2. Lexical structure [§1.2]

| Category | Tokens |
|---|---|
| Keywords | `func` `int` `bool` `void` `if` `else` `while` `for` `return` `print` `read` `true` `false` |
| Identifier | `[a-zA-Z_][a-zA-Z0-9_]*` (not a keyword) |
| Integer literal | `[0-9]+` (decimal) |
| Arithmetic | `+` `-` `*` `/` |
| Comparison | `<` `<=` `>` `>=` `==` `!=` |
| Logical | `&&` \|\| `!` |
| Assignment | `=` |
| Punctuation | `(` `)` `{` `}` `,` `;` |
| Ignored | whitespace, `// line comments` |

Notes from the topics document:

- Keywords are recognized after scanning an identifier, by a lookup in a keyword table; they are not rules of the DFA. [§1.2]
- The two-character operators (`<=` `>=` `==` `!=` `&&` `||`) need one extra DFA state each and are an example of *maximal munch*. [§1.2]
- A lone `&` or `|` is an `Unknown` token. [§1.2]
- There are no string, character or floating-point literals. [§1.2]

## 3. Grammar [§1.3]

EBNF, LL(1). Terminals are in quotes; `IDENT` is an identifier token and `NUMBER` an integer-literal token; `[ x ]` is optional, `( x )*` is repetition.

```
program     → item* EOF
item        → function | statement

function    → "func" returnType IDENT "(" [ params ] ")" block
params      → param ( "," param )*
param       → type IDENT
type        → "int" | "bool"
returnType  → type | "void"

block       → "{" statement* "}"

statement   → varDecl
            | simpleStmt
            | ifStmt
            | whileStmt
            | forStmt
            | returnStmt
            | printStmt
            | readStmt
            | block

varDecl     → type IDENT [ "=" expr ] ";"
simpleStmt  → IDENT ( "=" expr | "(" [ args ] ")" ) ";"      // assignment or call (left factored)
ifStmt      → "if" "(" expr ")" block [ "else" ( ifStmt | block ) ]
whileStmt   → "while" "(" expr ")" block
forStmt     → "for" "(" [ forInit ] ";" [ expr ] ";" [ forUpdate ] ")" block
forInit     → type IDENT "=" expr | IDENT "=" expr
forUpdate   → IDENT "=" expr
returnStmt  → "return" [ expr ] ";"
printStmt   → "print" expr ";"
readStmt    → "read" IDENT ";"

expr        → orExpr
orExpr      → andExpr ( "||" andExpr )*
andExpr     → equality ( "&&" equality )*
equality    → relational ( ( "==" | "!=" ) relational )*
relational  → additive ( ( "<" | "<=" | ">" | ">=" ) additive )*
additive    → term ( ( "+" | "-" ) term )*
term        → unary ( ( "*" | "/" ) unary )*
unary       → ( "-" | "!" ) unary | primary
primary     → NUMBER | "true" | "false"
            | IDENT [ "(" [ args ] ")" ]                      // variable or call
            | "(" expr ")"
args        → expr ( "," expr )*
```

Design decisions stated in the topics document [§1.3]:

- **Assignment is a statement, not an expression**: `x = y = 3;` is a syntax error.
- **Blocks are mandatory** for `if` / `while` / `for` bodies. This removes the dangling-else problem; `else if` is `else` followed by an `ifStmt`.
- **Left factoring appears twice**: in `simpleStmt` (assignment vs call, both start with `IDENT`) and in `primary` (variable vs call).
- **Precedence**, from lowest to highest: `||`, `&&`, `==` `!=`, `<` `<=` `>` `>=`, `+` `-`, `*` `/`, unary `-` `!`. **All binary operators are left-associative.**
- Chained comparisons such as `a < b < c` parse correctly; they are a *type* error, found later by the semantic analysis.
- Functions are declared only at top level; statements may also appear at top level (script style), executed in order.

## 4. Static semantics (summary) [§1.4], not checked in Lab 1

- Types: `int`, `bool`; `void` only as a function return type. No implicit conversions.
- Declaration before use; one namespace for variables, parameters and functions.
- Scopes: global, one per function (parameters + body) and one per block. Inner scopes may shadow outer names; redeclaring a name in the *same* scope is an error.
- Functions are visible from their declaration onward, including inside their own body. No forward declarations, hence no mutual recursion.
- Operators: arithmetic `int × int → int`; relational `int × int → bool`; equality `T × T → bool` for `T` in `{int, bool}`; logical `bool × bool → bool`; unary `-` on `int`, `!` on `bool`.
- Conditions of `if` / `while` / `for` must be `bool`.
- `return` must match the enclosing function's return type; `return;` only in `void` functions; `return` outside a function is an error.
- Calls: the callee must be a function; the argument count and types must match; a `void` call cannot be used as a value.
- `read x;` requires `x` to be a declared `int` variable.

The topics document lists these as rules S01 to S19 (plus the optional S20 and W01) with sample messages. **None of them is the parser's business**: the programs in `examples/valid/semantic_errors_still_parse.txt` break several of them and must be *accepted* by your parser.

## 5. Dynamic semantics (summary) [§1.5], not executed in Lab 1

- `int` is a 32-bit signed integer (overflow is not detected); `bool` is `true` / `false`.
- A variable declared without initializer gets `0` / `false`.
- Evaluation is left to right; `&&` and `||` are short-circuit.
- Runtime errors: division by zero, `read` of non-numeric input, unbounded recursion (call-depth limit).
- Arguments are passed by value. `print e;` writes the value and a newline; `read x;` reads one integer. A program ends when its last top-level item has executed.

## 6. What MiniLang does not have

Because the definition above is closed, these are **not** part of MiniLang. Do not add them, and do not accept them:

- string, character and floating-point literals (so `"hi"` and `3.14` are not valid);
- block comments `/* ... */` (only `// ...` is listed);
- assignment as an expression (`x = y = 3;`), `++` / `--`, compound assignment (`+=`), the conditional operator `?:`;
- `else` without a block, `if` / `while` / `for` bodies without braces;
- the empty statement (a lone `;`), expression statements other than calls (`x + 1;`);
- nested functions and functions inside blocks; forward declarations;
- `void` variables or parameters; arrays; `break` / `continue`.

## 7. Lab 1 conventions [Lab 1 convention]

The topics document does not say the following; the lab fixes it so that every test has exactly one right answer. Each convention has a number you can quote.

| # | Convention |
|---|---|
| C1 | **Whitespace** is the four characters space, tab, line feed (`\n`) and carriage return (`\r`). Every other control character is not whitespace (it becomes an `Unknown` token). |
| C2 | A `//` comment extends to the end of the line; the line feed is **not** part of the comment (it is whitespace). A comment may end at the end of the file. Comments do not nest. |
| C3 | Keywords are **case-sensitive** and exactly the 13 listed (`If`, `INT` and `iff` are identifiers). Identifiers are ASCII only. |
| C4 | An integer literal is a maximal sequence of digits (leading zeros are allowed). The **lexer** accepts any such sequence: `99999999999999999999` is one `Number` token, and `12abc` is the two tokens `12` and `abc`. The **parser** rejects a literal whose value does not fit in a 32-bit signed `int` [§1.5], that is, greater than **2147483647** (leading zeros do not count: `007` is 7). `-` is a separate operator, so `-2147483648` is also rejected (the literal 2147483648 does not fit). The error is reported at the literal. *(Instructor decision.)* |
| C5 | **Unknown**: a character that cannot start any token is returned as a one-character `Unknown` token, and a `&` or \| that is not followed by the same character is an `Unknown` token that contains only that character. The lexer never stops with an error: reporting it is the parser's job. |
| C6 | Input is processed byte by byte. What a program containing non-ASCII bytes does is **unspecified** and not tested. |
| C7 | **Positions** are 1-based `line:column` of the *first* character of a token; every character, tab included, counts as one column. The `EndOfFile` token is positioned where the next character would be (the column after the last character; `1` of the next line if the input ends with a line feed). |
| C8 | After the end of the input the lexer keeps returning `EndOfFile`. |
| C9 | The empty program (no tokens, or only comments) is valid, because `program → item* EOF`. |
| C10 | **Syntax-error position**: the first token that cannot continue any valid program, i.e. the first token at which the prefix of the input stops being a prefix of some valid program. An LL(1) parser that checks the lookahead before consuming it finds exactly this token. For the end of the input it is the `EndOfFile` token. A `Number` token whose value is out of range (C4) is such a token: the error is reported at the literal. |
| C11 | The parser stops at the first syntax error (no recovery) and builds no tree. The text of the message is free; its position is not. |
| C12 | MiniLang source files in this lab use the extension `.txt`, as the other course samples do. |
