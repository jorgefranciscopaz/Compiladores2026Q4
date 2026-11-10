#pragma once

#include <stdexcept>
#include <string>
#include "Lexer.hpp"

// Syntax error with the position of the offending token.
class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg, int ln, int col)
        : std::runtime_error(msg), line(ln), column(col)
    {}

    int line;
    int column;
};

// Recursive descent parser for the grammar (EBNF):
//
//   program → assign+
//   assign → IDENT "=" expr ";"
//   expr   → term ("+" term)*
//   term   → factor ("*" factor)*
//   factor → IDENT | NUMBER | "(" expr ")"
//
// Each non-terminal is implemented by one method. `token` holds the
// current lookahead token (LL(1)).
class Parser {
public:
    Parser(Lexer& lexer);

    // Parses the whole input. Throws ParseError if the input is invalid.
    void parse();

private:
    void program();
    void assign();
    void expr();
    void term();
    void factor();

    void nextToken();
    void expect(TokenId expected, const std::string& message);
    ParseError error(const std::string& message) const;
    static std::string describe(const Token& tok);

private:
    Lexer& lexer;
    Token token;
};
