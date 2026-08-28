#pragma once

#include "lexer.hh"
#include "token.hh"
#include "ast.hh"

#include <memory>

struct ScopeContext
{
    std::shared_ptr<ASTFunctionDeclaration> current_function;
    std::shared_ptr<ASTLoop> current_loop;
};

class Parser {
public:
    explicit Parser(Lexer&& lexer) : m_lexer(std::move(lexer))
    {
        // Advance cursors: set current_token and next_token
        consume_token();
        consume_token();
    }
    ~Parser() = default;

    ASTProgram parse();

    bool had_error() const noexcept { return m_had_error; }
private:
    Lexer m_lexer;
    Token m_current_token;
    Token m_next_token;
    bool m_had_error = false;

    void consume_token();

    bool is_at_end() const noexcept {
        return m_current_token.kind() == TokenKind::Illegal || m_current_token.kind() == TokenKind::Eof;
    }

    [[nodiscard]] std::shared_ptr<ASTNode> parse_statement(ScopeContext& ctx);
    [[nodiscard]] std::shared_ptr<ASTExpression> parse_expression(uint8_t precedence_limit = 0);
    [[nodiscard]] std::shared_ptr<ASTVariableAssignment> parse_assignment();
    [[nodiscard]] std::shared_ptr<ASTFunctionDeclaration> parse_function_declaration(ScopeContext& ctx);
    [[nodiscard]] std::shared_ptr<ASTFunctionCall> parse_function_call();
    [[nodiscard]] std::shared_ptr<ASTLoop> parse_loop(ScopeContext& ctx);
    [[nodiscard]] std::shared_ptr<ASTIf> parse_if(ScopeContext& ctx);
    [[nodiscard]] std::shared_ptr<ASTReturn> parse_return(ScopeContext& ctx);
    [[nodiscard]] std::shared_ptr<ASTPrint> parse_print();
};

void dump_ast(const ASTProgram& program);