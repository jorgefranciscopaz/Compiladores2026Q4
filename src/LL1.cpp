// LL1.cpp  ·  Parser LL(1) por tabla M[A, a]
//
// Adaptado para usar el Lexer del proyecto (Lexer.hpp: Token, TokenId,
// Lexer::nextToken) y ParseError (Parser.hpp). La gramática y la numeración
// de reglas son las de la presentación (ver LL1.hpp).

#include "LL1.hpp"

#include <algorithm>
#include <initializer_list>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace {

// ----------------------------------------------------------------------------
// Tabla M[A, a]:  (no terminal, token) -> número de regla
// ----------------------------------------------------------------------------
using Table = std::unordered_map<GrammarKey, int, KeyHasher>;

// ----------------------------------------------------------------------------
// Símbolos y reglas
// ----------------------------------------------------------------------------
// Un símbolo de la pila es un terminal (TokenId) o un no terminal.
struct Symbol {
    bool isTerminal;
    TokenId tok;          // válido si isTerminal
    NonTerminals nt;      // válido si !isTerminal
};

Symbol T(TokenId t)      { return {true, t, NonTerminals::PROGRAM}; }
Symbol N(NonTerminals n) { return {false, TokenId::EndOfFile, n}; }

struct Rule {
    NonTerminals lhs;
    std::vector<Symbol> rhs;   // vacío = ε
};

// RULES[n - 1] es la regla número n.
const std::vector<Rule> RULES = {
    /*  1 */ {NonTerminals::PROGRAM,   {N(NonTerminals::ASSIGN), N(NonTerminals::PROGRAM_P)}},
    /*  2 */ {NonTerminals::PROGRAM_P, {N(NonTerminals::ASSIGN), N(NonTerminals::PROGRAM_P)}},
    /*  3 */ {NonTerminals::PROGRAM_P, {}},
    /*  4 */ {NonTerminals::ASSIGN,    {T(TokenId::Ident), T(TokenId::OpEqual),
                                        N(NonTerminals::EXPR), T(TokenId::Semicolon)}},
    /*  5 */ {NonTerminals::EXPR,      {N(NonTerminals::TERM), N(NonTerminals::EXPR_P)}},
    /*  6 */ {NonTerminals::EXPR_P,    {T(TokenId::OpAdd), N(NonTerminals::TERM), N(NonTerminals::EXPR_P)}},
    /*  7 */ {NonTerminals::EXPR_P,    {}},
    /*  8 */ {NonTerminals::TERM,      {N(NonTerminals::FACTOR), N(NonTerminals::TERM_P)}},
    /*  9 */ {NonTerminals::TERM_P,    {T(TokenId::OpMul), N(NonTerminals::FACTOR), N(NonTerminals::TERM_P)}},
    /* 10 */ {NonTerminals::TERM_P,    {}},
    /* 11 */ {NonTerminals::FACTOR,    {T(TokenId::Ident)}},
    /* 12 */ {NonTerminals::FACTOR,    {T(TokenId::Number)}},
    /* 13 */ {NonTerminals::FACTOR,    {T(TokenId::OpenParen), N(NonTerminals::EXPR),
                                        T(TokenId::CloseParen)}},
};

// ----------------------------------------------------------------------------
// Nombres para imprimir (símbolos gramaticales, no los nombres internos
// de Lexer::tokenToString)
// ----------------------------------------------------------------------------
std::string tokenName(TokenId t)
{
    switch (t) {
        case TokenId::EndOfFile:  return "$";
        case TokenId::OpenParen:  return "(";
        case TokenId::CloseParen: return ")";
        case TokenId::Semicolon:  return ";";
        case TokenId::OpEqual:    return "=";
        case TokenId::OpAdd:      return "+";
        case TokenId::OpMul:      return "*";
        case TokenId::Ident:      return "IDENT";
        case TokenId::Number:     return "NUMBER";
        default:                  return Lexer::tokenToString(t);
    }
}

