#include <string>
#include <print>
#include <format>

#include "reporter.hh"

void
Reporter::report_error(std::string_view message)
{
    // Maybe, @TODO: add a red color for errors.
    std::print("error: {}\n", message);
}

void
Reporter::report_error_at(std::string_view message,
                          std::string_view source_code,
                          uint32_t line,
                          uint32_t column,
                          std::string_view hint)
{
    // FIXME(benja): This doesn't correctly display the source line.
    std::string source_line;
    size_t code_pos = line * column - column;
    char current = source_code[code_pos];

    while (current != '\n' && current != '\0') {
        source_line += current;
        code_pos += 1;
        current = source_code[code_pos];
    }

    uint32_t lines = line;
    int line_digits = 0;
    while (lines != 0) {
        lines /= 10;
        line_digits++;
    }

    // Show an error in the form of:
    // error: number cannot end with a dot.
    //-> [source_file]:1:2
    //     |
    // 521 | 0. fun
    //     |

    std::string spaces(line_digits, ' ');
    std::string context = std::format(R"(
{} |
{} | {}
{} |)", spaces, line, source_line, spaces);
    std::print("error: {}\n-> [source_file]:{}:{}{}\n", message, line, column, context);
}