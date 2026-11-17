#include <fstream>
#include <iostream>
#include <string>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "LL1.hpp"

// Parses one file. Returns true if it is syntactically valid.
// useLL1 = true  -> parser LL(1) por tabla (sin recursion)
// useLL1 = false -> parser recursivo descendente
static bool parseFile(const char *path, bool useLL1)
{
    std::ifstream in(path);

    if (!in.is_open()) {
        std::cerr << path << ": cannot open file\n";
        return false;
    }

    Lexer lexer(in);

    try {
        if (useLL1) {
            LL1Parser parser(lexer);
            parser.parse();
            std::cout << path << ": Parser sin recursion = Acepta\n";
        } else {
            Parser parser(lexer);
            parser.parse();
            std::cout << path << ": OK\n";
        }
    } catch (const ParseError& e) {
        std::cerr << path << ':' << e.line << ':' << e.column << ": error: " << e.what() << '\n';
        return false;
    }

    return true;
}

int main(int argc, const char *argv[]) {
    bool useLL1 = false;
    int files = 0;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--ll1") useLL1 = true;
        else ++files;
    }

    if (files == 0) {
        std::cerr << "Usage: " << argv[0] << " [--ll1] <file path>...\n";
        return 1;
    }

    bool allOk = true;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--ll1") continue;
        allOk = parseFile(argv[i], useLL1) && allOk;
    }

    return allOk ? 0 : 2;
}
