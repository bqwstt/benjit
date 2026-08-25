#include "token.hh"

#include <format>
#include <string>

std::string
Token::to_string() const
{
    const char* name = "";
    #define CASE(kind, id) case kind: \
        name = (id); \
        break

    switch (m_kind) {
        CASE(TokenKind::Plus, "Plus");
        CASE(TokenKind::Minus, "Minus");
        CASE(TokenKind::Multiply, "Multiply");
        CASE(TokenKind::Divide, "Divide");
        CASE(TokenKind::Exponent, "Exponent");
        CASE(TokenKind::Hash, "Hash");
        CASE(TokenKind::Comma, "Comma");
        CASE(TokenKind::OpenParenthesis, "OpenParen");
        CASE(TokenKind::CloseParenthesis, "CloseParen");
        CASE(TokenKind::GreaterThan, "GreaterThan");
        CASE(TokenKind::LessThan, "LessThan");
        CASE(TokenKind::Or, "Or");
        CASE(TokenKind::And, "And");
        CASE(TokenKind::NotEquals, "NotEquals");
        CASE(TokenKind::DoubleEquals, "DoubleEquals");
        CASE(TokenKind::GreaterEquals, "GreaterEquals");
        CASE(TokenKind::LessEquals, "LessEquals");
        CASE(TokenKind::Assignment, "Assignment");
        CASE(TokenKind::Identifier, "Identifier");
        CASE(TokenKind::NumericLiteral, "NumericLiteral");
        CASE(TokenKind::If, "If");
        CASE(TokenKind::True, "True");
        CASE(TokenKind::False, "False");
        CASE(TokenKind::For, "For");
        CASE(TokenKind::Do, "Do");
        CASE(TokenKind::Return, "Return");
        CASE(TokenKind::Variable, "Variable");
        CASE(TokenKind::Algorithm, "Algorithm");
        CASE(TokenKind::Is, "Is");
        CASE(TokenKind::Then, "Then");
        CASE(TokenKind::End, "End");
        CASE(TokenKind::Print, "Print");
        CASE(TokenKind::Illegal, "Illegal");
        CASE(TokenKind::Eof, "Eof");
    }

    #undef CASE

    return std::format("<{} ({})>", name, literal());
}

bool
Token::is_operator() const noexcept
{
    return m_kind == TokenKind::Plus
        || m_kind == TokenKind::Minus
        || m_kind == TokenKind::Multiply
        || m_kind == TokenKind::Divide
        || m_kind == TokenKind::Exponent;
}

unsigned
Token::operator_precedence() const noexcept
{
    // Based on Pratt's parser logic.
    switch (m_kind) {
        case TokenKind::Minus:
        case TokenKind::Plus:
            return 1;
        case TokenKind::Multiply:
        case TokenKind::Divide:
            return 2;
        case TokenKind::Exponent:
            return 3;
        default:
            return 0;
    }
}

OperatorAssociativity
Token::operator_associativity() const noexcept
{
    switch (m_kind) {
        case TokenKind::Minus:
        case TokenKind::Plus:
        case TokenKind::Multiply:
        case TokenKind::Divide:
            return OperatorAssociativity::Left;
        case TokenKind::Exponent:
            return OperatorAssociativity::Right;
        default:
            return OperatorAssociativity::Unknown;
    }
}
