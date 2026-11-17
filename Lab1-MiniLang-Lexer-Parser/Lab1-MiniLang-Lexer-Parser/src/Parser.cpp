#include "Parser.hpp"

#include <iostream>

#include "NotImplemented.hpp"

// =============================================================================
// Provided helpers: do not modify.
// =============================================================================

Parser::Parser(Lexer& lex)
    : lexer(lex), token(TokenId::Unknown, "")
{
}

void Parser::parse()
{
    nextToken();
    program();
    expect(TokenId::EndOfFile, "Expected end of input but found " + describe(token));
}

void Parser::nextToken()
{
    token = lexer.nextToken();
}

// Consumes the current token if it is `expected`; otherwise throws a ParseError with `message`.
// After a successful expect() the lookahead has ALREADY advanced: do not call nextToken().
void Parser::expect(TokenId expected, const std::string& message)
{
    if (token.tokenId != expected) {
        throw error(message);
    }
    traceLine(std::string("expect(") + Lexer::tokenToString(expected) + ")");
    nextToken();
}

ParseError Parser::error(const std::string& message) const
{
    return ParseError(message, token.line, token.column);
}

std::string Parser::describe(const Token& tok)
{
    switch (tok.tokenId) {
    case TokenId::EndOfFile: return "end of input";
    case TokenId::Unknown:   return "unknown character '" + tok.text + "'";
    default:                 return "'" + tok.text + "'";
    }
}

void Parser::traceLine(const std::string& text) const
{
    if (traceOn) {
        std::cout << std::string(static_cast<std::size_t>(depth) * 2, ' ') << text << '\n';
    }
}

// =============================================================================
// TODO(B3): the non-terminal methods
//
// One method per non-terminal, written from the grammar in Parser.hpp:
//   * sequence       -> call / expect() each symbol in order
//   * choice         -> `if` / `else if` / `switch` on the lookahead (`token.tokenId`);
//                       the final `else` reports a syntax error
//   * [ optional ]   -> `if (token is in FIRST of the optional part) { ... }`
//   * ( repeated )*  -> `while (token is in FIRST of the repeated part) { ... }`
//   * a terminal     -> expect(TokenId::X, "Expected ... but found " + describe(token))
// Never backtrack and never look beyond `token`. Do not call nextToken() after expect().
// Report errors with `throw error("...")` at the token that cannot continue the program.
//
// Replace each `throw NotImplementedError(...)` by the implementation. You may add private
// helper functions (declare them in Parser.hpp), for example "does the current token start an
// expression?".
// =============================================================================

// program → item* EOF            (EOF is checked by parse(); write only the item* loop)
void Parser::program()
{
    throw NotImplementedError("Parser::program");
}

// item → function | statement
void Parser::item()
{
    throw NotImplementedError("Parser::item");
}

// function → "func" returnType IDENT "(" [ params ] ")" block
void Parser::function()
{
    throw NotImplementedError("Parser::function");
}

// params → param ( "," param )*
void Parser::params()
{
    throw NotImplementedError("Parser::params");
}

// param → type IDENT
void Parser::param()
{
    throw NotImplementedError("Parser::param");
}

// type → "int" | "bool"
void Parser::type()
{
    throw NotImplementedError("Parser::type");
}

// returnType → type | "void"
void Parser::returnType()
{
    throw NotImplementedError("Parser::returnType");
}

// block → "{" statement* "}"
void Parser::block()
{
    throw NotImplementedError("Parser::block");
}

// statement → varDecl | simpleStmt | ifStmt | whileStmt | forStmt
//           | returnStmt | printStmt | readStmt | block
void Parser::statement()
{
    throw NotImplementedError("Parser::statement");
}

// varDecl → type IDENT [ "=" expr ] ";"
void Parser::varDecl()
{
    throw NotImplementedError("Parser::varDecl");
}

// simpleStmt → IDENT ( "=" expr | "(" [ args ] ")" ) ";"
void Parser::simpleStmt()
{
    throw NotImplementedError("Parser::simpleStmt");
}

// ifStmt → "if" "(" expr ")" block [ "else" ( ifStmt | block ) ]
void Parser::ifStmt()
{
    throw NotImplementedError("Parser::ifStmt");
}

// whileStmt → "while" "(" expr ")" block
void Parser::whileStmt()
{
    throw NotImplementedError("Parser::whileStmt");
}

// forStmt → "for" "(" [ forInit ] ";" [ expr ] ";" [ forUpdate ] ")" block
void Parser::forStmt()
{
    throw NotImplementedError("Parser::forStmt");
}

// forInit → type IDENT "=" expr | IDENT "=" expr
void Parser::forInit()
{
    throw NotImplementedError("Parser::forInit");
}

// forUpdate → IDENT "=" expr
void Parser::forUpdate()
{
    throw NotImplementedError("Parser::forUpdate");
}

// returnStmt → "return" [ expr ] ";"
void Parser::returnStmt()
{
    throw NotImplementedError("Parser::returnStmt");
}

// printStmt → "print" expr ";"
void Parser::printStmt()
{
    throw NotImplementedError("Parser::printStmt");
}

// readStmt → "read" IDENT ";"
void Parser::readStmt()
{
    throw NotImplementedError("Parser::readStmt");
}

// expr → orExpr
void Parser::expr()
{
    throw NotImplementedError("Parser::expr");
}

// orExpr → andExpr ( "||" andExpr )*
void Parser::orExpr()
{
    throw NotImplementedError("Parser::orExpr");
}

// andExpr → equality ( "&&" equality )*
void Parser::andExpr()
{
    throw NotImplementedError("Parser::andExpr");
}

// equality → relational ( ( "==" | "!=" ) relational )*
void Parser::equality()
{
    throw NotImplementedError("Parser::equality");
}

// relational → additive ( ( "<" | "<=" | ">" | ">=" ) additive )*
void Parser::relational()
{
    throw NotImplementedError("Parser::relational");
}

// additive → term ( ( "+" | "-" ) term )*
void Parser::additive()
{
    throw NotImplementedError("Parser::additive");
}

// term → unary ( ( "*" | "/" ) unary )*
void Parser::term()
{
    throw NotImplementedError("Parser::term");
}

// unary → ( "-" | "!" ) unary | primary
void Parser::unary()
{
    throw NotImplementedError("Parser::unary");
}

// primary → NUMBER | "true" | "false" | IDENT [ "(" [ args ] ")" ] | "(" expr ")"
//
// A NUMBER is also checked here: an integer literal whose value does not fit in a 32-bit signed
// int (more than 2147483647, leading zeros ignored) is a syntax error AT THAT LITERAL (Lab1.md,
// section 7.5, E6). The lexer does not check it: its DFA only knows [0-9]+. Report the error
// before consuming the token, and do not convert with a type that can overflow silently.
void Parser::primary()
{
    throw NotImplementedError("Parser::primary");
}

// args → expr ( "," expr )*
void Parser::args()
{
    throw NotImplementedError("Parser::args");
}
