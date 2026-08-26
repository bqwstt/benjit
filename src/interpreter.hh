#pragma once

#include "ast.hh"

#include <algorithm>

/// For now, all values are numbers, but it may be possible to add Strings or Bools too.
using Value = double;

class Variable {
public:
    Variable() = delete;
    explicit Variable(std::string name) : m_name(name), m_value(0.0) {}
    Variable(std::string name, Value value) : m_name(name), m_value(value) {}
    ~Variable() = default;

    std::string name() const noexcept { return m_name; }
    Value value() const noexcept { return m_value; }
private:
    std::string m_name;
    Value m_value;
};

class Function {
public:
    Function() = delete;
    explicit Function(std::string name) : m_name(name) {}
    ~Function() = default;
private:
    std::string m_name;
    std::vector<Variable> m_variables;
};

class Environment {
public:
    Environment() = default;
    ~Environment() = default;

    void add_variable(std::string name, Value value) { m_variables.emplace_back(name, value); }
    void add_function(std::string name) { m_functions.emplace_back(name); }

    Value get_variable_value(std::string name) {
        auto it = std::find_if(m_variables.begin(), m_variables.end(), [&](const Variable& v) {
            return v.name() == name;
        });

        return it->value();
    }
private:
    // @TODO: Reconsider usage of vector vs something like std::unordered_set
    std::vector<Variable> m_variables;
    std::vector<Function> m_functions;
};

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;

    void interpret(const ASTProgram& program);
private:
    void interpret_statement(const ASTPtr& stmt);
    double evaluate_expression(const ASTExprPtr& expr);

    Environment m_environment;
};