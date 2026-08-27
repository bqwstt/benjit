#pragma once

#include "ast.hh"

#include <unordered_map>

/// For now, all values are numbers, but it may be possible to add Strings or Bools too.
using Value = double;
using FunctionDecl = std::shared_ptr<ASTFunctionDeclaration>;

enum class ScopeKind {
    Global = 0,
    Function,
    ForBlock,
    IfBlock,
};

class Scope {
public:
    Scope() : m_kind(ScopeKind::Global) {}
    Scope(ScopeKind kind) : m_kind(kind) {}
    Scope(const Scope& other) = default;
    ~Scope() = default;

    void tie_variable(const std::string& var, Value value) { m_variables.insert_or_assign(var, std::move(value)); };
    Value get_variable_value(const std::string& var) { return m_variables[var]; };

    void set_kind(ScopeKind kind) { m_kind = kind; }
private:
    ScopeKind m_kind = ScopeKind::Global;
    std::unordered_map<std::string, Value> m_variables;
};

class Function {
public:
    Function() = delete;
    Function(const FunctionDecl& decl);
    Function(const FunctionDecl& decl, const Scope& scope);
    ~Function() = default;

    std::string func_name() const { return m_decl->func_name().name(); }
    std::vector<ASTPtr> func_body() const { return m_decl->body(); }
    ASTExprPtr return_expr() const { return m_decl->return_expr(); }

    Scope& scope() noexcept { return m_scope; }
private:
    FunctionDecl m_decl;
    Scope m_scope;
};

class Environment {
public:
    Environment() = default;
    ~Environment() = default;

    void add_function(const Function& func) { m_functions.push_back(func); }

    const Function& get_function(std::string name) const {
        auto it = std::find_if(m_functions.begin(), m_functions.end(), [&](const Function& f) {
            return f.func_name() == name;
        });

        return *it;
    }

    Scope& current_scope() noexcept { return m_current_scope; }
private:
    // @TODO: Reconsider usage of vector vs something like std::unordered_set
    std::vector<Function> m_functions;
    Scope m_current_scope;
};

class ScopeGuard {
public:
    ScopeGuard(Environment& env)
        : m_environment(env), m_old_scope(env.current_scope())
    {
        // Copy the scope (a.k.a. symbol table) to a new scope
        m_environment.current_scope() = Scope(m_environment.current_scope());
    }

    ~ScopeGuard()
    {
        // Restore scope back to original
        m_environment.current_scope() = m_old_scope;
    }
private:
    Environment& m_environment;
    Scope m_old_scope;
};

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void interpret(const ASTProgram& program);
private:
    void interpret_statement(const ASTPtr& stmt);
    void interpret_variable_assignment(const std::shared_ptr<ASTVariableAssignment>& assignment);
    void interpret_function_declaration(const std::shared_ptr<ASTFunctionDeclaration>& func_decl);
    void interpret_function_call(const std::shared_ptr<ASTFunctionCall>& func_call);
    void interpret_print(const std::shared_ptr<ASTPrint>& print);

    [[nodiscard]] double evaluate_expression(const ASTExprPtr& expr);
    [[nodiscard]] double evaluate_function_call(const std::shared_ptr<ASTFunctionCall>& func_call);
    [[nodiscard]] double evaluate_binop(const std::shared_ptr<ASTBinaryOp>& binop);

    Environment m_environment;
};