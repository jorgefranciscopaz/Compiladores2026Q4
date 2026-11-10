#pragma once

#include <cstddef>
#include <functional>
#include <unordered_map>
#include "Lexer.hpp"
#include "Parser.hpp"   // reutiliza ParseError (mensaje + línea + columna)

// No terminales de la gramática LL(1) (sin recursión izquierda):
//    1  program  -> assign program'
//    2  program' -> assign program'
//    3  program' -> ε
//    4  assign   -> IDENT "=" expr ";"
//    5  expr     -> term expr'
//    6  expr'    -> "+" term expr'
//    7  expr'    -> ε
//    8  term     -> factor term'
//    9  term'    -> "*" factor term'
//   10  term'    -> ε
//   11  factor   -> IDENT
//   12  factor   -> NUMBER
//   13  factor   -> "(" expr ")"
enum class NonTerminals {
    PROGRAM,
    PROGRAM_P,
    ASSIGN,
    EXPR,
    EXPR_P,
    TERM,
    TERM_P,
    FACTOR,
};

// Clave de la tabla M[A, a]: (no terminal, token del Lexer).
struct GrammarKey {
    NonTerminals nt;
    TokenId tkn;

    bool operator==(const GrammarKey& o) const { return nt == o.nt && tkn == o.tkn; }
};

struct KeyHasher {
    std::size_t operator()(const GrammarKey& k) const
    {
        return std::hash<int>()(static_cast<int>(k.nt)) * 31u
             ^ std::hash<int>()(static_cast<int>(k.tkn));
    }
};

// Parser predictivo descendente guiado por la tabla M[A, a].
// Consume los tokens del Lexer existente bajo demanda (igual que Parser)
// y reporta errores con ParseError, igual que el parser recursivo.
class LL1Parser {
public:
    explicit LL1Parser(Lexer& lexer, bool trace = false);

    // Analiza toda la entrada. Lanza ParseError si es inválida.
    void parse();

    // Imprime la tabla M[A, a] en stdout.
    static void printTable();

private:
    Lexer& lexer;
    bool trace;
};
