#include "parser.hh"
#include "ast.hh"
#include "reporter.hh"
#include "token.hh"
#include <cstdio>
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
    ScopeContext ctx;
    while (!is_at_end() && !m_had_error) {
        ASTPtr stmt = parse_statement(ctx);
        if (stmt != nullptr) {
            program.add_statement(stmt);
        }

        if (m_lexer.had_error())
            m_had_error = true;
    }

    return program;
}

std::shared_ptr<ASTNode>
Parser::parse_statement(ScopeContext& ctx)
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
            stmt = parse_function_declaration(ctx);
            break;
        }
        case TokenKind::Print: {
            stmt = parse_print();
            break;
        }
        case TokenKind::If: {
            stmt = parse_if(ctx);
            break;
        }
        case TokenKind::For: {
            stmt = parse_loop(ctx);
            break;
        }
        case TokenKind::Break: {
            stmt = std::make_shared<ASTBreak>(*ctx.current_loop);
            consume_token();
            break;
        }
        case TokenKind::Continue: {
            stmt = std::make_shared<ASTContinue>(*ctx.current_loop);
            consume_token();
            break;
        }
        case TokenKind::Return: {
            stmt = parse_return(ctx);
            break;
        }
        case TokenKind::Identifier: {
            if (m_next_token.kind() == TokenKind::OpenParenthesis) {
                stmt = parse_function_call();
            }

            if (m_next_token.kind() == TokenKind::Assignment) {
                // Variable re-assignment
                stmt = parse_assignment();
                break;
            }
            break;
        }
        default: {
            m_had_error = true;

            // @FIXME: Proper error reporting :)
            Reporter::report_error("Can't parse!");
            break;
        }
    }

    return stmt;
}

std::shared_ptr<ASTExpression>
Parser::parse_expression(uint8_t precedence_limit)
{
    // @FIXME: Check for parenthesis.
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

    if (m_current_token.kind() == TokenKind::True || m_current_token.kind() == TokenKind::False) {
        bool is_true_keyword = m_current_token.kind() == TokenKind::True;
        consume_token(); // Consume the boolean
        expr = std::make_shared<ASTBooleanExpr>(is_true_keyword);
    }

    if (m_current_token.is_logical_operator()) {
        Token op_token = m_current_token;
        consume_token(); // Consume the operator

        auto right = parse_expression();
        return std::make_shared<ASTBinaryOp>(op_token, expr, right);
    }

    while (m_current_token.is_math_operator()) {
        // Following Pratt parser's logic
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
        expr = std::make_shared<ASTBinaryOp>(op_token, expr, right);
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
Parser::parse_function_declaration(ScopeContext& ctx)
{
    consume_token(); // Consume 'algorithm' keyword
    ASTIdentifier func_name(m_current_token.literal());

    // @FIXME: Actually check for parameters and syntax errors
    consume_token(); // Consume func name
    consume_token(); // Consume open paren
    consume_token(); // Consume close paren
    consume_token(); // Consume 'is' keyword

    auto func_decl = std::make_shared<ASTFunctionDeclaration>(func_name);
    auto old_func = ctx.current_function;

    // Set context's func to this new one
    ctx.current_function = func_decl;

    std::vector<ASTPtr> body;
    while (!is_at_end() && m_current_token.kind() != TokenKind::End) {
        auto stmt = parse_statement(ctx);
        body.push_back(std::move(stmt));
    }

    ctx.current_function = old_func;

    consume_token(); // Consume 'end' keyword

    func_decl->set_body(body);
    return func_decl;
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

std::shared_ptr<ASTLoop>
Parser::parse_loop(ScopeContext& ctx)
{
    consume_token(); // Consume 'for' keyword
    auto condition = parse_expression();
    consume_token(); // Consume 'do' keyword

    auto loop = std::make_shared<ASTLoop>();
    auto old_loop = ctx.current_loop;

    // Set context's loop to this new one
    ctx.current_loop = loop;

    std::vector<ASTPtr> body;
    while (!is_at_end() && m_current_token.kind() != TokenKind::End) {
        auto stmt = parse_statement(ctx);
        body.push_back(std::move(stmt));
    }

    // Revert loop
    ctx.current_loop = old_loop;

    consume_token(); // Consume 'end' keyword

    loop->set_condition(condition);
    loop->set_body(body);
    return loop;
}

std::shared_ptr<ASTIf>
Parser::parse_if(ScopeContext& ctx)
{
    consume_token(); // Consume 'if' keyword
    auto condition = parse_expression();
    consume_token(); // Consume 'then' keyword

    std::vector<ASTPtr> body;
    auto is_end_of_branch = [&](TokenKind kind) {
        return kind == TokenKind::Else || kind == TokenKind::End;
    };

    while (!is_at_end() && !is_end_of_branch(m_current_token.kind())) {
        auto stmt = parse_statement(ctx);
        body.push_back(std::move(stmt));
    }

    // 'else' branch
    std::vector<ASTPtr> else_body;
    if (m_current_token.kind() == TokenKind::Else) {
        consume_token(); // Consume 'else' keyword

        while (!is_at_end() && m_current_token.kind() != TokenKind::End) {
            auto stmt = parse_statement(ctx);
            else_body.push_back(std::move(stmt));
        }
    }

    consume_token(); // Consume 'end' keyword

    return std::make_shared<ASTIf>(condition, body, else_body);
}

std::shared_ptr<ASTReturn>
Parser::parse_return(ScopeContext& ctx)
{
    consume_token(); // Consume 'return' keyword
    auto expr = parse_expression();
    return std::make_shared<ASTReturn>(expr, *ctx.current_function);
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