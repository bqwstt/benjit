#pragma once

#include <string>
#include <stdint.h>

using TokenLiteral = std::string;

enum class TokenKind : unsigned short {
    // Single-character tokens
    Plus = 0,           // +
    Minus,              // -
    Multiply,           // *
    Divide,             // /
    Hash,               // #
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

    // Literals
    Identifier = 48,
    NumericLiteral,

    // Keywords
    If,
    True,
    False,
    For,
    Do,
    Return,
    Variable,
    Algorithm,
    Is,
    Then,
    End,

    Illegal = 126,
    Eof = 127,
};

struct TokenPos {
    uint32_t line;
    uint32_t column;
};

class Token {
public:
    Token() = delete;
    Token(const TokenKind& kind, TokenPos pos) : m_kind(kind), m_pos(pos) {}
    Token(const char* literal, const TokenKind& kind, TokenPos pos)
        : m_literal(literal), m_kind(kind), m_pos(pos), m_lexeme(literal) {}
    Token(const char* literal, const TokenKind& kind, TokenPos pos, const char* lexeme)
        : m_literal(literal), m_kind(kind), m_pos(pos), m_lexeme(lexeme) {}
    Token(const Token& token) = default;
    Token(Token&& token) noexcept = default;
    ~Token() = default;

    [[nodiscard]] std::string to_string() const;
    [[nodiscard]] TokenKind kind() const { return m_kind; };
    [[nodiscard]] TokenLiteral literal() const { return m_literal; }
private:
    TokenLiteral m_literal;
    TokenKind m_kind;
    TokenPos m_pos;
    const char* m_lexeme = "";
};