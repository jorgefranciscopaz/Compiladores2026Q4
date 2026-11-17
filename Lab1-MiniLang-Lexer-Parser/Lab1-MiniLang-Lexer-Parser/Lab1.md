# Lab 1: MiniLang Lexer and Recursive-Descent Parser

| Field | Value |
|---|---|
| **Course** | Compiladores I (CCC501) · UNITEC · Q4 2026 |
| **Covers** | Unit 1 (lexical analysis) and Unit 2 (top-down syntactic analysis) |
| **Type** | Preparatory lab, **ungraded** (no points) |
| **Estimated effort** | 8 to 10 hours: Part A about 4 h, Part B about 4 h, tests and documents about 1 h |
| **Language and tools** | C++20, CMake 3.20 or later, RE/flex, Graphviz |
| **You work on** | this folder (`Lab1-MiniLang-Lexer-Parser/`) |

## 0. The lab in one page

You will build, by hand, the front end of MiniLang up to syntax checking:

1. **Part A, the lexer.** You write the MiniLang tokens as regular expressions in **RE/flex** syntax, ask RE/flex for the **DFA**, turn it into an **SVG picture** with Graphviz, and then write a **C++20 lexer that follows that picture state by state**. The lexer is not generated: it is yours.
2. **Part B, the parser.** You check that the MiniLang grammar is **LL(1)** (FIRST and FOLLOW sets), and you write a **recursive-descent parser** in C++20, one method per non-terminal, that accepts exactly the MiniLang programs and reports the first syntax error with its position.

The project already contains the interfaces, the driver, the build files, a large set of tests and the document templates. Every place where *you* must write something is marked `TODO`. Nothing else needs to change.

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

At the start every test fails, on purpose. The lab is done when that last command reports **no failures** (section 9).

## 1. Context and purpose

In class you saw how a lexer is built from the DFA that RE/flex draws (`LexerSample1`) and how a recursive-descent parser is built from an LL(1) grammar (`ParserSample1`). MiniLang is the language of the integrating project: you will later add the AST, the semantic analysis and the interpreter on top of the lexer and parser you write now. A lexer or parser that is *almost* right makes every later phase fail in confusing ways, so this lab is the place to get them exactly right.

Because the lab is ungraded, nobody checks that you did it. Later, **you will be assessed individually and in person** (section 10) on the same skills, **without access to your code, your notes or any tool**. Treat the lab as training for that: the point is what you can do *without* the project in front of you.

## 2. Prerequisites

- Regular expressions and finite automata (Unit 1); BNF and EBNF, left recursion, left factoring, FIRST and FOLLOW, LL(1) (Unit 2).
- The class samples `LexerSample1` and `ParserSample1` (and, for reference, `ParserSample2`): the code you write follows their style.
- Basic C++20: classes, `std::string`, `switch`, exceptions, `std::istream`.
- Basic CMake and a terminal.

## 3. Learning objectives

Each objective says what you will be able to **do**, what **evidence** you produce, and how it is **verified**: by the automated checks (`ctest`) and, later, in the in-person evaluation (section 10).

### Part A: lexical analysis

| ID | You will be able to… | Evidence | Verified by |
|---|---|---|---|
| **A1** | Write RE/flex regular expressions for every MiniLang token class (identifier, integer, 14 operator tokens including `=`, 6 punctuation tokens, ignored input), without keyword rules. | `spec/MiniLang.l` | `docs.spec`; in-person |
| **A2** | Reproduce the DFA picture from the spec with `reflex`, `dot` and the numbering helper. | `docs/MiniLang.gv`, `.svg`, `.numbered.svg` | `docs.traceability` |
| **A3** | **Read** a RE/flex DFA picture and, by hand, produce the exact token sequence for an input string, including maximal munch and the `Unknown` cases. | walkthrough table, `docs/TRACEABILITY.md` §2 | in-person |
| **A4** | Implement `Lexer::nextToken()` in C++20 as an explicit state machine, one `case` per DFA state, without `std::regex` or generated code. | `src/Lexer.cpp` | `lexer.*` tests (26 public + your own), `rules.no_generators` |
| **A5** | Show the correspondence between **every** DFA state and transition and the code that implements it, and say what the code does that the DFA does not. | `docs/TRACEABILITY.md` | `docs.traceability`; in-person |
| **A6** | Handle the error and boundary cases of a lexer: `Unknown` input, lone `&` and \|, comments at the end of the file, token positions (line, column). | `lexer.*` tests | `lexer.*` tests |

### Part B: syntactic analysis

| ID | You will be able to… | Evidence | Verified by |
|---|---|---|---|
| **B1** | Recognize when a rule is not LL(1) because of left recursion or a common prefix, and apply the transformation (iteration, left factoring). | `docs/LL1-ANALYSIS.md` §1 | `docs.ll1_analysis`; in-person |
| **B2** | Compute FIRST and FOLLOW sets for a grammar fragment and verify the LL(1) condition at a decision point. | `docs/LL1-ANALYSIS.md` §2 and §3 | `docs.ll1_analysis`; in-person |
| **B3** | Implement a recursive-descent parser from an EBNF grammar: one method per non-terminal, one token of lookahead, no backtracking, no parser generator. | `src/Parser.cpp` | `parser.*` and `example.*` tests |
| **B4** | Report a syntax error at the **right position** (the first token that cannot continue a valid program) with a message that says what was found. | `src/Parser.cpp` | `parser.invalid.*`, `example.invalid.*` |
| **B5** | Predict the sequence of method calls of the parser for a given input. | `docs/LL1-ANALYSIS.md` §4 | in-person |
| **B6** | Extend a language with one construct and name every artifact that must change (spec, DFA, lexer, token enum, grammar, parser, tests). | `docs/TRACEABILITY.md` §4, stretch exercises | in-person |

### Cross-cutting

| ID | You will be able to… | Evidence | Verified by |
|---|---|---|---|
| **T1** | Write tests whose expected results come **from the language definition, not from your own program**: at least 5 lexer tests, 3 valid and 5 invalid programs of your own. | `tests/student/` | `student.minimum_tests` |

## 4. Scope and non-goals

**In scope:** the lexer (Part A) and the syntax checker (Part B) for the language defined in `docs/MiniLang-Reference.md`.

**Not in this lab:**

- **Semantic analysis**: types, declarations, scopes, `return` placement, call arity. `int x = true;` and `return 1;` at top level are *valid* here.
- **The AST.** The parser only *recognizes*: it builds no tree and its methods return `void`. The AST is the next unit; keeping the method names equal to the grammar's non-terminals makes that step easy.
- **Interpretation** (no program is executed).
- **Error recovery**: the parser stops at the first syntax error.
- **Unicode.** Input is treated byte by byte; non-ASCII input is unspecified.
- Adding language features. MiniLang is closed: if it is not in `docs/MiniLang-Reference.md` it is not MiniLang (section 6 of that file lists what is absent). Extensions are a *separate* exercise (section 13).

