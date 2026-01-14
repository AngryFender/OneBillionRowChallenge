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
        if((view[temp.first+i] >= ZERO_DIGIT && view[temp.first+i] <= NINE_DIGIT) || view[temp.first+i] == '.' )
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

inline size_t find_eof(std::string_view view, size_t start)
{
    while(view[start] != '\n')
    {
        ++start;
    }
    return start;
}

#endif //HELPER_H
