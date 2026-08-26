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
        m_environment.add_variable(variable_assignment->identifier().name(), result);
    } else if (const auto& func_decl = dynamic_pointer_cast<ASTFunctionDeclaration>(stmt)) {
        Function func(func_decl->func_name().name(), func_decl->body());
        m_environment.add_function(func);
    } else if (const auto& func_call = dynamic_pointer_cast<ASTFunctionCall>(stmt)) {
        const auto& body = m_environment.get_function_body(func_call->func_name().name());
        for (const auto& s : body) {
            interpret_statement(s);
        }
    } else if (const auto& print = dynamic_pointer_cast<ASTPrint>(stmt)) {
        ASTIdentifier param = print->param();
        double value = m_environment.get_variable_value(param.name());
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