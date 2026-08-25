#include "interpreter.hh"
#include "ast.hh"

#include <cmath>
#include <string>

void
Interpreter::interpret(const ASTProgram& program)
{
    for (const auto& node : program.nodes()) {
        interpret_statement(node);
    }
}

void
Interpreter::interpret_statement(const ASTPtr& stmt)
{
    if (const auto& variable_assignment = dynamic_pointer_cast<ASTVariableAssignment>(stmt)) {
        double result = evaluate_expression(variable_assignment->expression());
        m_environment.tie_value(variable_assignment->identifier(), result);
    } else if (const auto& func = dynamic_pointer_cast<ASTFunctionDeclaration>(stmt)) {
        for (const auto& node : func->body()) {
            interpret_statement(node);
        }
    } else if (const auto& print = dynamic_pointer_cast<ASTPrint>(stmt)) {
        ASTIdentifier param = print->param();
        double value = m_environment.get_value(param);
        print->print(value);
    }
}

double
Interpreter::evaluate_expression(const ASTExprPtr& expr)
{
    if (const auto& numeric = dynamic_pointer_cast<ASTNumericExpr>(expr)) {
        return std::stod(numeric->literal());
    } else if (const auto& binop = dynamic_pointer_cast<ASTBinaryOp>(expr)) {
        double left = evaluate_expression(binop->left()) ;
        double right = evaluate_expression(binop->right()) ;
        TokenKind op = binop->op().kind();

        switch (op) {
            case TokenKind::Plus: return left + right;
            case TokenKind::Minus: return left - right;
            case TokenKind::Multiply: return left * right;
            case TokenKind::Divide: return left / right;
            case TokenKind::Exponent: return std::powl(left, right);
            default: break;
        }
    }

    return NAN;
}