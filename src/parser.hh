#pragma once

#include "lexer.hh"
#include "token.hh"
#include "ast.hh"

#include <memory>

class Parser {
public:
    explicit Parser(Lexer&& lexer) : m_lexer(std::move(lexer)) {}
    ~Parser() = default;

    void parse();
private:
    Lexer m_lexer;
    Token m_current_token;
    Token m_next_token;

    void consume_token();

    [[nodiscard]] std::shared_ptr<ASTNode> parse_statement();
    [[nodiscard]] std::shared_ptr<ASTExpression> parse_expression(uint8_t precedence_limit = 0);
    // [[nodiscard]] std::shared_ptr<ASTNode> parse_identifier();
    [[nodiscard]] std::shared_ptr<ASTVariableAssignment> parse_assignment();
    [[nodiscard]] std::shared_ptr<ASTFunctionDeclaration> parse_function();
};

void dump_ast(const ASTProgram& program);