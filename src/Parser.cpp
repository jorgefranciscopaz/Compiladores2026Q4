#include "Parser.hpp"
#include <iostream>

Parser::Parser(Lexer &lex) : lexer(lex), token(TokenId::Unknown, "")
{
}

void Parser::parse()
{
    nextToken();
    program();
    expect(TokenId::EndOfFile, "Expected end of input but found " + describe(token));
}

// program → assign+
void Parser::program()
{
    assign();
    while (token.tokenId == TokenId::Ident)
    {
        assign();
    }
}

// assign → IDENT "=" expr ";"
void Parser::assign()
{
    Token target = token;
    expect(TokenId::Ident, "Expected identifier but found " + describe(token));
    expect(TokenId::OpEqual, "Expected '=' but found " + describe(token));
    ExprPtr value = expr();
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));

    std::cout << target.text << " =\n";
    printAST(*value, std::cout, 1);
}

// expr → term ("+" term)*
ExprPtr Parser::expr()
{
    ExprPtr left = term();
    while(token.tokenId == TokenId::OpAdd){
        SourceLoc opLoc = locOf(token);
        nextToken();
        ExprPtr right = term();
        left = std::make_unique<BinaryExpr>(BinaryOpKind::Add, std::move(left), std::move(right), opLoc);
    }
    return left;
}

// term → factor ("*" factor)*
ExprPtr Parser::term()
{
    ExprPtr left = factor();
    while(token.tokenId == TokenId::OpMul){
        SourceLoc opLoc = locOf(token);
        nextToken();
        ExprPtr right = factor();
        left = std::make_unique<BinaryExpr>(BinaryOpKind::Mul, std::move(left), std::move(right), opLoc);
    }
    return left;
}

// factor → IDENT | NUMBER | "(" expr ")"
ExprPtr Parser::factor()
{
    Token tok = token;
    switch (token.tokenId){
        case TokenId::Ident:
            nextToken();
            return std::make_unique<IdentExpr>(tok.text, locOf(tok));
        case TokenId::Number:
            nextToken();
            return std::make_unique<IntLiteral>(std::stoi(tok.text), locOf(tok));
        case TokenId::OpenParen:
            nextToken();
            ExprPtr inner = expr();
            expect(TokenId::CloseParen, "Expected ')' but found " + describe(token));
            return inner;
    }
    throw error("Expected identifier, number or '(' but found " + describe(token));
}

void Parser::nextToken()
{
    token = lexer.nextToken();
}

void Parser::expect(TokenId expected, const std::string &message)
{
    if (token.tokenId != expected)
    {
        throw error(message);
    }
    nextToken();
}

ParseError Parser::error(const std::string &message) const
{
    return ParseError(message, token.line, token.column);
}

std::string Parser::describe(const Token &tok)
{
    switch (tok.tokenId)
    {
    case TokenId::EndOfFile:
        return "end of input";
    case TokenId::Unknown:
        return "unknown character '" + tok.text + "'";
    default:
        return "'" + tok.text + "'";
    }
}
