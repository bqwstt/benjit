#include <fstream>
#include <sstream>
#include <cctype>
#include <format>

#include "lexer.hh"
#include "token.hh"
#include "reporter.hh"

std::string
read_file(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Unable to open file: " + path);

    std::stringstream contents;
    contents << file.rdbuf();

    file.close();
    return contents.str();
}

Lexer::Lexer(const std::filesystem::path& source_file)
{
    if (!std::filesystem::exists(source_file)) {
        std::string msg = std::format("file '{}' does not exist.", source_file.string());
        m_had_error = true;
        Reporter::report_error(msg);
        return;
    }

    m_source_code = read_file(source_file);
}

Token
Lexer::consume_token()
{
    if (is_at_end())
        return Token(TokenKind::Eof, TokenPos{m_line, m_column});

    char character = read_char();
    while (true) {
        if (is_at_end())
            return Token(TokenKind::Eof, TokenPos{m_line, m_column});

        // Read whitespace
        if (character == ' ' || character == '\r' || character == '\t') {
            character = read_char();
        } else if (character == '\n') {
            set_new_line();
            character = read_char();
        } else if (character == '#') {
            // Found comment, skip the whole line.
            while (character != '\n' && !is_at_end())
                character = read_char();
        } else {
            break;
        }
    }

    m_start = m_current;
    return translate(character);
}

Token
Lexer::translate(const char character) noexcept
{
    TokenPos pos = TokenPos{m_line, m_column};
    switch (character) {
        case '.': {
            const char next_char = peek();
            if (std::isdigit(next_char)) {
                // Found digit, advance cursor until no more digits left.
                std::string literal = ".";

                while (std::isdigit(peek())) {
                    literal += peek(); 
                    m_current += 1;
                }
                
                Reporter::report_error_at("number cannot start with a dot.",
                                      m_current_line,
                                      m_line,
                                      m_column);
                m_had_error = true;
                return Token(literal.c_str(), TokenKind::Illegal, pos);
            }
        }
        case '+': return Token("+", TokenKind::Plus, pos);
        case '-': return Token("-", TokenKind::Minus, pos);
        case '*': return Token("*", TokenKind::Multiply, pos);
        case '/': return Token("/", TokenKind::Divide, pos);
        case '^': return Token("^", TokenKind::Exponent, pos);
        case '|':
            if (match('|'))
                return Token("||", TokenKind::Or, pos);

            break;
        case '!':
            if (match('='))
                return Token("!=", TokenKind::NotEquals, pos);

            break;
        case ',':
            return Token(",", TokenKind::Comma, pos);
        case '=':
            if (match('='))
                return Token("==", TokenKind::DoubleEquals, pos);

            break;
        case ':':
            if (match('='))
                return Token(":=", TokenKind::Assignment, pos);

            break;
        case '&':
            if (match('&'))
                return Token("&&", TokenKind::And, pos);

            break;
        case '(':
            return Token("(", TokenKind::OpenParenthesis, pos);
        case ')':
            return Token(")", TokenKind::CloseParenthesis, pos);
        case '<':
            if (match('='))
                return Token("<=", TokenKind::LessEquals, pos);

            return Token("<", TokenKind::LessThan, pos);
        case '>':
            if (match('='))
                return Token(">=", TokenKind::GreaterEquals, pos);

            return Token(">", TokenKind::GreaterThan, pos);
        default:
            if (std::isalpha(character))
                return translate_sequence(character);
            else if (std::isdigit(character))
                return translate_digit_literal(character);
    }

    // Illegal token, advance cursor and display error
    uint32_t column = m_column;
    while (peek() != '\n') {
        std::ignore = read_char();
    }

    m_had_error = true;
    Reporter::report_error_at("Unknown token.",
                            m_current_line,
                            m_line,
                            column);
    return Token(TokenKind::Illegal, pos);
}

