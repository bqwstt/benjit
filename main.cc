#include <filesystem>
#include <vector>
#include <print>

#include "src/lexer.hh"

int
main(int argc, char** argv)
{
    if (argc != 2) {
        std::print("No source file specified. Exiting.\n");
        return -1;
    }

    const auto path = std::filesystem::path(argv[1]);

    Lexer lexer(path);
    const std::vector<Token> tokens = lexer.scan_tokens();

    for (const auto& tok : tokens) {
        std::print("{}\n", tok.to_string());
    }

    if (lexer.had_error())
        return 65;
}