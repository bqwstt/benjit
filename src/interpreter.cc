#include "interpreter.hh"
#include "ast.hh"
#include "reporter.hh"

#include <cmath>
#include <string>

Function::Function(const FunctionDecl& decl)
    : m_decl(decl), m_scope(ScopeKind::Function)
{
}

Function::Function(const FunctionDecl& decl, const Scope& scope)
    : m_decl(decl), m_scope(scope)
{
}

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
        interpret_variable_assignment(variable_assignment);
    }

    if (const auto& func_decl = dynamic_pointer_cast<ASTFunctionDeclaration>(stmt)) {
        interpret_function_declaration(func_decl);
    }

    if (const auto& func_call = dynamic_pointer_cast<ASTFunctionCall>(stmt)) {
        interpret_function_call(func_call);
    }

    if (const auto& break_stmt = dynamic_pointer_cast<ASTBreak>(stmt)) {
        m_stop_requested = true;
    }

    if (const auto& continue_stmt = dynamic_pointer_cast<ASTContinue>(stmt)) {
        continue_stmt->parent().set_need_skip(true);
    }

    if (const auto& return_stmt = dynamic_pointer_cast<ASTReturn>(stmt)) {
        m_hit_return = true;
        return_stmt->parent().set_ret_expr(return_stmt->ret_expr());
    }

    if (const auto& loop = dynamic_pointer_cast<ASTLoop>(stmt)) {
        interpret_loop(loop);
    }

    if (const auto& if_stmt = dynamic_pointer_cast<ASTIf>(stmt)) {
        interpret_if(if_stmt);
    }

    if (const auto& print = dynamic_pointer_cast<ASTPrint>(stmt)) {
        interpret_print(print);
    }
}

void
Interpreter::interpret_variable_assignment(const std::shared_ptr<ASTVariableAssignment>& assignment)
{
    std::string var_name = assignment->identifier().name();
    Value result = evaluate_expression(assignment->expression());
    m_environment.current_scope().tie_variable(var_name, result);
}

void
Interpreter::interpret_function_declaration(const std::shared_ptr<ASTFunctionDeclaration>& func_decl)
{
    m_environment.add_function(func_decl);
}

void
Interpreter::interpret_function_call(const std::shared_ptr<ASTFunctionCall>& func_call)
{
    // Evaluates the function ignoring the return value
    std::ignore = evaluate_function_call(func_call);
}

void
Interpreter::interpret_loop(const std::shared_ptr<ASTLoop>& loop)
{
    BlockVariableScopeGuard guard(m_environment);

    bool condition_holds = std::get<bool>(evaluate_expression(loop->condition()));
    while (condition_holds && !m_stop_requested && !m_hit_return) {
        loop->set_need_skip(false); // Reset skip flag if found a `continue` in previous iteration

        for (const auto& node : loop->body()) {
            interpret_statement(node);

            if (m_stop_requested || m_hit_return || loop->need_skip()) {
                // Hit `break` or `continue`
                // If `break`, then stop looping.
                // If `continue`, break the inner loop and continue execution.
                break;
            }
        }

        condition_holds = std::get<bool>(evaluate_expression(loop->condition()));
    }

    // Reset stop flag made by `break` (if set)
    m_stop_requested = false;
}

void
Interpreter::interpret_if(const std::shared_ptr<ASTIf>& if_stmt)
{
    BlockVariableScopeGuard guard(m_environment);

    bool cond_holds_true = std::get<bool>(evaluate_expression(if_stmt->condition()));
    const auto& body_to_run = cond_holds_true ? if_stmt->body() : if_stmt->else_body();
    for (const auto& node : body_to_run) {
        interpret_statement(node);

        if (m_stop_requested || m_hit_return)
            break;
    }
}

void
Interpreter::interpret_print(const std::shared_ptr<ASTPrint>& print)
{
    const auto& args = print->args();
    for (const auto& arg : args) {
        auto expr = evaluate_expression(arg);
        std::visit([&print](auto&& value) {
            print->print(value);
        }, expr);
    }
}

