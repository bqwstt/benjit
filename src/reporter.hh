#pragma once

#include <cstdint>
#include <string_view>

struct Reporter
{
    static void
    report_error(std::string_view message);

    static void
    report_error_at(std::string_view message,
                    std::string_view line,
                    uint32_t line_number,
                    uint32_t column_number,
                    std::string_view hint = "");
};