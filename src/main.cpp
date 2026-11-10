#include <fstream>
#include <iostream>
#include "Lexer.hpp"

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file path>\n";
        return 1;
    }

    std::ifstream in(argv[1]);

    if (!in.is_open()) {
        std::cout << "Cannot open file '" << argv[1] << '\n';
        return 2;
    }

    Lexer lexer(in);

    Token tok = lexer.nextToken();
    while (tok.tokenId != TokenId::ENDOFFILE) {
        std::cout << Lexer::tokenToString(tok.tokenId) << " '" << tok.text << "'\n";
        tok = lexer.nextToken();
    }

    std::cout << Lexer::tokenToString(tok.tokenId) << " '" << tok.text << "'\n";

    in.close();
    
    return 0;
}