#pragma once
#include <istream>

enum class TokenId
{
    IDENT,
    NUMBER,
    PLUS,
    MULT,
    OPPAREN,
    CLOSPAREN,
    ASSIGN,
    SEMICOLON,
    ENDOFFILE,
    UNKNOWN
};

struct Token
{
    TokenId tokenId;
    std::string text;

    Token(TokenId tid, std::string txt) : tokenId(tid), text(txt) {}
};

class Lexer
{
private:
    std::istream &input;
    int ch;
    void nextChar()
    {
        ch = input.get();
    }

public:
    Lexer(std::istream &in) : input(in), ch(0)
    {
        nextChar();
    }

    Token nextToken();
    static const char *tokenToString(TokenId tokenId);
};