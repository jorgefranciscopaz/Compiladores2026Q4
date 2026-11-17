// Driver of Lab 1. Provided: do not modify (the automated tests depend on its output).
//
//   minilang --tokens FILE        print the tokens of FILE, one per line:   LINE:COL TokenId 'text'
//   minilang [--trace] FILE...    check the syntax of each FILE
//
// Exit codes:  0 ok | 1 usage or unreadable file | 2 syntax error | 70 unfinished TODO
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

#include "Lexer.hpp"
#include "NotImplemented.hpp"
#include "Parser.hpp"

namespace {

constexpr int ExitOk = 0;
constexpr int ExitUsage = 1;
constexpr int ExitSyntaxError = 2;
constexpr int ExitNotImplemented = 70;

// Makes a lexeme printable on one line: \\ \' and \xHH for anything that is not printable ASCII.
std::string escape(const std::string& s)
{
    std::string out;
    for (const unsigned char c : s) {
        if (c == '\\') {
            out += "\\\\";
        } else if (c == '\'') {
            out += "\\'";
        } else if (c >= 0x20 && c < 0x7f) {
            out += static_cast<char>(c);
        } else {
            char buf[5];
            std::snprintf(buf, sizeof buf, "\\x%02x", c);
            out += buf;
        }
    }
    return out;
}

// Prints every token of the file, including the final EndOfFile (printed without its text).
int dumpTokens(const char *path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << path << ": cannot open file\n";
        return ExitUsage;
    }

    Lexer lexer(in);
    try {
        for (;;) {
            const Token tok = lexer.nextToken();
            std::cout << tok.line << ':' << tok.column << ' ' << Lexer::tokenToString(tok.tokenId);
            if (tok.tokenId == TokenId::EndOfFile) {
                std::cout << '\n';
                return ExitOk;
            }
            std::cout << " '" << escape(tok.text) << "'\n";
        }
    } catch (const NotImplementedError& e) {
        std::cerr << path << ": " << e.what() << '\n';
        return ExitNotImplemented;
    }
}

// Checks the syntax of one file. Returns ExitOk, ExitSyntaxError, ExitUsage or ExitNotImplemented.
int parseFile(const char *path, bool trace)
{
    std::ifstream in(path, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << path << ": cannot open file\n";
        return ExitUsage;
    }

    Lexer lexer(in);
    Parser parser(lexer);
    parser.setTrace(trace);

    try {
        parser.parse();
    } catch (const ParseError& e) {
        std::cerr << path << ':' << e.line << ':' << e.column << ": error: " << e.what() << '\n';
        return ExitSyntaxError;
    } catch (const NotImplementedError& e) {
        std::cerr << path << ": " << e.what() << '\n';
        return ExitNotImplemented;
    }

    std::cout << path << ": OK\n";
    return ExitOk;
}

void usage(const char *prog)
{
    std::cerr << "Usage: " << prog << " --tokens <file>\n"
              << "       " << prog << " [--trace] <file>...\n";
}

} // namespace

int main(int argc, const char *argv[])
{
    bool tokens = false;
    bool trace = false;
    int first = 1;

    for (; first < argc && std::strncmp(argv[first], "--", 2) == 0; ++first) {
        if (std::strcmp(argv[first], "--tokens") == 0) {
            tokens = true;
        } else if (std::strcmp(argv[first], "--trace") == 0) {
            trace = true;
        } else {
            usage(argv[0]);
            return ExitUsage;
        }
    }

    if (first >= argc || (tokens && argc - first != 1)) {
        usage(argv[0]);
        return ExitUsage;
    }

    if (tokens) {
        return dumpTokens(argv[first]);
    }

    // Worst result wins: unfinished TODO (70) > syntax error (2) > unreadable file (1) > ok.
    int result = ExitOk;
    for (int i = first; i < argc; ++i) {
        const int rc = parseFile(argv[i], trace);
        if (rc == ExitNotImplemented || (rc == ExitSyntaxError && result != ExitNotImplemented) ||
            (rc == ExitUsage && result == ExitOk)) {
            result = rc;
        }
    }
    return result;
}