**Forbidden techniques** (they would defeat the purpose; `ctest -R rules.no_generators` looks for the obvious ones): `std::regex`, the `lex.yy.cpp` that RE/flex writes, parser generators (`bison`, `yacc`, ANTLR, Boost.Spirit, …), any third-party lexer or parser library. RE/flex is used **only** to obtain the DFA.

## 5. Setup

### 5.1 Tools

| Tool | Needed for | Check |
|---|---|---|
| C++20 compiler (g++ 11+, clang 14+, MSVC 19.30+) | build | `g++ --version` |
| CMake 3.20+ | build and tests | `cmake --version` |
| RE/flex 6.x | obtaining the DFA | `reflex --version` → `reflex 6.x` |
| Graphviz | `dot`: DFA picture | `dot -V` |

> **Warning: the wrong `reflex`.** On Ubuntu and Debian, `sudo apt install reflex` installs a **different program** ("Run a command when files change") that has nothing to do with lexers. The right one is **RE/flex** by Genivia (<https://github.com/Genivia/RE-flex>); build and install it from source following its README, as in the lab machines. After installing, `reflex --help` must list `--graphs-file`. If it does not, you have the wrong program.

Windows: use WSL (Ubuntu) and install everything inside it; the commands below are for a Unix shell.

### 5.2 Project layout

```
Lab1-MiniLang-Lexer-Parser/
├── Lab1.md                    this document
├── README.md                  one-page quick start
├── CMakeLists.txt             build + test registration (do not edit)
├── spec/
│   └── MiniLang.l             TODO(A1)  RE/flex specification
├── src/
│   ├── Lexer.hpp              provided   TokenId, Token, Lexer interface
│   ├── Lexer.cpp              TODO(A4, A5)  the DFA, state by state
│   ├── Parser.hpp             provided   ParseError, Parser, the EBNF grammar
│   ├── Parser.cpp             TODO(B3)   one method per non-terminal
│   ├── NotImplemented.hpp     provided   makes unfinished TODOs fail loudly
│   └── Main.cpp               provided   command-line driver (do not edit)
├── docs/
│   ├── MiniLang-Reference.md  the language definition (read it first)
│   ├── TRACEABILITY.md        TODO(A6)   DFA ↔ code, filled by you
│   ├── LL1-ANALYSIS.md        TODO(B1, B2, B5)   FIRST/FOLLOW, trace
│   └── MiniLang.gv, .svg, .numbered.svg   you generate them (Part A)
├── examples/{valid,invalid}/  sample MiniLang programs (also registered as tests)
├── tests/
│   ├── RunTest.cmake          the test runner (do not edit)
│   ├── lexer/                 public lexer tests:   NAME.txt + NAME.tokens
│   ├── parser/valid/          public valid programs:  NAME.txt
│   ├── parser/invalid/        public invalid programs: NAME.txt + NAME.expected
│   └── student/               YOUR tests (same three formats), auto-registered
└── tools/                     helper scripts for the checks (do not edit)
```

You edit **only** the files marked `TODO`, `Parser.hpp` (to declare helper methods, if you add any) and `tests/student/`.

### 5.3 Build and test

```
cmake -S . -B build                      # once
cmake --build build                      # after every change
ctest --test-dir build --output-on-failure
ctest --test-dir build -L lexer          # one group: lexer | parser | examples | student | docs
ctest --test-dir build -R parser.invalid.missing_expression   # one test, by name
```

The driver `build/minilang`:

| Command | What it does |
|---|---|
| `build/minilang --tokens FILE` | prints the tokens of `FILE`, one per line: `LINE:COL TokenId 'text'` |
| `build/minilang FILE...` | checks the syntax; prints `FILE: OK`, or `FILE:LINE:COL: error: MESSAGE` on stderr |
| `build/minilang --trace FILE` | same, and prints the parser call trace (step B5) |

Exit codes: `0` ok · `1` usage or unreadable file · `2` syntax error · `70` you have not implemented that part yet (the TODO is still there). While a part is unfinished its tests **fail with exit 70**; they can never pass by accident.

## 6. Part A: the lexer

### 6.1 What the lexer must recognize

The complete definition is in `docs/MiniLang-Reference.md`, section 2 and the conventions C1 to C8. This is the lab's contract, with the token name each lexeme must produce (the names are the enumerators of `TokenId` in `src/Lexer.hpp`):

| Category | Lexemes | `TokenId` |
|---|---|---|
| Keywords | `func` `int` `bool` `void` `if` `else` `while` `for` `return` `print` `read` `true` `false` | `KeywordFunc` `KeywordInt` `KeywordBool` `KeywordVoid` `KeywordIf` `KeywordElse` `KeywordWhile` `KeywordFor` `KeywordReturn` `KeywordPrint` `KeywordRead` `KeywordTrue` `KeywordFalse` |
| Identifier | `[a-zA-Z_][a-zA-Z0-9_]*`, if it is not a keyword | `Ident` |
| Integer literal | `[0-9]+` | `Number` |
| Arithmetic | `+` `-` `*` `/` | `OpAdd` `OpSub` `OpMul` `OpDiv` |
| Comparison | `<` `<=` `>` `>=` `==` `!=` | `OpLess` `OpLessEqual` `OpGreater` `OpGreaterEqual` `OpEqual` `OpNotEqual` |
| Logical | `&&` \|\| `!` | `OpAnd` `OpOr` `OpNot` |
| Assignment | `=` | `Assign` |
| Punctuation | `(` `)` `{` `}` `,` `;` | `LParen` `RParen` `LBrace` `RBrace` `Comma` `Semicolon` |
| **Ignored** | whitespace (space, tab, `\n`, `\r`) and `// line comments` | (no token) |
| End of input | | `EndOfFile` |
| **Error case** | any character that cannot start a token (`@`, `$`, `"`, `.`, …), and a `&` or \| not followed by the same character | `Unknown` (one token, containing just that character) |

Rules you must respect:

- **Maximal munch**: `<=` is one token, not `<` then `=`; `===` is `==` then `=`; `12abc` is `12` then `abc`.
- **Keywords are not DFA rules.** The DFA has one accepting state for "identifier or keyword"; the C++ looks the lexeme up in a keyword table (step A5). Keywords are case-sensitive: `iff`, `If` and `while1` are identifiers.
- A `//` comment ends at the line feed **without consuming it**, or at the end of the file. There are **no** `/* */` comments: `/*` is the two tokens `/` and `*`.
- There are no negative literals (`-5` is `OpSub` then `Number`), no floats (`3.14` is `Number`, `Unknown`, `Number`) and no strings.
- **The lexer does not check the value of a literal.** `99999999999999999999` is one `Number` token: a DFA for `[0-9]+` cannot and need not compare numbers. The **parser** rejects literals that do not fit in a 32-bit `int` (section 7.5, E6).
- The lexer **never throws and never stops**: bad input becomes an `Unknown` token and scanning goes on. The parser decides what to do with it.
- **Positions** are 1-based `line:column` of the **first** character of the token; a tab counts as one column. `EndOfFile` is positioned just after the last character (column 1 of the next line if the file ends with a line feed). `nextChar()` already tracks `line` and `column` for you: your job is to record them **when the token starts**.

### 6.2 Interface and token representation (provided)

```cpp
struct Token {
    TokenId tokenId;      // which kind of token
    std::string text;     // the exact characters of the lexeme
    int line, column;     // position of its first character (1-based)
};

class Lexer {
public:
    explicit Lexer(std::istream& in);
    Token nextToken();                              // next token; EndOfFile forever at the end
    static const char *tokenToString(TokenId tid);  // "OpAdd", "KeywordIf", ...
};
```

Inside `nextToken()` you have, as in the class samples: `ch` (the current character, **not yet consumed**; `EOF` at the end) and `nextChar()` (consumes it). The text of `EndOfFile` is not checked (the class samples use `"<<EOF>>"`).

### 6.3 Step by step (A1 to A7)

#### A1. Write the RE/flex specification

Edit `spec/MiniLang.l`: delete the `TODO(A1)` line and write one rule per line between the two `%%`, in the form used in class:

```
<regular expression>      // name of the token
```

The `//` comment after the pattern is the empty action; the file contains **no code**. RE/flex syntax you will need:

| You want | Write | Example |
|---|---|---|
| a fixed string | the string in double quotes | `"<="` |
| a set of characters | `[ ... ]` with ranges | `[a-zA-Z_]`, `[ \t\r\n]` |
| zero or more, one or more | `*`, `+` after an item | `[0-9]+` |
| any character except a line feed | `.` | `"//".*` |
| alternation | \| **outside** quotes | a\|b |

Three requirements and two traps:

- **One rule per token.** Do not merge different tokens into one rule (for example `+ - * /` as a single character class): the DFA would then have *one* accepting state for four different tokens, and the picture could no longer tell you which one was recognized. Only whitespace, which produces no token, is a single rule. The order of the rules is yours.
- **Do not write rules for the keywords.** The identifier rule already matches them; the C++ tells them apart (step A5).
- Comments are part of the spec too: write the rule for `// line comments`.
- *Trap:* a `|` you want as a character (as in `||`) **must be inside quotes**.
- *Trap:* a character that is special in regular expressions (`+ * ( ) { } . ?`) must be quoted too, or escaped.

*Done when:* `spec/MiniLang.l` has no `TODO(A1)` and `ctest -R docs.spec` passes.

#### A2. Generate the DFA with RE/flex

```
mkdir -p build
reflex --graphs-file=docs/MiniLang.gv --outfile=build/lex.yy.cpp spec/MiniLang.l
```

`--graphs-file` writes the DFA as a Graphviz file. RE/flex also writes a C++ scanner (`build/lex.yy.cpp`): **you never use or keep it**; `--outfile` just keeps it out of your source folder. Open `docs/MiniLang.gv` in a text editor: every state is a line starting with `/*START*/`, `/*STATE*/` or `/*ACCEPT n*/`, where `n` is the number of the rule of your spec that the state accepts (rules are counted from 1, in the order you wrote them; blank lines and comment-only lines are not rules).

*Done when:* `docs/MiniLang.gv` exists and you can say how many states it has.

#### A3. Draw it with Graphviz

```
dot -Tsvg docs/MiniLang.gv -o docs/MiniLang.svg
dot -Tpdf docs/MiniLang.gv -o docs/MiniLang.pdf       # optional, the class convention
```

#### A4. Read the DFA and give its states names

RE/flex names states after memory addresses, which is useless for talking about them. The helper renames them `S0`, `S1`, … **in the order they appear in the `.gv`** (`S0` is the start state) and writes the name in each circle:

```
cmake -DIN=docs/MiniLang.gv -DOUT=docs/MiniLang.numbered.gv -P tools/NumberDfa.cmake
dot -Tsvg docs/MiniLang.numbered.gv -o docs/MiniLang.numbered.svg
```

Open `docs/MiniLang.numbered.svg` in a browser. How to read it:

- `INITIAL → S0`: `S0` is the start state.
- A **double circle** is an **accepting** state; `[n]` is the RE/flex rule it accepts. A single circle is not accepting.
- An arrow is labeled with the characters that activate it. `a-z` is a range; `\t-\n` is the range from tab to line feed; `\x20` is the space; `\v-\xff` is from vertical tab (just after the line feed) to byte 255.
- The DFA follows the arrow of the current character. **If there is no arrow, the token ends and that character is not consumed:** in an accepting state the token of that state is returned (that is maximal munch); in a non-accepting state nothing was matched and the lexer must return `Unknown` with what it has read.
- `S0` has no arrow for the end of the input or for characters that start no token: your code decides what happens there (6.1).

**Before writing code**, fill the walkthrough table of `docs/TRACEABILITY.md` §2 for the four inputs it lists, using only the picture. This is the skill A3. Then compare with what your lexer does once it works.

*Done when:* you can point to every state of the picture and say whether it is accepting and which arrows leave it.

#### A5. Implement the lexer

In `src/Lexer.cpp` write `Lexer::nextToken()` following **your** picture, in the style of `LexerSample1`:

- `int state = 0;` and `switch (state)` inside a loop; **one `case` per DFA state**, and `state = k;` for every arrow.
- Put the state's name **in a comment on the same line as the case label**: `case 7:   // S7  '<' [8]`. `docs.traceability` looks for exactly that.
- Follow an arrow = add the character to `text`, call `nextChar()`, set `state`. No arrow = `return {TokenId::…, text, startLine, startColumn};`.
- State 0 runs again after whitespace and comments: it must **clear `text`** and **record the start position** every time.
- Implement `identifierKind` (the keyword table) and call it from the state that accepts identifiers.
- Build and test as you go; a missing state throws `not implemented yet … DFA state k`, which tells you which one to write next.

A good order: state 0 with whitespace and `EOF` → single-character tokens (`( ) { } , ; + - *`) → two-character operators (`< <= > >= = == ! !=`) → `&` and `|` → identifiers and keywords → numbers → `/` and comments.

*Done when:* `ctest --test-dir build -L lexer` passes completely and `rules.no_generators` passes.

#### A6. Prove the correspondence

Complete `docs/TRACEABILITY.md`: one row per state (arrows, `case` and line, what happens when no arrow matches), the four walkthroughs, the places where the code does more than the DFA, and the `%` question. `ctest -R docs.traceability` checks that **no state is missing** in the graph, in `Lexer.cpp` and in the document; whether what you wrote is **true** is for you to verify, and it is what the in-person evaluation will ask.

*Done when:* `docs.traceability` passes and, for any state `Sk` someone names, you can show its `case` in under ten seconds.

#### A7. Write your own lexer tests

Add to `tests/student/lexer/` at least **5** tests of your own: `NAME.txt` (the input) and `NAME.tokens` (the expected output, in the format of `build/minilang --tokens`). **Write the expected file by reasoning from the definition**, never by pasting your program's output: a test that only repeats your program's behavior cannot find a bug. Look at the public tests in `tests/lexer/` for the format and for the kind of case worth testing.

*Done when:* `student.minimum_tests` counts at least 5 lexer tests and they pass.

## 7. Part B: the parser

### 7.1 Why the grammar is LL(1), and what was transformed

> **Try first.** Step B1 asks you to derive these transformations yourself. If you want the practice, do B1 **before** reading the table below: it contains the answers.

The grammar of `docs/MiniLang-Reference.md` (section 3, repeated in 7.2) is already in **LL(1)** form: a recursive-descent parser can decide every alternative, optional part and loop by looking at **one** token. Writing it that way required these transformations from the "natural" grammar a language designer would write first (the left-hand column is how that natural form looks; it is *not* part of MiniLang):

| # | Natural form | Problem | Transformation | Result (in the grammar) |
|---|---|---|---|---|
| T1 | orExpr → orExpr "\|\|" andExpr \| andExpr (same for `andExpr`, `equality`, `relational`, `additive`, `term`) | **left recursion**: `orExpr()` would call itself without consuming a token | replace recursion by **iteration** (EBNF loop) | orExpr → andExpr ( "\|\|" andExpr )\* |
| T2 | `assignment → IDENT "=" expr ";"` and `callStmt → IDENT "(" [args] ")" ";"` as two alternatives of `statement` | **common prefix** `IDENT`: the FIRST sets of the alternatives overlap | **left factoring**: read `IDENT` once, then decide on the next token | simpleStmt → IDENT ( "=" expr \| "(" [ args ] ")" ) ";" |
| T3 | `variable → IDENT` and `call → IDENT "(" [args] ")"` as two alternatives of `primary` | common prefix `IDENT` | left factoring; the rest becomes optional | primary → … \| IDENT [ "(" [ args ] ")" ] \| … |
| T4 | `if (c) stmt [ else stmt ]` | the **dangling else**: which `if` does an `else` belong to? | not a rewriting but a **language decision**: bodies are always `block`s | ifStmt → "if" "(" expr ")" block [ "else" ( ifStmt \| block ) ] |

Two more points that need **no** transformation, and why: `forInit → type IDENT "=" expr | IDENT "=" expr` (its alternatives start with different tokens: `int` / `bool` versus `IDENT`) and `unary → ( "-" | "!" ) unary | primary` (right recursion is fine for LL(1): the recursive call comes **after** consuming a token). The EBNF operators `[ ]` and `( )*` are allowed because they preserve LL(1) behavior, as long as the **decision conditions** below hold.

**Associativity.** The loops in T1 do not build a tree, but they are what makes all binary operators **left-associative**: `a - b - c` is `(a - b) - c`, because each new operand is combined with the result accumulated so far. A recognizer cannot observe it; the AST of the next unit will.

### 7.2 The grammar (EBNF), authoritative

```
program     → item* EOF
item        → function | statement

function    → "func" returnType IDENT "(" [ params ] ")" block
params      → param ( "," param )*
param       → type IDENT
type        → "int" | "bool"
returnType  → type | "void"

block       → "{" statement* "}"

statement   → varDecl | simpleStmt | ifStmt | whileStmt | forStmt
            | returnStmt | printStmt | readStmt | block

varDecl     → type IDENT [ "=" expr ] ";"
simpleStmt  → IDENT ( "=" expr | "(" [ args ] ")" ) ";"
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
            | IDENT [ "(" [ args ] ")" ]
            | "(" expr ")"
args        → expr ( "," expr )*
```

`IDENT` is `TokenId::Ident`, `NUMBER` is `TokenId::Number`, `EOF` is `TokenId::EndOfFile`, and every quoted terminal is the token of Appendix A with that lexeme.

### 7.3 Justification that the grammar is LL(1)

> **Try first.** Step B2 asks you to compute FIRST and FOLLOW and to check decision points yourself. Do it before reading the table below and Appendix C: they contain the answers.

The condition, for EBNF: at every **choice** the sets of tokens that select each branch must be **disjoint**. For `A | B` those sets are FIRST(A) and FIRST(B). For an optional `[ X ]` or a loop `( X )*` they are FIRST(X) and what may **follow** the whole construct: the tokens that come after it in the rule (for example `}` after the `statement*` of a `block`) and, when the construct ends the rule, FOLLOW of the rule. The parser enters `X` when the lookahead is in FIRST(X) and leaves otherwise. The mechanical check of every decision of MiniLang (done on the grammar expanded to plain BNF, 53 non-terminals) finds no conflict:

| Rule | Decision the parser makes | Set A | Set B | A ∩ B = ∅ |
|---|---|---|---|:-:|
| `program` | `item*`: enter the loop / exit | FIRST(item) | { EOF } | yes |
| `item` | `function` / `statement` | { `func` } | FIRST(statement) | yes |
| `function` | `[ params ]`: present / absent | { `int`, `bool` } | { `)` } | yes |
| `params` | `( "," param )*`: continue / stop | { `,` } | { `)` } | yes |
| `type` | `int` / `bool` | { `int` } | { `bool` } | yes |
| `returnType` | `type` / `void` | { `int`, `bool` } | { `void` } | yes |
| `block` | `statement*`: enter / exit | FIRST(statement) | { `}` } | yes |
| `statement` | 9 alternatives | { `int`,`bool` } · { IDENT } · { `if` } · { `while` } · { `for` } · { `return` } · { `print` } · { `read` } · { `{` } | pairwise | yes |
| `varDecl` | `[ "=" expr ]` | { `=` } | { `;` } | yes |
| `simpleStmt` | `"=" expr` / `"(" [ args ] ")"` | { `=` } | { `(` } | yes |
| `simpleStmt` | `[ args ]` inside the call | FIRST(expr) | { `)` } | yes |
| `ifStmt` | `[ "else" … ]`: present / absent | { `else` } | FOLLOW(ifStmt) = FIRST(statement) ∪ { `func`, `}`, EOF } | yes |
| `ifStmt` | after `else`: `ifStmt` / `block` | { `if` } | { `{` } | yes |
| `forStmt` | `[ forInit ]` | { `int`, `bool`, IDENT } | { `;` } | yes |
| `forStmt` | `[ expr ]` | FIRST(expr) | { `;` } | yes |
| `forStmt` | `[ forUpdate ]` | { IDENT } | { `)` } | yes |
| `forInit` | `type IDENT …` / `IDENT …` | { `int`, `bool` } | { IDENT } | yes |
| `returnStmt` | `[ expr ]` | FIRST(expr) | { `;` } | yes |
| `orExpr` … `term` | each operator loop: continue / stop | the operators of the level (\|\|; `&&`; `==` `!=`; `<` `<=` `>` `>=`; `+` `-`; `*` `/`) | FOLLOW of the level (only `)`, `,`, `;` and the operators of lower precedence) | yes |
| `unary` | ( "-" \| "!" ) unary / `primary` | { `-`, `!` } | FIRST(primary) | yes |
| `primary` | 5 alternatives | { NUMBER } · { `true` } · { `false` } · { IDENT } · { `(` } | pairwise | yes |
| `primary` | `IDENT [ "(" … ")" ]`: call / variable | { `(` } | FOLLOW(primary) (no `(` in it) | yes |
| `args` | `( "," expr )*`: continue / stop | { `,` } | { `)` } | yes |

where FIRST(statement) = { `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{` } and FIRST(expr) = { NUMBER, `true`, `false`, IDENT, `-`, `!`, `(` }. **The complete FIRST and FOLLOW tables are in Appendix C**: compute yours first (step B2).

Two decisions deserve a second look, because they are where LL(1) usually fails:

- **`else`.** `else` is in FOLLOW(`block`), but **not** in FOLLOW(`ifStmt`) (`ifStmt` is always followed by something that can start a statement, `}`, `func` or the end of the input). So at `[ "else" … ]` the parser can always decide.
- **A call.** `(` never follows an expression, so after `IDENT` the token `(` always means "call".

### 7.4 Parser interface and expectations (provided)

```cpp
class Parser {
public:
    explicit Parser(Lexer& lexer);
    void parse();                      // whole program; throws ParseError at the first syntax error
private:
    void program();  void item();  void function();  /* ... one void method per non-terminal ... */
    void nextToken();                                          // provided
    void expect(TokenId expected, const std::string& message); // provided: consume or throw
    ParseError error(const std::string& message) const;        // provided: error at `token`
    static std::string describe(const Token& tok);             // provided: "'x'", "end of input"
    Token token;                                               // the lookahead
};
class ParseError : public std::runtime_error { int line, column; /* ... */ };
```

- **One token of lookahead** (`token`), **no backtracking**, no looking ahead in the lexer, no re-reading the input.
- **One method per non-terminal**, named after it. `program()` implements only the `item*` loop: `parse()` already checks the `EOF`.
- **`expect` advances the lookahead.** After a successful `expect(...)` do **not** call `nextToken()`.
- **A recognizer, not a translator.** There is **no AST** in this lab: methods return `void` and create no nodes. The AST is the next unit.
- **No semantic checks of any kind** (the range of integer literals is the one value check, and it is a rule of the syntax here: E6 in section 7.5). The parser accepts `int x = true;`, `y = 1;` for an undeclared `y`, `return 1;` at top level, `print f(1, 2);` for a one-parameter `f`, and `a < b < c`. `examples/valid/semantic_errors_still_parse.txt` has them all.
- You may add private helper methods to `Parser.hpp` / `Parser.cpp` (for example "does the current token start an expression?").

### 7.5 Syntax-error requirements

| # | Requirement |
|---|---|
| E1 | The parser reports the **first** syntax error and stops; it throws `ParseError` (via `throw error("...")` or a failing `expect`). |
| E2 | The error position is the position of the **first token that cannot continue any valid program** (convention C10). For the end of input it is the `EndOfFile` token. Check the lookahead **before** consuming it, so that the position is that token's. |
| E3 | An `Unknown` token from the lexer is a syntax error at the position of that token (it belongs to no FIRST set); its message should say "unknown character". |
| E4 | The diagnostic printed by the driver is `FILE:LINE:COL: error: MESSAGE`. **The position is checked by the tests; the wording is yours**, but a good message says what was **found** (`describe(token)`) and what was **expected**. |
| E5 | A valid program must never be rejected and an invalid one never accepted; trailing input after the last item is an error (`parse()` checks it). |
| E6 | **Integer literals must fit in a 32-bit signed `int`.** A `Number` whose value, leading zeros ignored, is greater than **2147483647** is a syntax error **at that literal** (`x = 2147483648;` fails at 1:5). `-` is a separate operator, so `-2147483648` is rejected too. The lexer still returns the `Number` token: the check belongs to `primary()`, and it must not overflow while converting (compare digit strings, or catch the conversion error). |

Examples (the position is where the error must be reported):

```
int x = ;                 error at 1:9   (';' where an expression was expected)
if (x < 3) print x;       error at 1:12  ('print': blocks are mandatory)
x = y = 3;                error at 1:7   (the second '=': assignment is not an expression)
func int f( { }           error at 1:13  ('{' where a parameter or ')' was expected)
b = x & y;                error at 1:7   (Unknown token '&')
x = 2147483648;           error at 1:5   (literal larger than 2147483647)
```

### 7.6 Step by step (B1 to B6)

#### B1. Transformations

Complete `docs/LL1-ANALYSIS.md` §1 **before** reading the table of 7.1 (if you have not already seen it): for each of the three natural forms, say why it is not directly implementable, name the transformation and write the result. Then compare.

*Done when:* §1 of the document has no `TODO` and you can explain the difference between removing left recursion and left factoring in one sentence each.

#### B2. FIRST, FOLLOW and the LL(1) condition

Complete `docs/LL1-ANALYSIS.md` §2 (FIRST and FOLLOW of 8 non-terminals) and §3 (five decision points) **on paper first**. Only then compare with Appendix C and write down every difference and why it happened.

*Done when:* §2 and §3 have no `TODO` and every difference with Appendix C is explained.

#### B3. Implement the parser

Write the methods of `src/Parser.cpp`, replacing each `throw NotImplementedError(...)`. Translation table (class style, `ParserSample1`):

| EBNF | C++ |
|---|---|
| `A B` | `A(); B();` |
| a terminal `"t"` | `expect(TokenId::T, "Expected 't' but found " + describe(token));` |
| A \| B (disjoint FIRST) | `if (token is in FIRST(A)) A(); else if (token is in FIRST(B)) B(); else throw error("...");` |
| `[ A ]` | `if (token is in FIRST(A)) A();` |
| `( A )*` | `while (token is in FIRST(A)) A();` |
| `A ( "," A )*` | `A(); while (token.tokenId == TokenId::Comma) { expect(TokenId::Comma, "..."); A(); }` |

"Token is in FIRST(A)" is a comparison with the tokens of that set (Appendix C): put it in a small helper function when it is used more than once. Never use FOLLOW sets to *decide*; they only tell you that an optional part is absent.

Suggested order, testing each step (`ctest -R parser.valid.…`): the expression chain from `primary` up to `expr` → `varDecl`, `simpleStmt`, `printStmt`, `readStmt`, `returnStmt` → `block`, `ifStmt`, `whileStmt`, `forStmt` → `type`, `returnType`, `param`, `params`, `function` → `statement`, `item`, `program`.

*Done when:* `ctest --test-dir build -L parser` and `-L examples` pass completely.

#### B4. Make the errors right

Run `ctest -L parser --output-on-failure`: a failing `parser.invalid.*` test says the position you reported and the position expected. Typical causes are in section 11. Then write messages that help a human: *what* was found and *what* was expected.

*Done when:* all `parser.invalid.*` and `example.invalid.*` tests pass and your messages follow E4.

#### B5. Trace the calls

For the input `x = 1 + 2 * y;`, write by hand, in `docs/LL1-ANALYSIS.md` §4, the sequence of calls made from `parse()`. Then add `TraceScope trace(*this, "name");` as the first line of each of your non-terminal methods and run `build/minilang --trace file.txt`. **If your trace and the program's differ, find out who is right.**

*Done when:* §4 is complete and you can explain why `*` appears deeper in the trace than `+`.

#### B6. Write your own parser tests

Add to `tests/student/parser/valid/` at least **3** valid programs and to `tests/student/parser/invalid/` at least **5** invalid ones (`NAME.txt` plus `NAME.expected` containing only `LINE:COL`). Derive the expected position **from the grammar** (the first token that cannot continue a valid program), not from what your parser prints. Look for the cases that public tests do not cover: an error inside a nested block, in the second argument of a call, in the `else if` chain, at the end of the file.

*Done when:* `student.minimum_tests` passes.

## 8. Deliverables and submission checklist

Even though the lab is ungraded, the submission checklist is the **definition of "finished"**. If you want feedback, hand in the folder as your instructor announces in class, **without** `build/`.

| # | Deliverable | Checked by |
|---|---|---|
| 1 | `spec/MiniLang.l`: RE/flex specification | `docs.spec` |
| 2 | `docs/MiniLang.gv`: the RE/flex DFA, **as generated** (not edited by hand) | `docs.traceability` |
| 3 | `docs/MiniLang.svg` and `docs/MiniLang.numbered.svg`: the DFA drawings | `docs.traceability` |
| 4 | `src/Lexer.cpp`: the C++20 lexer, one `case` per DFA state | `lexer.*`, `rules.no_generators` |
| 5 | `docs/TRACEABILITY.md`: DFA ↔ code correspondence | `docs.traceability`; you |
| 6 | `docs/LL1-ANALYSIS.md`: transformations, FIRST/FOLLOW, decision points, trace | `docs.ll1_analysis`; you |
| 7 | `src/Parser.cpp` (and `Parser.hpp` if you added helpers) | `parser.*`, `example.*` |
| 8 | `tests/student/`: at least 5 lexer tests, 3 valid and 5 invalid programs | `student.*` |

Checklist before you stop:

- [ ] `ctest --test-dir build --output-on-failure` reports **100% tests passed**.
- [ ] You deleted every `TODO(...)` marker comment you completed: `grep -rn "TODO(" spec src` prints nothing. And no placeholder is left in your documents: `grep -rn "<TODO>" docs` prints nothing.
- [ ] The DFA files were generated **from the current spec**: if you edited `spec/MiniLang.l` after step A2, you regenerated `.gv`, `.svg` and `.numbered.svg`, **renumbered**, and updated `Lexer.cpp` and `docs/TRACEABILITY.md` (the numbers may change).
- [ ] `ctest --test-dir build -R rules.no_generators` passes: no `std::regex`, generated scanner or parser generator in `src/`.
- [ ] The compiler prints **no warnings** (`-Wall -Wextra -Wpedantic` are on).
- [ ] You can explain, without looking, one state of the DFA, one decision of the parser and one syntax error of the tests.
- [ ] `build/` and `build*/` are deleted.

## 9. Acceptance criteria

**Automated (all must hold):**

| Criterion | How to check |
|---|---|
| The project builds with no warnings under C++20. | `cmake --build build` |
| Every public lexer test passes: exact token stream, kinds, texts and positions. | `ctest -L lexer` |
| Every public valid program is accepted and every invalid one is rejected with exit 2 at the expected `LINE:COL`. | `ctest -L parser`, `ctest -L examples` |
| Your own tests exist (5 / 3 / 5 minimum) and pass. | `ctest -L student` |
| The spec is accepted by RE/flex, has no keyword rules, and the DFA, the lexer and `TRACEABILITY.md` agree on the number of states. | `ctest -R docs.spec` and `ctest -R docs.traceability` |
| `LL1-ANALYSIS.md` is complete. | `ctest -R docs.ll1_analysis` |
| No `std::regex`, generated scanner or parser generator is used. | `ctest -R rules.no_generators` |

**Not automated (you check them, and the in-person evaluation will):**

| Criterion |
|---|
| Your lexer **is** the DFA of your picture: every state of the SVG corresponds to one `case`, and every arrow to one `if` or `else if` inside it. |
| Your explanations in `TRACEABILITY.md` and `LL1-ANALYSIS.md` are **true**, and your own FIRST/FOLLOW answers were computed before looking at Appendix C. |
| The parser uses exactly one lookahead token and never backtracks; `expect` is never followed by `nextToken()`. |
| Your tests' expected results come from the language definition. |

## 10. How you will be evaluated later

This lab has no grade. Later, each student is assessed **individually, in person, on a computer, with the instructor's CBE Platform Tool**. What follows describes the **kind** of task so that you can practice for it; the exact items, the time limit and the weight will be announced by the instructor, and they are not published in advance.

- **Format** (as in the earlier tests of the course): you answer in fields of the platform (short values, text boxes, code boxes); **no notes, no previous code and no external tools are allowed**; only what you type in the answer fields is evaluated.
- **New items.** The evaluation does not reuse the files of this lab, and it may use MiniLang or a different small language built with the same techniques. It measures the **skills** in section 3, not whether you remember MiniLang.

| Objective | A task of this kind in the evaluation | Practice now |
|---|---|---|
| **A1, A2** | Given a token description, say or write the RE/flex pattern; given a spec, say which rule an input matches. | Cover your `.l`, rewrite it from section 6.1. |
| **A3** | Given a RE/flex DFA picture and an input string, write the exact token sequence. | The walkthrough table (A4), on new inputs that you invent. |
| **A4, A5** | Given the DFA picture and a partly written `nextToken()`, write the missing `case` blocks. | Delete three cases of your `Lexer.cpp` and rewrite them from the picture. |
| **A6** | Say what the lexer returns, and at which position, for tricky input. | Your own lexer tests; positions at end of file. |
| **B1, B2** | Remove left recursion or factor a rule; compute FIRST and FOLLOW; fill an LL(1) table cell. | `LL1-ANALYSIS.md` on paper, then a small invented grammar. |
| **B3** | Write parser methods for an EBNF fragment in the style of `ParserSample1`. | Rewrite `expr` and `statement` without looking. |
| **B4** | Say where the parser reports the error for a given input. | Predict the position of each `tests/parser/invalid` file, then check. |
| **B5** | Write the sequence of calls of the parser for an input. | `LL1-ANALYSIS.md` §4 on more inputs, verified with `--trace`. |
| **B6** | Add a new statement or operator and say what changes in the token enum, the lexer, the grammar and the parser. | Section 13. |
| **T1** | Review tests whose expected results were copied from a program's output and find the wrong ones; write a test that exposes a described bug, with its expected position. | Write the expected files of your own tests from the definition, then swap them with a classmate and review each other's. |

## 11. Common pitfalls and debugging

**Tools**

| Symptom | Cause and fix |
|---|---|
| `reflex: command not found`, or `unrecognized option --graphs-file` | RE/flex is not installed, or you installed the wrong `reflex` (section 5.1). |
| `reflex` cannot write `build/lex.yy.cpp` | the folder does not exist: `mkdir -p build`. |
| The state numbers changed after you edited the spec | normal: RE/flex lists the states in the order it creates them, which depends on the order of your rules. Regenerate, renumber, and update `Lexer.cpp` and `TRACEABILITY.md`. Do not edit the spec after A5 unless you have to. |
| Every test fails with `not implemented yet` | that part still has its `TODO`; the message names the state or method. |

**Lexer**

| Symptom | Cause and fix |
|---|---|
| `<=` is read as `<` then `=` | the accepting state of `<` returned immediately instead of checking for `=`: an accepting state with an outgoing arrow must look at the next character first (maximal munch). |
| A token is missing its last character, or the next token loses its first | off-by-one with `nextChar()`: consume the character **only** when following an arrow; a state with no arrow must leave `ch` alone. |
| Positions are wrong after spaces or comments | state 0 did not record `startLine` / `startColumn` (and clear `text`) **every time** it is re-entered. |
| Line numbers are off by one after a comment | the comment state consumed the line feed. It must stop **at** the line feed. |
| Infinite loop on a file that ends inside a comment | the comment state does not check for `EOF` (`ch` is an `int`: compare with `EOF`, never with `char`). |
| `&` or \| followed by a letter swallows the letter | the non-accepting state must return `Unknown` with the text so far **without** consuming the letter. |
| `iff`, `While` or `printx` come out as keywords | match **the whole lexeme** against the keyword table, case-sensitive; a prefix is not a keyword. |
| `12abc` is reported as one token or an error | it is two tokens, `Number` and `Ident`; the lexer does not decide that it is wrong, the parser does. |
| You used `isalpha` / `isdigit` | they depend on the locale and on `char` signedness; write the ranges `'a' <= c && c <= 'z'` explicitly. |

**Parser**

| Symptom | Cause and fix |
|---|---|
| Stack overflow or infinite loop | a method calls itself (directly or through others) without consuming a token: left recursion, or a loop whose condition never changes. |
| Everything after a statement is "unexpected" | you called `nextToken()` after `expect(...)`: `expect` already advanced. |
| Error reported one token late or early | you consumed the offending token before calling `error`, or you reported it at the previous token. Check the lookahead, then `throw error(...)` **without** consuming. |
| A valid `for (;;)` or `return;` is rejected | an optional part was parsed unconditionally: test FIRST of the part (`[ X ]` → `if (token in FIRST(X))`). |
| `x = 2147483648;` is accepted, or `99999999999999999999` crashes the parser | E6 is missing or converts with a type that overflows (`int`, `long`) or throws an unhandled exception. Compare the digit strings (skip leading zeros first), or catch `std::out_of_range` from `std::stoi`; report at the literal **before** consuming it. |
| `f(1,);` is accepted | the loop of `args` consumed the comma and then allowed an empty expression: after `,` an `expr` is **mandatory**. |
| A semantic error is reported as a syntax error | the parser must not check types, declarations or `return` placement (section 7.4). |
| `x = 1;;` is accepted | you accept an empty statement; MiniLang has none. |

**Tests**

| Symptom | Cause and fix |
|---|---|
| Your `.expected` / `.tokens` pass but you have doubts | you probably copied them from your program's output. Rewrite them from the definition. |
| A token test fails only on the last line | the expected file must end with a newline, and `EndOfFile` has no text. |
| A test is not listed by `ctest` | a `.txt` without its companion file (`.tokens` or `.expected`) is not registered; CMake warns when it configures. Re-run `cmake -S . -B build` after adding files. |

**General debugging**

- `build/minilang --tokens file.txt` shows what the lexer sees; compare with your expectation line by line (`build/minilang --tokens f.txt | diff - f.tokens`).
- `build/minilang --trace file.txt` shows which parser methods ran.
- Build with sanitizers: `cmake -S . -B build-asan -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -g"`, then `cmake --build build-asan` (delete the folder afterwards).
- Reduce a failing input to the smallest file that still fails before looking at the code.

## 12. Academic integrity

This lab has **no grade**, so there is nothing to gain by copying and a lot to lose: the in-person evaluation tests what **you** can do alone, without code, notes or tools. The rules below protect the one thing the lab is for, your own learning.

**Encouraged**

- Discussing ideas with classmates: how to read a state, why a grammar is or is not LL(1), what a test should check.
- Sharing **test inputs** and comparing the results of your programs.
- Using the class samples (`LexerSample1`, `ParserSample1`, `ParserSample2`), the RE/flex and Graphviz documentation, and your class notes.
- Asking the instructor.

**Not acceptable**

- Copying another student's lexer, parser, DFA, FIRST/FOLLOW tables or traceability document, or letting someone copy yours.
- Using the scanner **generated** by RE/flex (`lex.yy.cpp`), or any generator or library, as your lexer or parser (section 4).
- Having an AI assistant or another person write the lexer, the parser or the documents for you. If you use an assistant, use it to **explain** a concept or to **review code you already wrote**, and be able to say, in your own words, what every line of your code does. In the in-person evaluation no assistant is available.

**A simple test of honesty:** close the project and try to rebuild one state of the DFA, one parser method and one FIRST set on paper. If you cannot, the lab did not do its job, whoever wrote the code.

## 13. Stretch exercises (optional, ungraded)

Do them in **a copy** of the project (`cp -r Lab1-MiniLang-Lexer-Parser ../Lab1-extension`), never in the files you keep: the tests of this lab describe MiniLang as it is.

1. **Modulo.** Add `%` with the precedence of `*` and `/`. List every artifact that changes **before** you touch any (spec, DFA, `TokenId`, `Lexer.cpp`, `tokenToString`, grammar, `Parser.cpp`, tests), then do it and add tests.
2. **`do … while`.** Add `do block while ( expr ) ;` as a statement. Show that the grammar stays LL(1): which set is new in FIRST(`statement`)?
3. **Mechanical check.** Encode the MiniLang grammar, without the EBNF operators, in the `Grammar` class of `ParserSample2` and let it compute FIRST, FOLLOW and the LL(1) table; compare with Appendix C.
4. **A preview of the AST.** Make the parser print every expression fully parenthesized (`1 + 2 * 3` → `(1 + (2 * 3))`) and verify left associativity on `a - b - c`.
5. **Many errors.** Use `;` as a synchronization token (a FOLLOW-set decision) to report several syntax errors in one run.

---

## Appendix A. Token reference

| `TokenId` | Lexeme | Notes |
|---|---|---|
| `Unknown` | one character | not part of any token; or a lone `&` / \| |
| `EndOfFile` | | end of the input |
| `Ident` | `[a-zA-Z_][a-zA-Z0-9_]*` | not a keyword |
| `Number` | `[0-9]+` | any digit sequence; the parser rejects values above 2147483647 |
| `KeywordFunc` … `KeywordFalse` | `func` `int` `bool` `void` `if` `else` `while` `for` `return` `print` `read` `true` `false` | one token each, in this order |
| `OpAdd` `OpSub` `OpMul` `OpDiv` | `+` `-` `*` `/` | |
| `OpLess` `OpLessEqual` `OpGreater` `OpGreaterEqual` | `<` `<=` `>` `>=` | |
| `OpEqual` `OpNotEqual` | `==` `!=` | |
| `OpAnd` `OpOr` `OpNot` | `&&` \|\| `!` | |
| `Assign` | `=` | |
| `LParen` `RParen` `LBrace` `RBrace` `Comma` `Semicolon` | `(` `)` `{` `}` `,` `;` | |

37 `TokenId` values in total.

## Appendix B. Test formats and driver contract

| File | Format |
|---|---|
| `tests/lexer/NAME.txt` + `NAME.tokens` | `.tokens` = output of `minilang --tokens NAME.txt`: one line `LINE:COL TokenId 'text'` per token; the last line is `LINE:COL EndOfFile` (no text). In the text, `\` is written `\\`, `'` is written `\'`, and a byte that is not printable ASCII is written `\xHH`. The whole file must match, line by line. |
| `tests/parser/valid/NAME.txt` | `minilang NAME.txt` must exit 0. |
| `tests/parser/invalid/NAME.txt` + `NAME.expected` | `minilang NAME.txt` must exit 2 and print `NAME.txt:LINE:COL: error: …` with the `LINE:COL` written (alone, on one line) in `NAME.expected`. |

Test names in `ctest`: `lexer.NAME`, `parser.valid.NAME`, `parser.invalid.NAME`, `example.valid.NAME`, `example.invalid.NAME`, `student.lexer.NAME`, `student.valid.NAME`, `student.invalid.NAME`, plus `docs.spec`, `docs.traceability`, `docs.ll1_analysis`, `student.minimum_tests`, `rules.no_generators`.

## Appendix C. FIRST and FOLLOW of MiniLang (reference for step B2)

**Compute your own first** (step B2). EOF is the end of the input; `IDENT` and `NUMBER` are the token kinds `Ident` and `Number`. Computed on the grammar of 7.2 with `item*`, `statement*`, `[ ]` and `( )*` expanded to plain BNF (53 non-terminals, 24 of them helper rules); FOLLOW(`program`) is empty because `EOF` is inside the rule. Only the 29 non-terminals of the EBNF grammar are listed; none can derive the empty string.

| Non-terminal | Nullable | FIRST | FOLLOW |
|---|:-:|---|---|
| `program` | no | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{` } | { } |
| `item` | no | { `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{` } |
| `function` | no | { `func` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{` } |
| `params` | no | { `int`, `bool` } | { `)` } |
| `param` | no | { `int`, `bool` } | { `)`, `,` } |
| `type` | no | { `int`, `bool` } | { IDENT } |
| `returnType` | no | { `int`, `bool`, `void` } | { IDENT } |
| `block` | no | { `{` } | { EOF, `func`, `int`, `bool`, `if`, `else`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `statement` | no | { `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `varDecl` | no | { `int`, `bool` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `simpleStmt` | no | { IDENT } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `ifStmt` | no | { `if` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `whileStmt` | no | { `while` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `forStmt` | no | { `for` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `forInit` | no | { `int`, `bool`, IDENT } | { `;` } |
| `forUpdate` | no | { IDENT } | { `)` } |
| `returnStmt` | no | { `return` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `printStmt` | no | { `print` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `readStmt` | no | { `read` } | { EOF, `func`, `int`, `bool`, `if`, `while`, `for`, `return`, `print`, `read`, IDENT, `{`, `}` } |
| `expr` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `)`, `,`, `;` } |
| `orExpr` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `)`, `,`, `;` } |
| `andExpr` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { \|\|, `)`, `,`, `;` } |
| `equality` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `&&`, \|\|, `)`, `,`, `;` } |
| `relational` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `==`, `!=`, `&&`, \|\|, `)`, `,`, `;` } |
| `additive` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `<`, `<=`, `>`, `>=`, `==`, `!=`, `&&`, \|\|, `)`, `,`, `;` } |
| `term` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `+`, `-`, `<`, `<=`, `>`, `>=`, `==`, `!=`, `&&`, \|\|, `)`, `,`, `;` } |
| `unary` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `+`, `-`, `*`, `/`, `<`, `<=`, `>`, `>=`, `==`, `!=`, `&&`, \|\|, `)`, `,`, `;` } |
| `primary` | no | { `true`, `false`, IDENT, NUMBER, `(` } | { `+`, `-`, `*`, `/`, `<`, `<=`, `>`, `>=`, `==`, `!=`, `&&`, \|\|, `)`, `,`, `;` } |
| `args` | no | { `true`, `false`, IDENT, NUMBER, `-`, `!`, `(` } | { `)` } |

## Appendix D. Index of the sample programs

| File | What it shows |
|---|---|
| `examples/valid/factorial.txt` | functions, recursion, `if`, `while`, `print` |
| `examples/valid/booleans_for_shadowing.txt` | `for` with a declaration, boolean expressions, nested blocks |
| `examples/valid/read_input.txt` | `read` |
| `examples/valid/semantic_errors_still_parse.txt` | programs that are wrong **later** but valid **here** |
| `examples/valid/runtime_errors_still_parse.txt` | programs that fail only when executed |
| `examples/invalid/*.txt` | the four syntax errors of 7.5, with `.expected` positions |
| `tests/lexer/*.txt` | one file per lexical topic: keywords, numbers, maximal munch, comments, `Unknown`, positions, end of file |
| `tests/parser/valid/*.txt` | one file per construct of the grammar |
| `tests/parser/invalid/*.txt` | 61 syntax errors: statements, control flow, functions, expressions, blocks, integer literals out of range, lexical errors seen by the parser |