Token
Lexer::translate_sequence(const char begin) noexcept
{
    // TODO(benja): Right now we are checking for keywords, and if none is found,
    // we return an identifier. We should check for syntax errors too.
    std::string sequence;
    char current_char = begin;

    auto eligible_for_identifier = [](const char character) -> bool {
        return std::isalnum(character) || character == '_';
    };

    while (eligible_for_identifier(current_char)) {
        sequence += current_char;
        const char next_char = peek();

        current_char = next_char;
        advance_if(eligible_for_identifier(next_char));
    }

    // The current line already had the first character from the former iteration,
    // skip it.
    m_current_line += sequence.substr(1);

    if (k_keywords.contains(sequence)) {
        // Found keyword.
        return Token(sequence.c_str(), k_keywords[sequence], TokenPos{m_line, m_column});
    }

    return Token(sequence.c_str(), TokenKind::Identifier, TokenPos{m_line, m_column});
}

Token
Lexer::translate_digit_literal(const char begin) noexcept
{
    // There may be two sequences of characters for a digit:
    // * 0 -> whole number (one token)
    // * 0.1 -> decimal number (one token)

    std::string literal;
    char current_char = begin;

    bool already_found_dot = false;

    while (std::isdigit(current_char)) {
        literal += current_char;
        const char next_char = peek();

        if (next_char == '.') {
            if (already_found_dot) {
                // The literal had more than one dot, which is an error.
                while (!is_stop(peek())) {
                    // Advance cursor until we reach the end of token.
                    // To give info to the user, also append the invalid characters to
                    // the literal.
                    literal += peek();
                    m_current += 1;
                }

                Reporter::report_error_at("number cannot contain more than one dot.",
                                       m_current_line,
                                       m_line,
                                       m_column);
                m_had_error = true;
                return Token(literal.c_str(), TokenKind::Illegal, TokenPos{m_line, m_column+1});
            }

            //  ┌──> next_char
            // 0.12
            // │ └─> to skip to
            // └───> current_char

            literal += next_char;
            current_char = skip_chars(2);

            already_found_dot = true;
        } else {
            current_char = next_char;
            advance_if(std::isdigit(next_char));
        }
    }

    if (literal.ends_with(".")) {
        Reporter::report_error_at("number cannot end with a dot.",
                               m_current_line,
                               m_line,
                               m_column);
        m_had_error = true;
        return Token(literal.c_str(), TokenKind::Illegal, TokenPos{m_line, m_column});
    }

    return Token(literal.c_str(), TokenKind::NumericLiteral, TokenPos{m_line, m_column});
}

inline char
Lexer::read_char()
{
    char current_char = m_source_code[m_current];
    m_column += 1;
    m_current += 1;
    m_current_line += current_char;
    return current_char;
}

bool
Lexer::match(const char expected) noexcept
{
    if (is_at_end() || m_source_code[m_current] != expected)
        return false;

    m_current += 1;
    return true;
}

char
Lexer::peek() const noexcept
{
    if (is_at_end())
        return '\0';

    return m_source_code[m_current];
}

char
Lexer::peek_ahead(uint32_t amount) const noexcept
{
    if (m_current + amount >= m_source_code.length())
        return '\0';

    return m_source_code[m_current + amount];
}

char
Lexer::peek_behind(uint32_t amount) const noexcept
{
    if (m_current - amount <= 0)
        return m_source_code[0];

    return m_source_code[m_current - amount];
}

char
Lexer::skip_chars(uint32_t amount) noexcept
{
    // peek() always is one step ahead from the current char we're reading,
    // so we need to substract 1 from the specified amount
    const char target = peek_ahead(amount - 1);
    m_current += amount;
    return target;
}

inline bool
Lexer::is_at_end() const noexcept
{
    return m_current >= m_source_code.length();
}

inline bool
Lexer::is_whitespace(const char character) const noexcept
{
    return character == ' '
        || character == '\n'
        || character == '\t'
        || character == '\r';
}

inline bool
Lexer::is_stop(const char character) const noexcept
{
    return is_whitespace(character) || is_at_end();
}

inline void
Lexer::advance_if(bool cond) noexcept
{
    if (cond) {
        m_current += 1;
        m_column += 1;
    }
}

inline void
Lexer::set_new_line() noexcept
{
    m_line += 1;
    m_column = 0;
    m_current_line = "";
}