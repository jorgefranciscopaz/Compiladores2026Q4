# Your own tests

Put here the tests you write (Lab 1, steps A7 and B6). They are registered automatically the next time you run `cmake -S . -B build`, with the names `student.lexer.NAME`, `student.valid.NAME` and `student.invalid.NAME`.

Minimum required: **5** lexer tests, **3** valid programs and **5** invalid programs (`ctest -R student.minimum_tests`).

The expected results must come **from the language definition**, not from what your program prints: a test that only repeats your program's behavior cannot find a bug in it.

| Folder | Files | Format |
|---|---|---|
| `lexer/` | `NAME.txt`, `NAME.tokens` | `.tokens`: one line `LINE:COL TokenId 'text'` per token, last line `LINE:COL EndOfFile` |
| `parser/valid/` | `NAME.txt` | a program that `build/minilang` must accept |
| `parser/invalid/` | `NAME.txt`, `NAME.expected` | `.expected`: only `LINE:COL`, the position of the first token that cannot continue a valid program |

See `Lab1.md`, Appendix B, and the public tests in `tests/lexer/` and `tests/parser/` for examples.
