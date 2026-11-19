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
// the non-terminal methods
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
    TraceScope trace(*this, "program");
    //Mientras sea uno de los iniciales de **ITEM**
    while(startsItem()){
        item();
    }
}

// item → function | statement
void Parser::item()
{
    TraceScope trace(*this, "item");
    if(token.tokenId == TokenId::KeywordFunc){
        function();
    }else if(startsStatement()){
        statement();
    }else{
        throw error("Expected a function or a statement but found " + describe(token));
    }
}

// function → "func" returnType IDENT "(" [ params ] ")" block
void Parser::function()
{
    TraceScope trace(*this, "function");
    expect(TokenId::KeywordFunc, "Expected 'func' but found " + describe(token));
    returnType();
    expect(TokenId::Ident, "Expected 'Identifier' but found " + describe(token));
    expect(TokenId::LParen, "Expected '(' but found " + describe(token));
    if(token.tokenId == TokenId::KeywordInt || token.tokenId == TokenId::KeywordBool){
        params();
    }
    expect(TokenId::RParen, "Expected ')' but found " + describe(token));
    block();
}

// params → param ( "," param )*
void Parser::params()
{
    TraceScope trace(*this, "params");
    param();
    while(token.tokenId == TokenId::Comma){
        expect(TokenId::Comma, "Expected ',' but found " + describe(token));
        param();
    }
}

// param → type IDENT
void Parser::param()
{
    TraceScope trace(*this, "param");
    type();
    expect(TokenId::Ident, "Expected 'Identifier' but found " + describe(token));
}

// type → "int" | "bool"
void Parser::type()
{
    TraceScope trace(*this, "type");
    if(token.tokenId == TokenId::KeywordInt){
        expect(TokenId::KeywordInt, "Expected 'int' but found " + describe(token));
    }else if(token.tokenId == TokenId::KeywordBool){
        expect(TokenId::KeywordBool, "Expected 'bool' but found " + describe(token));
    }else{
        throw error("Expected an int or bool but found " + describe(token));
    }
}

// returnType → type | "void"
void Parser::returnType()
{
    TraceScope trace(*this, "returnType");
    if(token.tokenId == TokenId::KeywordInt || token.tokenId == TokenId::KeywordBool){
        type();
    }else if(token.tokenId == TokenId::KeywordVoid){
        expect(TokenId::KeywordVoid, "Expected 'void' but found " + describe(token));
    }else{
        throw error("Expected int, bool or 'void' but found " + describe(token));
    }
}

// block → "{" statement* "}"
void Parser::block()
{
    TraceScope trace(*this, "block");
    expect(TokenId::LBrace, "Expected '{' but found " + describe(token));
    while(startsStatement()){
        statement();
    }
    expect(TokenId::RBrace, "Expected '}' but found " + describe(token));
}

// statement → varDecl | simpleStmt | ifStmt | whileStmt | forStmt
//           | returnStmt | printStmt | readStmt | block
void Parser::statement()
{
    TraceScope trace(*this, "statement");
    if(token.tokenId == TokenId::KeywordInt || token.tokenId == TokenId::KeywordBool){
        varDecl();
    }else if(token.tokenId == TokenId::Ident){
        simpleStmt();
    }else if(token.tokenId == TokenId::KeywordIf){
        ifStmt();
    }else if(token.tokenId == TokenId::KeywordWhile){
        whileStmt();
    }else if(token.tokenId == TokenId::KeywordFor){
        forStmt();
    }else if(token.tokenId == TokenId::KeywordReturn){
        returnStmt();
    }else if(token.tokenId == TokenId::KeywordPrint){
        printStmt();
    }else if(token.tokenId == TokenId::KeywordRead){
        readStmt();
    }else if(token.tokenId == TokenId::LBrace){
        block();
    }else{
        throw error("Expected int, bool, ident, 'if', 'while', 'for', 'return', 'print', 'read' or '{' but found " + describe(token));
    }
}

// varDecl → type IDENT [ "=" expr ] ";"
void Parser::varDecl()
{
    TraceScope trace(*this, "varDecl");
    type();
    expect(TokenId::Ident, "Expected 'Identifier' but found " + describe(token));
    if(token.tokenId == TokenId::Assign){
        expect(TokenId::Assign, "Expected '=' but found " + describe(token));
        expr();
    }
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
}

