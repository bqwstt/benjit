#include "parser.hh"
#include "ast.hh"
#include "token.hh"
#include <memory>
#include <print>

void
Parser::consume_token()
{
    // @FIXME: If the file ends with a newline, this always returns Illegal and loops infinitely
    m_current_token = m_next_token;
    if (is_at_end())
        return;

    m_next_token = m_lexer.consume_token();
}

ASTProgram
Parser::parse()
{
    ASTProgram program;
    while (!is_at_end() && !m_had_error) {
        ASTPtr stmt = parse_statement();
        if (stmt != nullptr) {
            program.add_statement(stmt);
        }

        if (m_lexer.had_error())
            m_had_error = true;
    }

    return program;
}

std::shared_ptr<ASTNode>
Parser::parse_statement()
{
    ASTPtr stmt;
    switch (m_current_token.kind()) {
        case TokenKind::NumericLiteral: {
            stmt = parse_expression(/*precendece_limit=*/0);
            break;
        }
        case TokenKind::Variable: {
            consume_token(); // Consume 'variable' keyword
            stmt = parse_assignment();
            break;
        }
        case TokenKind::Algorithm: {
            stmt = parse_function_declaration();
            break;
        }
        case TokenKind::Print: {
            stmt = parse_print();
            break;
        }
        case TokenKind::Identifier: {
            if (m_next_token.kind() == TokenKind::OpenParenthesis) {
                stmt = parse_function_call();
            }
            break;
        }
        default: break;
    }

    return stmt;
}

std::shared_ptr<ASTExpression>
Parser::parse_expression(uint8_t precedence_limit)
{
    std::shared_ptr<ASTExpression> expr;
    if (m_current_token.kind() == TokenKind::NumericLiteral) {
        expr = std::make_shared<ASTNumericExpr>(m_current_token.literal());
        consume_token(); // Consume the number
    } else if (m_current_token.kind() == TokenKind::Identifier) {
        // If found identifier + parenthesis, that's a function call.
        if (m_next_token.kind() == TokenKind::OpenParenthesis) {
            expr = parse_function_call();
        } else {
            expr = std::make_shared<ASTIdentifier>(m_current_token.literal());
            consume_token(); // Consume the identifier
        }
    }

    while (m_current_token.is_operator()) {
        uint8_t prec = m_current_token.operator_precedence();
        uint8_t final_prec = prec;

        if (m_current_token.operator_associativity() == OperatorAssociativity::Right) {
            final_prec -= 1;
        }

        if (prec <= precedence_limit) {
            return expr;
        }

        Token op_token = m_current_token;

        consume_token(); // Consume the operator

        auto right = parse_expression(final_prec);
        auto binop = std::make_shared<ASTBinaryOp>(op_token, expr, right);
        expr = std::move(binop);
    }

    return expr;
}

std::shared_ptr<ASTVariableAssignment>
Parser::parse_assignment()
{
    ASTIdentifier identifier(m_current_token.literal());
    consume_token(); // Consume name
    consume_token(); // Consume assignment operator

    auto expr = parse_expression();
    return std::make_shared<ASTVariableAssignment>(identifier, std::move(expr));
}

std::shared_ptr<ASTFunctionDeclaration>
Parser::parse_function_declaration()
{
    consume_token(); // Consume 'algorithm' keyword
    ASTIdentifier func_name(m_current_token.literal());

    // @FIXME: Actually check for parameters and syntax errors
    consume_token(); // Consume func name
    consume_token(); // Consume open paren
    consume_token(); // Consume close paren
    consume_token(); // Consume 'is' keyword

    std::vector<ASTPtr> body;
    auto is_end_of_function = [&](const TokenKind kind) {
        return kind == TokenKind::Return || kind == TokenKind::End;
    };

    while (!is_at_end() && !is_end_of_function(m_current_token.kind())) {
        auto stmt = parse_statement();
        body.push_back(std::move(stmt));
    }

    ASTExprPtr return_expr;
    if (m_current_token.kind() == TokenKind::Return) {
        consume_token(); // Consume 'return' keyword
        return_expr = parse_expression();
    }

    consume_token(); // Consume 'end' keyword

    // @TODO: Anything other than variables!
    return std::make_shared<ASTFunctionDeclaration>(func_name, body, return_expr);
}

std::shared_ptr<ASTFunctionCall>
Parser::parse_function_call()
{
    ASTIdentifier func_name(m_current_token.literal());
    consume_token(); // Consume func name
    consume_token(); // Consume open paren
    consume_token(); // Consume close paren

    return std::make_shared<ASTFunctionCall>(func_name);
}

std::shared_ptr<ASTPrint>
Parser::parse_print()
{
    consume_token(); // Consume 'print' keyword
    consume_token(); // Consume open paren

    // @FIXME: This print only takes one parameter.
    ASTIdentifier ident(m_current_token.literal());
    consume_token(); // Consume param

    consume_token(); // Consume close paren

    return std::make_shared<ASTPrint>(ident);
}

void dump_ast(const ASTProgram& program)
{
    std::print("│[Program]\n");
    for (const auto& node : program.nodes()) {
        std::print("└──│[Statement]");
        dump_node(node, 1, true);
        std::print("\n");
    }
}