#include <fstream>
#include <iostream>
#include "Lexer.hpp"
#include "Parser.hpp"

// Parses one file. Returns true if it is syntactically valid.
static bool parseFile(const char *path)
{
    std::ifstream in(path);

    if (!in.is_open()) {
        std::cerr << path << ": cannot open file\n";
        return false;
    }

    Lexer lexer(in);
    Parser parser(lexer);

    try {
        parser.parse();
    } catch (const ParseError& e) {
        std::cerr << path << ':' << e.line << ':' << e.column << ": error: " << e.what() << '\n';
        return false;
    }

    std::cout << path << ": OK\n";
    return true;
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file path>...\n";
        return 1;
    }

    bool allOk = true;
    for (int i = 1; i < argc; ++i) {
        allOk = parseFile(argv[i]) && allOk;
    }

    return allOk ? 0 : 2;
}