Value
Interpreter::evaluate_expression(const ASTExprPtr& expr)
{
    if (const auto& identifier = dynamic_pointer_cast<ASTIdentifier>(expr)) {
        return m_environment.current_scope().get_variable_value(identifier->name());
    }

    if (const auto& numeric = dynamic_pointer_cast<ASTNumericExpr>(expr)) {
        return std::stod(numeric->literal());
    }

    if (const auto& boolean = dynamic_pointer_cast<ASTBooleanExpr>(expr)) {
        return boolean->value();
    }

    if (const auto& func_call = dynamic_pointer_cast<ASTFunctionCall>(expr)) {
        return evaluate_function_call(func_call);
    }

    if (const auto& binop = dynamic_pointer_cast<ASTBinaryOp>(expr)) {
        return evaluate_binop(binop);
    }

    // @TODO: Replace NAN with std::monostate, maybe?
    return NAN;
}

Value
Interpreter::evaluate_function_call(const std::shared_ptr<ASTFunctionCall>& func_call)
{
    FunctionVariableScopeGuard guard(m_environment);

    auto& func = m_environment.get_function(func_call->func_name().name());
    const auto& parameters = func.parameters();
    const auto& arguments = func_call->arguments();

    // Evaluate function parameters
    if (parameters.size() != arguments.size()) {
        Reporter::report_error(std::format("Arguments for function {} do not match expected amount of parameters: {} vs {}",
            func.func_name(),
            parameters.size(),
            arguments.size()));
        return NAN;
    }

    for (int i = 0; i < parameters.size(); i++) {
        const auto& param = parameters[i];
        const auto& arg = arguments[i];

        auto param_name = param.name();
        auto expr = evaluate_expression(arg);
        m_environment.current_scope().tie_variable(param_name, expr);
    }

    for (const auto& s : func.func_body()) {
        interpret_statement(s);

        // Hit return: break loop
        // Otherwise we would still interpret everything else and mistakenly
        // grab the wrong return statement, if any.
        if (m_hit_return)
            break;
    }

    if (auto expr = func.return_expr(); expr) {
        // Restore flag set by return
        m_hit_return = false;
        return evaluate_expression(expr);
    }

    return NAN;
}

Value
Interpreter::evaluate_binop(const std::shared_ptr<ASTBinaryOp>& binop)
{
    Value left = evaluate_expression(binop->left());
    Value right = evaluate_expression(binop->right());
    TokenKind op = binop->op().kind();

    switch (op) {
        // #region Doubles
        case TokenKind::Plus:
            return std::get<double>(left) + std::get<double>(right);
        case TokenKind::Minus:
            return std::get<double>(left) - std::get<double>(right);
        case TokenKind::Multiply:
            return std::get<double>(left) * std::get<double>(right);
        case TokenKind::Divide:
            return std::get<double>(left) / std::get<double>(right);
        case TokenKind::IntegerDivide:
            return std::trunc(std::get<double>(left) / std::get<double>(right));
        case TokenKind::Exponent:
            return static_cast<double>(std::powl(std::get<double>(left), std::get<double>(right)));
        case TokenKind::LessThan:
            return std::get<double>(left) < std::get<double>(right);
        case TokenKind::LessEquals:
            return std::get<double>(left) <= std::get<double>(right);
        case TokenKind::GreaterThan:
            return std::get<double>(left) > std::get<double>(right);
        case TokenKind::GreaterEquals:
            return std::get<double>(left) >= std::get<double>(right);
        // #region Booleans
        case TokenKind::And:
            return std::get<bool>(left) && std::get<bool>(right);
        case TokenKind::Or:
            return std::get<bool>(left) || std::get<bool>(right);
        // == and != work for either booleans and numbers - no need to std::get
        case TokenKind::DoubleEquals:
            return left == right;
        case TokenKind::NotEquals:
            return left != right;
        default: break;
    }

    return NAN;
}