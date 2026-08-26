#include <filesystem>
#include <print>

#include "src/lexer.hh"
#include "src/parser.hh"
#include "src/interpreter.hh"

int
main(int argc, char** argv)
{
    if (argc != 2) {
        std::print("No source file specified. Exiting.\n");
        return -1;
    }

    const auto path = std::filesystem::path(argv[1]);

    Lexer lexer(path);
	Parser parser(std::move(lexer));
	ASTProgram ast = parser.parse();
	if (parser.had_error()) {
		return 65;
	}

	Interpreter interpreter;
	interpreter.interpret(ast);
}