std::string ntName(NonTerminals n)
{
    switch (n) {
        case NonTerminals::PROGRAM:   return "program";
        case NonTerminals::PROGRAM_P: return "program'";
        case NonTerminals::ASSIGN:    return "assign";
        case NonTerminals::EXPR:      return "expr";
        case NonTerminals::EXPR_P:    return "expr'";
        case NonTerminals::TERM:      return "term";
        case NonTerminals::TERM_P:    return "term'";
        case NonTerminals::FACTOR:    return "factor";
    }
    return "?";
}

std::string symName(const Symbol& s) { return s.isTerminal ? tokenName(s.tok) : ntName(s.nt); }

std::string ruleText(int n)
{
    const Rule& r = RULES[n - 1];
    std::string s = std::to_string(n) + ": " + ntName(r.lhs) + " →";
    if (r.rhs.empty()) return s + " ε";
    for (const Symbol& x : r.rhs) s += " " + symName(x);
    return s;
}

// Descripción del token recibido, con el mismo estilo que Parser::describe.
std::string describe(const Token& tok)
{
    switch (tok.tokenId) {
        case TokenId::EndOfFile: return "end of input";
        case TokenId::Unknown:   return "unknown character '" + tok.text + "'";
        default:                 return "'" + tok.text + "'";
    }
}

// ----------------------------------------------------------------------------
// Construcción de la tabla
//   - regla sin ε  : columnas = FIRST(lado derecho)
//   - regla con ε  : columnas = FOLLOW(lado izquierdo)
// ----------------------------------------------------------------------------
void put(Table& table, int rule, NonTerminals nt, std::initializer_list<TokenId> cols)
{
    for (TokenId a : cols) {
        auto result = table.emplace(GrammarKey{nt, a}, rule);
        if (!result.second) {   // la celda ya tenía otra regla: no es LL(1)
            throw std::logic_error("Conflicto en M[" + ntName(nt) + ", " + tokenName(a) +
                                   "]: reglas " + std::to_string(result.first->second) +
                                   " y " + std::to_string(rule));
        }
    }
}

Table buildTable()
{
    using NT = NonTerminals;
    using TK = TokenId;
    Table t;

    put(t,  1, NT::PROGRAM,   {TK::Ident});
    put(t,  2, NT::PROGRAM_P, {TK::Ident});
    put(t,  3, NT::PROGRAM_P, {TK::EndOfFile});                         // FOLLOW(program')

    put(t,  4, NT::ASSIGN,    {TK::Ident});

    put(t,  5, NT::EXPR,      {TK::Ident, TK::Number, TK::OpenParen});
    put(t,  6, NT::EXPR_P,    {TK::OpAdd});
    put(t,  7, NT::EXPR_P,    {TK::Semicolon, TK::CloseParen});         // FOLLOW(expr')

    put(t,  8, NT::TERM,      {TK::Ident, TK::Number, TK::OpenParen});
    put(t,  9, NT::TERM_P,    {TK::OpMul});
    put(t, 10, NT::TERM_P,    {TK::OpAdd, TK::Semicolon, TK::CloseParen}); // FOLLOW(term')

    put(t, 11, NT::FACTOR,    {TK::Ident});
    put(t, 12, NT::FACTOR,    {TK::Number});
    put(t, 13, NT::FACTOR,    {TK::OpenParen});

    return t;
}

const Table& table()
{
    static const Table t = buildTable();   // lanza logic_error si no es LL(1)
    return t;
}

// Rellena con espacios contando caracteres (no bytes), por ε y →.
std::string pad(const std::string& s, std::size_t width)
{
    std::size_t visible = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80) ++visible;
    }
    return s + std::string(width > visible ? width - visible : 0, ' ');
}

} // namespace

// ----------------------------------------------------------------------------
// LL1Parser
// ----------------------------------------------------------------------------
LL1Parser::LL1Parser(Lexer& lex, bool tr) : lexer(lex), trace(tr) {}

