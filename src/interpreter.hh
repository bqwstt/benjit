#pragma once

#include "ast.hh"

#include <unordered_map>

/// For now, all values are numbers, but it may be possible to add Strings or Bools too.
using Value = double;

class Environment {
public:
    Environment() = default;
    ~Environment() = default;

    void tie_value(const ASTIdentifier& ident, Value value) { m_identifiers[ident.name()] = value; };
    Value get_value(const ASTIdentifier& ident) const { return m_identifiers.at(ident.name()); };
private:
    std::unordered_map<std::string, Value> m_identifiers;
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