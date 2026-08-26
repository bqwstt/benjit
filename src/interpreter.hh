#pragma once

#include "ast.hh"

#include <unordered_map>

/// For now, all values are numbers, but it may be possible to add Strings or Bools too.
using Value = double;

using Variable = std::shared_ptr<ASTVariableAssignment>;
using Function = std::shared_ptr<ASTFunctionDeclaration>;

class Environment {
public:
    Environment() = default;
    ~Environment() = default;

    // void add_variable(const Variable2& var) { m_variables.emplace_back(var); }
    void set_variable(const std::string& var, Value value) { m_variables[var] = value; }
    Value get_variable_value(std::string name) { return m_variables[name]; }

    void add_function(const Function& func) { m_functions.push_back(func); }

    std::vector<ASTPtr> get_function_body(std::string name) {
        auto it = std::find_if(m_functions.begin(), m_functions.end(), [&](const Function& f) {
            return f->func_name().name() == name;
        });

        return (*it)->body();
    }
private:
    // @TODO: Reconsider usage of vector vs something like std::unordered_set
    std::unordered_map<std::string, Value> m_variables;
    std::vector<Function> m_functions;
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

    double evaluate_expression(const ASTExprPtr& expr);

    Environment m_environment;
};