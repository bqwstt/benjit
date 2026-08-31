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
        CASE(TokenKind::Else, "Else");
        CASE(TokenKind::True, "True");
        CASE(TokenKind::False, "False");
        CASE(TokenKind::For, "For");
        CASE(TokenKind::Do, "Do");
        CASE(TokenKind::Break, "Break");
        CASE(TokenKind::Continue, "Continue");
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

unsigned
Token::operator_precedence() const noexcept
{
    switch (m_kind) {
        case TokenKind::Or:
            return 1;
        case TokenKind::And:
            return 2;
        case TokenKind::DoubleEquals:
        case TokenKind::NotEquals:
            return 3;
        case TokenKind::LessThan:
        case TokenKind::LessEquals:
        case TokenKind::GreaterThan:
        case TokenKind::GreaterEquals:
            return 4;
        case TokenKind::Minus:
        case TokenKind::Plus:
            return 5;
        case TokenKind::Multiply:
        case TokenKind::Divide:
            return 6;
        case TokenKind::Exponent:
            return 7;
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
        case TokenKind::And:
        case TokenKind::Or:
            return OperatorAssociativity::Left;
        case TokenKind::Exponent:
            return OperatorAssociativity::Right;
        case TokenKind::DoubleEquals:
        case TokenKind::NotEquals:
        case TokenKind::LessThan:
        case TokenKind::LessEquals:
        case TokenKind::GreaterThan:
        case TokenKind::GreaterEquals:
            return OperatorAssociativity::NonAssociative;
        default:
            return OperatorAssociativity::Unknown;
    }
}