void LL1Parser::printTable()
{
    const TokenId cols[] = {TokenId::Ident, TokenId::Number, TokenId::OpEqual,
                            TokenId::OpAdd, TokenId::OpMul, TokenId::OpenParen,
                            TokenId::CloseParen, TokenId::Semicolon, TokenId::EndOfFile};
    const NonTerminals rows[] = {NonTerminals::PROGRAM, NonTerminals::PROGRAM_P, NonTerminals::ASSIGN,
                                 NonTerminals::EXPR, NonTerminals::EXPR_P, NonTerminals::TERM,
                                 NonTerminals::TERM_P, NonTerminals::FACTOR};
    const Table& M = table();

    std::cout << "M[A,a]    ";
    for (TokenId c : cols) {
        std::string n = tokenName(c);
        std::cout << std::string(8 - n.size(), ' ') << n;
    }
    std::cout << "\n";
    for (NonTerminals r : rows) {
        std::string n = ntName(r);
        std::cout << n << std::string(10 - n.size(), ' ');
        for (TokenId c : cols) {
            auto it = M.find({r, c});
            std::string cell = (it == M.end()) ? "" : std::to_string(it->second);
            std::cout << std::string(8 - cell.size(), ' ') << cell;
        }
        std::cout << "\n";
    }
}

void LL1Parser::parse()
{
    const Table& M = table();

    // La cima de la pila es el ÚLTIMO elemento.
    std::vector<Symbol> stack = {T(TokenId::EndOfFile), N(NonTerminals::PROGRAM)};
    Token tok = lexer.nextToken();   // lookahead, se pide al Lexer bajo demanda

    std::vector<std::tuple<std::string, std::string, std::string>> rows;
    auto flush = [&]() {
        if (!trace || rows.empty()) return;
        const std::string head = "Pila (cima a la izq.)";
        std::size_t w = head.size();
        for (const auto& r : rows) w = std::max(w, std::get<0>(r).size());
        std::cout << pad(head, w) << "  " << pad("Entrada", 7) << "  Acción\n";
        for (const auto& r : rows) {
            std::cout << pad(std::get<0>(r), w) << "  " << pad(std::get<1>(r), 7) << "  "
                      << std::get<2>(r) << "\n";
        }
        rows.clear();
    };
    auto fail = [&](const std::string& msg) {
        flush();
        return ParseError(msg, tok.line, tok.column);
    };

    while (!stack.empty()) {
        const Symbol X = stack.back();

        std::string pila;                         // cima a la izquierda, como en la PPT
        if (trace) {
            for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
                if (!pila.empty()) pila += " ";
                pila += symName(*it);
            }
        }

        if (X.isTerminal) {
            if (X.tok != tok.tokenId) {
                throw fail("Expected '" + tokenName(X.tok) + "' but found " + describe(tok));
            }
            if (trace) rows.emplace_back(pila, tokenName(tok.tokenId), "match " + tokenName(X.tok));
            stack.pop_back();
            if (X.tok != TokenId::EndOfFile) {    // match: se consume el token
                tok = lexer.nextToken();
            }
        } else {
            auto cell = M.find({X.nt, tok.tokenId});
            if (cell == M.end()) {                // celda vacía = error de sintaxis
                std::set<std::string> esperados;
                for (const auto& kv : M) {
                    if (kv.first.nt == X.nt) esperados.insert(tokenName(kv.first.tkn));
                }
                std::string lista;
                for (const std::string& e : esperados) lista += (lista.empty() ? "" : ", ") + e;
                throw fail("Unexpected " + describe(tok) + " while expanding " + ntName(X.nt) +
                           "; expected one of: " + lista);
            }
            const int n = cell->second;           // número de regla
            if (trace) rows.emplace_back(pila, tokenName(tok.tokenId), ruleText(n));
            stack.pop_back();
            const std::vector<Symbol>& rhs = RULES[n - 1].rhs;
            for (auto r = rhs.rbegin(); r != rhs.rend(); ++r) {
                stack.push_back(*r);              // apilar el lado derecho AL REVÉS
            }
        }
    }
    flush();                                      // pila vacía y $ consumido: aceptado
}
