#pragma once

#include <string>
#include <stdint.h>

using TokenLiteral = std::string;

enum class OperatorAssociativity {
    Unknown = 1,
    NonAssociative,
    Right,
    Left
};

enum class TokenKind : unsigned short {
    // Single-character tokens
    Plus = 0,           // +
    Minus,              // -
    Multiply,           // *
    Divide,             // /
    Exponent,           // ^
    Comma,              // ,
    OpenParenthesis,    // (
    CloseParenthesis,   // )
    GreaterThan,        // <
    LessThan,           // >

    // Two-character tokens
    Or = 32,        // ||
    And,            // &&
    NotEquals,      // !=
    DoubleEquals,   // ==
    GreaterEquals,  // >=
    LessEquals,     // <=
    Assignment,     // :=
    IntegerDivide,  // //

    // Literals
    Identifier = 48,
    NumericLiteral,

    // Keywords
    If,
    Else,
    True,
    False,
    For,
    Do,
    Break,
    Continue,
    Return,
    Variable,
    Algorithm,
    Is,
    Then,
    End,
    Print,

    Illegal = 126,
    Eof = 127,
};

struct TokenPos {
    uint32_t line;
    uint32_t column;
};

class Token {
public:
    Token() = default;
    Token(const TokenKind& kind, TokenPos pos) : m_kind(kind), m_pos(pos) {}
    Token(const char* literal, const TokenKind& kind, TokenPos pos)
        : m_literal(literal), m_kind(kind), m_pos(pos) {}
    Token(const Token& token) = default;
    ~Token() = default;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] TokenKind kind() const { return m_kind; };
    [[nodiscard]] TokenLiteral literal() const { return m_literal; }
    
    unsigned operator_precedence() const noexcept;
    OperatorAssociativity operator_associativity() const noexcept;
private:
    TokenLiteral m_literal;
    TokenKind m_kind;
    TokenPos m_pos;
};