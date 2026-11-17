#pragma once

#include <istream>
#include <string>
#include <utility>

// One enumerator per token kind of MiniLang. The names are part of the lab contract:
// the expected files in tests/ use exactly these names (see Lab1.md, Appendix A).
enum class TokenId {
    // special tokens
    Unknown,            // input that is not part of any token (see Lab1.md, section 6.1)
    EndOfFile,

    // identifier and integer literal
    Ident,
    Number,

    // keywords
    KeywordFunc,
    KeywordInt,
    KeywordBool,
    KeywordVoid,
    KeywordIf,
    KeywordElse,
    KeywordWhile,
    KeywordFor,
    KeywordReturn,
    KeywordPrint,
    KeywordRead,
    KeywordTrue,
    KeywordFalse,

    // arithmetic operators
    OpAdd,              // +
    OpSub,              // -
    OpMul,              // *
    OpDiv,              // /

    // comparison operators
    OpLess,             // <
    OpLessEqual,        // <=
    OpGreater,          // >
    OpGreaterEqual,     // >=
    OpEqual,            // ==
    OpNotEqual,         // !=

    // logical operators
    OpAnd,              // &&
    OpOr,               // ||
    OpNot,              // !

    // assignment
    Assign,             // =

    // punctuation
    LParen,             // (
    RParen,             // )
    LBrace,             // {
    RBrace,             // }
    Comma,              // ,
    Semicolon,          // ;
};

// A token: its kind, the exact characters it was made of (its lexeme) and the position
// (1-based line and column) of its FIRST character in the input.
struct Token {
    TokenId tokenId;
    std::string text;
    int line;
    int column;

    Token(TokenId tid, std::string txt, int ln = 0, int col = 0)
        : tokenId(tid), text(std::move(txt)), line(ln), column(col)
    {}
};

// Hand-written scanner. Its states are the states of the DFA that RE/flex builds from
// spec/MiniLang.l (docs/MiniLang.gv), see Lab1.md, Part A.
class Lexer {
public:
    explicit Lexer(std::istream& in) : input(in) { nextChar(); }

    // Returns the next token. Whitespace and comments are skipped. At the end of the input it
    // returns TokenId::EndOfFile, and keeps returning it if called again.
    // It never throws: input that is not a token is reported as TokenId::Unknown.
    Token nextToken();

    // Name of a token kind, identical to the enumerator ("KeywordIf", "OpAdd", ...).
    static const char *tokenToString(TokenId tid);

private:
    // Makes `ch` the next character of the input (or EOF) and keeps `line` / `column`
    // up to date. `ch` is the character AFTER the ones already consumed: it is at
    // position (line, column).
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
