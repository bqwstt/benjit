#pragma once

#include "ast.hh"

#include <variant>
#include <unordered_map>

/// For now, all values are numbers, but it may be possible to add Strings or Bools too.
using Value = std::variant<double, bool>;
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

    // @FIXME: If the variable does not exist, we should print an error. Separate this function into two.
    void tie_variable(const std::string& var, Value value) { m_variables.insert_or_assign(var, std::move(value)); };
    Value get_variable_value(const std::string& var) { return m_variables[var]; };

    void set_kind(ScopeKind kind) { m_kind = kind; }

    template <bool>
    friend class VariableScopeGuard;
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
    // ASTExprPtr return_expr() const { return m_decl->return_expr(); }

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

/// Guard that pushes and pops variable state from a function.
/// Whenever we start interpreting a new code block, we copy the current state of the variables
/// into a new scope and modify its contents. Once we're done (i.e. we exit the block), we restore
/// the state to the previous one.
///
/// @FIXME: Not really sure if I like the 'Preserving' approach.
/// Functions will copy the whole scope and NOT update the variables, but restore state back to original.
/// This is because they create a new stack frame. So if a function gets called inside another function,
/// it will have its own track of variables and will not pollute other functions.
/// Blocks, on the other hand, can updates variable freely since they share stack.
template <bool Preserving>
class VariableScopeGuard {
public:
    VariableScopeGuard(Environment& env)
        : m_environment(env), m_old_scope(env.current_scope())
    {
        // Copy the scope (a.k.a. symbol table) to a new scope
        m_environment.current_scope() = Scope(m_environment.current_scope());
    }

    ~VariableScopeGuard()
    {
        // Update any variable already existing in the old scope with its new value.
        // Anything in the new scope that is not in the old scope is discarded.
        // For example:
        //
        // Scope A (old)   Scope B (new)
        // var a = 5       var a = 5 (same)
        // var b = 6       var b = 8 (updated)
        //                 var c = 2 (new)
        //
        // Resulting scope will be scope A with updated values:
        //
        // Scope A
        // var a = 5
        // var b = 8
        if constexpr (Preserving) {
            Scope new_scope = m_environment.current_scope();
            for (auto& [var, value] : m_old_scope.m_variables) {
                if (auto it = new_scope.m_variables.find(var); it != new_scope.m_variables.end()) {
                    value = it->second;
                }
            }
        }

        // Restore scope back to original
        m_environment.current_scope() = m_old_scope;
    }
private:
    Environment& m_environment;
    Scope m_old_scope;
};

using FunctionVariableScopeGuard = VariableScopeGuard<false>;
using BlockVariableScopeGuard = VariableScopeGuard<true>;

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
    void interpret_loop(const std::shared_ptr<ASTLoop>& loop);
    void interpret_if(const std::shared_ptr<ASTIf>& if_stmt);
    void interpret_return(const std::shared_ptr<ASTReturn>& ret);
    void interpret_print(const std::shared_ptr<ASTPrint>& print);

    [[nodiscard]] Value evaluate_expression(const ASTExprPtr& expr);
    [[nodiscard]] Value evaluate_function_call(const std::shared_ptr<ASTFunctionCall>& func_call);
    [[nodiscard]] Value evaluate_binop(const std::shared_ptr<ASTBinaryOp>& binop);

    Environment m_environment;
};