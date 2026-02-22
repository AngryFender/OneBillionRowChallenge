#ifndef HELPER_H
#define HELPER_H
#include <cstdint>
#include <iostream>
#include <string_view>

constexpr int ZERO_DIGIT = 48;
constexpr int NINE_DIGIT = 57;

inline uint32_t parse_value_view(std::string_view view, const std::pair<size_t, size_t>& temp)
{
    uint32_t value = 0;
    for(size_t i = 0 ; i < temp.second; ++i)
    {
        if((view[temp.first+i] >= ZERO_DIGIT && view[temp.first+i] <= NINE_DIGIT))
        {
            value = value * 10 + static_cast<int>(view[temp.first + i]) - ZERO_DIGIT;
        }
    }
    return value;
}

inline uint32_t parse_value_str(const std::string& str)
{
    uint32_t value = 0;
    for(const char ch: str)
    {
        if((ch >= ZERO_DIGIT && ch <= NINE_DIGIT) || ch == '.' )
        {
            value = value * 10 + static_cast<int>(ch) - ZERO_DIGIT;
        }
    }
    return value;
}

inline size_t find_eol(std::string_view view, size_t start, const size_t eof)
{
    while(start < eof && view[start] != '\n')
    {
        ++start;
    }
    return start;
}

inline size_t find_eol_reverse(std::string_view view, size_t start, const size_t eof)
{
    while(view[start] != '\n' && start > 0 && start != eof)
    {
        --start;
    }
    return start;
}

#endif //HELPER_H
