#pragma once

#include <stdexcept>
#include <string>

#include "Lexer.hpp"

// Syntax error with the position of the offending token (1-based line and column).
class ParseError : public std::runtime_error
{
public:
    ParseError(const std::string &msg, int ln, int col)
        : std::runtime_error(msg), line(ln), column(col)
    {
    }

    int line;
    int column;
};

// Recursive descent parser for MiniLang (LL(1), one token of lookahead).
// It only CHECKS the syntax: it builds no tree and performs no semantic checks. The one check
// that looks at a VALUE is the range of integer literals (see primary()).
//
// Grammar (EBNF; the same one as in Lab1.md, section 7.2). Terminals in quotes are the
// tokens of Appendix A, IDENT is TokenId::Ident, NUMBER is TokenId::Number.
//
//   program     → item* EOF
//   item        → function | statement
//
//   function    → "func" returnType IDENT "(" [ params ] ")" block
//   params      → param ( "," param )*
//   param       → type IDENT
//   type        → "int" | "bool"
//   returnType  → type | "void"
//
//   block       → "{" statement* "}"
//
//   statement   → varDecl | simpleStmt | ifStmt | whileStmt | forStmt
//               | returnStmt | printStmt | readStmt | block
//
//   varDecl     → type IDENT [ "=" expr ] ";"
//   simpleStmt  → IDENT ( "=" expr | "(" [ args ] ")" ) ";"
//   ifStmt      → "if" "(" expr ")" block [ "else" ( ifStmt | block ) ]
//   whileStmt   → "while" "(" expr ")" block
//   forStmt     → "for" "(" [ forInit ] ";" [ expr ] ";" [ forUpdate ] ")" block
//   forInit     → type IDENT "=" expr | IDENT "=" expr
//   forUpdate   → IDENT "=" expr
//   returnStmt  → "return" [ expr ] ";"
//   printStmt   → "print" expr ";"
//   readStmt    → "read" IDENT ";"
//
//   expr        → orExpr
//   orExpr      → andExpr ( "||" andExpr )*
//   andExpr     → equality ( "&&" equality )*
//   equality    → relational ( ( "==" | "!=" ) relational )*
//   relational  → additive ( ( "<" | "<=" | ">" | ">=" ) additive )*
//   additive    → term ( ( "+" | "-" ) term )*
//   term        → unary ( ( "*" | "/" ) unary )*
//   unary       → ( "-" | "!" ) unary | primary
//   primary     → NUMBER | "true" | "false"
//               | IDENT [ "(" [ args ] ")" ]
//               | "(" expr ")"
//   args        → expr ( "," expr )*
//
// One method per non-terminal, named after it. `token` always holds the lookahead:
// the first token that has not been consumed yet.
class Parser
{
public:
    explicit Parser(Lexer &lexer);

    // Parses the whole input. Throws ParseError at the first syntax error.
    void parse();

    // Enables the optional call trace (step B5): see TraceScope below.
    void setTrace(bool on) { traceOn = on; }

private:
    // ---- one method per non-terminal  ----
    void program();
    void item();

    void function();
    void params();
    void param();
    void type();
    void returnType();

    void block();

    void statement();
    void varDecl();
    void simpleStmt();
    void ifStmt();
    void whileStmt();
    void forStmt();
    void forInit();
    void forUpdate();
    void returnStmt();
    void printStmt();
    void readStmt();

    void expr();
    void orExpr();
    void andExpr();
    void equality();
    void relational();
    void additive();
    void term();
    void unary();
    void primary();
    void args();

    // ---- provided helpers (same as ParserSample1) ----
    void nextToken();                                          // token = next token of the lexer
    void expect(TokenId expected, const std::string &message); // consume `expected` or throw
    ParseError error(const std::string &message) const;        // error at the current token
    static std::string describe(const Token &tok);             // "'x'", "end of input", ...

    // FUNCION AUXILIAR QUE DETECTA SI ES DE LOS INICIALES DE **ITEM**
    bool startsStatement() const
    {
        switch (token.tokenId)
        {
        case TokenId::KeywordInt:
        case TokenId::KeywordBool:
        case TokenId::KeywordIf:
        case TokenId::KeywordWhile:
        case TokenId::KeywordFor:
        case TokenId::KeywordReturn:
        case TokenId::KeywordPrint:
        case TokenId::KeywordRead:
        case TokenId::Ident:
        case TokenId::LBrace:
            return true;
        default:
            return false;
        }
    }
    
    bool startsItem() const
    {
        return token.tokenId == TokenId::KeywordFunc || startsStatement();
    }

    bool startsExpr() const{
        switch (token.tokenId) {
        case TokenId::Number:
        case TokenId::KeywordTrue:
        case TokenId::KeywordFalse:
        case TokenId::Ident:
        case TokenId::OpSub:
        case TokenId::OpNot:
        case TokenId::LParen:
            return true;
        default:
            return false;
        }
    }

    bool startsPrimary() const{
        switch(token.tokenId){
            case TokenId::Number:
            case TokenId::KeywordTrue:
            case TokenId::KeywordFalse:
            case TokenId::Ident:
            case TokenId::LParen:
                return true;
            default:
                return false;
        }
    }

    // ---- optional call trace (step B5) ----
    // Put `TraceScope trace(*this, "expr");` as the FIRST line of a non-terminal method and run
    // `minilang --trace file.txt`: every method entry is printed, indented by its nesting depth,
    // and so is every successful expect(). Comparing it with your hand-made trace is a good
    // way to check that you understand the call structure. It has no effect otherwise.
    class TraceScope
    {
    public:
        TraceScope(Parser &p, const char *name) : parser(p)
        {
            parser.traceLine(std::string(name) + "()");
            ++parser.depth;
        }
        ~TraceScope() { --parser.depth; }
        TraceScope(const TraceScope &) = delete;
        TraceScope &operator=(const TraceScope &) = delete;

    private:
        Parser &parser;
    };

    void traceLine(const std::string &text) const;

private:
    Lexer &lexer;
    Token token; // the lookahead token (the ONLY lookahead: LL(1))
    bool traceOn = false;
    int depth = 0;
};