// simpleStmt → IDENT ( "=" expr | "(" [ args ] ")" ) ";"
void Parser::simpleStmt()
{
    TraceScope trace(*this, "simpleStmt");
    expect(TokenId::Ident, "Expected 'Identifier' but found " + describe(token));
    if(token.tokenId == TokenId::Assign){
        expect(TokenId::Assign, "Expected '=' but found " + describe(token));
        expr();
    }else if(token.tokenId == TokenId::LParen){
        expect(TokenId::LParen, "Expected '(' but found " + describe(token));
        if(startsExpr()){
            args();
        }
        expect(TokenId::RParen, "Expected ')' but found " + describe(token));
    }else{
        throw error("Expected '=', or '(' but found " + describe(token));
    }
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
}

// ifStmt → "if" "(" expr ")" block [ "else" ( ifStmt | block ) ]
void Parser::ifStmt()
{
    TraceScope trace(*this, "ifStmt");
    expect(TokenId::KeywordIf, "Expected 'if' but found " + describe(token));
    expect(TokenId::LParen, "Expected '(' but found " + describe(token));
    expr();
    expect(TokenId::RParen, "Expected ')' but found " + describe(token));
    block();
    if(token.tokenId == TokenId::KeywordElse){
        expect(TokenId::KeywordElse, "Expected 'else' but found " + describe(token));
        if(token.tokenId == TokenId::KeywordIf){
            ifStmt();
        }else if(token.tokenId == TokenId::LBrace){
            block();
        }else{
            throw error("Expected 'if' or '{' but found " + describe(token));
        }
    }
}

// whileStmt → "while" "(" expr ")" block
void Parser::whileStmt()
{
    TraceScope trace(*this, "whileStmt");
    expect(TokenId::KeywordWhile, "Expected 'while' but found " + describe(token));
    expect(TokenId::LParen, "Expected '(' but found " + describe(token));
    expr();
    expect(TokenId::RParen, "Expected ')' but found " + describe(token));
    block();
}

// forStmt → "for" "(" [ forInit ] ";" [ expr ] ";" [ forUpdate ] ")" block
void Parser::forStmt()
{
    TraceScope trace(*this, "forStmt");
    expect(TokenId::KeywordFor, "Expected 'for' but found " + describe(token));
    expect(TokenId::LParen, "Expected '(' but found " + describe(token));
    if(token.tokenId == TokenId::KeywordInt || token.tokenId == TokenId::KeywordBool || token.tokenId == TokenId::Ident){
        forInit();
    }
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
    if(startsExpr()){
        expr();
    }
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
    if(token.tokenId == TokenId::Ident){
        forUpdate();
    }
    expect(TokenId::RParen, "Expected ')' but found " + describe(token));
    block();
}

// forInit → type IDENT "=" expr | IDENT "=" expr
void Parser::forInit()
{
    TraceScope trace(*this, "forInit");
    if(token.tokenId == TokenId::KeywordInt || token.tokenId == TokenId::KeywordBool){
        type();
        expect(TokenId::Ident, "Expected 'identifier' but found " + describe(token));
        expect(TokenId::Assign, "Expected '=' but found " + describe(token));
        expr();
    }else if(token.tokenId == TokenId::Ident){
        expect(TokenId::Ident, "Expected an identifier but found " + describe(token));
        expect(TokenId::Assign, "Expected '=' but found " + describe(token));
        expr();
    }else{
        throw error("Expected type 'int', 'bool' or 'identifier' but found " + describe(token));
    }
}

// forUpdate → IDENT "=" expr
void Parser::forUpdate()
{
    TraceScope trace(*this, "forUpdate");
    expect(TokenId::Ident, "Expected 'identifier' but found " + describe(token));
    expect(TokenId::Assign, "Expected '=' but found " + describe(token));
    expr();
}

// returnStmt → "return" [ expr ] ";"
void Parser::returnStmt()
{
    TraceScope trace(*this, "returnStmt");
    expect(TokenId::KeywordReturn, "Expected 'return' but found " + describe(token));
    if(startsExpr()){
        expr();
    }
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
}

// printStmt → "print" expr ";"
void Parser::printStmt()
{
    TraceScope trace(*this, "printStmt");
    expect(TokenId::KeywordPrint, "Expected 'print' but found " + describe(token));
    expr();
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
}

// readStmt → "read" IDENT ";"
void Parser::readStmt()
{
    TraceScope trace(*this, "readStmt");
    expect(TokenId::KeywordRead, "Expected 'read' but found " + describe(token));
    expect(TokenId::Ident, "Expected an identifier but found " + describe(token));
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
}

// expr → orExpr
void Parser::expr()
{
    TraceScope trace(*this, "expr");
    orExpr();
}

// orExpr → andExpr ( "||" andExpr )*
void Parser::orExpr()
{
    TraceScope trace(*this, "orExpr");
    andExpr();
    while(token.tokenId == TokenId::OpOr){
        expect(TokenId::OpOr, "Expected '||' but found " + describe(token));
        andExpr();
    }
}

