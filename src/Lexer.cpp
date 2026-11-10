#include "Lexer.hpp"

Token Lexer::nextToken()
{
    std::string text;
    int state = 0;
    int startLine = line;
    int startColumn = column;

    do {
        switch (state)
        {
        case 0:
            startLine = line;
            startColumn = column;
            text.clear();

            if (ch == '\t' || ch == '\n' || ch == '\r' || ch == '\x20') {
                nextChar();
                state = 1;
            } else if (ch == '(') {
                text += static_cast<char>(ch);
                nextChar();
                state = 6;
            } else if (ch == ')') {
                text += static_cast<char>(ch);
                nextChar();
                state = 7;
            } else if (ch == '*') {
                text += static_cast<char>(ch);
                nextChar();
                state = 5;
            } else if (ch == '+') {
                text += static_cast<char>(ch);
                nextChar();
                state = 4;
            } else if (ch == ';') {
                text += static_cast<char>(ch);
                nextChar();
                state = 8;
            } else if (ch == '/') {
                text += static_cast<char>(ch);
                nextChar();
                state = 11;
            } else if (ch >= '0' && ch <= '9') {
                text += static_cast<char>(ch);
                nextChar();
                state = 10;
            } else if (ch == '=') {
                text += static_cast<char>(ch);
                nextChar();
                state = 3;
            } else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_') {
                text += static_cast<char>(ch);
                nextChar();
                state = 9;
            } else if (ch == EOF) {
                return {TokenId::EndOfFile, "<<EOF>>", startLine, startColumn};
            } else {
                text += static_cast<char>(ch);
                nextChar();
                return {TokenId::Unknown, text, startLine, startColumn};
            }
            break;
        case 1:
            state = 0;
            break;
        case 2:
            if (ch != '\n' && ch != EOF) {
                nextChar();
            } else {
                state = 0;
            }
            break;
        case 3:
            return {TokenId::OpEqual, text, startLine, startColumn};
        case 4:
            return {TokenId::OpAdd, text, startLine, startColumn};
        case 5:
            return {TokenId::OpMul, text, startLine, startColumn};
        case 6:
            return {TokenId::OpenParen, text, startLine, startColumn};
        case 7:
            return {TokenId::CloseParen, text, startLine, startColumn};
        case 8:
            return {TokenId::Semicolon, text, startLine, startColumn};
        case 9:
            if ((ch >= 'A' && ch <= 'Z')
                || (ch >= 'a' && ch <= 'z')
                || (ch >= '0' && ch <= '9')
                || ch == '_') {
                text += static_cast<char>(ch);
                nextChar();
                state = 9;
            } else {
                return {TokenId::Ident, text, startLine, startColumn};
            }
            break;
        case 10:
            if (ch >= '0' && ch <= '9') {
                text += static_cast<char>(ch);
                nextChar();
                state = 10;
            } else {
                return {TokenId::Number, text, startLine, startColumn};
            }
            break;
        case 11:
            if (ch == '/') {
                nextChar();
                state = 2;
            } else {
                return {TokenId::Unknown, text, startLine, startColumn};
            }
            break;
        default:
            nextChar();
            return {TokenId::Unknown, text, startLine, startColumn};
        }

    } while (true);
}

const char *Lexer::tokenToString(TokenId tid)
{
    switch (tid)
    {
        case TokenId::Unknown: return "Unknown";
        case TokenId::Ident: return "Ident";
        case TokenId::Number: return "Number";
        case TokenId::OpEqual: return "OpEqual";
        case TokenId::OpAdd: return "OpAdd";
        case TokenId::OpMul: return "OpMul";
        case TokenId::OpenParen: return "OpenParen";
        case TokenId::CloseParen: return "CloseParen";
        case TokenId::Semicolon: return "Semicolon";
        case TokenId::EndOfFile: return "EndOfFile";
        default:
            return "NotDefined";
    }
}
