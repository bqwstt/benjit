#pragma once

#include "token.hh"

#include <format>
#include <print>
#include <vector>
#include <span>
#include <memory>

enum class ASTKind {
    Unknown,
    NumericExpr,
    BinaryExpr,
    Identifier,
    Keyword,
    VariableAssignment,
    FunctionDeclaration,
    FunctionCall,
    Print,
};

class ASTNode {
public:
    ASTNode() = default;
    ASTNode(ASTKind kind) : m_kind(kind) {}
    ASTNode(ASTKind kind, const Token& token) : m_kind(kind), m_token(token) {}
    virtual ~ASTNode() = default;

    ASTKind kind() const noexcept { return m_kind; }
    std::string to_string() const;
private:
    ASTKind m_kind;
    Token m_token;
};

using ASTPtr = std::shared_ptr<ASTNode>;

class ASTProgram : public ASTNode {
public:
    ASTProgram() = default;
    ~ASTProgram() = default;

    void add_statement(ASTPtr node) { m_children.push_back(node); }

    const std::vector<ASTPtr>& nodes() const noexcept { return m_children; }
private:
    std::vector<ASTPtr> m_children;
};

class ASTExpression : public ASTNode {
public:
    ASTExpression() : ASTNode(ASTKind::NumericExpr) {}
    ASTExpression(ASTKind expr_kind) : ASTNode(expr_kind) {}
    ~ASTExpression() = default;
};

using ASTExprPtr = std::shared_ptr<ASTExpression>;

class ASTNumericExpr : public ASTExpression {
public:
    explicit ASTNumericExpr(const std::string& literal)
        : ASTExpression(ASTKind::NumericExpr)
        , m_literal(literal) {}
    explicit ASTNumericExpr(int number)
        : ASTExpression(ASTKind::NumericExpr)
        , m_literal(std::format("{}", number)) {}

    const std::string& literal() const noexcept { return m_literal; }
private:
    std::string m_literal;
};

class ASTBinaryOp : public ASTExpression {
public:
    explicit ASTBinaryOp(Token op, ASTExprPtr left, ASTExprPtr right)
        : ASTExpression(ASTKind::BinaryExpr)
        , m_op(op)
        , m_left(std::move(left))
        , m_right(std::move(right)) {}
    ~ASTBinaryOp() = default;

    const Token& op() const noexcept { return m_op; }
    const ASTExprPtr& left() const noexcept { return m_left; }
    const ASTExprPtr& right() const noexcept { return m_right; }
private:
    Token m_op;
    ASTExprPtr m_left;
    ASTExprPtr m_right;
};

class ASTIdentifier : public ASTExpression {
public:
    ASTIdentifier() = default;
    ASTIdentifier(const char* name)
        : ASTExpression(ASTKind::Identifier)
        , m_name(name) {}
    ASTIdentifier(const std::string& name)
        : ASTExpression(ASTKind::Identifier)
        , m_name(name) {}
    ASTIdentifier(const ASTIdentifier& other) = default;
    ~ASTIdentifier() = default;

    const std::string& name() const noexcept { return m_name; }
private:
    std::string m_name;
};

class ASTVariableAssignment : public ASTNode {
public:
    ASTVariableAssignment(const ASTIdentifier& ident, ASTExprPtr expr)
        : ASTNode(ASTKind::VariableAssignment)
        , m_identifier(ident)
        , m_expression(std::move(expr)) {}
    ~ASTVariableAssignment() = default;

    const ASTIdentifier& identifier() const noexcept { return m_identifier; }
    const ASTExprPtr& expression() const noexcept { return m_expression; }
private:
    ASTIdentifier m_identifier;
    ASTExprPtr m_expression;
};

// @TODO: Move ctors to .cc file?
class ASTFunctionDeclaration : public ASTNode {
public:
    ASTFunctionDeclaration(const ASTIdentifier& name, std::span<ASTPtr> body)
        : ASTNode(ASTKind::FunctionDeclaration)
        , m_func_name(name)
        , m_body(std::from_range, body) {}
    ASTFunctionDeclaration(const ASTIdentifier& name, std::span<ASTPtr> body, const ASTExprPtr& return_expr)
        : ASTNode(ASTKind::FunctionDeclaration)
        , m_func_name(name)
        , m_body(std::from_range, body)
        , m_return_expr(return_expr) {}
    ASTFunctionDeclaration(const ASTIdentifier& name, std::span<ASTIdentifier> parameters, std::span<ASTPtr> body)
        : ASTNode(ASTKind::FunctionDeclaration)
        , m_func_name(name)
        , m_parameters(std::from_range, parameters)
        , m_body(std::from_range, body) {}
    ~ASTFunctionDeclaration() = default;

    const ASTIdentifier& func_name() const noexcept { return m_func_name; }
    const std::vector<ASTPtr>& body() const noexcept { return m_body; }
    const ASTExprPtr& return_expr() const noexcept { return m_return_expr; }
private:
    ASTIdentifier m_func_name;
    std::vector<ASTIdentifier> m_parameters;
    std::vector<ASTPtr> m_body;
    ASTExprPtr m_return_expr;
};

class ASTFunctionCall : public ASTExpression {
public:
    ASTFunctionCall(const ASTIdentifier& name)
        : ASTExpression(ASTKind::FunctionCall)
        , m_func_name(name) {}
    ASTFunctionCall(const ASTIdentifier& name, std::span<ASTIdentifier> parameters)
        : ASTExpression(ASTKind::FunctionCall)
        , m_func_name(name)
        , m_parameters(std::from_range, parameters) {}
    ~ASTFunctionCall() = default;

    const ASTIdentifier& func_name() const noexcept { return m_func_name; }
private:
    ASTIdentifier m_func_name;
    std::vector<ASTIdentifier> m_parameters;
};

class ASTPrint : public ASTNode {
public:
    ASTPrint(const ASTIdentifier& ident)
        : ASTNode(ASTKind::Print)
        , m_param(ident) {}
    ~ASTPrint() = default;

    template <typename T>
    static void print(T element) { std::print("{}\n", element); };

    const ASTIdentifier& param() const noexcept { return m_param; }
private:
    ASTIdentifier m_param;
};

void dump_node(ASTPtr node, uint8_t depth, bool has_child);