// andExpr → equality ( "&&" equality )*
void Parser::andExpr()
{
    TraceScope trace(*this, "andExpr");
    equality();
    while(token.tokenId == TokenId::OpAnd){
        expect(TokenId::OpAnd, "Expected '&&' but found " + describe(token));
        equality();
    }
}

// equality → relational ( ( "==" | "!=" ) relational )*
void Parser::equality()
{
    TraceScope trace(*this, "equality");
    relational();
    while(token.tokenId == TokenId::OpEqual || token.tokenId == TokenId::OpNotEqual){
        expect(token.tokenId, "Expected '==' or '!=' but found " + describe(token));
        relational();
    }
}

// relational → additive ( ( "<" | "<=" | ">" | ">=" ) additive )*
void Parser::relational()
{
    TraceScope trace(*this, "relational");
    additive();
    while(token.tokenId == TokenId::OpLess || token.tokenId == TokenId::OpLessEqual
        || token.tokenId == TokenId::OpGreater || token.tokenId == TokenId::OpGreaterEqual){
            expect(token.tokenId, "Expected '<', '<=', '>', or '>=' but found " + describe(token));
            additive();
    }
}

// additive → term ( ( "+" | "-" ) term )*
void Parser::additive()
{
    TraceScope trace(*this, "additive");
    term();
    while(token.tokenId == TokenId::OpAdd || token.tokenId == TokenId::OpSub){
        expect(token.tokenId, "Expected '+' or '-' but found " + describe(token));
        term();
    }
}

// term → unary ( ( "*" | "/" ) unary )*
void Parser::term()
{
    TraceScope trace(*this, "term");
    unary();
    while(token.tokenId == TokenId::OpMul || token.tokenId == TokenId::OpDiv){
        expect(token.tokenId, "Expected '*' or '/' but found " + describe(token));
        unary();
    }
}

// unary → ( "-" | "!" ) unary | primary
void Parser::unary()
{
    TraceScope trace(*this, "unary");
    if(token.tokenId == TokenId::OpSub || token.tokenId == TokenId::OpNot){
        expect(token.tokenId, "Expected '-' or '!' but found " + describe(token));
        unary();
    }else if(startsPrimary()){
        primary();
    }else{
        throw error("Expected '-', '!' or a primary but found " + describe(token));
    }
}

// primary → NUMBER | "true" | "false" | IDENT [ "(" [ args ] ")" ] | "(" expr ")"
//
// A NUMBER is also checked here: an integer literal whose value does not fit in a 32-bit signed
// int (more than 2147483647, leading zeros ignored) is a syntax error AT THAT LITERAL (Lab1.md,
// section 7.5, E6). The lexer does not check it: its DFA only knows [0-9]+. Report the error
// before consuming the token, and do not convert with a type that can overflow silently.
void Parser::primary()
{
    TraceScope trace(*this, "primary");
    if(token.tokenId == TokenId::Number){
        //Esto quita los ceros de la izquierda
        std::size_t i = 0;
        while(i + 1 < token.text.size() && token.text[i] == '0'){
            ++i;
        }
        std::string digits = token.text.substr(i);
        //Esto hace que no sea mayor que 2147483647
        if(digits.size() > 10 || (digits.size() == 10 && digits > "2147483647")){
            throw error("Integral literar " + token.text + " is larger than allowed (2147483647)");
        }
        expect(TokenId::Number, "Expected a number but found " + describe(token));
    }else if(token.tokenId == TokenId::KeywordTrue){
        expect(TokenId::KeywordTrue, "Expected 'true' but found " + describe(token));
    }else if(token.tokenId == TokenId::KeywordFalse){
        expect(TokenId::KeywordFalse, "Expected 'false' but found " + describe(token));
    }else if(token.tokenId == TokenId::Ident){
        expect(TokenId::Ident, "Expected an identifier but found " + describe(token));
        if(token.tokenId == TokenId::LParen){
            expect(TokenId::LParen, "Expected '(' but found " + describe(token));
            if(startsExpr()){
                args();
            }
            expect(TokenId::RParen, "Expected ')' but found " + describe(token));
        }
    }else if(token.tokenId == TokenId::LParen){
        expect(TokenId::LParen, "Expected '(' but found " + describe(token));
        expr();
        expect(TokenId::RParen, "Expected ')' but found " + describe(token));
    }else{
        throw error("Expected a Number, 'true', 'false', an identifier, or '(' but found " + describe(token));
    }
}

// args → expr ( "," expr )*
void Parser::args()
{
    TraceScope trace(*this, "args");
    expr();
    while(token.tokenId == TokenId::Comma){
        expect(TokenId::Comma, "Expected ',' but found " + describe(token));
        expr();
    }
}
