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
        CASE(TokenKind::Illegal, "Illegal");
        CASE(TokenKind::Eof, "Eof");
    }

    #undef CASE

    return std::format("<{} ({})>", name, std::string(m_literal));
}