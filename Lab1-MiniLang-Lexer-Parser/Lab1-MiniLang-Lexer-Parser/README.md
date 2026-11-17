# Lab 1: MiniLang Lexer and Recursive-Descent Parser

Ungraded preparatory lab, Compiladores I (CCC501). **Read `Lab1.md` first**: it is the full handout (objectives, scope, step-by-step instructions, deliverables, how you will be evaluated, pitfalls). This page is only the quick start.

## What you build

| Part | You write | Starting from |
|---|---|---|
| A. Lexer | `spec/MiniLang.l` (RE/flex), `src/Lexer.cpp` (C++20, one `case` per DFA state), `docs/TRACEABILITY.md` | RE/flex DFA → Graphviz SVG |
| B. Parser | `src/Parser.cpp` (recursive descent, LL(1)), `docs/LL1-ANALYSIS.md` | the EBNF grammar in `Lab1.md` §7.2 |
| Both | your own tests in `tests/student/` | the public tests in `tests/` |

Everything else (interfaces, driver, build, tests, templates) is provided. Look for `TODO`.

## Quick start

```
cmake -S . -B build                          # configure (once)
cmake --build build                          # build (after every change)
ctest --test-dir build --output-on-failure   # all tests fail at first, on purpose
```

Part A, the DFA (details in `Lab1.md` §6.3):

```
mkdir -p build
reflex --graphs-file=docs/MiniLang.gv --outfile=build/lex.yy.cpp spec/MiniLang.l
dot -Tsvg docs/MiniLang.gv -o docs/MiniLang.svg
cmake -DIN=docs/MiniLang.gv -DOUT=docs/MiniLang.numbered.gv -P tools/NumberDfa.cmake
dot -Tsvg docs/MiniLang.numbered.gv -o docs/MiniLang.numbered.svg
```

> Needs **RE/flex** (<https://github.com/Genivia/RE-flex>) and **Graphviz**. Careful: `apt install reflex` installs a *different* program. `reflex --help` must list `--graphs-file`.

Run the program:

```
build/minilang --tokens examples/valid/factorial.txt     # the token stream
build/minilang examples/valid/factorial.txt              # syntax check: "OK" or FILE:LINE:COL: error: ...
build/minilang --trace examples/valid/factorial.txt      # + the parser call trace (optional aid, step B5)
```

Run part of the tests: `ctest --test-dir build -L lexer` (also `parser`, `examples`, `student`, `docs`) or `-R name`.

## Language

MiniLang is defined in `docs/MiniLang-Reference.md` (lexical structure, grammar, semantics, what it does **not** have, and the lab's conventions). Lab 1 checks **syntax only**: `int x = true;` is a valid program here.

## Before you stop

`ctest` reports 100% passed · no `TODO(` left in `spec/` and `src/` · no `<TODO>` left in `docs/` · `build/` deleted. The full checklist is in `Lab1.md` §8.
