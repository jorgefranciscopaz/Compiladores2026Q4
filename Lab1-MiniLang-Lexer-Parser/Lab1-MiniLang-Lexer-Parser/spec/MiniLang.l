// =============================================================================================
// MiniLang lexical specification for RE/flex                                  (Lab 1, Part A)
//
//
// One rule per line:      <regular expression>     // <name of the token it recognizes>
// The `//` comment after a pattern is the (empty) action, as in the class samples
// (LexerSample1.l, AstSample1/MiniLang.l). There is NO code in the actions: this file is only
// used to obtain the DFA; the lexer itself is the C++ you write by hand.
//
// What to specify (Lab1.md, section 6.1 and Appendix A):
//   - ignored input: whitespace and `// line comments`
//   - the 4 arithmetic operators, the 6 comparison operators, the 3 logical operators and `=`
//   - the 6 punctuation tokens
//   - identifiers (they also match every keyword: do NOT write rules for keywords)
//   - integer literals
// Anything else (a lone `&`, `@`, ...) is the job of the "no match" case of your C++ lexer.
//
// Generate the DFA with the commands of Lab1.md, step A2:
//   reflex --graphs-file=docs/MiniLang.gv --outfile=build/lex.yy.cpp spec/MiniLang.l
// =============================================================================================
%%
[a-zA-Z_][a-zA-Z0-9_]* //Ident
[0-9]+ //Number
[ \t\r\n]+ //whitespace
"//".* //LineComment
"+" //OpAdd
"-" //OpSub
"*" //OpMul
"/" //OpDiv
"<" //OpLess
"<=" //OpLessEqual
">" //OpGreater
">=" //OpGreaterEqual
"==" //OpEqual
"!=" //OpNotEqual
"&&" //OpAnd
"||" //OpOr
"!" //OpNot
"=" //Assign
"(" //LParen
")" //RParen
"{" //LBrace
"}" //RBrace
"," //Comma
";" //Semicolon
%%
