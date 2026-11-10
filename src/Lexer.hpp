#pragma once

#include <istream>
#include <string>

enum class TokenId {
    Unknown,
    Ident,
    Number,
    OpEqual,
    OpAdd,
    OpMul,
    OpenParen,
    CloseParen,
    Semicolon,
    EndOfFile,
};

struct Token {
    TokenId tokenId;
    std::string text;
    int line;
    int column;

    Token(TokenId tid, std::string txt, int ln = 0, int col = 0)
        : tokenId(tid), text(std::move(txt)), line(ln), column(col)
    {}
};

class Lexer {
public:
    Lexer(std::istream& in): input(in) { nextChar(); }

    Token nextToken();
    static const char *tokenToString(TokenId tid);

private:
    // Reads the next character and keeps track of its line/column.
    void nextChar()
    {
        if (ch == '\n') {
            ++line;
            column = 0;
        }
        ch = input.get();
        ++column;
    }

private:
    std::istream& input;
    int ch = '\0';
    int line = 1;
    int column = 0;
};
