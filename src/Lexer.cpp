#pragma once

#include "Lexer.hpp"
#include <iostream>

Token Lexer::nextToken()
{
    std::string tokenText;
    int state = 0;

    do
    {
        switch (state)
        {
        case 0:
            if (ch == EOF)
            {
                return Token(TokenId::ENDOFFILE, "");
            }
            else if (ch == '+')
            {
                nextChar();
                return Token(TokenId::PLUS, "+");
            }
            else if (ch == '*')
            {
                nextChar();
                return Token(TokenId::MULT, "*");
            }
            else if (ch == '(')
            {
                nextChar();
                return Token(TokenId::OPPAREN, "(");
            }
            else if (ch == ')')
            {
                nextChar();
                return Token(TokenId::CLOSPAREN, ")");
            }
            else if (ch == '=')
            {
                nextChar();
                return Token(TokenId::ASSIGN, "=");
            }
            else if (ch == ';')
            {
                nextChar();
                return Token(TokenId::SEMICOLON, ";");
            }
            else if (std::isspace(ch))
            {
                nextChar();
            }
            else if (std::isalpha(ch))
            {
                tokenText += ch;
                nextChar();
                state = 1;
            }
            else if (std::isdigit(ch))
            {
                tokenText += ch;
                nextChar();
                state = 2;
            }
            else
            {
                tokenText += ch;
                nextChar();
                return Token(TokenId::UNKNOWN, tokenText);
            }
            break;

        case 1:
            if (std::isalnum(ch))
            {
                tokenText += ch;
                nextChar();
            }
            else
            {
                return Token(TokenId::IDENT, tokenText);
            }
            break;

        case 2:
            if (std::isdigit(ch))
            {
                tokenText += ch;
                nextChar();
            }
            else
            {
                return Token(TokenId::NUMBER, tokenText);
            }
            break;
        }
    } while (true);
}

const char *Lexer::tokenToString(TokenId tokenId)
{
    switch (tokenId)
    {
    case TokenId::IDENT:
        return "IDENT";
    case TokenId::NUMBER:
        return "NUMBER";
    case TokenId::PLUS:
        return "PLUS";
    case TokenId::MULT:
        return "MULT";
    case TokenId::OPPAREN:
        return "OPPAREN";
    case TokenId::CLOSPAREN:
        return "CLOSPAREN";
    case TokenId::ASSIGN:
        return "ASSIGN";
    case TokenId::SEMICOLON:
        return "SEMICOLON";
    case TokenId::ENDOFFILE:
        return "ENDOFFILE";
    case TokenId::UNKNOWN:
        return "UNKNOWN";
    }
    return "UNKNOWN";
}