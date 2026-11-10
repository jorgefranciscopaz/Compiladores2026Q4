#include "Parser.hpp"

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
    expect(TokenId::Ident, "Expected identifier but found " + describe(token));
    expect(TokenId::OpEqual, "Expected '=' but found " + describe(token));
    expr();
    expect(TokenId::Semicolon, "Expected ';' but found " + describe(token));
}

// expr → term ("+" term)*
void Parser::expr()
{
    term();
    while (token.tokenId == TokenId::OpAdd)
    {
        nextToken();
        term();
    }
}

// term → factor ("*" factor)*
void Parser::term()
{
    factor();
    while (token.tokenId == TokenId::OpMul)
    {
        nextToken();
        factor();
    }
}

// factor → IDENT | NUMBER | "(" expr ")"
void Parser::factor()
{
    if (token.tokenId == TokenId::Ident)
    {
        nextToken();
    }
    else if (token.tokenId == TokenId::Number)
    {
        nextToken();
    }
    else if (token.tokenId == TokenId::OpenParen)
    {
        nextToken();
        expr();
        expect(TokenId::CloseParen, "Expected ')' but found " + describe(token));
    }
    else
    {
        throw error("Expected identifier, number or '(' but found " + describe(token));
    }
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