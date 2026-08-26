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
                          std::string_view line,
                          uint32_t line_number,
                          uint32_t column_number,
                          std::string_view hint)
{
    uint32_t lines = line_number;
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
{} |)", spaces, line_number, line, spaces);
    std::print("error: {}\n-> [source_file]:{}:{}{}\n", message, line_number, column_number, context);
}