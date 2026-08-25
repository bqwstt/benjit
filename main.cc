#include <filesystem>
#include <vector>
#include <print>

#include "src/lexer.hh"
#include "src/parser.hh"

int
main(int argc, char** argv)
{
    if (argc != 2) {
        std::print("No source file specified. Exiting.\n");
        return -1;
    }

    const auto path = std::filesystem::path(argv[1]);

    Lexer lexer(path);
    if (lexer.had_error()) {
        return 65;
	}

	Parser parser(std::move(lexer));
	parser.parse();
}