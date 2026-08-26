#pragma once

#include <filesystem>
#include <unordered_map>
#include <stdint.h>
#include <string>

#include "token.hh"

static
std::unordered_map<std::string, TokenKind> k_keywords {
    {"if", TokenKind::If},
    {"true", TokenKind::True},
    {"false", TokenKind::False},
    {"for", TokenKind::For},
    {"do", TokenKind::Do},
    {"return", TokenKind::Return},
    {"variable", TokenKind::Variable},
    {"algorithm", TokenKind::Algorithm},
    {"is", TokenKind::Is},
    {"then", TokenKind::Then},
    {"end", TokenKind::End},
    {"print", TokenKind::Print},
};

class Lexer {
public:
    explicit Lexer(const std::filesystem::path& source_file);
    explicit Lexer(const std::string& source_code) : m_source_code(source_code) {}
    ~Lexer() = default;

    [[nodiscard]] Token consume_token();

    [[nodiscard]] bool had_error() const noexcept { return m_had_error; }
private:
    uint32_t m_start = 0;
    uint32_t m_current = 0;
    uint32_t m_line = 1;
    uint32_t m_column = 1;

    std::string m_current_line;

    /// Whether it failed scanning.
    bool m_had_error = false;

    /// Owned view into the source code.
    std::string m_source_code;

    /// Consumes the character under the cursor and advances it.
    [[nodiscard]] char read_char();
    
    /// Checks whether `expected` is found under the cursor, and if so,
    /// advances the it.
    [[nodiscard]] bool match(const char expected) noexcept;

    /// Returns the character under the cursor.
    char peek() const noexcept;

    /// Returns the character under the cursor plus `amount`.
    char peek_ahead(uint32_t amount) const noexcept;

    /// Returns the character under the cursor minus `amount`.
    char peek_behind(uint32_t amount) const noexcept;

    /// Advances the cursor by `amount` and returns the character under it.
    char skip_chars(uint32_t amount) noexcept;

    [[nodiscard]] Token translate(const char character) noexcept;
    [[nodiscard]] Token translate_string_literal() noexcept;
    [[nodiscard]] Token translate_digit_literal(const char begin) noexcept;
    [[nodiscard]] Token translate_sequence(const char begin) noexcept;

    /// Checks whether the cursor has passed the end of the source code.
    bool is_at_end() const noexcept;

    /// Checks whether `character` is whitespace.
    bool is_whitespace(const char character) const noexcept;

    /// Checks whether `character` is a stop character.
    /// A stop character is either:
    /// - a whitespace character
    /// - at the end of the source code
    bool is_stop(const char character) const noexcept;

    /// Advances the cursor by 1 if `cond` is true.
    void advance_if(bool cond) noexcept;

    /// Sets the line and column cursors to point to a new line.
    void set_new_line() noexcept;
};