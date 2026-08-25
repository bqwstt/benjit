#pragma once

#include <cstdint>
#include <string_view>

struct Reporter
{
    static void
    report_error(std::string_view message);

    static void
    report_error_at(std::string_view message,
                    std::string_view source_code,
                    uint32_t line,
                    uint32_t column,
                    std::string_view hint = "");
};